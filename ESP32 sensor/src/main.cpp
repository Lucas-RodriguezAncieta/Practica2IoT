#include <Arduino.h>

#include "TcpSensorClient.h"
#include "UltrasonicSensor.h"
#include "WiFiConnection.h"

#if __has_include("WiFiSecrets.h")
#include "WiFiSecrets.h"
#else
#include "WiFiSecrets.example.h"
#endif

#ifndef PIO_UNIT_TESTING
namespace {
constexpr uint8_t kTriggerPin = 18;
constexpr uint8_t kEchoPin = 19;
constexpr float kMinimumDistanceCm = 2.0F;
constexpr float kMaximumDistanceCm = 100.0F;
constexpr unsigned long kSampleIntervalMs = 200UL;
constexpr unsigned long kReconnectIntervalMs = 2000UL;
constexpr unsigned long kTcpConnectTimeoutMs = 100UL;

UltrasonicSensor sensor(kTriggerPin, kEchoPin, kMinimumDistanceCm,
                        kMaximumDistanceCm);
WiFiConnection wifiConnection(kWifiSsid, kWifiPassword, kReconnectIntervalMs);
TcpSensorClient tcpClient(kServerIp, kServerPort, kReconnectIntervalMs,
                          kTcpConnectTimeoutMs);
unsigned long lastSampleAtMs = 0;

void printMeasurement(const Measurement& measurement) {
  switch (measurement.status) {
    case MeasurementStatus::VALID:
      Serial.print("[SENSOR] Distancia: ");
      Serial.print(measurement.distanceCm, 4);
      Serial.println(" cm");
      break;
    case MeasurementStatus::TIMEOUT:
      Serial.println("[SENSOR] INVALID: TIMEOUT");
      break;
    case MeasurementStatus::OUT_OF_RANGE:
      Serial.println("[SENSOR] INVALID: OUT_OF_RANGE");
      break;
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println("[SENSOR] Iniciando...");
  sensor.begin();
  Serial.println("[SENSOR] HC-SR04 inicializado");
  if (kUsingExampleWiFiSecrets) {
    Serial.println("[SENSOR] Configure include/WiFiSecrets.h antes de conectar");
  }
  if (!kUsingExampleWiFiSecrets) {
    wifiConnection.begin();
  }
}

void loop() {
  const unsigned long now = millis();
  if (now - lastSampleAtMs >= kSampleIntervalMs) {
    lastSampleAtMs = now;
    const Measurement measurement = sensor.measure();
    printMeasurement(measurement);
    if (!kUsingExampleWiFiSecrets) {
      tcpClient.sendMeasurement(measurement);
    }
  }
  if (!kUsingExampleWiFiSecrets) {
    wifiConnection.update(now);
    tcpClient.update(now, wifiConnection.isConnected());
  }
}
#endif
