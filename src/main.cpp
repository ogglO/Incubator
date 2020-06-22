#include <Arduino.h>
#include <definitions.h>
#include <sharedVars.h>
#include <hw.h>
#include <TFT.h>
#include <sensors.h>
#include <controller.h>

void evaluateEncoder();

//encoder
bool encoderALast;
int16_t encoderValue;

//timing
const uint16_t timingDisplay = TIMING_DISPLAY;             //ms //draw only dynamics //draw statics on toggling display LED
const uint16_t timingSensors = TIMING_SENSORS;         //ms //read data then request for next round
const uint16_t timingControllerHeater = TIMING_CONTROLLER_HEATER;   //ms //inner control loop
const uint16_t timingControllerApp = TIMING_CONTROLLER_APP;      //ms //outer controll loop
const uint16_t timingEncoder = TIMING_ENCODER;             //ms

//timing vars
uint32_t timingDisplayLast;
uint32_t timingSensorsLast;
uint32_t timingControllerHeaterLast;
uint32_t timingControllerAppLast;
uint32_t timingEncoderLast;

bool firstStartup = 1;

void setup()
{
    Serial.begin(SERIAL_BAUD);

    //setupPins();
    setupHW();
    setupController();
    setupSensors();
    setupTFT();

    digitalWrite(PIN_TFT_BACKLIGHT, 1);

    drawDisplayStatic();
}

void loop()
{
    if ((millis() - timingEncoderLast) >= timingEncoder)
    {
        evaluateEncoder();
        timingEncoderLast = millis();
    }
    else if ((millis() - timingDisplayLast) >= timingDisplay)
    {
        drawDisplayDynamic(encoderValue);
        timingDisplayLast = millis();
    }
    else if ((millis() - timingSensorsLast) >= timingSensors)
    {
        controllerSet(1); //pause
        getSensorData();
        controllerSet(2); //reactivate;
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

    controllerSet(2); //set controller active if enabled

    if (firstStartup)
    {
        firstStartup = 0;
    }
}

//this should go in encoder.cpp
//evaluate encoder
void evaluateEncoder()
{

    bool encA;
    bool encB;

    //encA = digitalRead(PIN_ENC_A); //switch to more basic readout if too slow
    //encB = digitalRead(PIN_ENC_B);
    encA = PIND & B00100000; //faster way (pin 2 is 2rd pin in register D. pin 3 3rd)
    encB = PIND & B00001000;

    //switch signs to change direction
    if (encA != encoderALast)
    { //if pinA changed
        if (encA != encB)
        { //increase if A=B (one click for KY-040)
            encoderValue += 1;
        }
        else
        { //decrase if A!=B
            encoderValue -= 1;
        }
    }

    encoderALast = encA;
}