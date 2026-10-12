# Revisión del Informe Técnico — Práctica 1

**Asignatura:** Internet de las Cosas [SIS-234] — 2-2026
**Actividad:** Integración de sensores y actuadores en un objeto inteligente
**Repositorio:** https://github.com/Lucas-RodriguezAncieta/Parking-Assistant
**Informe:** `docs/report.md`
**Autores de commits:** Lucas-RodriguezAncieta (5 de 5 commits). Josue-Camacho figura como colaborador del repositorio, pero no tiene commits.
**Fecha límite:** 15-Sep-2026, 17:45 (UTC-4)
**Componente evaluado:** Informe Técnico (1/3 de la nota). La Demo y la Defensa Individual se califican por separado.

---

## 1. Resumen de la calificación

| Criterio | Puntaje |
|---|:---:|
| Requerimientos, análisis y diseño | **3** / 4 |
| Desarrollo e implementación (código y documentación) | **3** / 4 |
| Pruebas y validaciones | **2** / 4 |
| Resultados, conclusiones y recomendaciones | **3** / 4 |
| **Total** | **11 / 16** |
| **Nota del componente** = (11 / 16) × 100 | **68,75** |

---

## 2. Estado del repositorio en la fecha límite

Historial de *push* a `main` (registro público de eventos de GitHub, convertido a UTC-4):

| Commit | Push (UTC-4) | Contenido |
|---|---|---|
| `ca6d250` | 15-Sep 01:40 | Commit inicial |
| `14dffb7` | 15-Sep 11:33 | Documentación y diagrama de circuito |
| `b0be2b9` | 15-Sep 15:59 | Documentación completa y evidencias |
| `aaa40f0` | 15-Sep 16:44 | Corrección de `report.md` |
| `7df72e0` | **15-Sep 16:56** | Actualización de `report.md` |

- **Entrega dentro de plazo.** Se evalúa `7df72e0`, que es el estado actual de `main`.
- Los anexos (CSV de mediciones, fotografías, registro de compilación y diagramas) están **dentro del repositorio**, como pide §6 de la consigna.

---

## 3. Observaciones por criterio

### 3.1 Requerimientos, análisis y diseño — **3 / 4 (Satisfactorio)**

**Fortalezas**
- **Tabla de requerimientos clara y medible.** Los requerimientos de la consigna (RF-01 a RF-03) se separan de los derivados del diseño (RF-04 y RF-05). Cada RNF tiene un criterio de aceptación concreto.
- **Verificación formal de los intervalos (§2.2):**
  - DANGER `2 ≤ d < 20`, WARNING `20 ≤ d < 50`, SAFE `50 ≤ d ≤ 100`.
  - La unión es [2,100] y no hay intersecciones.
  - Se aclara a qué zona pertenece cada frontera (20, 50, 2 y 100) y la regla de "no redondear antes de clasificar".
- **Estado `INVALID` con todos los LEDs apagados** cuando no hay eco o la lectura está fuera de rango. Así se evita mostrar "zona segura" cuando el sensor falla.
- Las tablas de **ambigüedades (A-01 a A-05) y riesgos técnicos** muestran que el diseño se pensó de antemano.
- Los diagramas de **flujo de control** y de **máquina de estados** (en `docs/diagrams/design.md`) son claros. El de estados incluye el nodo de decisión y las transiciones desde cualquier estado.

**Por qué no llega a 4**
- **Al diagrama de circuito le falta el divisor de tensión.** `circuit.png` solo muestra una línea directa ECHO → GPIO19 con la etiqueta "Nodo: 3.33 V nominal", sin resistencias. El código dice `// ECHO reaches this input through the specified 1 kOhm / 2 kOhm divider` (`UltrasonicSensor.cpp:10`), y las tablas de riesgos y recomendaciones hablan del "divisor". Sin embargo, **ni el esquema, ni la tabla de conexiones (§2.5), ni la lista de hardware (§2.1, que solo menciona tres resistencias de 220 Ω)** incluyen las resistencias de 1 kΩ y 2 kΩ. Además, la fila de la tabla de conexiones "HC-SR04 ECHO → nodo conectado a GPIO19 → GND" no se puede interpretar.
- **El diagrama de clases no coincide con el código:**
  - Muestra los atributos `triggerPin`/`echoPin` y `greenPin`/`yellowPin`/`redPin`, que **no existen**: los pines se leen de `ParkingConfig`.
  - Muestra `readDistanceCm(distanceCm)`, cuando la firma real tiene un segundo parámetro `MeasurementStatus*`.
  - Omite el atributo `sampleNumber` y el enum `MeasurementStatus`.
- **La documentación de diseño está desactualizada.** `design.md` empieza con "Estos diagramas describen el diseño, **no un montaje ni software ya verificado**" y termina con "Las firmas son **propuestas, no código existente**". El `README.md` principal todavía dice "firmware y validación experimental **pendientes**; `src/main.cpp` sigue siendo la plantilla original".
- **Diagramas fuera del informe:** las secciones §2.4, §2.6 y §2.7 del informe solo tienen enlaces; los diagramas no se ven en `report.md`. El diagrama de arquitectura es mínimo (Objeto → HC-SR04 → ESP32 → algoritmo → LED). No muestra alimentación, GND común ni la conexión serie con la PC.

