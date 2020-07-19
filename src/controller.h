#pragma once

#include <Arduino.h>

void controllerEnable();
bool getControllerEnabled();
void controllerHeaterRun();
void controllerAppRun();
void controllerState(char state); //set controller state
char controllerState();           //get controller state
float getTemperatureHeaterTarget();
void setupController();