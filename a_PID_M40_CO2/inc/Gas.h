#pragma once

class Gas
{
	String m_name;	
	double m_slope;
	double m_intercept;
	double m_secondp;
	
public:
	
	void setSlope(double s) { m_slope = s; }
	void setIntercept(double i) { m_intercept = i; }
    void setSecondp(double p) { m_secondp = p; }

    double calculateSLM(int voltage) {
		return (voltage + m_intercept ) / m_secondp;
    }
};