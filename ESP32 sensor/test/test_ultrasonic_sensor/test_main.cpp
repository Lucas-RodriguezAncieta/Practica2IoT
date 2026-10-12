#include <Arduino.h>
#include <unity.h>

#include "UltrasonicSensor.h"
#include "SensorProtocol.h"

namespace {
UltrasonicSensor sensor(18, 19, 2.0F, 100.0F);

void test_limits_are_inclusive() {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::VALID),
                        static_cast<int>(sensor.fromDurationUs(116).status));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::VALID),
                        static_cast<int>(sensor.fromDurationUs(1160).status));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::VALID),
                        static_cast<int>(sensor.fromDurationUs(2900).status));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::VALID),
                        static_cast<int>(sensor.fromDurationUs(5800).status));
}

void test_conversion_and_invalid_results() {
  const Measurement converted = sensor.fromDurationUs(1160);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::VALID),
                        static_cast<int>(converted.status));
  TEST_ASSERT_FLOAT_WITHIN(0.0001F, 20.0F, converted.distanceCm);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::TIMEOUT),
                        static_cast<int>(sensor.fromDurationUs(0).status));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::OUT_OF_RANGE),
                        static_cast<int>(sensor.fromDurationUs(115).status));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(MeasurementStatus::OUT_OF_RANGE),
                        static_cast<int>(sensor.fromDurationUs(5801).status));
}

void test_protocol_measurement_lines() {
  char line[32];
  const Measurement valid{MeasurementStatus::VALID, 19.999F};
  TEST_ASSERT_TRUE(buildMeasurementLine(valid, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("DISTANCE 19.99", line);

  const Measurement invalid{MeasurementStatus::TIMEOUT, 0.0F};
  TEST_ASSERT_TRUE(buildMeasurementLine(invalid, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("INVALID", line);
}
}  // namespace

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_limits_are_inclusive);
  RUN_TEST(test_conversion_and_invalid_results);
  RUN_TEST(test_protocol_measurement_lines);
  UNITY_END();
}

void loop() {}
