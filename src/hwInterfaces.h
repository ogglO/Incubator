#pragma once
#include <Arduino.h>

void setupHWInterfaces();
void setLight();
void setLight(bool t_state);
void lightAutoOff();
void setFan(bool state);
void setHeater(uint16_t pwm);
void setMotor(uint16_t steps);
void evaluateEncoder();
int16_t getEncoderValue();
void stepperStep(uint16_t t_steps);

void encClick(); //handling encoder click
void encClickHandler();
uint8_t getEncClicks(); //for debug
void encDebounce();