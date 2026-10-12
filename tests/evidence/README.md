# Evidencias de pruebas

Esta carpeta está intencionalmente vacía de resultados iniciales. Al ejecutar pruebas, conservar datos originales y nombrarlos con fecha, caso y componente, por ejemplo `2026-10-12-M-01-servidor.log`. No guardar SSID, contraseña, `WiFiSecrets.h` ni capturas que los revelen.

- `sensor/`: logs, fotos y CSV de la medición HC-SR04 y su divisor.
- `actuator/`: fotos o video de LEDs, estados, exclusividad y temporizaciones.
- `stability/`: logs crudos de una ejecución continua de al menos 10 minutos, incluidos reinicios, bloqueos e inválidas.
- `accuracy/`: CSV de mediciones y cálculos reproducibles, con unidades, referencia física y condiciones.
- `latency/`: logs sincronizados o evidencia audiovisual para medir sensor → servidor → actuador.
- `sampling/`: logs crudos y cálculo de lecturas por segundo.
- `tcp/`: logs de registro, `ACK`, errores, rechazo de duplicados y reconexión.
- `build/`: salida completa de comprobación estática, pytest, compilación PlatformIO y Unity.

Ubicación de la Etapa 6: M-01, M-02 y M-11 en `tcp/`; M-03 en `accuracy/`; M-04 en `actuator/`; M-05 en `tcp/` y `actuator/`; M-06 en `sampling/`; M-07 en `stability/`; M-08 en `latency/`; M-09 en `sensor/`/`actuator/`/`tcp/`; y M-10 en `build/`. Cada evidencia debe enlazarse desde la ficha del caso, y conservar responsable, fecha, versión de firmware/servidor y condición de prueba. Junto a cada conjunto guardar una ficha basada en `tests/manual/plantilla-resultados.md`.
