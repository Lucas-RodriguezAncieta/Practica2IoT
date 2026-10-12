---
title: 'Etapa 6: planificación de pruebas físicas y evidencias'
type: 'chore'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** El repositorio tiene casos manuales resumidos, pero requiere procedimientos reproducibles y evidencia trazable para validar todos los RF y RNF durante la integración física final.

**Approach:** Documentar preparación, ejecución, criterios PASS/FAIL y conservación de evidencias para pruebas de los tres componentes, sin modificar firmware, protocolo ni servidor y sin afirmar resultados no obtenidos.

</frozen-after-approval>

## Implementation Notes

- Se reorganizó el plan manual en diez casos, con materiales, preparación, pasos, resultados esperados, evidencia y criterio de aceptación.
- Se añadieron una ficha reutilizable y reglas de preservación de evidencia; no se modificó firmware, protocolo ni servidor, ni se registraron resultados físicos.
- La revisión independiente detectó trazabilidad de IDs obsoleta, ausencia de prueba RF7 y ambigüedad de frecuencia; se alineó la matriz del informe, se añadió M-11 y se fijó el umbral PASS del grupo en 5/s.

## Review Triage Log

- Corregido: la matriz de requerimientos de `informe.md` usa los IDs definitivos de Etapa 6.
- Corregido: M-11 verifica errores TCP, conexión viva y ausencia de acción para RF7.
- Corregido: M-06 define FAIL para toda frecuencia menor de 5/s; 2/s queda como referencia docente.
