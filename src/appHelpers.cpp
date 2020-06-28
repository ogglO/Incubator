#include "appHelpers.h"
#include "sensors.h"
#include "controller.h"
#include "hwInterfaces.h"
#include "definitions.h"

uint16_t timeSinceLastRotation = 0; //secons since last rotation

uint16_t getTimeUntilRotation()
{
    return ROTATION_INTERVAL * 60 - timeSinceLastRotation;
}

void rotate()
{
    timeSinceLastRotation += 1;

    if (timeSinceLastRotation >= ROTATION_INTERVAL * 60)
    {
        stepperStep(ROTATION_STEPS);
        timeSinceLastRotation = 0;
    }
}

void watchman()
{
    if (getStatusLidClosed())
    {
        if (getControllerEnabled())
        {
            controllerState(CONTROLLER_ACTIVE);
            setFan(1);
        }
        else
        {
            controllerState(CONTROLLER_INACTIVE);
            setFan(0);
        }
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
    }
}