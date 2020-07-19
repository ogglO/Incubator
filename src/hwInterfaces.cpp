#include <Arduino.h>
#include "hwInterfaces.h"
#include "definitions.h"
#include "controller.h"
#include "menu.h"
#include "appHelpers.h"

//set all hw states except TFT
//heater
//lights
//fan

bool statusLight = LIGHT_ON_START_ENABLE;
uint16_t timeSinceLightsOn = 0;

//encoder
bool encoderALast;
int16_t encoderValue;
volatile uint8_t encoderRegisteredClicks = 0;
uint32_t encoderLastClick;
volatile bool encoderDebouncing = 0;

void setupPWM();
void setupTimer2();
void singleClick();
void doubleClick();
void tripleClick();

void singleClick()
{
    /*if(menuIsOpen())
    {
        menuSingleClick();
    }
    else
    {
        setLight();
    }*/
    setLight();
}

void doubleClick()
{
    //menuDoubleClick();
    rotationEnable();
}

void tripleClick()
{
    //menuTripleClick();
    controllerEnable();
}

void encDebounce()
{
    encoderDebouncing = 0;
}

uint8_t getEncClicks()
{
    return encoderRegisteredClicks;
}

void encClickHandler()
{
    if (millis() - encoderLastClick >= ENCODER_RESET)
    {
        switch (encoderRegisteredClicks)
        {
        case 1:
            singleClick();
            break;
        case 2:
            doubleClick();
            break;
        case 3:
            tripleClick();
            break;
        }
        encoderRegisteredClicks = 0;
    }
}

void encClick()
{
    if (!encoderDebouncing)
    {
        encoderDebouncing = 1;
        encoderRegisteredClicks += 1;
        encoderLastClick = millis();
    }
}

void stepperStep(uint16_t t_steps)
{
    controllerState(CONTROLLER_PAUSED);    //pause controller
    digitalWrite(PIN_STEPPER_DISABLE, 0);  //enable stepper
    for (uint16_t i = 0; i < t_steps; i++) //step
    {
        digitalWrite(PIN_STEPPER_STEP, 1),
            delayMicroseconds(STEPPER_PULSE_DURATION);
        digitalWrite(PIN_STEPPER_STEP, 0);
        delayMicroseconds(STEPPER_STEP_DELAY);
    }
    digitalWrite(PIN_STEPPER_DISABLE, 1); //disable stepper
    controllerState(CONTROLLER_ACTIVE);   //reactivate controller
}

void setupTimer2(char t_counterInit)
{
    //https://busylog.net/arduino-timer-interrupt-isr-example/
    //http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

    //calculation
    //timer 2 is an 8-bit timer. it counts from 1 to 255
    //16 MHz / (255 - t_counterInit) ticks = min. 62 kHz
    //62 kHz / prescaler 128 = min. 490 Hz
    //490 Hz is about 2 ms
    //t_counterInit > 0 (max 254) makes overflow happen faster

    //Atmel 328p datasheet p. 116
    TCCR2B = 0x00;                       //reset timer 2
    TCNT2 = t_counterInit;               //set the timer count start (counts from here to 255, then resets)
    TCCR2A = 0x00;                       //normal operationg mode, no pin connected p. 127-128
    TCCR2B |= (1 << CS22) | (1 << CS20); // Prescale 128 (Timer/Counter started) p.131
    TCCR2B &= ~(1 << CS21);              // CS22=1 CS21=0 CS20=1 -> prescale = 128
    TIMSK2 |= (1 << TOIE2);              //enable overflow interrupt p. 132
}

ISR(TIMER2_OVF_vect)
{
    evaluateEncoder();
    encDebounce();
}

int16_t getEncoderValue()
{
    return encoderValue;
}

//turn fan on or off
void setFan(bool state)
{
    digitalWrite(PIN_FAN, state);
}

void setLight()
{
    statusLight = !statusLight;
    digitalWrite(PIN_LED, statusLight);
    if(statusLight == 0)
    {
        timeSinceLightsOn = 0;
    }
}
void setLight(bool t_state)
{
    statusLight = t_state;
    digitalWrite(PIN_LED, statusLight);
    if(statusLight == 0)
    {
        timeSinceLightsOn = 0;
    }
}

void lightAutoOff()
{
    if (statusLight)
    {
        timeSinceLightsOn += 1;
        if(timeSinceLightsOn >= LIGHT_AUTO_OFF_DELAY)
        {
            setLight(0);
        }
    }
}


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
            menuUp();
        }
        else
        { //decrase if A!=B
            encoderValue -= 1;
            menuDown();
        }
    }

    encoderALast = encA;
}

void setupHWInterfaces()
{
    pinMode(PIN_HEATER, OUTPUT); //define before changing pwm settings! (setupPWM())
    pinMode(PIN_STEPPER_DISABLE, OUTPUT);
    pinMode(PIN_STEPPER_STEP, OUTPUT);
    pinMode(PIN_TFT_BACKLIGHT, OUTPUT);
    pinMode(PIN_ENC_A, INPUT);
    pinMode(PIN_ENC_B, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    //pinMode(PIN_SENSOR_LID, INPUT_PULLUP);    //A6, A7 don't have digital features. Just analog pins
    setupPWM();     //set pwm to 20 kHz
    setupTimer2(0); //set timer 2 for timer interrupt encoder

    digitalWrite(PIN_STEPPER_DISABLE, 1);          //make sure stepper is disabled or motor will overheat
    analogWrite(PIN_HEATER, 0);                    //init with the heater turned off
    digitalWrite(PIN_FAN, 0);                      //init with fan turned off
    digitalWrite(PIN_LED, LIGHT_ON_START_ENABLE); //init LED

    //attach interrupt to encoder button
    //attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), controllerEnable, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), encClick, FALLING);
}

void setupPWM()
{
    //Timer1 FastPWM Datasheet p.102
    //set PIN9 as output but not here for overview
    //pinMode(PIN_PWM, OUTPUT);

    //reset timer registers
    TCCR1A = 0;
    TCCR1B = 0;

    //COM1A1 = 2 for non-inverted PWM //COM1A0 = 0
    TCCR1A |= (1 << COM1A1);

    //define pwm mode to fast pwm with icr1 as TOP
    //TCCR1A |= (1 << WGM10);
    TCCR1A |= (1 << WGM11); //WGM10 = 0 Datasheet p.109
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << WGM13);

    //fclk = 16 MHz
    //fpwm = fclk/(N*(1+TOP))
    //n = 1,8,64,256,1024 <- prescaler // no prescaling
    TCCR1B |= (1 << CS10);
    //TCCR1B |= (1 << CS11);
    //TCCR1B |= (1 << CS12);

    //ICR1 as TOP
    //set ICR1 16-bit register //set to 800;
    ICR1 = 800;
    //OCR1A as Duty cycle 16-bit register OCR1A = TOP equals 100%
    //OCR1A = 0;
    //analogWrite works too. Better than setting registers, because analogWrite to 0 actually turns off pwm
    //setting register to 0 leads to a 1 cycle long spike on every reset
}
