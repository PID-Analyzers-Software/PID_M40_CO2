#pragma once

#include "AnalogSourceInput.h"

class DataSource {
    GasManager* m_gasManager;
    AnalogSourceInput* m_analogSourceInput;

public:
    DataSource(GasManager* gasManager, AnalogSourceInput* analogSourceInput) : m_gasManager(gasManager),
                                                                               m_analogSourceInput(analogSourceInput) {}

    ~DataSource() = default;

    void readAllValues() const {
        m_analogSourceInput->readAllValues();
    }

    uint16_t getO2Value() const {
        return m_analogSourceInput->getO2Value();
    }

    uint16_t getCH4Value() const {
        return m_analogSourceInput->getCH4Value();
    }

    uint16_t getCOValue() const {
        return m_analogSourceInput->getCOValue();
    }

    uint16_t getH2SValue() const {
        return m_analogSourceInput->getH2SValue();
    }

    double getDoubleValue() {
        uint16_t o2Value = m_analogSourceInput->getO2Value();
        return m_gasManager->calculateSLM(o2Value / 1000.0);
    }
};
