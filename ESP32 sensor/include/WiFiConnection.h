#pragma once

#include <Arduino.h>

/** Mantiene la conexion WiFi en modo estacion y la reintenta sin bloquear. */
class WiFiConnection {
 public:
  WiFiConnection(const char* ssid, const char* password,
                 unsigned long reconnectIntervalMs);

  void begin();
  void update(unsigned long nowMs);
  bool isConnected() const;
  String localIp() const;

 private:
  const char* ssid_;
  const char* password_;
  unsigned long reconnectIntervalMs_;
  unsigned long lastAttemptAtMs_ = 0;
  bool wasConnected_ = false;
};
