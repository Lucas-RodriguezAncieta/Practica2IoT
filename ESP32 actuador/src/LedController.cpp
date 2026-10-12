#include "LedController.h"
namespace { constexpr unsigned long kDangerMs=150UL, kWarningMs=500UL; }
LedController::LedController(uint8_t r,uint8_t y,uint8_t g):redPin_(r),yellowPin_(y),greenPin_(g){}
void LedController::begin(){pinMode(redPin_,OUTPUT);pinMode(yellowPin_,OUTPUT);pinMode(greenPin_,OUTPUT);writeOutputs(false,false,false);}
void LedController::setState(LedState state,unsigned long now){if(state_==state)return; state_=state; phaseStartedAtMs_=now; phaseOn_=state==LedState::DANGER||state==LedState::WARNING; update(now);}
void LedController::update(unsigned long now){if(state_==LedState::SAFE){writeOutputs(false,false,true);return;} if(state_==LedState::INVALID){writeOutputs(false,false,false);return;} const unsigned long interval=state_==LedState::DANGER?kDangerMs:kWarningMs; if(now-phaseStartedAtMs_>=interval){phaseStartedAtMs_=now;phaseOn_=!phaseOn_;} writeOutputs(state_==LedState::DANGER&&phaseOn_,state_==LedState::WARNING&&phaseOn_,false);}
LedState LedController::state() const{return state_;}
void LedController::writeOutputs(bool r,bool y,bool g){digitalWrite(redPin_,r);digitalWrite(yellowPin_,y);digitalWrite(greenPin_,g);}
