#pragma once
#include <Arduino.h>
class WiFiConnection { public: WiFiConnection(const char* ssid,const char* password,unsigned long retryMs); void begin(); void update(unsigned long now); bool isConnected() const; private: const char* ssid_;const char* password_;unsigned long retryMs_,lastAttempt_=0;bool wasConnected_=false;};
