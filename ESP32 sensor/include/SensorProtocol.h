#pragma once

#include <Arduino.h>

#include "UltrasonicSensor.h"

/** Genera una linea de medicion compatible con el protocolo TCP del servidor. */
bool buildMeasurementLine(const Measurement& measurement, char* buffer,
                          size_t bufferSize);
