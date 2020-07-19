#pragma once
#include <Arduino.h>

void watchman();
void rotate();
void rotationEnable();
bool getRotationStatus();
uint16_t getTimeUntilRotation();
char getRotationIntervalUnit();
void oneSecondTicker();