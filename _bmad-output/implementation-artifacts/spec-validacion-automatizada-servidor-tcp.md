---
title: 'Validación automatizada del servidor TCP'
type: 'feature'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** El servidor TCP inicial no cuenta aún con un entorno aislado de pytest ni con evidencia automatizada de sus comportamientos de protocolo y seguridad.

**Approach:** Crear un entorno virtual local ignorado por Git, ampliar la suite en `tests/automated/`, ejecutar pruebas sin hardware y corregir exclusivamente defectos del servidor revelados por ellas; registrar resultados reales en el informe.

</frozen-after-approval>

## Implementation Notes

- Alcance único: validación automatizada local del servidor Python; no se modificará firmware, hardware ni se harán commits.
- La comprobación de Git no está disponible en este entorno (`git` no puede resolver el directorio de trabajo), por lo que no se puede establecer limpieza del árbol.
- Se creó `Server TCP/.venv` con Python 3.13 y se instaló pytest únicamente allí.
- Se añadieron clientes TCP locales simulados y se corrigieron la serialización de `sendall`, el cierre del listener y el descarte acotado de líneas excesivas.
- La revisión independiente amplió la cobertura de desconexión del actuador, transición intermedia de líneas concatenadas, publicación de expiración, recuperación tras UTF-8 inválido y sobrelongitud, y trazabilidad de `SET`/`ACK`.
- Verificación final ejecutada: `Server TCP/.venv/Scripts/python.exe -m pytest tests/automated -q` produjo 24 aprobadas y 0 fallidas.

## Review Triage Log

- medio, corregido: se agregó la desconexión y el re-registro del ACTUATOR para verificar que el rol se libera.
- medio, corregido: la prueba de fragmentación/concatenación ahora verifica WARNING antes de INVALID.
- medio, corregido: la expiración se verifica con ACTUATOR y recepción de `SET INVALID`.
- medio, corregido: se cubren UTF-8 inválido y línea mayor a `MAX_LINE_BYTES`, seguidos de una distancia válida.
- medio, corregido: `TcpServer` conserva último `SET` y último `ACK`, y registra advertencia cuando no coinciden.
- bajo, corregido: el informe ya no declara `TcpServer` como propuesto.
- medio, corregido: la documentación marca el timeout local del actuador como pendiente, no cubierto.
