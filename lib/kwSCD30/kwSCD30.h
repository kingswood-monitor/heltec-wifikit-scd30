/**
 * @file kwSCD30.h
 * @version 1.0
 *
 * Copyright (C) 2021, Richard Lyon
 */

#ifndef kwSCD30_H
#define kwSCD30_H

#include "SparkFun_SCD30_Arduino_Library.h"

/**
 * The payload structure
 */
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
        bool dataAvilable();

        float temperature();
        uint8_t humidity();
        uint16_t co2();

        void setTemperatureOffset(float offset);
};

#endif