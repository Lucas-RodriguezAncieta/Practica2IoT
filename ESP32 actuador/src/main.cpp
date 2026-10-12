#include <Arduino.h>
#include "LedController.h"
#include "TcpActuatorClient.h"
#include "WiFiConnection.h"
#if __has_include("WiFiSecrets.h")
#include "WiFiSecrets.h"
#else
#include "WiFiSecrets.example.h"
#endif
namespace { constexpr uint8_t kRed=25,kYellow=26,kGreen=27; constexpr unsigned long kRetry=2000UL,kTcpTimeout=100UL; LedController leds(kRed,kYellow,kGreen); WiFiConnection wifi(kWifiSsid,kWifiPassword,kRetry); TcpActuatorClient tcp(kServerIp,kServerPort,leds,kRetry,kTcpTimeout); }
void setup(){Serial.begin(115200);Serial.println("[ACTUATOR] Iniciando...");leds.begin();Serial.println("[ACTUATOR] LEDs apagados");if(kUsingExampleWiFiSecrets)Serial.println("[ACTUATOR] Configure WiFiSecrets.h antes de conectar");else wifi.begin();}
void loop(){unsigned long now=millis();leds.update(now);if(!kUsingExampleWiFiSecrets){wifi.update(now);tcp.update(now,wifi.isConnected());}}
