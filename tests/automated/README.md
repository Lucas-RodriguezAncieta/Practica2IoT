# Pruebas automatizadas

La etapa del sensor incluye `ESP32 sensor/test/test_ultrasonic_sensor/test_main.cpp`. Cubre la conversión `duración_us / 58.0`, los límites inclusivos de 2 y 100 cm, el timeout (duración cero), las lecturas fuera de rango y la serialización pura de `DISTANCE`/`INVALID`. Puede ejecutarse con PlatformIO y Unity en una placa de prueba:

```text
pio test -e esp32doit-devkit-v1
```

La ejecución requiere una placa conectada; no se afirma PASS hasta conservar su salida. La compilación del firmware se verifica por separado con `pio run -e esp32doit-devkit-v1`.

### Verificación estática de WiFiSecrets

Sin crear `WiFiSecrets.h`, credenciales ni conexión de red, ejecutar desde la raíz:

```text
python tests/automated/check_wifi_secrets_config.py
```

La comprobación estructural inspecciona ambos firmwares y plantillas: confirma el fallback mediante `__has_include`, los marcadores no secretos, `kServerPort = 5000`, `kUsingExampleWiFiSecrets = true` en la plantilla, la exclusión de `WiFiSecrets.h` en Git y las guardas que habilitan WiFi/TCP solo cuando la bandera local es `false`. No compila firmware ni ejecuta las ramas; no crea archivos y no sustituye las pruebas físicas ni la revisión manual final de que ambos archivos locales usan la misma IPv4 y puerto.

| Suite Python implementada | Comportamiento cubierto | Relación |
|---|---|---|
| Clasificación pura | 2, 19.99, 20, 49.99, 50, 100; no finitos y fuera de rango; sin redondeo. | RF2, RF6 |
| Parser de líneas | Registro, `DISTANCE` en cm con punto y hasta dos decimales, `INVALID`, formato incompleto, roles incorrectos y comandos desconocidos. | RF4, RF7 |
| Estado seguro servidor | Ausencia de datos durante 1 s y publicación de `SET INVALID`. | RF3, RF5 |
| Cliente actuador (pendiente) | Aplicación de `SET` válido y fallback INVALID tras 1,5 s; requiere firmware del actuador. | RF3, RF5 |
| Integración de roles | Registro, rechazo de duplicado y comandos de rol incorrecto mediante clientes localhost simulados. | RF4, RF7 |

La suite Python está en `test_distance_controller.py` y `test_tcp_server.py`. Se ejecuta desde la raíz con:

```text
Server TCP\\.venv\\Scripts\\python.exe -m pytest tests/automated -q
```

El 12 de octubre de 2026 se ejecutaron 24 pruebas locales y todas aprobaron. Cubren clasificación y límites, parser, errores, registro por rol, fragmentación y concatenación de líneas TCP, expiración publicada al actuador, desconexión y re-registro de ambos roles, recuperación tras entradas inválidas y trazabilidad de `SET`/`ACK`. No sustituyen las pruebas físicas: exactitud, estabilidad de 10 minutos, frecuencia real de muestreo y tiempo extremo a extremo siguen pendientes de evidencia manual.
