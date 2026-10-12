#pragma once
#include <Arduino.h>
// Copie como WiFiSecrets.h, complete valores locales y no lo suba a Git.
constexpr char kWifiSsid[] = "REPLACE_WITH_WIFI_SSID";
constexpr char kWifiPassword[] = "REPLACE_WITH_WIFI_PASSWORD";
constexpr char kServerIp[] = "REPLACE_WITH_PC_IPV4";
constexpr uint16_t kServerPort = 5000;
constexpr bool kUsingExampleWiFiSecrets = true;
