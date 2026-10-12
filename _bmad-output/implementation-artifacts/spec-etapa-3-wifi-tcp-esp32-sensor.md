---
title: 'Etapa 3: WiFi y TCP del ESP32 sensor'
type: 'feature'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** El ESP32 sensor ya mide con HC-SR04, pero aún no comunica mediciones al servidor TCP central mediante WiFi.

**Approach:** Incorporar clientes WiFi y TCP pequeños, con configuración local ignorada por Git, registro obligatorio antes del envío, lectura de respuestas por líneas y reconexión no bloqueante cada dos segundos. Mantener el sensor como fuente de mediciones sin clasificar.

</frozen-after-approval>

## Implementation Notes

- Alcance único: `ESP32 sensor/`, documentación y pruebas aplicables; no modificar servidor ni actuador.
- El servidor real acepta `REGISTER SENSOR`, responde `OK`, usa UTF-8 delimitado por `\n`, limita líneas a 128 bytes y requiere `DISTANCE` con hasta dos decimales o `INVALID`.
- No hay decisiones pendientes ni efectos irreversibles. La configuración real queda local en `WiFiSecrets.h` y el ejemplo no contiene secretos.
- Se implementaron `WiFiConnection`, `TcpSensorClient` y `SensorProtocol`; `main.cpp` conserva el muestreo con `millis()` y coordina red solo si existen secretos locales reales.
- La conexión TCP espera `OK` antes de enviar; procesa líneas fragmentadas/acumuladas, limita el buffer a 128 bytes y reinicia el registro después de caída o timeout.
- Se añadió prueba Unity para la serialización pura de `DISTANCE` e `INVALID`; la ejecución Unity y la integración WiFi/TCP quedan pendientes de placa física.
- Verificación real: PlatformIO compiló correctamente `esp32doit-devkit-v1` (RAM 7.7 %, flash 27.8 %); la suite Python del servidor mantuvo 24 aprobadas y 0 fallidas.

## Review Triage Log

- bajo, corregido: se reforzó la exclusión de `ESP32 sensor/include/WiFiSecrets.h` también en el `.gitignore` raíz; el `.gitignore` del subproyecto ya lo excluía.
- medio, corregido: sin secretos reales el firmware conserva la adquisición local pero no inicia WiFi ni TCP.
- medio, corregido: el muestreo se realiza antes del mantenimiento de red y el timeout TCP se redujo a 100 ms.
- medio, corregido: `sendLine` verifica ambas escrituras y desconecta ante envío parcial o fallido.
- medio, corregido parcialmente: se añadió prueba Unity para la serialización del protocolo; los casos que requieren `WiFiClient` y placa siguen pendientes de integración física.
- bajo, corregido: `diagramas/arquitectura.md` y el informe identifican las nuevas clases implementadas.
- bajo, corregido: se eliminó la dependencia UML inexistente entre `TcpSensorClient` y `WiFiConnection`; `main.cpp` coordina por booleano.
- bajo, corregido: la secuencia visible incluye `REGISTER SENSOR` seguido de `OK` y la regla de no enviar antes del registro.
