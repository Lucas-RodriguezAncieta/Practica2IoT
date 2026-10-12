import math
import re
from dataclasses import dataclass
from typing import Optional


DISTANCE_PATTERN = re.compile(r"^DISTANCE ([0-9]+(?:\.[0-9]{1,2})?)$")


@dataclass(frozen=True)
class Message:
    command: str
    value: Optional[str] = None


def parse_line(line: str) -> Message:
    """Interpreta una línea ya delimitada; lanza ValueError con código protocolar."""
    if line in {"REGISTER SENSOR", "REGISTER ACTUATOR", "INVALID"}:
        return Message(line)
    if line.startswith("ACK ") and line[4:] in {"DANGER", "WARNING", "SAFE", "INVALID"}:
        return Message("ACK", line[4:])
    match = DISTANCE_PATTERN.fullmatch(line)
    if match:
        value = float(match.group(1))
        if math.isfinite(value):
            return Message("DISTANCE", match.group(1))
    if line.startswith("DISTANCE"):
        raise ValueError("INVALID_DISTANCE")
    if not line or " " in line.strip():
        raise ValueError("INVALID_FORMAT")
    raise ValueError("INVALID_COMMAND")
