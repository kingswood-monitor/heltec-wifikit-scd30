#include "kwSCD30.h"

// kwSCD30 constructor
kwSCD30::kwSCD30()
{

}

// Start the sensor with maximum update frequency (2 seconds)
bool kwSCD30::start()
{
    m_hasSCD30 = m_scd30.begin();
    if (m_hasSCD30)
    {
        m_scd30.setAutoSelfCalibration(true);
        m_scd30.setMeasurementInterval(2);
        m_scd30.setTemperatureOffset(0);
    }
    return m_hasSCD30;
}

// Update the readings if available
bool kwSCD30::dataAvailable()
{
    if (m_hasSCD30 && m_scd30.dataAvailable())
    {
        m_payload.temperature = m_scd30.getTemperature();
        m_payload.humidity = (int)m_scd30.getHumidity();
        m_payload.co2 = (int)m_scd30.getCO2();

        return true;
    }
    else{
        return false;
    }
}

// Readings
float kwSCD30::temperature() { return m_payload.temperature; }
uint8_t kwSCD30::humidity() { return m_payload.humidity; }
uint16_t kwSCD30::co2() { return m_payload.co2; }

// Readings saved to a buffer for publishing
void kwSCD30::temperature(char *pBuffer) { sprintf(pBuffer, "%.1f", (int)(m_payload.temperature * 10) / 10.0); }
void kwSCD30::humidity(char *pBuffer) { sprintf(pBuffer, "%d", m_payload.humidity);}
void kwSCD30::co2(char *pBuffer) { sprintf(pBuffer, "%d", m_payload.co2); }

// Set the temperature offset. Positive reduces indicated temperature
void kwSCD30::setTemperatureOffset(float offset)
{
    m_scd30.setTemperatureOffset(offset);
}