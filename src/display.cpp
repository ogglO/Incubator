#include "display.h"
#include "definitions.h"
#include "controller.h"
#include "sensors.h"
#include "appHelpers.h"
#include "hwInterfaces.h"
#include "menu.h"
#include <Adafruit_GFX.h>    // Adafruit Grafik-Bibliothek
#include <Adafruit_ST7735.h> // Adafruit ST7735-Bibliothek

uint16_t getPosFromRight(uint16_t characters);
uint8_t getSpacesTo1000(uint16_t number);

Adafruit_ST7735 TFT = Adafruit_ST7735(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

//VARS TFT
uint16_t textSize = 1;
uint16_t currentY = 0;
uint16_t currentX = 0;
uint16_t padding = 6;
uint16_t minX = padding;
uint16_t minY = padding;
uint16_t maxX = 127 - (padding - 1); //1 for space after character
uint16_t maxY = 160 - (padding - 1);
uint16_t fontMinWidth = 6;       //multiply by text size
uint16_t fgColor = ST77XX_WHITE; //default foreground color
uint16_t bgColor = ST77XX_BLACK; //default bgColor

void setupDisplay()
{
    TFT.initR(INITR_BLACKTAB); // init ST7735-Chip
    TFT.setRotation(2);
    TFT.fillScreen(bgColor);
    TFT.setTextColor(fgColor, bgColor); //set default colors
}

void drawDisplayDynamic()
{
    //initial values
    TFT.setTextColor(fgColor, bgColor);
    currentX = minX;
    currentY = minY;
    TFT.setTextSize(textSize = 1);

    //app temperature change
    /*
    currentY += (2 + 8) * textSize; //2 free space at textSize=1, 8 text height at textSize=1
    currentY += 25;                 //Space //41
    TFT.drawCircle(currentX + 3, currentY + 3, 3, ST77XX_GREEN);
    TFT.setCursor((currentX + 3) + 3 + 1 + 1, currentY); //circle middle +radius +linewidth +space
    TFT.print("0.07");
    */

    //menu Y41

    currentY = 70;
    TFT.setCursor(currentX = minX, currentY);
    TFT.setTextColor(fgColor, bgColor);

    if (menuIsOpen())
    {
        TFT.print(getCurrentItem());
        TFT.print(" ");
        if (menuItemIsActive())
        {
            TFT.setTextColor(ST77XX_BLUE, bgColor);
        }

        TFT.print(getCurrentValue());
        //TFT.print("inaktiv");
    }
    else
    {
        TFT.print("                    ");
    }

    //heater temperature
    TFT.setTextColor(fgColor, bgColor);
    currentY += (2 + 8) * textSize;
    currentY += 40; //91
    currentY = 91;
    TFT.setCursor(getPosFromRight(11), currentY);
    TFT.print(getTemperatureHeaterCurrent(), 1);
    TFT.print("/");
    TFT.print(getTemperatureHeaterTarget(), 1);
    TFT.print(" C");

    //humidity
    currentY += (2 + 8) * textSize;
    TFT.setCursor(getPosFromRight(5), currentY);
    if (getHumidity() < 100.0)
    {
        TFT.print(" ");
    }
    TFT.print((int)(getHumidity() + 0.5)); //+0.5 for correct rounding
    TFT.print(" %");

    //Stepper
    currentY += (2 + 8) * textSize;
    TFT.setCursor(getPosFromRight(7), currentY);
    if (!getRotationStatus())
    {
        TFT.setTextColor(ST77XX_RED, bgColor);
        TFT.print("inaktiv");
    }
    else
    {
        TFT.setCursor(getPosFromRight(7), currentY);
        TFT.print(" ");
        for (uint8_t i = 0; i < getSpacesTo1000(getTimeUntilRotation()); i++)
        {
            TFT.print(" ");
        }
        TFT.print(getTimeUntilRotation());
        TFT.setCursor(getPosFromRight(2), currentY);
        TFT.print(" ");
        TFT.print(getRotationIntervalUnit());
    }

    TFT.setTextColor(fgColor, bgColor);

    //lid status
    currentY += (2 + 8) * textSize;
    TFT.setCursor(getPosFromRight(11), currentY);
    if (getStatusLidClosed())
    {
        TFT.setTextColor(ST77XX_GREEN, bgColor);
        TFT.print("geschlossen");
    }
    else
    {
        TFT.setTextColor(ST77XX_RED, bgColor);
        TFT.print("  geoeffnet");
    }

    //controller status
    currentY += (2 + 8) * textSize;
    TFT.setCursor(getPosFromRight(8), currentY);
    switch (controllerState())
    {
    case CONTROLLER_INACTIVE:
        TFT.setTextColor(ST77XX_RED, bgColor);
        TFT.print(" inaktiv");
        break;
    case CONTROLLER_PAUSED:
        TFT.setTextColor(ST77XX_YELLOW, bgColor);
        TFT.print("pausiert");
        break;
    case CONTROLLER_ACTIVE:
        TFT.setTextColor(ST77XX_GREEN, bgColor);
        TFT.print("   aktiv");
        break;
    }

    //debug counter
    currentY += (2 + 8) * textSize;
    TFT.setCursor(getPosFromRight(8), currentY);
    //TFT.print(counter);
    //TFT.print(getEncClicks());

    //application temperature
    TFT.setCursor(48, 10);
    TFT.setTextSize(textSize = 3);
    TFT.setTextColor(ST77XX_RED, bgColor);
    if (getTemperatureAppCurrent() < -100.0) //error value = -127.0
    {
        TFT.print("err");
    }
    else
    {
        if (abs(getTemperatureAppCurrent() - TEMPERATURE_APP_TARGET) <= TEMPERATURE_APP_TOLERANCE)
        {
            TFT.setTextColor(ST7735_GREEN, bgColor);
        }
        TFT.print(getTemperatureAppCurrent(), 1);
    }

    TFT.setTextColor(fgColor);
    TFT.setCursor(48, 10 + textSize * 8 + 2 + 2);
    TFT.print(TEMPERATURE_APP_TARGET, 1);
}

void drawDisplayStatic()
{
    //"Temp [C]"
    currentX = minX;
    currentY = minY;
    TFT.setTextSize(textSize = 1);
    TFT.setCursor(currentX, currentY);
    TFT.print("Temp");
    currentY += (2 + 8) * textSize; //2 free space at textSize=1, 8 text height at textSize=1
    TFT.setCursor(currentX, currentY);
    TFT.print("[C]");

    //stability value

    currentY += 25; //Space
    //draw change status circle and print change rate
    currentY += (2 + 8) * textSize;
    TFT.setCursor(currentX, currentY);
    //TFT.print("C/60s");

    //Additional info
    currentY += 40;
    TFT.setCursor(currentX, currentY);
    TFT.print("Heizung");

    currentY += (2 + 8) * textSize;
    TFT.setCursor(currentX, currentY);
    TFT.print("Feuchtigkeit");

    //stepper
    currentY += (2 + 8) * textSize;
    TFT.setCursor(currentX, currentY);
    TFT.print("Rotation in");

    currentY += (2 + 8) * textSize;
    TFT.setCursor(currentX, currentY);
    TFT.print("Deckel");

    currentY += (2 + 8) * textSize;
    TFT.setCursor(currentX, currentY);
    TFT.print("Regelung");

    TFT.setTextSize(textSize = 3);
    TFT.drawFastVLine(44, 6, textSize * 8 * 2 + 9, fgColor);
    TFT.drawFastHLine(44, 6, 10, fgColor);
    TFT.drawFastHLine(44, textSize * 8 * 2 + 7 + 7, 10, fgColor);

    TFT.drawFastVLine(120, 6, textSize * 8 * 2 + 9, fgColor);
    TFT.drawFastHLine(120, 6, -10, fgColor);
    TFT.drawFastHLine(120, textSize * 8 * 2 + 7 + 7, -10, fgColor);

    TFT.drawFastHLine(48, 10 + textSize * 8, textSize * 6 * 4 - 2, fgColor);
}

uint16_t getPosFromRight(uint16_t characters)
{
    return maxX - characters * fontMinWidth * textSize;
};

/*
void drawDisplayDynamic(int16_t counter)
{
    digitalWrite(PIN_TFT_BACKLIGHT, 1);
    TFT.setCursor(10,10);
    TFT.setTextSize(3);
    TFT.print(counter);
};*/

uint8_t getSpacesTo1000(uint16_t number)
{
    uint8_t spaces = 0;
    if (number < 1000)
    {
        spaces += 1;
    }
    if (number < 100)
    {
        spaces += 1;
    }
    if (number < 10)
    {
        spaces += 1;
    }
    return spaces;
}