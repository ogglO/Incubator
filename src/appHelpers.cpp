#include "appHelpers.h"
#include "sensors.h"
#include "controller.h"
#include "hwInterfaces.h"

void watchman()
{
    if (getStatusLidClosed())
    {
        if (getControllerEnabled())
        {
            controllerState(1);
            setFan(1);
        }
        else
        {
            controllerState(0);
            setFan(0);
        }
    }
    else
    {
        setFan(0);
        if (getControllerEnabled())
        {
            controllerState(2);
        }
        else
        {
            controllerState(0);
        }
    }
}