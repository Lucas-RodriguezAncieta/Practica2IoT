---
title: 'Documentación inicial de la Práctica 2'
type: 'feature'
created: '2026-10-11'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'NO_VCS'
context:
  - 'Práctica_2.md'
  - 'requisitos_funcionales_parqueo_practica_1.md'
  - 'Revision_P1_Grupo_3.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** El repositorio solo contiene las plantillas PlatformIO de los dos ESP32 y una carpeta vacía para el servidor. Antes de implementar, falta documentación trazable de requisitos, arquitectura, circuito, protocolo y pruebas.

**Approach:** Crear `informe.md`, diagramas Markdown y procedimientos de pruebas vacíos de resultados. La documentación reflejará las decisiones acordadas, distinguirá estado previsto de evidencia real y preservará la estructura existente de los proyectos.

## Boundaries & Constraints

**Always:** Mantener la clasificación exclusivamente en el servidor; usar TCP sobre WiFi con mensajes de texto terminados en `\n`; conservar `INVALID` como estado seguro; documentar el divisor de ECHO, GND común, evidencia física y logs crudos; enlazar requisitos, diseño, pruebas y futura implementación.

**Never:** Modificar firmware, servidor Python, `platformio.ini`, archivos internos de BMAD, dependencias o resultados de pruebas; declarar como realizado aquello que está pendiente; inventar firmas definitivas para el diagrama de clases.

**Decisions approved:** Ante un mensaje TCP malformado, el servidor responde `ERROR <reason>`, registra el evento y mantiene la conexión sin ejecutar acción alguna. Los únicos errores iniciales documentados son `ERROR INVALID_COMMAND`, `ERROR INVALID_DISTANCE` y `ERROR INVALID_FORMAT`. El informe tendrá tres filas de integrantes con nombre pendiente y responsabilidades iniciales: ESP32 sensor, ESP32 actuador y servidor TCP.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|---|---|---|---|
| Registro correcto | Cliente TCP envía `REGISTER SENSOR` o `REGISTER ACTUATOR` | El servidor asocia el rol y responde `OK`; al actuador le entrega el estado vigente | Rechazar un segundo cliente del mismo rol |
| Medición válida | Sensor envía `DISTANCE <valor finito>` | El servidor clasifica sin redondear y publica `SET <estado>` | Valores fuera de `[2,100]` producen `SET INVALID` |
| Medición ausente o inválida | Sensor envía `INVALID` o pasan 1 s sin datos | El servidor invalida el estado y envía `SET INVALID` | El actuador apaga LEDs |
| Pérdida de comandos | Actuador no recibe comandos durante 1,5 s | El actuador adopta `INVALID` localmente | Apaga todos los LEDs hasta nuevo comando válido |

</frozen-after-approval>

## Code Map

- `Práctica_2.md` — fuente de requisitos mínimos, entregables, rúbrica y restricciones académicas.
- `requisitos_funcionales_parqueo_practica_1.md` — fuente reutilizable para rangos, LEDs, límites, `INVALID` y reglas de parpadeo; no define la arquitectura TCP.
- `Revision_P1_Grupo_3.md` — evidencia de riesgos que la documentación debe prevenir: divisor ausente, UML desalineado, pruebas sin logs y contribuciones individuales no trazables.
- `ESP32 sensor/platformio.ini` y `ESP32 actuador/platformio.ini` — proyectos Arduino/ESP32 existentes que se conservan sin cambios.
- `ESP32 sensor/src/main.cpp`, `ESP32 actuador/src/main.cpp` y `Server TCP/` — plantillas/vacío; no son evidencia de funcionalidad actual y no se modificarán.

## Tasks & Acceptance

