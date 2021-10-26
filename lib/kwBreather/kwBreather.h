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
#define DATA_PIN 15

CRGBPalette256 myPal = CRGBPalette256(CRGB::Green, CRGB::Red);

/**
 * The configuration structure
 */
typedef struct {
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
            else if (data >= m_config.rangeMax) { 
                fraction = 255;
            }
            else {
                fraction = (int)((float)(data - m_config.rangeMin) / (float)(m_config.rangeMax - m_config.rangeMin) * 255.0);
            }

            m_currentHue = ColorFromPalette(myPal, fraction);
            
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
            
            leds[0] = m_currentHue;
            FastLED.setBrightness(value);
            FastLED.show();
        }

        void setMaxBrightess(int brightness)
        {
            m_config.maxBrightness = brightness;
        }
    
    protected:
        
        CRGB leds[NUM_LEDS];
        CRGB m_currentHue = CRGB::Black;
        
        BreatherConfig_t m_config = 
        {
            .maxBrightness = 255,
            .periodSecs = 2,
            .rangeMin = 0,
            .rangeMax = 100
        };
}; 
    
#endif