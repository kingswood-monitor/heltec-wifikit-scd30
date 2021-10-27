#ifndef kwSCD30_H
#define kwSCD30_H

#include "SparkFun_SCD30_Arduino_Library.h"

typedef struct {
  float temperature;
  uint8_t humidity;
  uint16_t co2;
} __attribute__((__packed__))payload_t;

class kwSCD30
{
    protected:
        SCD30 m_scd30;
        bool m_hasSCD30 = false;
        payload_t m_payload;
    
    public:
        kwSCD30();

        bool start();
        bool dataAvailable();

        float temperature();
        uint8_t humidity();
        uint16_t co2();

        void temperature(char *pBuffer);
        void humidity(char *pBuffer);
        void co2(char *pBuffer);

        void setTemperatureOffset(float offset);
};

#endif