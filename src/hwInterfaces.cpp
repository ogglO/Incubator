#include <Arduino.h>
#include "hwInterfaces.h"
#include "definitions.h"
#include "controller.h"

//set all hw states except TFT
//heater
//lights
//fan

//encoder
bool encoderALast;
int16_t encoderValue;

void setupPWM();
void setupTimer2();

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
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
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
        }
        else
        { //decrase if A!=B
            encoderValue -= 1;
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

    digitalWrite(PIN_STEPPER_DISABLE, 1); //make sure stepper is disabled or motor will overheat
    analogWrite(PIN_HEATER, 0);           //init with the heater turned off
    digitalWrite(PIN_FAN, 0);             //init with fan turned off
    digitalWrite(PIN_LED, 0);             //init with led turned off

    //attach interrupt to encoder button
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), controllerEnable, FALLING);
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