### 3.2 Desarrollo e implementación — **3 / 4 (Satisfactorio)**

**Fortalezas**
- **Código limpio y ordenado:**
  - Tres clases con responsabilidades separadas: `UltrasonicSensor` mide, `LedIndicator` controla los LEDs y `ParkingController` coordina.
  - `main.cpp` solo llama a `begin()` y `update()` del controlador.
- **Sin números mágicos:** todos los pines, umbrales, tiempos y constantes físicas están en `ParkingConfig.h` como `constexpr`, dentro de un *namespace*.
- **Control de tiempos no bloqueante:**
  - El muestreo se programa por instante de inicio (200 ms) y el parpadeo usa `millis()`.
  - Las restas sin signo funcionan aunque `millis()` se desborde.
  - `LedIndicator::update` recupera la fase si un `pulseIn` retrasó un cambio.
  - `writeOutputs()` apaga todos los LEDs antes de encender el que corresponde.
- Las lecturas inválidas tienen un motivo (`TIMEOUT` / `OUT_OF_RANGE`), y el registro serie tiene un formato fácil de procesar (`n=… t_ms=… cm=… state=…`).
- Hay evidencia de compilación (`S-05-platformio-build.txt`) y se nombran las convenciones de código (§3.2).

**Por qué no llega a 4**
- **Las clases no son reutilizables.** `UltrasonicSensor` y `LedIndicator` leen los pines directamente de `ParkingConfig`, en lugar de recibirlos en el constructor. No se podrían crear dos sensores ni cambiar los pines sin modificar la clase. En POO se espera que cada objeto guarde su propia configuración, y es justo lo que mostraba el diagrama de clases.
- **La validación del rango está duplicada:** se revisa en `UltrasonicSensor::readDistanceCm` y otra vez en `ParkingController::classifyDistance`. El informe lo presenta como "responsabilidades complementarias", pero si cambia el rango hay que modificarlo en dos lugares.
- **Documentación de código escasa:** los encabezados tienen solo 2 comentarios de contrato (en `LedIndicator::setState` y `UltrasonicSensor::readDistanceCm`). Las clases y la mayoría de los métodos públicos no tienen descripción.
- **§3.2 "Código Fuente Documentado" está escrita como lista de instrucciones**, no como descripción de lo que se hizo: "Evitar globales innecesarias…", "Documentar contratos…", "Registrar arranque una vez…". No incluye fragmentos de código.
- `test/` está vacío: no hay pruebas unitarias de `classifyDistance` ni de `LedIndicator`, aunque las dos son funciones puras que se podían probar fácilmente. El informe lo reconoce.

### 3.3 Pruebas y validaciones — **2 / 4 (Satisfactorio con recomendaciones)**

**Fortalezas**
- **Plan de pruebas estructurado:** 13 casos funcionales (F-01 a F-13) y 4 no funcionales (N-01 a N-04). Cada uno tiene requisito asociado, precondiciones, entrada, procedimiento, resultado esperado, resultado real y estado. Los casos de borde están bien elegidos (19, 20, 49, 50, 100, < 2 y ~110 cm, y sin eco).
- **Hay datos numéricos reales:**
  - Rangos de lectura por caso.
  - `measurement-attempts.csv` con 60 lecturas individuales (10 por cada distancia).
  - `measurements.csv` con promedios de 56 a 67 muestras por distancia.
- **Las limitaciones se informan con honestidad:** se dice qué no se validó, se distingue el error promedio del error individual y se separan las fallas del sensor de las del software.

**Por qué baja a 2**
- **No se validó el comportamiento de los LEDs (RF-03).** El informe dice que "el PASS acredita comportamiento registrado en Serial, **no observación luminosa**" y que las duraciones de 150/150 y 500/500 ms "son configuradas, no medidas". El requerimiento central de la práctica (activar los actuadores según el rango) se marca como cumplido solo con lo que imprime el puerto serie, sin observar ni registrar los LEDs.
- **RNF-01 (estabilidad ≥ 10 min) no se cumplió:** la prueba duró **5 minutos**, la mitad de lo declarado, y no se contaron las lecturas inválidas (`invalid_readings` vacío).
- **RNF-03 (respuesta ≤ 1 s): `PENDING EXECUTION`.** No se hizo ninguna medición. Bastaba con usar las marcas `t_ms` del propio registro serie para calcular el tiempo entre la primera lectura de la nueva zona y el cambio de estado.
- **No hay registros serie crudos en el repositorio.** La evidencia de F-01 a F-12 se describe como "*interpretación Serial validada por el usuario; no se dispone de un archivo Serial enlazado*". Como F-10 y F-12 dependen de una "confirmación del usuario", no se pueden comprobar.
- **Los datos de exactitud no son coherentes entre sí:**

  | Ref. | Rango de lecturas informado en F-xx | Promedio en `measurements.csv` | 10 lecturas en `measurement-attempts.csv` |
  |---:|---|---:|---|
  | 10 cm | 10.17–10.52 (F-01) | **10.90** | 10.17–10.19 |
  | 20 cm | 20.22–21.21 (F-03) | **21.39** | 20.55–20.57 |
  | 50 cm | 50.05–50.41 (F-06) | **50.63** | 50.07–50.40 |
  | 70 cm | 69.47–70.91 (F-07) | **71.45** | 69.91–70.50 |

  En 4 de las 6 distancias, el promedio del resumen es **mayor que la lectura más alta** que se informa para esa misma distancia. Las 10 lecturas "representativas" también quedan lejos del promedio (por ejemplo, 10.17 frente a 10.90). El informe solo aclara que "sus promedios no tienen que coincidir", sin explicar de qué sesiones vienen. Por eso **no se puede rastrear de dónde salen los 1.45 cm de error máximo** que se informan.
