import logging
import socket
import threading
import time
from typing import Optional

from config import (
    MAX_LINE_BYTES,
    MEASUREMENT_TIMEOUT_SECONDS,
    STATE_PUBLISH_INTERVAL_SECONDS,
)
from distance_controller import DistanceController, ProximityState
from protocol import parse_line


class TcpServer:
    """Servidor TCP concurrente para un SENSOR y un ACTUATOR."""

    def __init__(self, host: str, port: int) -> None:
        self.host, self.port = host, port
        self.controller = DistanceController()
        self.state = ProximityState.INVALID
        self.last_measurement: Optional[float] = None
        self.last_sent_state: Optional[ProximityState] = None
        self.last_ack_state: Optional[ProximityState] = None
        self.clients: dict[str, socket.socket] = {}
        self.lock = threading.RLock()
        self._send_lock = threading.Lock()
        self._listener: Optional[socket.socket] = None
        self.ready = threading.Event()
        self.running = threading.Event()

    def serve_forever(self) -> None:
        """Escucha clientes hasta que ``stop`` solicite el cierre."""
        self.running.set()
        self.ready.clear()
        threading.Thread(target=self._state_loop, daemon=True).start()
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as listener:
            self._listener = listener
            listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            listener.bind((self.host, self.port))
            self.port = listener.getsockname()[1]
            listener.listen()
            listener.settimeout(0.2)
            self.ready.set()
            logging.info("[SERVER] Esperando conexiones en %s:%s", self.host, self.port)
            try:
                while self.running.is_set():
                    try:
                        conn, _ = listener.accept()
                    except socket.timeout:
                        continue
                    except OSError:
                        break
                    threading.Thread(
                        target=self._handle_client, args=(conn,), daemon=True
                    ).start()
            finally:
                self._listener = None

    def stop(self) -> None:
        """Detiene la aceptación de conexiones sin dejar ``accept`` bloqueado."""
        self.running.clear()
        listener = self._listener
        if listener:
            try:
                listener.close()
            except OSError:
                pass

    def _handle_client(self, conn: socket.socket) -> None:
        role: Optional[str] = None
        buffer = b""
        discarding_oversize = False
        with conn:
            while self.running.is_set():
                try:
                    data = conn.recv(256)
                except OSError:
                    break
                if not data:
                    break
                buffer += data
                while True:
                    if discarding_oversize:
                        newline = buffer.find(b"\n")
                        if newline < 0:
                            buffer = b""
                            break
                        buffer = buffer[newline + 1 :]
                        discarding_oversize = False
                        self._send(conn, "ERROR INVALID_FORMAT")
                        continue
                    newline = buffer.find(b"\n")
                    if newline < 0:
                        if len(buffer) > MAX_LINE_BYTES:
                            buffer = b""
                            discarding_oversize = True
                        break
                    raw, buffer = buffer.split(b"\n", 1)
                    if len(raw) > MAX_LINE_BYTES:
                        self._send(conn, "ERROR INVALID_FORMAT")
                        continue
                    try:
                        line = raw.decode("utf-8").rstrip("\r")
                    except UnicodeDecodeError:
                        self._send(conn, "ERROR INVALID_FORMAT")
                        continue
                    role = self._process(conn, role, line)
        self._disconnect(role, conn)

    def _process(self, conn: socket.socket, role: Optional[str], line: str) -> Optional[str]:
        try:
            message = parse_line(line)
        except ValueError as error:
            self._send(conn, f"ERROR {error}")
            return role
        if message.command.startswith("REGISTER"):
            requested = message.command.split()[1]
            with self.lock:
                if role or requested in self.clients:
                    self._send(conn, "ERROR INVALID_COMMAND")
                    return role
                self.clients[requested] = conn
                state = self.state
            self._send(conn, "OK")
            if requested == "ACTUATOR":
                self._send_state(conn, state)
            logging.info("[SERVER] %s conectado", requested)
            return requested
        if role == "SENSOR" and message.command == "DISTANCE":
            state = self.controller.classify(float(message.value))
            with self.lock:
                self.last_measurement = time.monotonic()
            self._set_state(state)
            logging.info("[SERVER] Distancia recibida: %s cm", message.value)
        elif role == "SENSOR" and message.command == "INVALID":
            self._invalidate_sensor()
        elif role == "ACTUATOR" and message.command == "ACK":
            acknowledged = ProximityState(message.value)
            with self.lock:
                self.last_ack_state = acknowledged
                sent = self.last_sent_state
            if sent != acknowledged:
                logging.warning(
                    "[SERVER] ACK %s no coincide con el ultimo SET %s",
                    acknowledged.value,
                    sent.value if sent else "NINGUNO",
                )
            else:
                logging.info("[SERVER] Confirmacion: ACK %s", acknowledged.value)
        else:
            self._send(conn, "ERROR INVALID_COMMAND")
        return role

    def _state_loop(self) -> None:
        while self.running.is_set():
            self._expire_stale_measurement()
            self._publish_state()
            self.running.wait(STATE_PUBLISH_INTERVAL_SECONDS)

    def _expire_stale_measurement(self) -> None:
        """Invalida una lectura que ya no representa el entorno actual."""
        with self.lock:
            is_stale = (
                self.last_measurement is not None
                and time.monotonic() - self.last_measurement
                > MEASUREMENT_TIMEOUT_SECONDS
            )
        if is_stale:
            self._invalidate_sensor()

    def _invalidate_sensor(self) -> None:
        with self.lock:
            self.last_measurement = None
        self._set_state(ProximityState.INVALID)

    def _set_state(self, state: ProximityState) -> None:
        with self.lock:
            if self.state != state:
                self.state = state
                logging.info("[SERVER] Estado: %s", state.value)

    def _publish_state(self) -> None:
        with self.lock:
            actuator = self.clients.get("ACTUATOR")
            state = self.state
        if actuator:
            self._send_state(actuator, state)

    def _send_state(self, conn: socket.socket, state: ProximityState) -> None:
        with self.lock:
            self.last_sent_state = state
        self._send(conn, f"SET {state.value}")

    def _send(self, conn: socket.socket, line: str) -> None:
        """Serializa escrituras para que dos hilos no mezclen mensajes TCP."""
        try:
            with self._send_lock:
                conn.sendall(f"{line}\n".encode("utf-8"))
            logging.info("[SERVER] Enviando: %s", line)
        except OSError:
            pass

    def _disconnect(self, role: Optional[str], conn: socket.socket) -> None:
        sensor_disconnected = False
        with self.lock:
            if role and self.clients.get(role) is conn:
                del self.clients[role]
                sensor_disconnected = role == "SENSOR"
                logging.info("[SERVER] %s desconectado", role)
        if sensor_disconnected:
            self._invalidate_sensor()
            logging.info("[SERVER] Estado seguro: INVALID")
            self._publish_state()
