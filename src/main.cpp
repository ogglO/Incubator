#include <Arduino.h>
#include "definitions.h"
#include "hwInterfaces.h"
#include "display.h"
#include "sensors.h"
#include "controller.h"
#include <appHelpers.h>

//timing
const uint16_t timingDisplay = TIMING_DISPLAY;                    //ms //draw only dynamics //draw statics on toggling display LED
const uint16_t timingSensors = TIMING_SENSORS;                    //ms //read data then request for next round
const uint16_t timingControllerHeater = TIMING_CONTROLLER_HEATER; //ms //inner control loop
const uint16_t timingControllerApp = TIMING_CONTROLLER_APP;       //ms //outer controll loop
const uint16_t timingEncoder = TIMING_ENCODER;                    //ms
const uint16_t timingStats = 1000;

//timing vars
uint32_t timingDisplayLast;
uint32_t timingSensorsLast;
uint32_t timingControllerHeaterLast;
uint32_t timingControllerAppLast;
uint32_t timingEncoderLast;
uint32_t timingStatsLast;

bool firstStartup = 1;

void setup()
{
    Serial.begin(SERIAL_BAUD);

    setupHWInterfaces();
    setupSensors();
    setupDisplay();

    digitalWrite(PIN_TFT_BACKLIGHT, 1);

    drawDisplayStatic();
}

void loop()
{
    watchman();

    if ((millis() - timingEncoderLast) >= timingEncoder)
    {
        //evaluateEncoder();
        timingEncoderLast = millis();
    }
    else if ((millis() - timingDisplayLast) >= timingDisplay)
    {
        drawDisplayDynamic(0);
        timingDisplayLast = millis();
    }
    else if ((millis() - timingStatsLast) >= timingStats)
    {
        //do stats here
        timingStatsLast = millis();
    }
    else if ((millis() - timingSensorsLast) >= timingSensors)
    {
        controllerState(2); //pause
        updateSensorData();
        controllerState(1); //reactivate;
        timingSensorsLast = millis();
    }
    else if ((millis() - timingControllerHeaterLast) >= timingControllerHeater)
    {
        controllerHeaterRun();
        timingControllerHeaterLast = millis();
    }
    else if ((millis() - timingControllerAppLast) >= timingControllerApp)
    {
        controllerAppRun();
        timingControllerAppLast = millis();
    }

    if (firstStartup)
    {
        firstStartup = 0;
    }
}