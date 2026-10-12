#include "UltrasonicSensor.h"

namespace {
constexpr unsigned long kEchoTimeoutUs = 30000UL;
constexpr float kMicrosecondsPerCentimeter = 58.0F;
}

UltrasonicSensor::UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin,
                                   float minimumCm, float maximumCm)
    : triggerPin_(triggerPin),
      echoPin_(echoPin),
      minimumCm_(minimumCm),
      maximumCm_(maximumCm) {}

void UltrasonicSensor::begin() {
  pinMode(triggerPin_, OUTPUT);
  pinMode(echoPin_, INPUT);
  digitalWrite(triggerPin_, LOW);
}

Measurement UltrasonicSensor::measure() {
  digitalWrite(triggerPin_, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin_, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin_, LOW);

  return fromDurationUs(pulseIn(echoPin_, HIGH, kEchoTimeoutUs));
}

Measurement UltrasonicSensor::fromDurationUs(unsigned long durationUs) const {
  if (durationUs == 0) {
    return {MeasurementStatus::TIMEOUT, 0.0F};
  }

  const float distanceCm = static_cast<float>(durationUs) / kMicrosecondsPerCentimeter;
  if (distanceCm < minimumCm_ || distanceCm > maximumCm_) {
    return {MeasurementStatus::OUT_OF_RANGE, distanceCm};
  }

  return {MeasurementStatus::VALID, distanceCm};
}
