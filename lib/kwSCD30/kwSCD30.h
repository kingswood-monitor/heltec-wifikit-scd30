/**
 * @file mySCD30.h
 * @version 1.0
 *
 * Copyright (C) 2021, Richard Lyon
 */

#ifndef SCD30_H
#define SCD30_H

#include "SparkFun_SCD30_Arduino_Library.h"
#include <Wire.h>

/**
 * The payload structure
 */
typedef struct {
  float temperature;
  uint8_t humidity;
  uint16_t co2;
} __attribute__((__packed__))payload_t;

/**
 * The SCD30 class is a wrapper round the SCD30 sensor to simplify access and provide
 * data in a payload buffer suitable for transmission over LoRa.
 */
class kwSCD30 {
    public:
        // Construct the SCD30
        kwSCD30(){}

        /**
         * Start the device.
         * @return true if present and started.
         */

        bool start()
        {
            // m_i2cPort = &wirePort;

            int delay = 1000;
            for (int i=0; i<delay; i++) {
                if (m_scd30.begin(Wire)) {
                m_hasSCD30 = true;
                break;
                }
            }
            if (!m_hasSCD30) return false;

            m_scd30.setAltitudeCompensation(50);

            return true;
        }

        /**
         * Returns true if the SCD30 is present and has started, otherwise false.
         * @return bool.
         */
        bool started()
        {
            return m_hasSCD30;
        }

        /**
         * Returns true if the SCD is present and has data available, otherwise false.
         * If data is available, update the payload with new readings.
         * @return bool.
         */
        bool hasData()
        {
            if (!m_hasSCD30)
            { 
                return false;
            }

            if (m_scd30.dataAvailable())
            {
                m_payload.temperature = m_scd30.getTemperature();
                m_payload.humidity = (int)m_scd30.getHumidity();
                m_payload.co2 = (int)m_scd30.getCO2();

                return true;
            }
            
            else {
                return false;
            }
        }

        /**
         * Returns a structure containing the current readings.
         * @return payload_t
         */
        payload_t payload()
        {
            return m_payload;
        }

        /**
         * Updates a supplied buffer with the current readings payload.
         */
        void buffer(uint8_t *pBuffer)
        {
            size_t uBuffSize = sizeof(payload_t);
            memcpy(pBuffer, &m_payload, uBuffSize);
        }

        /**
         * Returns the size of the payload.
         * @return size_t
         */
        size_t buffSize()
        {
            return sizeof(payload_t);
        }

    protected:
        TwoWire *m_i2cPort;
        SCD30 m_scd30;
        bool m_hasSCD30 = false;
        payload_t m_payload = { .temperature = -999.9, .humidity = 0, .co2 = 0 };
};

#endif