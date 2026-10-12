#pragma once

#include <Arduino.h>

// Copie este archivo como WiFiSecrets.h y reemplace todos los valores locales.
// WiFiSecrets.h esta ignorado por Git y no debe contenerse en evidencias.
constexpr char kWifiSsid[] = "REPLACE_WITH_WIFI_SSID";
constexpr char kWifiPassword[] = "REPLACE_WITH_WIFI_PASSWORD";
constexpr char kServerIp[] = "REPLACE_WITH_PC_IPV4";
constexpr uint16_t kServerPort = 5000;
constexpr bool kUsingExampleWiFiSecrets = true;
