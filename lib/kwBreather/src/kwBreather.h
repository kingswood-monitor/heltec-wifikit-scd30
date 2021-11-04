/**
 * Breather.h
 * Copyright (c) 2021 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

/**
 * Defines an LED display to represent the value of a parameter as a hue that varies
 * in brightness with a 'breathing' pattern.
 */

#ifndef KWBREATHER_H
#define KWBREATHER_H

#include <stdint.h>
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 12

/**
 * The configuration structure
 */
typedef struct {
    uint8_t hueMin;
    uint8_t hueMax;
    uint8_t maxBrightness;
    uint8_t periodSecs;
    uint16_t rangeMin;
    uint16_t rangeMax;
} BreatherConfig_t;

/**
 * The kwBreather class defines a display comprising an LED that represents a value as a colour, 
 * and an animated "breathing" illumination feature.
 */
class kwBreather
{
    public:
        /**
         * Construct a breather with the given configuratio data.
         * @param config configration data.
         */
        kwBreather(BreatherConfig_t config) : m_config(config)
        {
            FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
        }
        
        /**
         * Start the breather
         */
        void start()
        {
            leds[0].setHSV(0, 255, 128);
            FastLED.show();
        }

        /**
         * Update the pixel hue between two endpoints in proportion to the fraction. 
         * Note: this runs "backward" i.e. from the higher hue to the lower
         * @param data the data to update
         */
        void update(uint16_t data)
        {
            uint8_t fraction = 0;
            
            if (data < m_config.rangeMin) { 
                fraction = 0; 
            }
            else if (data > m_config.rangeMax) { 
                fraction = 255;
            }
            else {
                fraction = (int)((float)(data - m_config.rangeMin) / (float)(m_config.rangeMax - m_config.rangeMin) * 255.0);
            }
            
            m_currentHue =  map8(fraction, m_config.hueMax, m_config.hueMin);
        }

        /** 
         * Modulate pixel brightness with a breathing function: 
         * see https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
         */
        void breath()
        {
            static uint16_t m = 0;
            uint8_t brightness = (exp(sin(millis() / (m_config.periodSecs * 1000.0) * PI)) - 0.36787944) * 66 + 100;

            if (brightness > m)
            {
                m = brightness;
            }

            uint8_t value = scale8_video(brightness, m_config.maxBrightness);
            leds[0].setHSV(m_currentHue, 255, value);
            delay(10);
            FastLED.show();
        }
    
    protected:
        
        uint8_t interpolateHue(uint16_t data);
        uint8_t m_currentHue = 0;
        
        BreatherConfig_t m_config = 
        {
            .hueMin = 0,
            .hueMax = 128,
            .maxBrightness = 255,
            .periodSecs = 2,
            .rangeMin = 0,
            .rangeMax = 100
        };

        CRGB leds[NUM_LEDS];
}; 
    

#endif