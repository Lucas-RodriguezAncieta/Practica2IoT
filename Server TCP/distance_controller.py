from enum import Enum
import math


class ProximityState(str, Enum):
    DANGER = "DANGER"
    WARNING = "WARNING"
    SAFE = "SAFE"
    INVALID = "INVALID"


class DistanceController:
    """Clasifica distancias; esta es la única lógica de rangos del sistema."""

    def classify(self, distance_cm: float) -> ProximityState:
        if not math.isfinite(distance_cm) or not 2 <= distance_cm <= 100:
            return ProximityState.INVALID
        if distance_cm < 20:
            return ProximityState.DANGER
        if distance_cm < 50:
            return ProximityState.WARNING
        return ProximityState.SAFE
