#pragma once

#include <HardwareSerial.h>

// Assuming you have the HardwareSerial object initialized for your UART readings
extern HardwareSerial sensorSerial;

class AnalogSourceInput
{
protected:
    int             m_refreshRate = 2; //refreshes per second
    unsigned long   m_lastReadValueTick = -5000000;
    int             m_refreshRate_b = 1; //refreshes per second
    unsigned long   m_lastReadValueTick_b = -5000000;
    uint16_t        m_lastReadValue;
    uint16_t        m_lastReadValue_battery;

public:
    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;
};

class UARTAnalogSourceInput : public AnalogSourceInput
{
public:
    UARTAnalogSourceInput() {}

    ~UARTAnalogSourceInput() = default;

    uint16_t getMiliVolts()
    {
        unsigned long now = millis();

        if (now - m_lastReadValueTick > 1000 / m_refreshRate)
        {
            m_lastReadValueTick = now;
            uint16_t o2Value = readUARTValue(2); // Read CO value from UART
            m_lastReadValue = o2Value/10.0;
            Serial.println(m_lastReadValue);
        }
        return m_lastReadValue;
    }

    uint16_t getMiliVolts_battery()
    {
        unsigned long now = millis();

        if (now - m_lastReadValueTick_b > 5000 / m_refreshRate_b)
        {
            m_lastReadValueTick_b = now;
            uint16_t batteryValue = readUARTValue(1); // Read battery value from UART
            m_lastReadValue_battery = batteryValue;
            Serial.println(batteryValue);

        }
        return m_lastReadValue_battery;
    }

private:
    uint16_t readUARTValue(uint8_t dataPosition)
    {
        uint8_t rawData[11];
        int length;

        // Attempt to read the correct length of data up to a maximum of 5 retries
        for (int retries = 0; retries < 2; ++retries) {
            length = sensorSerial.readBytes(rawData, sizeof(rawData));

            if (length == 11) {
                break;
            } else if (length == 0) {
                //Serial.println("Data length is zero, retrying...");
                delay(10); // Short delay before retrying
            } else {
                //Serial.println("Data length mismatch");
                return 0;
            }
        }

        Serial.print("Read length: ");
        Serial.println(length);
        Serial.print("Raw Data: ");
        for (int i = 0; i < length; i++) {
            //Serial.print(rawData[i], HEX);
            //Serial.print(" ");
        }
        Serial.println();

        if (length == 11)
        {
            uint8_t checksum = 0;
            for (int i = 1; i < 10; i++)
            {
                checksum += rawData[i];
            }
            checksum = (~checksum) + 1;

            if (checksum == rawData[10])
            {
                uint16_t value = (rawData[dataPosition * 2 + 2] << 8) | rawData[dataPosition * 2 + 3];
                return value;
            }
            else
            {
                //Serial.println("Checksum invalid");
            }
        }
        return 0;
    }
};
