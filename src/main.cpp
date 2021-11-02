#include <Arduino.h>
#include <kwHeltecWifikit32.h>
#include <kwNeoTimer.h>
#include <kwSCD30.h>
#include <secrets.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "2.1.0"
#define TOPIC_ROOT "kw_sensors"
#define TEMPERATURE_OFFSET 1.3

uint8_t temperatureField;
uint8_t humidityField;
uint8_t co2Field;
uint8_t temperatureOffsetCommandField;

struct HeltecConfig config = 
{
  .ssid = WIFI_SSID,
  .pwd = WIFI_PASSWORD,
  .mqtt_host = IPAddress(192, 168, 1, 240), // Mac Mini M1
  .rotateDisplay = true,
  .firmwareVersion = FIRMWARE_VERSION,
  .topicRoot = TOPIC_ROOT
};

kwHeltecWifikit32 heltec{ config };
kwSCD30 scd30;
kwNeoTimer publishDataTimer = kwNeoTimer();

void setup() 
{
    Serial.begin(115200);
    
    temperatureField = heltec.registerField("Temp", "degC", "temperature", "SCD30");
    humidityField = heltec.registerField("Hum", "%", "humidity", "SCD30");
    co2Field = heltec.registerField("CO2", "ppm", "co2", "SCD30");
    
    heltec.init();
    scd30.start( TEMPERATURE_OFFSET );

    Serial.printf("\n------------------%s sensor------------------------------------------------\n\n", SENSOR_TYPE);
    Serial.printf("%-12s : %s\n", "Firmware", FIRMWARE_VERSION);
    Serial.printf("%-12s : %s\n", "Device ID", heltec.deviceID);
    Serial.printf("%-12s : %s\n", "SCD30", scd30.hasSCD30() ? "OK" : "Not found");
    Serial.printf("%-12s : %.1f degC\n", "Offset", scd30.temperatureOffset());
    Serial.printf("%-12s : %s\n", "Status", heltec.metaTopics[heltec.statusTopicID].c_str());
    for (int i=0; i < heltec.dataTopics.size(); i++)
    {
      Serial.printf("%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(), heltec.dataTopics[i].topicString.c_str());
    }
    publishDataTimer.set(1000);
  }

void loop() 
{  
  if (publishDataTimer.repeat() && scd30.dataAvailable())
  {
      heltec.publish(temperatureField, scd30.temperature());
      heltec.publish(humidityField, scd30.humidity());
      heltec.publish(co2Field, scd30.co2());

      heltec.update(temperatureField, scd30.temperature());
      heltec.update(humidityField, scd30.humidity());
      heltec.update(co2Field, scd30.co2());
  }
  heltec.run();
}
