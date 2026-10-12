#include "WiFiConnection.h"

#include <WiFi.h>

WiFiConnection::WiFiConnection(const char* ssid, const char* password,
                               unsigned long reconnectIntervalMs)
    : ssid_(ssid),
      password_(password),
      reconnectIntervalMs_(reconnectIntervalMs) {}

void WiFiConnection::begin() {
  WiFi.mode(WIFI_STA);
  lastAttemptAtMs_ = millis() - reconnectIntervalMs_;
}

void WiFiConnection::update(unsigned long nowMs) {
  const bool connected = isConnected();
  if (connected) {
    if (!wasConnected_) {
      Serial.println("[SENSOR] WiFi conectado");
      Serial.print("[SENSOR] IP local: ");
      Serial.println(localIp());
    }
    wasConnected_ = true;
    return;
  }

  if (wasConnected_) {
    Serial.println("[SENSOR] WiFi desconectado");
    wasConnected_ = false;
  }
  if (nowMs - lastAttemptAtMs_ < reconnectIntervalMs_) {
    return;
  }

  lastAttemptAtMs_ = nowMs;
  Serial.println("[SENSOR] Conectando a WiFi...");
  WiFi.begin(ssid_, password_);
}

bool WiFiConnection::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiConnection::localIp() const {
  return WiFi.localIP().toString();
}
