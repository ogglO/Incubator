#include "sensors.h"
#include "definitions.h"
#include <DallasTemperature.h>
#include <SPI.h>
#include <SimpleDHT.h>
#include <Adafruit_INA219.h>

float getNTCtemp();

//library initialization
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensorAppTemp(&oneWire);
SimpleDHT22 sensorHumidity(PIN_SENSOR_HUMIDITY);
Adafruit_INA219 sensorCurrent;

//DS18 setup
DeviceAddress appTemp1; // = {0x28, 0xDA, 0x0B, 0x79, 0x97, 0x20, 0x03, 0x2F}; //address of DS18B20 application temp sensor

float temperatureAppCurrent;
float temperatureHeaterCurrent;
float humidity;
bool statusLidClosed;

//getNTCtemp//NTC steinhart-hart coefficients
float Rseries = 9.7; //kOhm
float SH_a = 0.002241298;
float SH_b = 0.000223227;
float SH_c = 0.000000545;

float getTemperatureAppCurrent()
{
    return temperatureAppCurrent;
}
float getTemperatureHeaterCurrent()
{
    return temperatureHeaterCurrent;
}
float getHumidity()
{
    return humidity;
}
bool getStatusLidClosed()
{
    return statusLidClosed;
}

void updateSensorData()
{
    //application temp
    //getSensorData should be timed so one conversion can happen in between two calls

    temperatureAppCurrent = sensorAppTemp.getTempC(appTemp1); //get temp from last conversion
    sensorAppTemp.requestTemperaturesByAddress(appTemp1);     //request new conversion

    //heater temp
    temperatureHeaterCurrent = getNTCtemp();

    //humidity
    sensorHumidity.read2(NULL, &humidity, NULL);

    //lid
    if (analogRead(PIN_SENSOR_LID) < 500)
    {
        statusLidClosed = 0;
    }
    else
    {
        statusLidClosed = 1;
    }

    //power
    //Serial.println(sensorCurrent.getPower_mW());
}

void setupSensors()
{
    sensorAppTemp.begin();
    sensorAppTemp.getAddress(appTemp1, 0);
    sensorAppTemp.setResolution(appTemp1, 12);
    sensorAppTemp.setWaitForConversion(false); //makes it non-blocking (conversion takes about 750ms!)
    sensorCurrent.begin();
}

float getNTCtemp()
{

    //get adc value and sample
    long ADCntc = 0;
    int samples = 10;
    for (int i = 0; i < samples; i++)
    {
        ADCntc += analogRead(PIN_SENSOR_TEMP_HEATER);
        delayMicroseconds(100);
    }
    ADCntc = ADCntc / samples;

    //calculate voltage and resistance
    float Untc = (float)ADCntc * 5.0 / 1023;    //* 1000 -> mV
    float Rntc = Untc * Rseries / (5.0 - Untc); // Ohm

    //caluclate temperature
    float Tntc = SH_a + SH_b * log(Rntc) + SH_c * pow(log(Rntc), 3);
    Tntc = 1 / Tntc - 273.15;
    return Tntc;
}