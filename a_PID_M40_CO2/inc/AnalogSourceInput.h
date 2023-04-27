#pragma once

#include <Adafruit_ADS1015.h>

class AnalogSourceInput
{
protected:

    int 			m_refreshRate = 2; //refreshes per second
    unsigned long 	m_lastReadValueTick = -5000000;
    uint16_t 		m_lastReadValue;
    uint16_t 		m_lastReadValue_battery;



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
            const int numReadings  = 5;
            long total  = 0;
            m_lastReadValueTick = now;
            const float multiplier = 0.125F; //GAIN 1

            for (int i = 0; i<numReadings; i++) {
                total = total +m_ads1115->readADC_SingleEnded(0) * multiplier;
            }
            m_lastReadValue = total / numReadings;
        }
        return m_lastReadValue;
    }

    uint16_t getMiliVolts_battery()
    {

        const float multiplier = 0.125F; //GAIN 1

        m_lastReadValue_battery = m_ads1115->readADC_SingleEnded(1) * multiplier;// / 1000.0;

        return m_lastReadValue_battery;
    }



};