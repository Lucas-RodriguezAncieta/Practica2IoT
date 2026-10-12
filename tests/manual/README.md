# Procedimientos físicos finales

Estado: plan de ejecución. Una sola persona puede ejecutar cada caso; anotar fecha, responsable, versiones, red usada, resultado observado y enlaces a archivos crudos. No marcar PASS sin evidencia.

## Preparación común

Materiales: PC con Python y PlatformIO, dos cables USB de datos, dos ESP32, HC-SR04, divisor ECHO de 1 kΩ/2 kΩ, tres LEDs con resistencias de 220 Ω, cinta métrica, superficie plana y cámara a 60 fps o más.

1. Sensor: VCC del HC-SR04 a 5 V, GND común, TRIG a GPIO18; ECHO → 1 kΩ → GPIO19 y GPIO19 → 2 kΩ → GND.
2. Actuador: GPIO25/26/27 a LED rojo/amarillo/verde mediante 220 Ω; cátodos a GND común.
3. En ambos proyectos, copiar `include/WiFiSecrets.example.h` como `WiFiSecrets.h`; configurar SSID, contraseña e IPv4 real de la PC; conservar puerto 5000; cambiar la bandera a `false`; comparar IP/puerto. No guardar secretos ni su valor en evidencias.
4. En cada proyecto PlatformIO: `pio run -e esp32doit-devkit-v1`, luego `pio run -e esp32doit-devkit-v1 -t upload`; abrir dos monitores seriales a 115200 y guardar la salida.
5. Desde `Server TCP/`, iniciar `python main.py` y guardar la consola como log del servidor. Confirmar escucha en `0.0.0.0:5000`.
6. Crear una ficha desde [plantilla-resultados.md](plantilla-resultados.md) por caso y guardar archivos según [../evidence/README.md](../evidence/README.md).

## Casos

### M-01 — Registro y flujo TCP

- Requisitos: RF4, RF6. Materiales: preparación común y los tres logs. Preparación: servidor iniciado; ambos ESP32 cargados y configurados.
- Pasos: 1) encender sensor y actuador; 2) esperar `WiFi conectado`, `TCP conectado` y `Registro aceptado`; 3) colocar objeto a 30 cm; 4) localizar `DISTANCE`, `SET WARNING` y `ACK WARNING` en los logs.
- Esperado: un registro por rol; servidor recibe distancia, publica estado y recibe ACK.
- Evidencia: logs crudos de sensor, actuador y servidor en `tcp/`.
- PASS/FAIL: PASS solo si los tres logs muestran la secuencia; FAIL ante ausencia de registro, comando o ACK.

### M-02 — Límites y clasificación

- Requisitos: RF1, RF2, RF3, RF6. Materiales: cinta, cámara y logs. Preparación: sistema registrado; medir desde la cara del sensor a superficie plana.
- Pasos: estabilizar el objeto en 2, 20, 50 y 100 cm; para cada posición grabar LED y capturar cinco lecturas; repetir cerca de 19.99/49.99 si la resolución física lo permite.
- Esperado: 2 cm DANGER/rojo, 20 cm WARNING/amarillo, 50 y 100 cm SAFE/verde; el servidor decide el estado.
- Evidencia: CSV de referencia/lectura/estado, vídeo por límite y logs.
- PASS/FAIL: PASS si estado publicado y LED coinciden con el rango; FAIL ante clasificación o LED distinto.

### M-03 — Exactitud HC-SR04

- Requisitos: RF1, RNF2. Materiales: cinta rígida, superficie plana y foto del montaje. Preparación: sensor fijo y perpendicular.
- Pasos: en 2, 20, 50, 75 y 100 cm registrar 10 lecturas consecutivas por referencia; calcular por fila `abs(lectura_cm-referencia_cm)` y el máximo.
- Esperado: error absoluto máximo ≤3 cm en todas las referencias del rango.
- Evidencia: CSV crudo y cálculo derivado en `accuracy/`, foto con cinta y divisor.
- PASS/FAIL: PASS solo con 10 muestras por referencia, trazabilidad y máximo ≤3 cm; FAIL en otro caso.

### M-04 — Estados y temporización LED

- Requisitos: RF3, RF5. Materiales: cámara ≥60 fps, cinta y logs. Preparación: encuadrar LEDs y referencia de tiempo fiable.
- Pasos: provocar DANGER, WARNING, SAFE e INVALID; grabar 10 s por estado; contar cuadros entre cambios de rojo/amarillo; comprobar verde fijo, exclusividad y apagado en INVALID.
- Esperado: rojo 150/150 ms ±50 ms, amarillo 500/500 ms ±50 ms, verde fijo; un `SET` repetido no reinicia fase.
- Evidencia: vídeos originales y tabla de cuadros/tiempo en `actuator/`, log del actuador.
- PASS/FAIL: PASS si los cuatro estados físicos y tolerancias se cumplen; FAIL ante LED simultáneo, tiempo fuera de rango o INVALID encendido.

### M-05 — Pérdida y recuperación WiFi/TCP

- Requisitos: RF4, RF5. Materiales: acceso al AP o posibilidad de apagar servidor, logs y cámara. Preparación: sistema registrado.
- Pasos: 1) detener servidor 10 s y reiniciarlo; 2) observar nuevo registro; 3) apagar/restaurar AP solo si no afecta a terceros; 4) verificar que sensor sigue midiendo y actuador pasa a INVALID tras 1.5 s sin `SET`.
- Esperado: desconexión detectada, reintento, nuevo `REGISTER`; sin bloqueo; LEDs apagados en estado seguro.
- Evidencia: logs de tres componentes y vídeo en `tcp/` y `actuator/`.
- PASS/FAIL: PASS si ambos roles recuperan conexión e INVALID es visible; FAIL ante reinicio, bloqueo o falta de recuperación.