- **Exactitud solo hasta 90 cm**, aunque el rango declarado llega a 100 cm.

### 3.4 Resultados, conclusiones y recomendaciones — **3 / 4 (Satisfactorio)**

**Fortalezas**
- **Resultados medibles:** tabla de exactitud con 6 distancias (error de los promedios ≤ 1.45 cm), muestreo a 5 Hz (200 ms entre inicios) y 5 min sin reinicios. También hay una tabla de estado global que separa lo que pasó, lo parcial y lo pendiente.
- **Análisis crítico presente:**
  - El error del promedio no es el error máximo individual (A-02).
  - Que la lectura física cruce un umbral es un efecto del sensor, no un error de clasificación (A-03).
  - Las lecturas inestables aparecen al mover el objetivo.
- **Las conclusiones coinciden con la evidencia:** no se presenta como validado nada que no lo esté.
- Los anexos en el repositorio (CSV, fotografías del montaje con cinta métrica y registro de compilación) son pertinentes.

**Por qué no llega a 4**
- **Las recomendaciones son muy pobres:** un solo párrafo con indicaciones de procedimiento ("Comprobar montaje y divisor… usar superficie de referencia reproducible… Limitar ajustes al prototipo académico"). No hay **recomendaciones técnicas** de mejora, por ejemplo:
  - histéresis en los umbrales 20 y 50 cm (el propio informe muestra que el estado alterna en el límite de 50 cm);
  - filtrado (mediana o media móvil);
  - compensación por temperatura;
  - captura de ECHO por interrupción en lugar de `pulseIn`;
  - calibración del factor 58 µs/cm, ya que hay un sesgo positivo sistemático en los promedios.
- **Conclusiones breves:** un párrafo que repite el estado de las validaciones, sin reflexionar sobre el diseño, lo aprendido o las decisiones tomadas.
- Las inconsistencias de datos descritas en §3.3 debilitan la cifra principal de exactitud.

---

## 4. Recomendaciones para próximas entregas

1. **Dibujar el divisor de tensión** en el esquema, con sus valores (1 kΩ / 2 kΩ), y agregarlo a la lista de hardware y a la tabla de conexiones.
2. **Verificar los actuadores, no solo el puerto serie.** Grabar un video corto por zona, o contar parpadeos en 10 s, para validar RF-03. El tiempo de respuesta se puede calcular con las marcas `t_ms` del registro.
3. **Guardar los registros serie crudos** (`.txt` o `.csv`) de cada ensayo en `docs/evidence/` y calcular los resúmenes a partir de ellos, para que cada número del informe se pueda rastrear.
4. Completar la prueba de estabilidad con la duración declarada (≥ 10 min) y contar las lecturas inválidas.
5. Pasar los pines por el constructor de las clases, centralizar la validación de rango en un solo lugar y documentar las clases y métodos públicos en los `.h`.
6. Actualizar o eliminar la documentación que ya no corresponde (`README.md` y los encabezados de `design.md`) y mostrar los diagramas dentro del informe.
7. Incluir la **lista de integrantes** en el informe, que hoy no aparece.

---

## 5. Nota para la Defensa Individual

- El informe **no identifica a los integrantes**, y todos los commits son de un solo autor (Lucas-RodriguezAncieta). Conviene confirmar en la defensa quiénes forman el grupo y cuál fue el aporte de cada uno.
- La documentación tiene redacción propia de un asistente o agente (BMad): "*validada por el usuario*", "*Esta revisión no verifica publicación ni sincronización con GitHub*", "*A-01 resuelta por el usuario*". Conviene comprobar que cada integrante pueda explicar:
  - el cálculo del divisor;
  - la fórmula `t / 58`;
  - por qué el parpadeo no reinicia su fase en cada muestra;
  - cómo funciona la recuperación de la fase en `LedIndicator::update`;
  - de dónde salen los promedios de `measurements.csv`.
