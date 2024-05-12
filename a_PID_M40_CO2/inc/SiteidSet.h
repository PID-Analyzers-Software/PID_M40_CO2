#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>

class Siteid
{
    std::array<int,9> m_Array{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
    int m_selectedSiteid = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;



public:

    Siteid()
    {
        m_selectedSiteid = 0;
    }

    ~Siteid()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectSiteidByIndex(int index)
    {
        if(index >= 0 && index < m_Array.size())
        {
            m_selectedSiteid = index;
            //EEPROM.writeInt(220, index);
            EEPROM.commit();
            Serial.print("Site ID saved");
            Serial.println(index);
        }

        return;
    }



    void selectNextSiteid()
    {
        m_selectedSiteid = (m_selectedSiteid + 1) % m_Array.size();
    }

    void selectPreviousHour()
    {
        if(m_selectedSiteid == 0)
            m_selectedSiteid = m_Array.size() - 1;
        else
            m_selectedSiteid = m_selectedSiteid - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedSiteid() {
        int index = EEPROM.read(220);
        return m_Array[index];
    }

};
