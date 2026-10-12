#pragma once
#include <Arduino.h>
enum class LedState { DANGER, WARNING, SAFE, INVALID };
class LedController {
 public:
  LedController(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);
  void begin(); void setState(LedState state, unsigned long nowMs); void update(unsigned long nowMs);
  LedState state() const;
 private:
  void writeOutputs(bool red, bool yellow, bool green);
  uint8_t redPin_, yellowPin_, greenPin_; LedState state_ = LedState::INVALID;
  unsigned long phaseStartedAtMs_ = 0; bool phaseOn_ = false;
};
