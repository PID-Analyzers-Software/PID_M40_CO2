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
        if (now - m_lastReadValueTick > 2000) {  // Adjust timing as necessary
            m_lastReadValueTick = now;

            uint8_t buffer[9];
            int length = 0;

            // Attempt to read data until we get a valid start byte
            while (true) {
                if (m_uart->available() >= 1) {
                    uint8_t byte = m_uart->read();

                    // Start byte found, read the rest of the data
                    if (byte == 0xFF) {
                        buffer[0] = byte;
                        length = m_uart->readBytes(buffer + 1, 8); // Read the remaining bytes

                        // Ensure the data is complete
                        if (length == 8) {
                            break;
                        }
                    }
                }
                delay(10); // Short delay to avoid busy-waiting
            }

            if (length == 8 && buffer[1] == 0x17) {  // Check the command byte if necessary
                uint8_t checksum = calculateChecksum(buffer, 9);

                if (checksum == buffer[8]) {  // Verify checksum
                    // Extract CH2O concentration from buffer[2] and buffer[3]
                    m_lastReadValue = (buffer[2] << 8) | buffer[3];
                    Serial.print("CH2O Value: " + String(m_lastReadValue) + " ppb");
                } else {
                    Serial.println("Checksum mismatch");
                }
            } else {
                Serial.println("Data not complete or start byte mismatch");
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
