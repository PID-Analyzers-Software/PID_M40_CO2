#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>

class Logtime
{
    std::array<int,6> m_Array{{1,5,10,20,30,999}};
    int m_selectedLogtime = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;
    
public:

    Logtime()
    {
        m_selectedLogtime = 0;
    }

    ~Logtime()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectLogtimeByIndex(int index)
    {
        if(index >= 0 && index < m_Array.size())
        {
            m_selectedLogtime = index;
            //EEPROM.writeInt(220, index);
            EEPROM.commit();
            Serial.print("Log Duration saved");
            Serial.println(index);
        }

        return;
    }



    void selectNextLogtime()
    {
        m_selectedLogtime = (m_selectedLogtime + 1) % m_Array.size();
    }

    void selectPreviousHour()
    {
        if(m_selectedLogtime == 0)
            m_selectedLogtime = m_Array.size() - 1;
        else
            m_selectedLogtime = m_selectedLogtime - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedLogtime() {
        int index = EEPROM.read(220);
        return m_Array[index];
    }

};
