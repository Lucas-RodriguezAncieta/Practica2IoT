import socket
import sys
import threading
import time
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).parents[2] / "Server TCP"))

from config import MEASUREMENT_TIMEOUT_SECONDS
from distance_controller import ProximityState
from tcp_server import TcpServer


def receive_line(connection: socket.socket) -> str:
    connection.settimeout(1)
    data = b""
    while b"\n" not in data:
        data += connection.recv(1)
    return data.split(b"\n", 1)[0].decode("utf-8")


def receive_until(connection: socket.socket, expected: str) -> None:
    deadline = time.monotonic() + 1
    while time.monotonic() < deadline:
        if receive_line(connection) == expected:
            return
    raise AssertionError(f"No se recibio {expected}")


def wait_until(condition: object) -> None:
    deadline = time.monotonic() + 1
    while time.monotonic() < deadline:
        if condition():
            return
        time.sleep(0.01)
    raise AssertionError("No se alcanzo el estado esperado")


@pytest.fixture
def server() -> TcpServer:
    instance = TcpServer("127.0.0.1", 0)
    thread = threading.Thread(target=instance.serve_forever, daemon=True)
    thread.start()
    assert instance.ready.wait(1)
    yield instance
    instance.stop()
    thread.join(1)


def connect(server: TcpServer) -> socket.socket:
    return socket.create_connection(("127.0.0.1", server.port), timeout=1)


def test_registration_and_duplicate_role_are_handled(server: TcpServer) -> None:
    sensor = connect(server)
    duplicate = connect(server)
    try:
        sensor.sendall(b"REGISTER SENSOR\n")
        assert receive_line(sensor) == "OK"
        duplicate.sendall(b"REGISTER SENSOR\n")
        assert receive_line(duplicate) == "ERROR INVALID_COMMAND"
    finally:
        sensor.close()
        duplicate.close()


def test_role_incorrect_command_keeps_connection_active(server: TcpServer) -> None:
    actuator = connect(server)
    try:
        actuator.sendall(b"REGISTER ACTUATOR\n")
        assert receive_line(actuator) == "OK"
        assert receive_line(actuator) == "SET INVALID"
        actuator.sendall(b"DISTANCE 20\n")
        receive_until(actuator, "ERROR INVALID_COMMAND")
        actuator.sendall(b"ACK INVALID\n")
        assert "ACTUATOR" in server.clients
    finally:
        actuator.close()


def test_fragmented_and_concatenated_tcp_lines(server: TcpServer) -> None:
    sensor = connect(server)
    try:
        sensor.sendall(b"REGISTER SE")
        sensor.sendall(b"NSOR\nDISTANCE 20\nDISTANCE 20\n")
        assert receive_line(sensor) == "OK"
        wait_until(lambda: server.state is ProximityState.WARNING)
        sensor.sendall(b"INVALID\n")
        wait_until(lambda: server.state is ProximityState.INVALID)
    finally:
        sensor.close()


@pytest.mark.parametrize(
    "line, expected",
    [
        (b"DISTANCE 20,5\n", "ERROR INVALID_DISTANCE"),
        (b"DISTANCE 20.123\n", "ERROR INVALID_DISTANCE"),
        (b"UNKNOWN\n", "ERROR INVALID_COMMAND"),
        (b"REGISTER SENSOR EXTRA\n", "ERROR INVALID_FORMAT"),
    ],
)
def test_malformed_messages_do_not_disconnect_client(
    server: TcpServer, line: bytes, expected: str
) -> None:
    sensor = connect(server)
    try:
        sensor.sendall(b"REGISTER SENSOR\n")
        assert receive_line(sensor) == "OK"
        sensor.sendall(line)
        assert receive_line(sensor) == expected
        sensor.sendall(b"DISTANCE 50\n")
        wait_until(lambda: server.state is ProximityState.SAFE)
    finally:
        sensor.close()


def test_measurement_expiration_and_recovery(server: TcpServer) -> None:
    sensor = connect(server)
    actuator = connect(server)
    try:
        actuator.sendall(b"REGISTER ACTUATOR\n")
        assert receive_line(actuator) == "OK"
        assert receive_line(actuator) == "SET INVALID"
        sensor.sendall(b"REGISTER SENSOR\nDISTANCE 18.45\n")
        assert receive_line(sensor) == "OK"
        wait_until(lambda: server.state is ProximityState.DANGER)
        receive_until(actuator, "SET DANGER")
        with server.lock:
            server.last_measurement = time.monotonic() - MEASUREMENT_TIMEOUT_SECONDS - 0.01
        server._expire_stale_measurement()
        server._publish_state()
        assert server.state is ProximityState.INVALID
        receive_until(actuator, "SET INVALID")
        sensor.sendall(b"DISTANCE 50\n")
        wait_until(lambda: server.state is ProximityState.SAFE)
    finally:
        sensor.close()
        actuator.close()


def test_sensor_disconnection_sets_safe_state(server: TcpServer) -> None:
    sensor = connect(server)
    sensor.sendall(b"REGISTER SENSOR\nDISTANCE 18\n")
    assert receive_line(sensor) == "OK"
    wait_until(lambda: server.state is ProximityState.DANGER)
    sensor.close()
    wait_until(
        lambda: "SENSOR" not in server.clients
        and server.state is ProximityState.INVALID
    )


def test_actuator_disconnection_releases_role(server: TcpServer) -> None:
    actuator = connect(server)
    actuator.sendall(b"REGISTER ACTUATOR\n")
    assert receive_line(actuator) == "OK"
    assert receive_line(actuator) == "SET INVALID"
    actuator.close()
    wait_until(lambda: "ACTUATOR" not in server.clients)
    replacement = connect(server)
    try:
        replacement.sendall(b"REGISTER ACTUATOR\n")
        assert receive_line(replacement) == "OK"
        assert receive_line(replacement) == "SET INVALID"
    finally:
        replacement.close()


@pytest.mark.parametrize(
    "invalid_line",
    [b"\xff\n", b"x" * 129 + b"\n"],
)
def test_invalid_bytes_or_oversize_line_allow_recovery(
    server: TcpServer, invalid_line: bytes
) -> None:
    sensor = connect(server)
    try:
        sensor.sendall(b"REGISTER SENSOR\n")
        assert receive_line(sensor) == "OK"
        sensor.sendall(invalid_line)
        assert receive_line(sensor) == "ERROR INVALID_FORMAT"
        sensor.sendall(b"DISTANCE 50\n")
        wait_until(lambda: server.state is ProximityState.SAFE)
    finally:
        sensor.close()


def test_ack_state_is_recorded_without_rejecting_stale_confirmation(
    server: TcpServer
) -> None:
    actuator = connect(server)
    try:
        actuator.sendall(b"REGISTER ACTUATOR\n")
        assert receive_line(actuator) == "OK"
        assert receive_line(actuator) == "SET INVALID"
        actuator.sendall(b"ACK DANGER\n")
        wait_until(lambda: server.last_ack_state is ProximityState.DANGER)
        assert server.last_sent_state is ProximityState.INVALID
    finally:
        actuator.close()
