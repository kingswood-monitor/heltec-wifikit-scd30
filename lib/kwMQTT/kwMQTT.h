#ifndef H_KWMQTT
#define H_KWMQTT

extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#include "globals.h"
#include "heltec.h"
#include <AsyncMqttClient.h>

#define MQTT_HOST IPAddress(192, 168, 1, 1) // Mac Mini M1
#define MQTT_PORT 1883

#define DATA_COL 70

AsyncMqttClient mqttClient;

TimerHandle_t mqttReconnectTimer;
TimerHandle_t publishDataTimer;

#define MAX_TOPIC_BUFFER_LEN 50

const char *topicRoot = "kw_sensors";
char topicMetaStatus[MAX_TOPIC_BUFFER_LEN];
char topicMetaFirmware[MAX_TOPIC_BUFFER_LEN];
char topicDataTemperatureSCD30[MAX_TOPIC_BUFFER_LEN];
char topicDataHumiditySCD30[MAX_TOPIC_BUFFER_LEN];
char topicDataCO2SCD30[MAX_TOPIC_BUFFER_LEN];

void connectToMqtt() {
  mqttClient.connect();
}

void publishData() 
{
  if (scd30.dataAvailable())
  {
    char buf[5];

    scd30.temperature(buf);
    mqttClient.publish(topicDataTemperatureSCD30, 2, true, buf);

    scd30.humidity(buf);
    mqttClient.publish(topicDataHumiditySCD30, 2, true, buf);

    scd30.co2(buf);
    mqttClient.publish(topicDataCO2SCD30, 2, true, buf);

    Heltec.display -> clear();

    Heltec.display -> drawString(0, 0, "temperature:");
    Heltec.display -> drawString(DATA_COL, 0, (String)(scd30.temperature()));
    Heltec.display -> drawString(0, 10, "humidity:");
    Heltec.display -> drawString(DATA_COL, 10, (String)(scd30.humidity()));
    Heltec.display -> drawString(0, 20, "co2:");
    Heltec.display -> drawString(DATA_COL, 20, (String)(scd30.co2()));
    
    Heltec.display -> display();

    led.command(LED_FLASH_ONCE);
  }
}

void onMqttConnect(bool sessionPresent) 
{
  led.command(LED_ON);
  
  mqttClient.publish(topicMetaFirmware, 2, true, g_firmwareVersion);
  mqttClient.publish(topicMetaStatus, 2, true, "ONLINE");

  // TODO: Add Command subscriptions
  
  xTimerStart(publishDataTimer, 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  led.command(LED_OFF);

  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "MQTT disconnected. Reconnecting...");
  Heltec.display -> display();
            
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId)
{
  Serial.printf("temp(degC) %.1f humidity(%%) %d co2(ppm)  %d\n", scd30.temperature(), scd30.humidity(), scd30.co2());
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {}
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {}
void onMqttUnsubscribe(uint16_t packetId) {}

void makeTopic(const char* type, const char* field, const char* g_sensorType, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s/%s", topicRoot, type, field, g_sensorType, g_deviceID);
}

void makeTopic(const char* type, const char* field, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s", topicRoot, type, field, g_deviceID);
}

void makeTopic(const char* type, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s", topicRoot, type, g_deviceID);
}

#endif