#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>

class Alarm
{
    std::array<int,3> m_alarmArray{{300,500,0}};
    int m_selectedAlarm = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    //U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI* m_u8g2;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;


public:

    Alarm()
    {
        m_selectedAlarm = 0;
    }

    ~Alarm()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectAlarmByValueNoEEPROMSave(int alarm)
    {
        for(int i=0; i < m_alarmArray.size(); i++)
            if(m_alarmArray[i] == alarm)
            {
                m_selectedAlarm = i;
                break;
            }

        return;
    }

    void selectAlarmByIndex(int index)
    {
        if(index >= 0 && index < m_alarmArray.size())
        {
            m_selectedAlarm = index;
            m_configurationManager->saveAlarmToEEPROM(index);
        }

        return;
    }

    void selectAlarmByValue(int alarm)
    {
        for(int i=0; i < m_alarmArray.size(); i++)
            if(m_alarmArray[i] == alarm)
            {
                m_selectedAlarm = i;
                m_configurationManager->saveAlarmToEEPROM(i);
                break;
            }

        return;
    }

    void selectNextAlarm()
    {
        m_selectedAlarm = (m_selectedAlarm + 1) % m_alarmArray.size();
    }

    void selectPreviousAlarm()
    {
        if(m_selectedAlarm == 0)
            m_selectedAlarm = m_alarmArray.size() - 1;
        else
            m_selectedAlarm = m_selectedAlarm - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedAlarm() { return m_alarmArray[m_selectedAlarm]; }

};
