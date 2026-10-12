---
title: 'Preparación documentada de WiFi para ambos ESP32'
type: 'chore'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** La documentación no especifica por completo cómo preparar los dos archivos locales `WiFiSecrets.h`, incluida la activación explícita de la red, y describe erróneamente el marcador de IPv4 del actuador.

**Approach:** Actualizar el informe y los procedimientos de prueba para ambos ESP32, y añadir una comprobación estática sin credenciales ni hardware para las ramas de configuración de ejemplo y real.

</frozen-after-approval>

## Implementation Notes

- Se actualizaron `informe.md` y los procedimientos M-03, M-10 y M-11 para exigir la misma IPv4 local, puerto 5000 y la bandera en `false` en ambos ESP32 durante la integración física.
- Se añadió una comprobación Python de solo lectura para el fallback, marcadores, puerto y guardas de red; no crea ni inspecciona credenciales reales, ni modifica firmware, protocolo o servidor.
- La revisión ciega pidió reforzar la comprobación estática. Se validan también la exclusión de los archivos locales en Git y las guardas concretas de inicio, actualización y TCP; la documentación la denomina verificación estructural y conserva la comparación real de IP/puerto como paso manual previo a la prueba física.

## Review Triage Log

- Corregido: la exclusión de `WiFiSecrets.h` en los `.gitignore` de ambos ESP32 ahora se comprueba estáticamente.
- Corregido: las plantillas deben declarar exactamente los tres marcadores de texto previstos, evitando valores adicionales que parezcan credenciales.
- Corregido: el verificador comprueba las guardas concretas de inicio, actualización WiFi y actualización TCP.
- Corregido: la verificación exige los marcadores y constantes de plantilla requeridos.
- Corregido: la documentación describe la prueba como estructural; no afirma ejecutar las ramas del preprocesador.
- Corregido: `tests/automated/README.md` preserva la igualdad de IPv4/puerto como revisión manual previa a hardware.
- Corregido: M-10 pide registrar mensajes disponibles y log del servidor, sin convertir textos no garantizados en criterio independiente.
