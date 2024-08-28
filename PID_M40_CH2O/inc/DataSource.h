#pragma once

#include "AnalogSourceInput.h"

class DataSource
{
    GasManager* m_gasManager;
    AnalogSourceInput* m_analogSourceInput;

public:
    DataSource(GasManager* gasManager, AnalogSourceInput* analogSourceInput)
            : m_gasManager(gasManager), m_analogSourceInput(analogSourceInput)
    {
    }
    ~DataSource() = default;

    uint16_t getRawMiliVolts() const
    {
        return m_analogSourceInput->getCH2OValue();
    }

    uint16_t getRawMiliVolts_battery() const
    {
        return m_analogSourceInput->getCH2OValue();
    }

    double getDoubleValue()
    {
        return m_analogSourceInput->getCH2OValue();

    }

    uint16_t getTemperature() const
    {
        return m_analogSourceInput->getTemperature();
    }

    uint16_t getHumidity() const
    {
        return m_analogSourceInput->getHumidity();
    }
};
