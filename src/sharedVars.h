#pragma once
//declarations for shared vars
//need to be defined somewhere else

#include <Arduino.h>

//timing
//defined in main.cpp
extern const uint16_t timingDisplay;
extern const uint16_t timingSensors;
extern const uint16_t timingControllerHeater;
extern const uint16_t timingControllerApp;
extern const uint16_t timingEncoder;
extern bool firstStartup;

//sensors and status vars
//defined in sensors.cpp
extern float temperatureAppCurrent;
extern float temperatureHeaterCurrent;
extern float humidity;
extern bool statusLidClosed;

//controller
//defined in controller.cpp
extern bool controllerEnabled;
extern char controllerState;
extern float temperatureHeaterTarget;
extern float temperatureAppTarget;