### M-06 — Frecuencia de muestreo

- Requisitos: RNF4, RF1. Materiales: log del sensor y hoja de cálculo. Preparación: objeto fijo y TCP activo.
- Pasos: capturar 60 s de `Distancia`/`INVALID`; contar intentos válidos e inválidos; calcular `total/60`.
- Esperado: promedio ≥5 intentos/s; referencia mínima docente ≥2/s.
- Evidencia: log sin editar y cálculo reproducible en `sampling/`.
- PASS/FAIL: PASS con 60 s completos y ≥5/s; FAIL si es menor de 5/s o no hay log. El mínimo docente de 2/s se conserva solo como referencia, no como criterio PASS del grupo.

### M-07 — Estabilidad de 10 minutos

- Requisitos: RNF1, RF4, RF5. Materiales: alimentación estable, logs y cronómetro. Preparación: sistema registrado y objeto fijo a 30 cm.
- Pasos: registrar hora inicial/final; operar sin intervención durante 10 min completos; contar reinicios, desconexiones, bloqueos, válidas e inválidas; conservar todo incidente.
- Esperado: operación continua sin reinicio ni bloqueo durante ≥600 s.
- Evidencia: tres logs crudos y resumen derivado en `stability/`.
- PASS/FAIL: PASS con ≥600 s y cero reinicios/bloqueos; FAIL ante duración menor o incidente.

### M-08 — Latencia extremo a extremo

- Requisitos: RNF3, RF3, RF4. Materiales: cámara ≥60 fps, regla visible y logs. Preparación: encuadrar objeto, regla, LEDs y reloj de alta resolución en un solo vídeo.
- Pasos: repetir 10 transiciones bruscas entre 30 y 70 cm; identificar el primer cuadro con el objeto estable y el primer cuadro con cambio LED; calcular `(cuadros/fps)*1000`; correlacionar `DISTANCE`, `SET` y `ACK` del ensayo.
- Esperado: cada transición ≤1000 ms y cadena verificable sensor → servidor → actuador.
- Evidencia: vídeo original, tabla de cuadros/fps/latencia y tres logs en `latency/`.
- PASS/FAIL: PASS solo si las 10 latencias ≤1 s y hay correlación; FAIL si falta sincronización o una excede límite.

### M-09 — Seguridad eléctrica e inválidos

- Requisitos: RF1, RF3. Materiales: fotos, logs y cámara. Preparación: fotografiar divisor, GND común y resistencias antes de energizar.
- Pasos: ocultar/desconectar ECHO para TIMEOUT; colocar objeto fuera de 2–100 cm; restaurar montaje.
- Esperado: `INVALID: TIMEOUT` u `INVALID: OUT_OF_RANGE`, `SET INVALID` y LEDs apagados.
- Evidencia: fotos y logs en `sensor/`, `actuator/` y `tcp/`.
- PASS/FAIL: PASS si el estado seguro es extremo a extremo; FAIL si un LED de rango permanece activo.

### M-10 — Validación automática y compilación

- Requisitos: soporte RF1–RF4. Materiales: PC, Python, PlatformIO y placa solo para Unity. Preparación: no crear ni exponer secretos.
- Pasos: ejecutar `python tests/automated/check_wifi_secrets_config.py`; desde raíz ejecutar `Server TCP\\.venv\\Scripts\\python.exe -m pytest tests/automated -q`; en cada proyecto `pio run -e esp32doit-devkit-v1`; con placa, `pio test -e esp32doit-devkit-v1` donde corresponda.
- Esperado: cada comando termina código 0; Unity solo se marca ejecutado con salida de placa.
- Evidencia: salidas completas en `tests/evidence/build/` y ficha del caso.
- PASS/FAIL: PASS por subcomando con salida completa y código 0; sin placa, Unity es NO EJECUTADO, nunca PASS ficticio.

### M-11 — Mensajes TCP inválidos y conexión conservada

- Requisitos: RF4, RF7. Materiales: PC con un cliente TCP de prueba, log del servidor y actuador registrado. Preparación: iniciar servidor y dejar ACTUATOR registrado; abrir un segundo cliente TCP desde la PC para registrar SENSOR y enviar líneas de prueba.
- Pasos: enviar `DISTANCE 20,5`, `DISTANCE 20.123`, `DISTANCE NaN`, `DISTANCE` sin valor, una línea malformada, un comando de rol incorrecto y una línea de más de 128 bytes seguida de `\n`; tras cada error enviar un mensaje válido permitido para el rol.
- Esperado: el servidor responde con el `ERROR` aplicable, conserva la conexión, registra el evento y no publica una acción causada por el mensaje inválido; el mensaje válido posterior se procesa normalmente.
- Evidencia: transcripción completa del cliente y log del servidor en `tcp/`.
- PASS/FAIL: PASS solo si cada error esperado conserva la conexión y el siguiente mensaje válido funciona; FAIL ante cierre, acción indebida o error ausente.

## Cierre y trazabilidad

Ejecutar M-10, M-01, M-02–M-09 y M-11 en ese orden. Una sola persona puede ejecutar todos los casos; tres integrantes solo ayudan a repartir cámara, objeto y observación. No editar logs ni reemplazar originales por resúmenes.
