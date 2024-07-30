// AnalogSourceInput.h
#pragma once
#include <HardwareSerial.h>

// External declaration of the HardwareSerial object for UART readings
extern HardwareSerial sensorSerial;

// Base class for analog source input
class AnalogSourceInput {
protected:
    int m_refreshRate = 1; // Refreshes per second
    unsigned long m_lastReadValueTick = -5000000; // Last read value timestamp for O2
    int m_refreshRate_b = 1; // Refreshes per second for battery
    unsigned long m_lastReadValueTick_b = -5000000; // Last read value timestamp for battery
    uint16_t m_lastReadValue; // Last read O2 value
    uint16_t m_lastReadValue_battery; // Last read battery value
    int m_refreshRate_co = 1; // Refreshes per second for CO
    unsigned long m_lastReadValueTick_co = -5000000; // Last read value timestamp for CO
    uint16_t m_lastReadValue_co; // Last read CO value
    int m_refreshRate_h2s = 1; // Refreshes per second for H2S
    unsigned long m_lastReadValueTick_h2s = -5000000; // Last read value timestamp for H2S
    uint16_t m_lastReadValue_h2s; // Last read H2S value

public:
    // Pure virtual functions for getting millivolt values
    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;
    // Virtual functions for gas values with default implementation returning 0
    virtual uint16_t getCOValue() = 0;
    virtual uint16_t getH2SValue() = 0;
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
        if (now - m_lastReadValueTick > 4000) {
            m_lastReadValueTick = now;
            uint16_t o2Value = readUARTValue(2); // Read O2 value from UART
            m_lastReadValue = o2Value / 1.0;
            Serial.println("O2 Value: " + String(m_lastReadValue / 10.0, 1) + " %VOL");
        }
        return m_lastReadValue;
    }

    // Override function to get battery (CH4) value in millivolts
    uint16_t getMiliVolts_battery() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick_b > 4000) {
            m_lastReadValueTick_b = now;
            uint16_t batteryValue = readUARTValue(3); // Read battery value from UART
            m_lastReadValue_battery = batteryValue;
            Serial.println("Battery (CH4) Value: " + String(batteryValue / 10.0, 1) + " %LEL");
        }
        return m_lastReadValue_battery;
    }

    // Override function to get CO value
    uint16_t getCOValue() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick_co > 4000) {
            m_lastReadValueTick_co = now;
            uint16_t coValue = readUARTValue(0); // Read CO value from UART
            m_lastReadValue_co = coValue;
            Serial.println("CO Value: " + String(coValue / 10.0, 1) + " ppm");
        }
        return m_lastReadValue_co;
    }

    // Override function to get H2S value
    uint16_t getH2SValue() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick_h2s > 4000) {
            m_lastReadValueTick_h2s = now;
            uint16_t h2sValue = readUARTValue(1); // Read H2S value from UART
            m_lastReadValue_h2s = h2sValue;
            Serial.println("H2S Value: " + String(h2sValue / 10.0, 1) + " ppm");
        }
        return m_lastReadValue_h2s;
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
