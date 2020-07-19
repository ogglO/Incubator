#include "appHelpers.h"
#include "sensors.h"
#include "controller.h"
#include "hwInterfaces.h"
#include "definitions.h"

uint16_t timeSinceLastRotation = 0; //secons since last rotation
bool rotationEnabled = ROTATION_ENABLED;
char rotationIntervalUnit;
bool statusLidClosedLast;
uint16_t controllerDelay;

uint16_t getTimeUntilRotation()
{
    uint16_t timeUntilRotation = ROTATION_INTERVAL * 60 - timeSinceLastRotation;

    if (timeUntilRotation >= 120)
    {
        timeUntilRotation = (timeUntilRotation + 30) / 60;
        rotationIntervalUnit = 'm';
    }
    else
    {
        rotationIntervalUnit = 's';
    }

    return timeUntilRotation;
}

char getRotationIntervalUnit()
{
    return rotationIntervalUnit;
}

void rotationEnable()
{
    rotationEnabled = !rotationEnabled;
}

bool getRotationStatus()
{
    return rotationEnabled;
}

void rotate()
{
    if (rotationEnabled)
    {
        timeSinceLastRotation += 1;

        if (timeSinceLastRotation >= ROTATION_INTERVAL * 60)
        {
            stepperStep(ROTATION_STEPS);
            timeSinceLastRotation = 0;
        }
    }
}

void oneSecondTicker()
{
    if (controllerDelay > 0)
    {
        controllerDelay -= 1;
    }
}

void watchman()
{
    if (getStatusLidClosed())
    {
        if (getControllerEnabled())
        {
            if (statusLidClosedLast == 0)
            {
                controllerDelay = CONTROLLER_RESTART_DELAY;
            }
            if (controllerDelay != 0)
            {
                controllerState(CONTROLLER_PAUSED);
            }
            else
            {
                controllerState(CONTROLLER_ACTIVE);
            }
            
            setFan(1);
        }
        else
        {
            controllerState(CONTROLLER_INACTIVE);
            setFan(0);
        }
        statusLidClosedLast = 1;
    }
    else
    {
        setFan(0);
        if (getControllerEnabled())
        {
            controllerState(CONTROLLER_PAUSED);
        }
        else
        {
            controllerState(CONTROLLER_INACTIVE);
        }
        statusLidClosedLast = 0;
    }
}