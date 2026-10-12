#include "WiFiConnection.h"
#include <WiFi.h>
WiFiConnection::WiFiConnection(const char*s,const char*p,unsigned long r):ssid_(s),password_(p),retryMs_(r){}
void WiFiConnection::begin(){WiFi.mode(WIFI_STA);lastAttempt_=millis()-retryMs_;}
void WiFiConnection::update(unsigned long now){if(isConnected()){if(!wasConnected_){Serial.println("[ACTUATOR] WiFi conectado");Serial.print("[ACTUATOR] IP local: ");Serial.println(WiFi.localIP());}wasConnected_=true;return;}if(wasConnected_){Serial.println("[ACTUATOR] WiFi desconectado");wasConnected_=false;}if(now-lastAttempt_>=retryMs_){lastAttempt_=now;Serial.println("[ACTUATOR] Conectando a WiFi...");WiFi.begin(ssid_,password_);}}
bool WiFiConnection::isConnected()const{return WiFi.status()==WL_CONNECTED;}
