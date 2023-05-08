#pragma once

#include <vector>

#include <Arduino.h>

#include "ConfigurationManager.h"
#include "Globals.h"
#include <EEPROM.h>
#include "CalvalueSet.h"
#include "AlarmSet.h"


class Gas
{
    String m_name;
    double m_slope;
    double m_intercept;
    double m_secondp;
    double m_thermalConductivity;
public:

    Gas(String name, double thermalConductivity = 1.0) : m_name(name),
                                                         m_thermalConductivity(thermalConductivity)
    {

    }

    String getName() { return m_name; }

    double getThermalConductivity() const 					{ return m_thermalConductivity; }
    void setThermalConductivity(double thermalConductivity) { m_thermalConductivity = thermalConductivity; }

};



class GasManager : public ParamChangeListener
{
    std::vector<Gas> m_gases;

    int m_selectedGas = 0;

    double m_slope;
    double m_intercept;
    double m_secondp;
    double m_intercept2;
    double m_secondp2;
    double m_intercept3;
    double m_secondp3;
    double m_intercept4;
    double m_secondp4;
    double m_zero;
    double m_calgas;


    ConfigurationManager* m_configurationManager;

public:
    GasManager(double slope, double intercept, double secondp,
               double zero, double calgas, double intercept2,double secondp2,
               double intercept3,double secondp3,double intercept4,double secondp4) : m_slope(slope),
                                                                                      m_intercept(intercept),
                                                                                      m_secondp(secondp),
                                                                                      m_intercept2(intercept2),
                                                                                      m_secondp2(secondp2),
                                                                                      m_intercept3(intercept3),
                                                                                      m_secondp3(secondp3),
                                                                                      m_intercept4(intercept4),
                                                                                      m_secondp4(secondp4),
                                                                                      m_zero(zero),
                                                                                      m_calgas(calgas)
    {}

    ~GasManager() {}

    double getSlope() const { return m_slope; }
    double getIntercept() const { return m_intercept; }
    double getSecondp() const { return m_secondp; }
    double getIntercept2() const { return m_intercept2; }
    double getSecondp2() const { return m_secondp2; }
    double getIntercept3() const { return m_intercept3; }
    double getSecondp3() const { return m_secondp3; }
    double getIntercept4() const { return m_intercept4; }
    double getSecondp4() const { return m_secondp4; }
    double getZero() const {return m_zero; }
    double getCalgas() const {return m_calgas; }

    void setSlope(double s) { m_slope = s; }
    void setIntercept(double i) { m_intercept = i; }
    void setSecondp(double p) {m_secondp = p; }
    void setIntercept2(double ii) { m_intercept2 = ii; }
    void setSecondp2(double pp) {m_secondp2 = pp; }
    void setIntercept3(double iii) { m_intercept3 = iii; }
    void setSecondp3(double ppp) {m_secondp3 = ppp; }
    void setIntercept4(double iiii) { m_intercept4 = iiii; }
    void setSecondp4(double pppp) {m_secondp4 = pppp; }
    void setZero(double z) {m_zero = z; }
    void setCalgas(double c) {m_calgas = c;}


    double calculateSLM(double voltage) {
        //double val = (voltage*voltage*m_slope + voltage*m_secondp + m_intercept ) * getSelectedGas().getThermalConductivity();
        double val = 0;
        if(m_selectedGas==0){
            val = ((voltage - m_intercept ) / m_secondp) * 1000 ; //votlage in volts
        }else if(m_selectedGas==1){
            val = ((voltage - m_intercept2 ) / m_secondp2) * 1000 ; //votlage in volts
        }else if(m_selectedGas==2){
            val = ((voltage - m_intercept3 ) / m_secondp3) * 1000 ; //votlage in volts
        }else if(m_selectedGas==3){
            val = ((voltage - m_intercept4 ) / m_secondp4) * 1000 ; //votlage in volts
        }
        return val < 0 ? 0 : val;
    }



    void setConfigurationManager(ConfigurationManager* c) { m_configurationManager = c; }

    void addGas(Gas g) { m_gases.push_back(g); }

    void printinfo(){
        Serial.println("S Button Pushed!!!!!!  ");
    }

    void selectGasByIndex(int index)
    {
        Serial.println("selectGasByIndex");
        if(index >= 0 && index < m_gases.size())
        {
            m_selectedGas = index;
            //m_configurationManager->saveGasSelectedToEEPROM(index);
            Serial.println("gas index saved"+index);
            EEPROM.writeInt(80, index);
            EEPROM.commit();
        }
    }

