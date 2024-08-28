#pragma once

#include <Adafruit_ADS1015.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SHT4x.h>
#include <HardwareSerial.h>  // Include for UART communication

class AnalogSourceInput {
protected:
    int m_refreshRate = 2;  // refreshes per second for ADS1115
    unsigned long m_lastReadValueTick = -5000000;
    int m_refreshRate_b = 1;  // refreshes per second for INA219
    unsigned long m_lastReadValueTick_b = -5000000;
    int m_refreshRate_rht = 1;  // refreshes per second for SHT4x (i.e., every 2 seconds)
    unsigned long m_lastReadValueTick_rht = -5000000;
    uint16_t m_lastReadValue;
    uint16_t m_lastReadValue_battery;
    float m_lastTemperature;
    float m_lastHumidity;

public:
    virtual uint16_t getCH2OValue() = 0;  // Method to get CH2O concentration
    virtual float getTemperature() = 0;   // Method to get temperature from SHT4x
    virtual float getHumidity() = 0;      // Method to get humidity from SHT4x
    virtual ~AnalogSourceInput() = default;
};

class UARTAnalogSourceInput : public AnalogSourceInput {
    HardwareSerial* m_uart;
    Adafruit_INA219* m_ina219;
    Adafruit_SHT4x* m_sht4x;

public:
    UARTAnalogSourceInput(HardwareSerial* uart, Adafruit_INA219* ina219, Adafruit_SHT4x* sht4x)
            : m_uart(uart), m_ina219(ina219), m_sht4x(sht4x) {
        m_uart->begin(9600);  // Initialize UART for the Winsen sensor
    }

    ~UARTAnalogSourceInput() = default;

    uint16_t getCH2OValue() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick > 1000) {
            m_lastReadValueTick = millis();
            uint8_t buffer[9];
            int bytesRead = 0;

            // Attempt to read data from the sensor
            while (m_uart->available()) {
                buffer[0] = m_uart->read();
                if (buffer[0] == 0xFF) {  // Check for start byte
                    bytesRead = m_uart->readBytes(buffer + 1, 8);  // Read the rest of the packet
                    if (bytesRead == 8 && buffer[1] == 0x17) {  // Validate the gas type byte (0x17 for CH2O)
                        // Extract CH2O concentration from buffer[4] (High Byte) and buffer[5] (Low Byte)
                        uint16_t concentration = (buffer[4] << 8) | buffer[5];
                        uint8_t checksum = 0xFF - (buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5] + buffer[6] + buffer[7]) + 1;

                        if (checksum == buffer[8]) {  // Validate checksum
                            m_lastReadValue = concentration;
                            Serial.print("CH2O Value (ug/m3): ");
                            Serial.println(m_lastReadValue);
                        } else {
                            Serial.println("Checksum mismatch.");
                        }
                    }
                }
            }
        }
        return m_lastReadValue;
    }
    float getTemperature() override {
        unsigned long now = millis();
        if (now - m_lastReadValueTick_rht > 1000 / m_refreshRate_rht) {
            m_lastReadValueTick_rht = now;
            sensors_event_t humidity, temp;
            m_sht4x->getEvent(&humidity, &temp);
            m_lastTemperature = temp.temperature;
        }
        return m_lastTemperature;
    }

    float getHumidity() override {
        getTemperature();  // Temperature and humidity are read together
        return m_lastHumidity;
    }

private:
    uint8_t calculateChecksum(uint8_t* buffer, size_t length) {
        uint8_t checksum = 0;
        for (size_t i = 0; i < length - 1; ++i) {
            checksum += buffer[i];
        }
        return ~checksum + 1;
    }
};
