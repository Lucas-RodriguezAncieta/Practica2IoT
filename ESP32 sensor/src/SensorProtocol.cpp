#include "SensorProtocol.h"

#include <cstdio>

bool buildMeasurementLine(const Measurement& measurement, char* buffer,
                          size_t bufferSize) {
  if (measurement.status != MeasurementStatus::VALID) {
    return snprintf(buffer, bufferSize, "INVALID") <
           static_cast<int>(bufferSize);
  }

  // El truncado a centesimas conserva el lado del limite de cada rango.
  const unsigned long hundredths =
      static_cast<unsigned long>(measurement.distanceCm * 100.0F);
  return snprintf(buffer, bufferSize, "DISTANCE %lu.%02lu", hundredths / 100,
                  hundredths % 100) < static_cast<int>(bufferSize);
}
