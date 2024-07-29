// AnalogSourceInput.h
#pragma once
#include <HardwareSerial.h>

// External declaration of the HardwareSerial object for UART readings
extern HardwareSerial sensorSerial;

// Base class for analog source input
class AnalogSourceInput {
protected:
    int m_refreshRate = 2; // Refreshes per second
    unsigned long m_lastReadValueTick = -5000000; // Last read value timestamp for O2
    int m_refreshRate_b = 1; // Refreshes per second for battery
    unsigned long m_lastReadValueTick_b = -5000000; // Last read value timestamp for battery
    uint16_t m_lastReadValue; // Last read O2 value
    uint16_t m_lastReadValue_battery; // Last read battery value

public:
    // Pure virtual functions for getting millivolt values
    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;
    // Virtual functions for gas values with default implementation returning 0
    virtual uint16_t getCOValue() { return 0; }
    virtual uint16_t getH2SValue() { return 0; }
    virtual ~AnalogSourceInput() = default;
};

// Derived class for UART analog source input
class UARTAnalogSourceInput : public AnalogSourceInput {
public:
    UARTAnalogSourceInput() = default;
    ~UARTAnalogSourceInput() = default;

    // Override function to get O2 value in millivolts
    uint16_t getMiliVolts() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick > 1000 / m_refreshRate) {
            m_lastReadValueTick = now;
            uint16_t o2Value = readUARTValue(2); // Read O2 value from UART
            m_lastReadValue = o2Value / 10.0;
            Serial.println("O2 Value: " + String(m_lastReadValue));
        }
        return m_lastReadValue;
    }

    // Override function to get battery (CH4) value in millivolts
    uint16_t getMiliVolts_battery() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick_b > 5000 / m_refreshRate_b) {
            m_lastReadValueTick_b = now;
            uint16_t batteryValue = readUARTValue(3); // Read battery value from UART
            m_lastReadValue_battery = batteryValue;
            Serial.println("Battery (CH4) Value: " + String(batteryValue));
        }
        return m_lastReadValue_battery;
    }

    // Override function to get CO value
    uint16_t getCOValue() override {
        uint16_t coValue = readUARTValue(0); // Read CO value from UART
        Serial.println("CO Value: " + String(coValue));
        return coValue;
    }

    // Override function to get H2S value
    uint16_t getH2SValue() override {
        uint16_t h2sValue = readUARTValue(1); // Read H2S value from UART
        Serial.println("H2S Value: " + String(h2sValue));
        return h2sValue;
    }

private:
    // Function to read UART value based on data position
    uint16_t readUARTValue(uint8_t dataPosition) {
        uint8_t rawData[11];
        int length;

        // Attempt to read the correct length of data up to a maximum of 2 retries
        for (int retries = 0; retries < 2; ++retries) {
            length = sensorSerial.readBytes(rawData, sizeof(rawData));
            if (length == 11) {
                break;
            } else if (length == 0) {
                delay(10); // Short delay before retrying
            } else {
                return 0; // Data length mismatch
            }
        }

        if (length == 11) {
            uint8_t checksum = 0;
            for (int i = 1; i < 10; i++) {
                checksum += rawData[i];
            }
            checksum = (~checksum) + 1;

            if (checksum == rawData[10]) {
                uint16_t value = (rawData[dataPosition * 2 + 2] << 8) | rawData[dataPosition * 2 + 3];
                return value;
            }
        }
        return 0; // Checksum invalid
    }
};