**Execution:**
- [x] `informe.md` -- documentar requisitos, decisiones de grupo, estado pendiente, arquitectura, protocolo, hardware, diseño provisional, trazabilidad y secciones de resultados sin datos -- crear la base del informe exigido.
- [x] `diagramas/arquitectura.md` -- incorporar arquitectura Mermaid, circuito técnico del sensor y del actuador, secuencia TCP y clases provisionales sin firmas inventadas -- hacer el diseño inspeccionable dentro del repositorio.
- [x] `pruebas/manuales/README.md` -- definir procedimientos reproducibles para límites, LEDs, TCP, reconexión y los RNF -- impedir validaciones solo por afirmación.
- [x] `pruebas/automatizadas/README.md` -- identificar pruebas unitarias y de protocolo futuras, sin escribir ni ejecutar código -- dejar objetivos de automatización trazables.
- [x] `pruebas/evidencias/**/.gitkeep` y `pruebas/evidencias/README.md` -- preservar la estructura de evidencia y explicar formatos de logs, CSV, fotos y video -- evitar datos o resultados ficticios.

**Acceptance Criteria:**
- Given el informe inicial, when se consulta cualquier RF o RNF declarado, then se identifica su origen, responsabilidad de componente, prueba prevista y estado pendiente.
- Given el diagrama de circuito del sensor, when se inspecciona ECHO, then se muestran ECHO → 1 kΩ → GPIO19 y GPIO19 → 2 kΩ → GND, junto con VCC de 5 V y GND común.
- Given una lectura o fallo de red, when se consulta el protocolo y la secuencia, then se puede determinar qué mensaje se intercambia y cómo se llega al estado seguro.
- Given las carpetas de pruebas, when el equipo ejecute una validación futura, then existe un lugar inequívoco para procedimiento y evidencia sin que el repositorio afirme resultados anticipados.

## Implementation Notes

- Baseline de versión: `NO_VCS`; la herramienta Git no pudo resolver el directorio de trabajo por la ruta con espacios, por lo que la verificación se hará mediante inspección de archivos creados y sus contenidos.

## Spec Change Log

## Review Triage Log

| Hallazgo | Veredicto | Evidencia y resolución |
|---|---|---|
| Roles retenidos tras desconexión | medium / patch | Era real: no se especificaba liberar el rol. Se documentó liberación inmediata y `SET INVALID` si se cierra el sensor. |
| Comandos antes del registro o del rol incorrecto | medium / patch | Era real: la secuencia no restringía los emisores. Se añadió máquina de estados y `ERROR INVALID_COMMAND` sin acción. |
| Fallo de red sin ruta segura determinable | medium / patch | Era real para el cierre del sensor. La limpieza de conexión ahora invalida y publica el estado seguro. |
| Mojibake en los documentos | false | Los archivos fuente se leen como UTF-8 correctamente; la corrupción provenía del diff temporal generado con una conversión de consola. |
| Distancia finita fuera de rango ambigua | medium / patch | Era real: `ERROR INVALID_DISTANCE` podía confundirse con una medición fuera de rango. Se reservó el error para valor no finito o sintácticamente inválido. |
| Estado de registro incompleto | medium / patch | Era real: faltaban reglas para repetición, rol incorrecto y duplicado. Se documentaron como `ERROR INVALID_COMMAND`. |
| Línea TCP sin límite | medium / patch | Era real: no había límite de memoria/framing. Se fijó máximo de 128 bytes y descarte hasta salto de línea, preservando conexión. |
| `ACK` ambiguo con publicación periódica | low / patch | Era real: no se definía correlación ni fallo. Se declaró confirmación idempotente para diagnóstico, sin retransmisión ni transición. |
| Límites externos no explícitos en M-01 | low / patch | Era real: faltaban ambas fronteras externas. Se agregaron 1.99 y 100.01 cm y el resultado esperado. |
| Temporización LED no reproducible | medium / patch | Era real: no había método ni tolerancia. Se añadió video/log con semiperíodos y tolerancia de ±50 ms. |

## Design Notes

El servidor es la única autoridad de clasificación: el sensor informa medición o invalidez física; el actuador traduce únicamente un `SET` válido a salida LED. Sus temporizadores locales protegen el estado seguro y no desplazan la lógica de rangos al borde.

## Verification

**Manual checks (if no CLI):**
- Revisar que los enlaces desde `informe.md` a diagramas y pruebas sean relativos y existentes.
- Revisar que ningún archivo nuevo reporte un PASS, medición, fotografía, log o implementación inexistente.
- Revisar que no cambien archivos dentro de `ESP32 sensor/`, `ESP32 actuador/` ni `Server TCP/`.
