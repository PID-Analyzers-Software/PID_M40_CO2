#pragma once

#include <Adafruit_ADS1015.h>

class AnalogSourceInput
{
protected:

    int 			m_refreshRate = 40; //refreshes per second
    unsigned long 	m_lastReadValueTick = -5000000;
    int 			m_refreshRate_b = 1; //refreshes per second
    unsigned long 	m_lastReadValueTick_b = -5000000;
    uint16_t 		m_lastReadValue;
    uint16_t 		m_lastReadValue_battery;

    const int numReadings  = 20;
    long total  = 0;
    int readCount = 0;

public:

    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;

};

class ADS1115AnalogSourceInput : public AnalogSourceInput
{


    Adafruit_ADS1115* m_ads1115;



public:

    ADS1115AnalogSourceInput(Adafruit_ADS1115* ads1115) : m_ads1115(ads1115)
    {

    }
    ~ADS1115AnalogSourceInput()=default;


    uint16_t getMiliVolts()
    {
        unsigned long now = millis();

        if(now - m_lastReadValueTick > 1000 / m_refreshRate)
        {
            m_lastReadValueTick = now;
            const float multiplier = 0.125F; //GAIN 1
            readCount += 1;
            total = total +m_ads1115->readADC_SingleEnded(1) * multiplier;
            if(readCount == numReadings){
                m_lastReadValue = total / numReadings;
                readCount = 0;
                total = 0;
            }
        }
        return m_lastReadValue;
    }

    uint16_t getMiliVolts_battery()
    {
        unsigned long now = millis();

        if(now - m_lastReadValueTick_b > 5000 / m_refreshRate_b) {
            const int numReadings  = 2;
            long total  = 0;
            m_lastReadValueTick_b = now;
            const float multiplier = 0.125F; //GAIN 1

            for (int i = 0; i<numReadings; i++) {
                total = total +m_ads1115->readADC_SingleEnded(3) * multiplier;
            }
            m_lastReadValue_battery = total / numReadings;

        }
        return m_lastReadValue_battery;
    }



};