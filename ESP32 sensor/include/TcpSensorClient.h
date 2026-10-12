#pragma once

#include <Arduino.h>
#include <WiFiClient.h>

#include "UltrasonicSensor.h"

/** Cliente TCP del sensor: registra el rol y transmite mediciones sin clasificar. */
class TcpSensorClient {
 public:
  TcpSensorClient(const char* serverIp, uint16_t serverPort,
                  unsigned long reconnectIntervalMs,
                  unsigned long connectTimeoutMs);

  void update(unsigned long nowMs, bool wifiConnected);
  bool isRegistered();
  void sendMeasurement(const Measurement& measurement);

 private:
  static constexpr size_t kMaximumLineBytes = 128;

  void processIncoming();
  void processLine(const char* line);
  bool sendLine(const char* line);
  void disconnect();

  const char* serverIp_;
  uint16_t serverPort_;
  unsigned long reconnectIntervalMs_;
  unsigned long connectTimeoutMs_;
  unsigned long lastAttemptAtMs_ = 0;
  unsigned long registrationSentAtMs_ = 0;
  bool registered_ = false;
  bool awaitingRegistration_ = false;
  bool discardingOversizeLine_ = false;
  char lineBuffer_[kMaximumLineBytes + 1]{};
  size_t lineLength_ = 0;
  WiFiClient client_;
};
