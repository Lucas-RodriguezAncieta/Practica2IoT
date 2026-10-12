import math
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).parents[2] / "Server TCP"))

from distance_controller import DistanceController, ProximityState
from protocol import parse_line


def test_classification_boundaries() -> None:
    controller = DistanceController()
    assert controller.classify(2) is ProximityState.DANGER
    assert controller.classify(19.99) is ProximityState.DANGER
    assert controller.classify(20) is ProximityState.WARNING
    assert controller.classify(50) is ProximityState.SAFE
    assert controller.classify(100) is ProximityState.SAFE


def test_invalid_distances() -> None:
    controller = DistanceController()
    for value in (1.99, 100.01, math.nan, math.inf):
        assert controller.classify(value) is ProximityState.INVALID


@pytest.mark.parametrize(
    "distance, expected",
    [
        (2.01, ProximityState.DANGER),
        (19.999, ProximityState.DANGER),
        (20.001, ProximityState.WARNING),
        (49.999, ProximityState.WARNING),
        (50.001, ProximityState.SAFE),
        (99.999, ProximityState.SAFE),
        (1.999, ProximityState.INVALID),
        (100.001, ProximityState.INVALID),
    ],
)
def test_classification_values_adjacent_to_limits(
    distance: float, expected: ProximityState
) -> None:
    assert DistanceController().classify(distance) is expected


def test_protocol_distance_and_invalid_input() -> None:
    assert parse_line("DISTANCE 18.45").value == "18.45"
    for line in ("DISTANCE 20,5", "DISTANCE 20.123", "DISTANCE NaN"):
        try:
            parse_line(line)
        except ValueError as error:
            assert str(error) == "INVALID_DISTANCE"
        else:
            raise AssertionError("Se esperaba error de distancia")
