#ifndef H_KWLED
#define H_KWLED

#include <Arduino.h>

typedef enum {
    LED_ON = 0,
    LED_OFF,
    LED_TOGGLE,
    LED_FLASH_ONCE,
    LED_FLASH_SLOW,
    LED_FLASH_FAST
} led_id_t;


class kwLED
{
public:
    kwLED(int pin);
    void command(led_id_t command);
    void Callback();
    
    size_t getID();

protected:
    int m_pin;
    bool m_state = LOW;
    
    TimerHandle_t handle;
    static void callback(TimerHandle_t _handle);
    size_t id;

};


// kwLED constructor
kwLED::kwLED(int pin)
{
    m_pin = pin;
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);

    this->handle = xTimerCreate(
        "ledTimer",
        pdMS_TO_TICKS(200),
        pdFALSE,
        this,
        callback);
}

void kwLED::callback(TimerHandle_t _handle)
{
    kwLED* p = static_cast<kwLED*>(pvTimerGetTimerID(_handle));
    p->Callback();
}

void kwLED::Callback()
{

}

void kwLED::command(led_id_t command)
{
    switch(command) {
        
        case LED_ON:
            digitalWrite(m_pin, HIGH);
            m_state = HIGH;
            break;
        
        case LED_OFF:
            digitalWrite(m_pin, LOW);
            m_state = LOW;
            break;
        
        case LED_TOGGLE:
            if (m_state == LOW)
            {
                digitalWrite(m_pin, HIGH);
                m_state = HIGH;
            }
            else{
                digitalWrite(m_pin, LOW);
                m_state = LOW;
            }
            break;
        
        case LED_FLASH_ONCE:
            digitalWrite(m_pin, HIGH);
            m_state = HIGH;

    }
}

size_t kwLED::getID()
{
    return this->id;
}

#endif
