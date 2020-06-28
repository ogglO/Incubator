#pragma once

//pin definitions
#define PIN_SENSOR_LID A6
#define PIN_STEPPER_DISABLE A1
#define PIN_STEPPER_STEP 4
#define PIN_HEATER 9
#define PIN_SENSOR_TEMP_HEATER A7
#define PIN_SENSOR_HUMIDITY A2
#define PIN_ONEWIRE A3
#define PIN_TFT_CS 10       // TFT chip select
#define PIN_TFT_DC 7        // TFT command or display
#define PIN_TFT_RST 8       // TFT reset
#define PIN_TFT_BACKLIGHT 6 // TFT backlight
#define PIN_ENC_SW 2        //encoder switch
#define PIN_ENC_A 5         //bitwise reading used. Need to change byte value in encoderEvaluate() as well
#define PIN_ENC_B 3         //bitwise reading used. Need to change byte value in encoderEvaluate() as well
#define PIN_FAN A0
#define PIN_LED 12

//other
#define SERIAL_BAUD 115200

//timing
#define TIMING_DISPLAY 50               //ms //interval between display redraws (only dynamic values)
#define TIMING_SENSORS 1000             //ms //interval between sensor updates DS18B20 takes about 750ms! Non blocking request is updated next cycle, temp values are therefore min. this time old
#define TIMING_CONTROLLER_HEATER 1000   //ms //interval between running the heater bang-bang-controller
#define TIMING_CONTROLLER_APP 6000      //ms //interval between running the application pid-controller

//controller
#define CONTROLLER_KP 15.0
#define CONTROLLER_KI 0.00004
#define CONTROLLER_KD 0.0
#define CONTROLLER_OUTPUT_LIMIT_HEATER 350  //pwm //800 is max, defined in setupPWM()
#define CONTROLLER_OUTPUT_LIMIT_APP 80.0    //°C //max. allowed heater temperature
#define CONTROLLER_START_ACTIVE 1           //important in case of restart after power outage

//application
#define TEMPERATURE_APP_TARGET 37.5   //°C //application target temperature
#define TEMPERATURE_APP_TOLERANCE 0.2 //display temperature in green if within +- this tolerance

//other
#define DISPLAY_IDLE_TIME 60 //s //display turns off after this time has passed without input