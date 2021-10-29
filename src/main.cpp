#include <Arduino.h>
#include <kwTimer.h>
#include <kwHeltecWifikit32.h>
#include <secrets.h>
#include <kwSCD30.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "2.0.0"

// OLED pins
#define PIN_RST 16
#define PIN_SDA 4
#define PIN_SCL 15

// Network credentials
// #define WIFI_SSID xxx
// #define WIFI_PASSWORD xxx
#define MQTT_HOST IPAddress(192, 168, 1, 240) // Mac Mini M1
#define MQTT_PORT 1883
#define TOPIC_ROOT "kw_sensors"

// MQTT topics
// char topicDataBoilerStateLED[MAX_TOPIC_BUFFER_LEN];
// char topicDataBoilerCumulativeLED[MAX_TOPIC_BUFFER_LEN];
// char topicCommandResetTimer[MAX_TOPIC_BUFFER_LEN];

uint8_t temperatureField;
uint8_t humidityField;
uint8_t co2Field;

kwHeltecWifikit32 heltec;
kwSCD30 scd30;
kwTimer publishDataTimer = kwTimer();

void setup() 
{
    Serial.begin(115200);
    heltec.initDisplay(PIN_RST, PIN_SDA, PIN_SCL, true);

    temperatureField = heltec.registerDataTopic("temperature", "degC", "temperature", "SCD30");
    humidityField = heltec.registerDataTopic("humidity", "%", "humidity", "SCD30");
    co2Field = heltec.registerDataTopic("CO2", "ppm", "co2", "SCD30");
    
    Serial.printf("\n------------------%s sensor------------------------------------------------\n\n", SENSOR_TYPE);
    Serial.printf("%-12s : %s\n", "Firmware", FIRMWARE_VERSION);
    Serial.printf("%-12s : %s\n", "Device ID", heltec.deviceID);
    for (int i=0; i < heltec.dataTopics.size(); i++)
    {
      Serial.printf("%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(), heltec.dataTopics[i].topicString.c_str());
    }
    Serial.printf("%-12s : %s\n", "status", heltec.metaTopics[heltec.statusTopicID].c_str());
    
    heltec.initWiFi(WIFI_SSID, WIFI_PASSWORD);
    heltec.initMTTQ(MQTT_HOST, TOPIC_ROOT);

    scd30.start();
    
    publishDataTimer.set(1000);
  }

void loop() 
{  
  if (publishDataTimer.repeat() && scd30.dataAvailable())
  {
      heltec.publish(temperatureField, scd30.temperature());
      heltec.publish(humidityField, scd30.humidity());
      heltec.publish(co2Field, scd30.co2());
  }
  
  heltec.run();
}
