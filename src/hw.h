#pragma once
#include <Arduino.h>

void setupHW();
void setLight(bool state);
void setFan(bool state);
void setHeater(uint16_t pwm);
void setMotor(uint16_t steps);