    void calibrate(double zero){
        m_zero = zero/1000;
        delay(20);
        if(m_selectedGas==0) {
            m_intercept = m_zero;
            Serial.println("calibrating for gas 1");
            EEPROM.writeDouble(12+m_selectedGas*16, m_intercept);
        }else if(m_selectedGas ==1){
            m_intercept2 = m_zero;
            Serial.println("calibrating for gas 2");
            EEPROM.writeDouble(12+m_selectedGas*16, m_intercept2);
        }else if(m_selectedGas ==2){
            m_intercept3 = m_zero;
            Serial.println("calibrating for gas 3");
            EEPROM.writeDouble(12+m_selectedGas*16, m_intercept3);
        }else if(m_selectedGas ==3){
            m_intercept4 = m_zero;
            Serial.println("calibrating for gas 4");
            EEPROM.writeDouble(12+m_selectedGas*16, m_intercept4);
        }

        EEPROM.commit();


    }

    void calibrate2(double cal){
        m_calgas = cal / 1000;
        int calvalue=Calvalue().getSelectedCalvalue();
        double calgasv = calvalue/1000.0;
        Serial.println("!!!!!!!!!!!!!!!");
        Serial.println(calvalue);
        Serial.println(calgasv);
        m_secondp = (m_calgas - m_zero) / calgasv;
        if(m_selectedGas==0) {
            m_secondp = (m_calgas - m_zero) / calgasv;
            Serial.println("calibrating for gas 1");
            Serial.println(m_secondp);
            EEPROM.writeDouble(20+m_selectedGas*16, m_secondp);
        }else if(m_selectedGas ==1){
            m_secondp2 = (m_calgas - m_zero) / calgasv;
            EEPROM.writeDouble(20+m_selectedGas*16, m_secondp2);
        }else if(m_selectedGas ==2){
            m_secondp3 = (m_calgas - m_zero) / calgasv;
            EEPROM.writeDouble(20+m_selectedGas*16, m_secondp3);
        }else if(m_selectedGas ==3){
            m_secondp4 = (m_calgas - m_zero) / calgasv;
            EEPROM.writeDouble(20+m_selectedGas*16, m_secondp4);
        }
        EEPROM.commit();
    }

    void calibrate3(){

    }

    void selectPreviousGas()
    {
        Serial.println("selectPreviousGas");

        if(m_selectedGas == 0)
            m_selectedGas = m_gases.size() - 1;
        else
            m_selectedGas = m_selectedGas - 1;
    }

    Gas& getSelectedGas() { return m_gases[m_selectedGas]; }


    std::vector<Gas>& getAllGases() { return m_gases; }

    void onParamChange(String param, String value)
    {

        Serial.println("onGasParamChange: " + param + "=" + value);

        if(param.equals(c_SLOPE_PARAM_NAME))
        {
            m_slope = value.toDouble();
            Serial.println("Setting SLOPE: " + value);
        }
        else if(param.equals(c_INTERCEPT_PARAM_NAME))
        {
            m_intercept = value.toDouble();
            Serial.println("Setting INTERCEPT: " + value);
        }
        else if(param.equals(c_SECONDP_PARAM_NAME))
        {
            m_secondp = value.toDouble();
            Serial.println("Setting SECONDP: " + value);
        }
        else if(param.equals(c_INTERCEPT2_PARAM_NAME))
        {
            m_intercept2 = value.toDouble();
            Serial.println("Setting INTERCEPT2: " + value);
        }
        else if(param.equals(c_SECONDP2_PARAM_NAME))
        {
            m_secondp2 = value.toDouble();
            Serial.println("Setting SECONDP2: " + value);
        }
        else if(param.equals(c_INTERCEPT3_PARAM_NAME))
        {
            m_intercept3 = value.toDouble();
            Serial.println("Setting INTERCEPT3: " + value);
        }
        else if(param.equals(c_SECONDP3_PARAM_NAME))
        {
            m_secondp3 = value.toDouble();
            Serial.println("Setting SECONDP3: " + value);
        }
        else if(param.equals(c_INTERCEPT4_PARAM_NAME))
        {
            m_intercept4 = value.toDouble();
            Serial.println("Setting INTERCEPT4: " + value);
        }
        else if(param.equals(c_SECONDP4_PARAM_NAME))
        {
            m_secondp4 = value.toDouble();
            Serial.println("Setting SECONDP4: " + value);
        }
        else if(param.equals(c_GASINDEX_PARAM_NAME))
        {
            m_selectedGas = value.toDouble();
            Serial.println("Setting Gas_Index: " + value);
        }
        else
        {
            for (auto& gas : m_gases)
            {
                if (gas.getName().equals(param))
                {
                    double tc = value.toDouble();
                    gas.setThermalConductivity(tc);
                    Serial.println("Setting TC for gas: " + gas.getName() + " " + String(tc));
                    break;
                }
            }
        }
    }


};

