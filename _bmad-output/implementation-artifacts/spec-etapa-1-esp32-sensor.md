---
title: 'Etapa 1 del ESP32 sensor HC-SR04'
type: 'feature'
created: '2026-10-12'
status: 'in-review'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'NO_VCS'
context:
  - 'Práctica_2.md'
  - 'informe.md'
  - 'diagramas/arquitectura.md'
  - 'requisitos_funcionales_parqueo_practica_1.md'
  - 'Revision_P1_Grupo_3.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** `ESP32 sensor/` contiene solo la plantilla PlatformIO y aún no mide ni informa el estado físico del HC-SR04.

**Approach:** Implementar una clase reutilizable `UltrasonicSensor` que reciba pines y rango por constructor, mida con TRIG/ECHO, diferencie lectura válida, timeout y fuera de rango, y sea coordinada desde `main.cpp` a 5 Hz mediante Serial a 115200 baudios.

## Boundaries & Constraints

**Always:** Usar GPIO18 como TRIG, GPIO19 como ECHO, rango inclusivo de 2 a 100 cm, `pulseIn` con timeout explícito de 30 ms, conversión `duración_us / 58.0`, muestras cada 200 ms por `millis()` y salida Serial legible. Mantener en documentación ECHO → 1 kΩ → GPIO19 y GPIO19 → 2 kΩ → GND.

**Never:** Implementar WiFi, TCP, clasificación DANGER/WARNING/SAFE, LEDs, actuador, servidor o credenciales; afirmar resultados físicos no ejecutados; modificar otros proyectos PlatformIO.

## I/O & Edge-Case Matrix

| Escenario | Entrada/estado | Salida esperada | Manejo |
|---|---|---|---|
| Eco válido | Duración no nula y distancia en `[2,100]` | Estado válido y cm sin redondear | Serial `Distancia: <cm> cm` |
| Sin eco | `pulseIn` devuelve 0 en 30 ms | Estado `TIMEOUT` | Serial `INVALID: TIMEOUT` |
| Fuera de rango | Distancia `<2` o `>100` | Estado `OUT_OF_RANGE` | Serial `INVALID: OUT_OF_RANGE` |
| Intervalo | `millis()` avanza | Un intento cada 200 ms | No bloquear entre intentos |

</frozen-after-approval>

## Code Map

- `ESP32 sensor/platformio.ini` -- entorno Arduino para `esp32doit-devkit-v1`; no requiere nuevas dependencias.
- `ESP32 sensor/src/main.cpp` -- plantilla a reemplazar por coordinación de Serial y muestreo.
- `ESP32 sensor/include/` -- destino de `UltrasonicSensor.h`; expone contrato documentado.
- `ESP32 sensor/src/` -- destino de `UltrasonicSensor.cpp`; encapsula pulso, captura, conversión y validación.
- `ESP32 sensor/test/` -- destino de una prueba unitaria de límites, sin acceso físico al hardware.
- `informe.md`, `diagramas/arquitectura.md`, `pruebas/manuales/README.md`, `pruebas/automatizadas/README.md` -- documentación a actualizar solo para esta etapa.

## Tasks & Acceptance

**Execution:**
- [x] `ESP32 sensor/include/UltrasonicSensor.h` y `ESP32 sensor/src/UltrasonicSensor.cpp` -- implementar estado y adquisición encapsulados.
- [x] `ESP32 sensor/src/main.cpp` -- inicializar Serial/sensor y programar intentos de 200 ms.
- [x] `ESP32 sensor/test/` -- añadir prueba unitaria de límites y conversión, si la infraestructura PlatformIO lo permite sin dependencias nuevas.
- [x] `informe.md`, `diagramas/arquitectura.md`, `pruebas/manuales/README.md`, `pruebas/automatizadas/README.md` -- reflejar clase real, procedimiento de sensor y estado pendiente de prueba física.

**Acceptance Criteria:**
- Given un eco equivalente a 2, 20, 50 o 100 cm, when el sensor mide, then devuelve una distancia válida sin clasificarla.
- Given ausencia de eco durante 30 ms, when el sensor mide, then informa `TIMEOUT`.
- Given una distancia menor que 2 o mayor que 100 cm, when el sensor mide, then informa `OUT_OF_RANGE`.
- Given el firmware iniciado, when transcurren 200 ms, then realiza un único nuevo intento y escribe su resultado a 115200 baudios.

## Implementation Notes

- Baseline `NO_VCS`: la herramienta Git no puede resolver la ruta de trabajo en este entorno; se verificará con compilación PlatformIO e inspección directa.
- Compilación `pio run -e esp32doit-devkit-v1`: exitosa. Las pruebas Unity requieren `test_port` y una placa conectada, por lo que no se ejecutaron.

## Spec Change Log

## Review Triage Log

## Verification

**Commands:**
- `pio run -e esp32doit-devkit-v1` desde `ESP32 sensor/` -- esperado: compilación exitosa.

**Manual checks:**
- Inspeccionar Serial Monitor y ejecutar el procedimiento físico sin registrar PASS hasta tener logs crudos.

**Resultado de esta implementación:** `pio run -e esp32doit-devkit-v1` finalizó correctamente. El objetivo Unity se compiló con `pio test -e esp32doit-devkit-v1 --without-uploading`, pero no se ejecutó porque no se proporcionó `test_port` ni una placa; no hay resultado PASS físico.
