#pragma once

#include <Adafruit_ADS1015.h>
#include <Adafruit_INA219.h>

class AnalogSourceInput
{
protected:
    int m_refreshRate = 2; // refreshes per second for ADS1115
    unsigned long m_lastReadValueTick = -5000000;
    int m_refreshRate_b = 1; // refreshes per second for INA219
    unsigned long m_lastReadValueTick_b = -5000000;
    uint16_t m_lastReadValue;
    uint16_t m_lastReadValue_battery;

public:
    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;
};

class ADS1115AnalogSourceInput : public AnalogSourceInput
{
    Adafruit_ADS1115* m_ads1115;
    Adafruit_INA219* m_ina219; // Pointer to INA219 object

public:
    ADS1115AnalogSourceInput(Adafruit_ADS1115* ads1115, Adafruit_INA219* ina219)
            : m_ads1115(ads1115), m_ina219(ina219)
    {
    }
    ~ADS1115AnalogSourceInput() = default;

    uint16_t getMiliVolts()
    {
        unsigned long now = millis();

        if (now - m_lastReadValueTick > 1000 / m_refreshRate)
        {
            const int numReadings = 4; // Change number of readings to 4 for moving average
            long total = 0;
            m_lastReadValueTick = now;
            const float multiplier = 0.125F; // GAIN 1

            for (int i = 0; i < numReadings; i++) {
                total += m_ads1115->readADC_SingleEnded(0) * multiplier;
            }
            m_lastReadValue = total / numReadings; // Average the total by the number of readings
        }
        return m_lastReadValue;
    }

    uint16_t getMiliVolts_battery()
    {
        unsigned long now = millis();

        if (now - m_lastReadValueTick_b > 1000 / m_refreshRate_b)
        {
            m_lastReadValueTick_b = now;

            // Read battery voltage from INA219
            float busVoltage = m_ina219->getBusVoltage_V(); // Bus voltage in Volts
            m_lastReadValue_battery = static_cast<uint16_t>(busVoltage * 1000); // Convert to mV
        }
        return m_lastReadValue_battery;
    }
};
