#include <Arduino.h>
#include "menu.h"
#include "definitions.h"
#include "controller.h"

//build menu array
//0 "Menu"
//1 "Controller enable"
//2 "Temp"
//3 "Rotation Interval"
//4 "Close menu"

// simple click -> lights
// double click -> menu
    //turning select item
    //simple click activate item
    //turning select value
    //simple click set value

//step 1: enable simple/double click        //done
//step 2: enable toggling lights            //done
//step 3: enable opening menu               
//step 4: enable scrolling through menu
//step 5: enable closing menu
//step 6: enable selecting item
//step 7: enable changing values

const size_t strLength = 15;
const size_t numberOfMenuItems = 5;

//state vars
bool stateMenuOpen = 0;    //true if menu is open
bool stateItemActive = 0;  //true if item selected for changing
uint8_t currentItem = 1;


void changeTempValue(int8_t t_increase);

void menuSetup()
{
}


char menuItems[numberOfMenuItems][strLength] =
{
    "Regler        ", //0
    "Temperatur    ", //1
    "Rot. Intervall", //2
    "Rot. Schritte ",
    "Licht Auto-aus"
};

//setup for each item
//0 current value
//1 min value
//2 max value
//3 step size

float itemSetup[numberOfMenuItems][4]
{
    {0,     0,      1,      1},
    {37.5,  25.0,   45.0,   0.1},
    {0,     0,      30.0,   600.0},
    {0,     500,    10000,  500},
    {0,     0,      60,     5}
};

float itemTempValues[numberOfMenuItems]
{
    CONTROLLER_START_ACTIVE,
    TEMPERATURE_APP_TARGET,
    ROTATION_INTERVAL,
    ROTATION_STEPS,
    LIGHT_AUTO_OFF_DELAY
};

char* getCurrentItem()
{
    char* str = (char*) malloc(strLength * sizeof(char));
    str = menuItems[currentItem - 1];
    return str;
}

float getCurrentValue()
{
    return itemTempValues[currentItem - 1];
}

void changeTempValue(int8_t t_increase)
{
    itemTempValues[currentItem - 1] += 1 * t_increase;
}

void menuUp()
{
    if(!stateItemActive)
    {
        currentItem += 1;
        if (currentItem == numberOfMenuItems + 1)
        {
            currentItem = 1;
        }
    }
    else
    {
        changeTempValue(1);
    }
}

void menuDown()
{
    if(!stateItemActive)
    {
        currentItem -= 1;
        if (currentItem == 0)
        {
            currentItem = numberOfMenuItems;
        }
    }
    else
    {
        changeTempValue(-1);
    }
}

bool menuIsOpen()
{
    return stateMenuOpen;
}

bool menuItemIsActive()
{
    return stateItemActive;
}

void menuSingleClick()
{
    stateItemActive = !stateItemActive;
}

void menuDoubleClick()
{
    stateMenuOpen = !stateMenuOpen;
}