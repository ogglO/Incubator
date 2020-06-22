#include <hw.h>
#include <definitions.h>
#include <Arduino.h>
#include <sharedVars.h>

//set all hw states except TFT
//heater
//lights
//fan

void setupPWM();

void setupHW()
{
    pinMode(PIN_HEATER, OUTPUT); //define before changing pwm settings! (setupPWM)
    setupPWM();                  //set pwm to 20 kHz
    pinMode(PIN_STEPPER_DISABLE, OUTPUT);
    pinMode(PIN_STEPPER_STEP, OUTPUT);
    pinMode(PIN_TFT_BACKLIGHT, OUTPUT);
    pinMode(PIN_ENC_A, INPUT);
    pinMode(PIN_ENC_B, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    //pinMode(PIN_SENSOR_LID, INPUT_PULLUP); //A6, A7 don't have digital features. Just analog pins

    digitalWrite(PIN_STEPPER_DISABLE, 1);
    analogWrite(PIN_HEATER, 0);
    digitalWrite(PIN_FAN, 0);
    digitalWrite(PIN_LED, 0);
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
