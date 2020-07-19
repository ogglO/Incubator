#include "controller.h"
#include "definitions.h"
#include "sensors.h"

//controller setup
float temperatureAppTarget = TEMPERATURE_APP_TARGET;
const float kp = CONTROLLER_KP;
const float ki = CONTROLLER_KI;
const float kd = CONTROLLER_KD;
const float controllerOutputLimitHeater = CONTROLLER_OUTPUT_LIMIT_HEATER; //pwm ratio //20 kHz setting 800 is max
const float controllerOutputLimitApp = CONTROLLER_OUTPUT_LIMIT_APP;       //°C
bool controllerEnabled = CONTROLLER_START_ACTIVE;
char controllerCurrentState;
float temperatureHeaterTarget = temperatureAppTarget;

//controller vars
float controllerOutputHeater = 0;
float controllerOutputApp;
float errorCurrentHeater;
float errorCurrentApp, errorLastApp;
float derivative;
float integral;

//set inital value for inner control loop
//temperatureHeaterTarget = temperatureAppTarget;

void setupController()
{
    integral = TEMPERATURE_APP_TARGET * 85000.0 - 2066000; //calculated from integral value in stable condition at 37.5 °C. Assumption i is 0 at 24 °C. Slope lowered to avoid overshoot.
}

float getTemperatureHeaterTarget()
{
    return temperatureHeaterTarget;
}

char controllerState()
{
    return controllerCurrentState;
}

void controllerState(char state)
{
    switch (state)
    {
    case CONTROLLER_ACTIVE:
        analogWrite(PIN_HEATER, (int)controllerOutputHeater);
        controllerCurrentState = CONTROLLER_ACTIVE;
        break;
    case CONTROLLER_PAUSED:
        analogWrite(PIN_HEATER, 0);
        controllerCurrentState = CONTROLLER_PAUSED;
        break;
    case CONTROLLER_INACTIVE:
        analogWrite(PIN_HEATER, 0);
        controllerCurrentState = CONTROLLER_INACTIVE;
        break;
    }
}

bool getControllerEnabled()
{
    return controllerEnabled;
}

void controllerEnable()
{
    controllerEnabled = !controllerEnabled;
}

void controllerAppRun()
{
    if (controllerState() == 1)
    {
        errorCurrentApp = temperatureAppTarget - getTemperatureAppCurrent();
        integral += errorCurrentApp * TIMING_CONTROLLER_APP;
        derivative = (errorCurrentApp - errorLastApp) / TIMING_CONTROLLER_APP;

        controllerOutputApp = kp * errorCurrentApp + ki * integral + kd * derivative;

        errorLastApp = errorCurrentApp;

        if (controllerOutputApp > controllerOutputLimitApp)
        {
            controllerOutputApp = controllerOutputLimitApp;
            integral = integral - errorCurrentApp * TIMING_CONTROLLER_APP; //no i windup
        }

        if (controllerOutputApp < 0)
        {
            controllerOutputApp = 0;
        }

        temperatureHeaterTarget = controllerOutputApp;
    }
}
void controllerHeaterRun()
{
    if (controllerState() == 1)
    {
        errorCurrentHeater = temperatureHeaterTarget - getTemperatureHeaterCurrent();
        if (errorCurrentHeater > 0)
        {
            controllerOutputHeater = controllerOutputLimitHeater;
        }
        else
        {
            controllerOutputHeater = 0;
        }
    }
    else
    {
        controllerOutputHeater = 0;
    }
}
