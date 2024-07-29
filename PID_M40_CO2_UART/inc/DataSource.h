// DataSource.h
#pragma once

#include "AnalogSourceInput.h"

class DataSource {
    GasManager* m_gasManager;
    AnalogSourceInput* m_analogSourceInput;

public:
    DataSource(GasManager* gasManager, AnalogSourceInput* analogSourceInput) : m_gasManager(gasManager),
                                                                               m_analogSourceInput(analogSourceInput) {}

    ~DataSource() = default;

    uint16_t getRawMiliVolts() const {
        return m_analogSourceInput->getMiliVolts();
    }

    uint16_t getRawMiliVolts_battery() const {
        return m_analogSourceInput->getMiliVolts_battery();
    }

    double getDoubleValue() {
        uint16_t miliVolts = m_analogSourceInput->getMiliVolts();
        return m_gasManager->calculateSLM(miliVolts / 1000.0);
    }

    uint16_t getCOValue() const {
        return m_analogSourceInput->getCOValue();
    }

    uint16_t getH2SValue() const {
        return m_analogSourceInput->getH2SValue();
    }
};
