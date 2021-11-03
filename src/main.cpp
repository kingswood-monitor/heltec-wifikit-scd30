#include <Arduino.h>
#include <secrets.h>

#include <BlynkSimpleEsp32.h>
#include <kwHeltecWifikit32.h>
#include <kwNeoTimer.h>
#include <kwSCD30.h>

#define BLYNK_PRINT        Serial
#define SENSOR_TYPE        "energy"
#define FIRMWARE_VERSION   "2.2.0"
#define TOPIC_ROOT         "kw_sensors"
#define TEMPERATURE_OFFSET 2.5

uint8_t temperatureField;
uint8_t humidityField;
uint8_t co2Field;

struct HeltecConfig config = {
    .ssid = WIFI_SSID,
    .pwd = WIFI_PASSWORD,
    .mqtt_host = IPAddress( 192, 168, 1, 240 ),  // Mac Mini M1
    .rotateDisplay = true,
    .firmwareVersion = FIRMWARE_VERSION,
    .topicRoot = TOPIC_ROOT };

kwHeltecWifikit32 heltec{ config };
kwSCD30           scd30;
BlynkTimer        timer;

void printBanner();
void publishEvent();

void setup()
{
  Serial.begin( 115200 );

  temperatureField =
      heltec.registerField( "Temp", "degC", "temperature", "SCD30" );
  humidityField = heltec.registerField( "Hum", "%", "humidity", "SCD30" );
  co2Field = heltec.registerField( "CO2", "ppm", "co2", "SCD30" );

  heltec.init();
  Blynk.begin( BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD );
  scd30.start( TEMPERATURE_OFFSET );

  timer.setInterval( 1000L, publishEvent );
}

void loop()
{
  heltec.run();
  Blynk.run();
  timer.run();
}

BLYNK_CONNECTED() { Serial.println( "Connected to Blynk.Cloud" ); }

void printBanner()
{
  Serial.printf(
      "\n------------------%s "
      "sensor------------------------------------------------\n\n",
      SENSOR_TYPE );
  Serial.printf( "%-12s : %s\n", "Firmware", FIRMWARE_VERSION );
  Serial.printf( "%-12s : %s\n", "Device ID", heltec.deviceID );
  Serial.printf( "%-12s : %s\n", "SCD30",
                 scd30.hasSCD30() ? "OK" : "Not found" );
  Serial.printf( "%-12s : %.1f degC\n", "Offset", scd30.temperatureOffset() );
  Serial.printf( "%-12s : %s\n", "Status",
                 heltec.metaTopics[heltec.statusTopicID].c_str() );
  for ( int i = 0; i < heltec.dataTopics.size(); i++ )
  {
    Serial.printf( "%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(),
                   heltec.dataTopics[i].topicString.c_str() );
  }
}

void publishEvent()
{
  Blynk.virtualWrite( V0, millis() / 1000 );

  if ( scd30.dataAvailable() )
  {
    heltec.publish( temperatureField, scd30.temperature() );
    heltec.publish( humidityField, scd30.humidity() );
    heltec.publish( co2Field, scd30.co2() );

    Blynk.virtualWrite( V1, scd30.temperature() );
    Blynk.virtualWrite( V2, scd30.humidity() );
    Blynk.virtualWrite( V3, scd30.co2() );

    heltec.update( temperatureField, scd30.temperature() );
    heltec.update( humidityField, scd30.humidity() );
    heltec.update( co2Field, scd30.co2() );
  }
}
