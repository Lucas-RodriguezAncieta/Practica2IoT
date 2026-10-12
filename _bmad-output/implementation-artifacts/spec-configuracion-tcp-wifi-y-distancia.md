---
title: 'Configuración TCP, WiFi y formato de distancia'
type: 'chore'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** La documentación inicial no fija el puerto, la interfaz de escucha, la configuración local segura de WiFi/servidor ni la gramática final de `DISTANCE`.

**Approach:** Actualizar el informe, diagramas y planes de prueba para documentar TCP en el puerto 5000, IP local configurable, secretos ignorados por Git, formato decimal de distancia y los tres errores TCP, sin implementar componentes ejecutables.

</frozen-after-approval>

## Implementation Notes

- Actualizados `informe.md`, `diagramas/arquitectura.md` y los planes de prueba con `0.0.0.0:5000`, IPv4 local configurable, formato decimal de `DISTANCE` y comportamiento por rol incorrecto.
- Añadida la exclusión `include/WiFiSecrets.h` a los dos `.gitignore`; no se creó ningún archivo con credenciales.
- No se modificaron firmware, `platformio.ini` ni la carpeta `Server TCP`.

## Review Triage Log

- `medium` — Un `ACK` podía parecer inválido después de una publicación periódica; se convirtió en confirmación informativa sin comparación con el estado actual.
- `low` — Faltaba declarar el tratamiento de `\r\n`; ahora se acepta eliminando el retorno de carro final.
- `medium` — No se declaraba el estado inicial del servidor; se fijó `INVALID` y su envío al ACTUATOR que se registre antes de una lectura.
- `low` — M-04 no cubría el límite de 128 bytes; se añadió descarte hasta salto y recuperación con el mensaje siguiente.
- `medium` — M-02 no verificaba que las muestras repetidas conservaran la fase de parpadeo; se añadió el procedimiento.
- `low` — M-08 no fijaba duración ni cálculo; se definieron 60 s, conteo separado de válidas/inválidas y umbral de 5 intentos/s.
