"""Verifica estáticamente la configuración versionada de WiFiSecrets, sin credenciales."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PROJECTS = ("ESP32 sensor", "ESP32 actuador")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    for project in PROJECTS:
        main_cpp = (ROOT / project / "src" / "main.cpp").read_text(encoding="utf-8")
        example = (ROOT / project / "include" / "WiFiSecrets.example.h").read_text(
            encoding="utf-8"
        )
        gitignore = (ROOT / project / ".gitignore").read_text(encoding="utf-8")

        require(
            '#if __has_include("WiFiSecrets.h")' in main_cpp
            and '#include "WiFiSecrets.h"' in main_cpp
            and '#include "WiFiSecrets.example.h"' in main_cpp,
            f"{project}: falta el fallback __has_include de WiFiSecrets",
        )
        expected_markers = {
            "kWifiSsid": "REPLACE_WITH_WIFI_SSID",
            "kWifiPassword": "REPLACE_WITH_WIFI_PASSWORD",
            "kServerIp": "REPLACE_WITH_PC_IPV4",
        }
        declarations = dict(
            re.findall(r'constexpr\s+char\s+(kWifi\w+|kServerIp)\[\]\s*=\s*"([^"]*)"\s*;', example)
        )
        require(
            declarations == expected_markers,
            f"{project}: la plantilla debe declarar solo los tres marcadores WiFi esperados",
        )
        for name, marker in expected_markers.items():
            require(
                re.search(rf'{name}\[\]\s*=\s*"{marker}"\s*;', example) is not None,
                f"{project}: la plantilla debe conservar el marcador de {name}",
            )
        require(
            re.search(r"kServerPort\s*=\s*5000\s*;", example) is not None,
            f"{project}: la plantilla debe usar el puerto 5000",
        )
        require(
            re.search(r"kUsingExampleWiFiSecrets\s*=\s*true\s*;", example) is not None,
            f"{project}: la plantilla debe mantener la bandera en true",
        )
        require(
            "include/WiFiSecrets.h" in gitignore,
            f"{project}: WiFiSecrets.h debe estar ignorado por Git",
        )

        if project == "ESP32 sensor":
            required_guards = (
                r"if\s*\(\s*!kUsingExampleWiFiSecrets\s*\)\s*\{\s*wifiConnection\.begin\(\)",
                r"if\s*\(\s*!kUsingExampleWiFiSecrets\s*\)\s*\{\s*tcpClient\.sendMeasurement",
                r"if\s*\(\s*!kUsingExampleWiFiSecrets\s*\)\s*\{\s*wifiConnection\.update[\s\S]*tcpClient\.update",
            )
        else:
            required_guards = (
                r"if\s*\(\s*kUsingExampleWiFiSecrets\s*\).*else\s+wifi\.begin\(\)",
                r"if\s*\(\s*!kUsingExampleWiFiSecrets\s*\)\s*\{\s*wifi\.update[\s\S]*tcp\.update",
            )
        for guard in required_guards:
            require(
                re.search(guard, main_cpp) is not None,
                f"{project}: falta una guarda explícita de red para la configuración local",
            )

    print("WiFiSecrets: fallback, marcadores, puerto y guardas verificados.")


if __name__ == "__main__":
    main()
