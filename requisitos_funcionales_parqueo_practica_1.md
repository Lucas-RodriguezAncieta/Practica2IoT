# Requisitos funcionales — Asistente de estacionamiento (Práctica 1)

## 1. Objetivo del sistema

El proyecto consistía en un **asistente de estacionamiento** basado en un ESP32 y un sensor ultrasónico HC-SR04. El sistema medía continuamente la distancia entre el sensor y un objeto, clasificaba esa distancia y comunicaba la proximidad mediante tres LEDs (rojo, amarillo y verde).

> **Alcance:** Este documento describe el funcionamiento y los requisitos funcionales de la Práctica 1, como base para la Práctica 2. **No incluye todavía requisitos de comunicación TCP**, ya que no formaban parte del sistema anterior.

## 2. Requisitos funcionales

| ID | Requisito | Descripción y criterio de aceptación |
|---|---|---|
| **RF-01** | **Medición de distancia** | Medir la distancia desde el sensor HC-SR04 hasta el objeto y expresarla en centímetros. El rango de medición considerado válido por el proyecto era de **2 a 100 cm**, inclusive. |
| **RF-02** | **Clasificación de proximidad** | Clasificar cada distancia válida en exactamente uno de tres estados: **DANGER** si `2 ≤ d < 20 cm`; **WARNING** si `20 ≤ d < 50 cm`; **SAFE** si `50 ≤ d ≤ 100 cm`. La clasificación utiliza la lectura sin redondearla previamente. |
| **RF-03** | **Señalización mediante LEDs** | Indicar el estado de proximidad con un único LED correspondiente: **DANGER:** rojo intermitente (`150 ms` encendido / `150 ms` apagado); **WARNING:** amarillo intermitente (`500 ms` encendido / `500 ms` apagado); **SAFE:** verde permanentemente encendido. Los otros LEDs deben permanecer apagados. |
| **RF-04 (derivado)** | **Operación continua** | Continuar realizando mediciones y actualizando el indicador mientras el sistema está encendido, incluso durante el parpadeo de los LEDs o después de un timeout del sensor. |
| **RF-05 (derivado)** | **Manejo de mediciones inválidas** | Cuando no se recibe eco, la lectura no es finita o la distancia está fuera del intervalo `[2, 100] cm`, entrar al estado **INVALID**, apagar todos los LEDs y mantenerlos apagados hasta que se obtenga una lectura válida. En el arranque, los LEDs también permanecen apagados. |

**Nota sobre la numeración:** RF-01, RF-02 y RF-03 provienen de los requisitos funcionales de la consigna anterior; RF-04 y RF-05 fueron requisitos derivados del diseño documentado por el grupo.

## 3. Estados y comportamiento esperado

| Estado | Distancia medida | Indicador visual |
|---|---|---|
| `DANGER` | `2 ≤ d < 20 cm` | LED rojo parpadea: 150 ms ON / 150 ms OFF |
| `WARNING` | `20 ≤ d < 50 cm` | LED amarillo parpadea: 500 ms ON / 500 ms OFF |
| `SAFE` | `50 ≤ d ≤ 100 cm` | LED verde encendido continuamente |
| `INVALID` | `d < 2 cm`, `d > 100 cm`, ausencia de eco o lectura inválida | Todos los LEDs apagados |

Los límites son **contiguos y no se solapan**: una lectura de `20.00 cm` corresponde a `WARNING`, una de `50.00 cm` a `SAFE`, y las de `2.00 cm` y `100.00 cm` son válidas. Se clasifica la **medición real**, no la distancia de referencia a la que se colocó físicamente el objeto.

## 4. Flujo de funcionamiento

1. Al encender el ESP32, se inicializan el sensor y los tres LEDs. El indicador comienza apagado (`INVALID`).
2. El ESP32 activa el HC-SR04 para solicitar una medición de distancia.
3. Se evalúa si la medición es válida dentro del rango de 2 a 100 cm.
4. Si es válida, se determina el estado `DANGER`, `WARNING` o `SAFE` según los umbrales.
5. Se actualiza el LED correspondiente. En los estados de parpadeo, la temporización sigue funcionando mientras se toman nuevas lecturas.
6. Si la medición es inválida, todos los LEDs se apagan. Cuando vuelve a recibirse una medición válida, el sistema recupera la señalización correspondiente.
7. El proceso se repite continuamente mientras el dispositivo está funcionando.

## 5. Reglas importantes del comportamiento

- **Exclusividad:** nunca deben estar encendidos simultáneamente LEDs de distintos estados.
- **Sin redondeo previo:** por ejemplo, `19.99 cm` debe clasificarse como `DANGER`, no como `WARNING`.
- **Actualización de estado:** al cambiar de zona se desactiva el LED anterior y se activa el indicador del nuevo estado.
- **Parpadeo continuo:** si el estado sigue siendo `DANGER` o `WARNING`, las nuevas mediciones no deben reiniciar continuamente la fase del parpadeo.
- **Recuperación:** después de un `TIMEOUT` o una medición fuera de rango, el sistema debe volver al estado normal en cuanto reciba una lectura válida.
- **Arranque seguro:** ningún LED queda encendido antes de contar con una medición válida.

## 6. Componentes utilizados en la Práctica 1

- Microcontrolador **ESP32 DOIT DevKit V1**.
- Sensor ultrasónico **HC-SR04**.
- Tres LEDs: **rojo, amarillo y verde**.
- Resistencias limitadoras de corriente, protoboard, cables jumper y conexión USB.

El sistema anterior ejecutaba localmente la medición, clasificación y señalización. La comunicación TCP será una **nueva capacidad que deberá definirse en la consigna de la Práctica 2**, sin atribuirla a los requisitos originales.

---

**Fuente:** `report.md`, secciones 1, 2.2 y 3.1 de la Práctica 1. La revisión docente (`Revision_P1_Grupo_3.md`) confirmó esta definición funcional, aunque señaló deficiencias en la evidencia de validación física de los LEDs.
