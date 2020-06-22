#include <controller.h>
#include <definitions.h>
#include <sharedVars.h>

//controller setup
float temperatureAppTarget = TEMPERATURE_APP_TARGET;
const float kp = CONTROLLER_KP;
const float ki = CONTROLLER_KI;
const float kd = CONTROLLER_KD;
const float controllerOutputLimitHeater = CONTROLLER_OUTPUT_LIMIT_HEATER;  //pwm ratio //20 kHz setting 800 is max
const float controllerOutputLimitApp = CONTROLLER_OUTPUT_LIMIT_APP;    //°C 
bool controllerEnabled = 0; //start inactive
float temperatureHeaterTarget = temperatureAppTarget;
char controllerState = 0;

//controller vars
float controllerOutputHeater = 0;
float controllerOutputApp;
float errorCurrentHeater;
float errorCurrentApp, errorLastApp;
float derivative;
float integral;

//set inital value for inner control loop
//temperatureHeaterTarget = temperatureAppTarget;

void controllerSet(char state) //2 = active, 1 = paused, 0 = inactive
{

    if (!controllerEnabled)
    {
        state = 0; //disable if not enabled by user
    }
    else if (!statusLidClosed)
    {
        state = 1; //pause if lid open
    }

    switch (state)
    {
    case 2:
        analogWrite(PIN_HEATER, (int)controllerOutputHeater);
        digitalWrite(PIN_FAN, 1);
        controllerState = 2;
        break;
    case 1:
        analogWrite(PIN_HEATER, 0);
        controllerState = 1;
        break;
    case 0:
        analogWrite(PIN_HEATER, 0);
        digitalWrite(PIN_FAN, 0);
        controllerState = 0;
        break;
    }
}

void controllerEnable()
{
    controllerEnabled = !controllerEnabled;
}

void controllerAppRun()
{
    if (controllerState == 2)
    {
        errorCurrentApp = temperatureAppTarget - temperatureAppCurrent;
        integral += errorCurrentApp * timingControllerApp;
        derivative = (errorCurrentApp - errorLastApp) / timingControllerApp;

        controllerOutputApp = kp * errorCurrentApp + ki * integral + kd * derivative;

        errorLastApp = errorCurrentApp;

        if (controllerOutputApp > controllerOutputLimitApp)
        {
            controllerOutputApp = controllerOutputLimitApp;
            integral = integral - errorCurrentApp * timingControllerApp; //no i windup
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
    if (controllerState == 2)
    {
        errorCurrentHeater = temperatureHeaterTarget - temperatureHeaterCurrent;
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

void setupController()
{
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), controllerEnable, FALLING);
}