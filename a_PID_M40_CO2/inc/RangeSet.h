#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>

class Range
{
    std::array<int,2> m_rangeArray{{1000,5000}};
    int m_selectedRange = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    //U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI* m_u8g2;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;


public:

    Range()
    {
        m_selectedRange = 0;
    }

    ~Range()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectRangeByValueNoEEPROMSave(int range)
    {
        for(int i=0; i < m_rangeArray.size(); i++)
            if(m_rangeArray[i] == range)
            {
                m_selectedRange = i;
                break;
            }

        return;
    }

    void selectRangeByIndex(int index)
    {
        if(index >= 0 && index < m_rangeArray.size())
        {
            m_selectedRange = index;
            m_configurationManager->saveRangeToEEPROM(index);
        }

        return;
    }

    void selectRangeByValue(int range)
    {
        for(int i=0; i < m_rangeArray.size(); i++)
            if(m_rangeArray[i] == range)
            {
                m_selectedRange = i;
                m_configurationManager->saveRangeToEEPROM(i);
                break;
            }

        return;
    }

    void selectNextRange()
    {
        m_selectedRange = (m_selectedRange + 1) % m_rangeArray.size();
    }

    void selectPreviousRange()
    {
        if(m_selectedRange == 0)
            m_selectedRange = m_rangeArray.size() - 1;
        else
            m_selectedRange = m_selectedRange - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedRange() { m_rangeArray[m_selectedRange]; }

};
