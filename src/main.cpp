#include <Arduino.h>
#include "globals.h"
#include "heltec.h"
#include <kwWiFi.h>
#include <kwMQTT.h>

// Required to avoid conflict with OLED
// See: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
#define PIN_SDA 4
#define PIN_SCL 15

#define SENSOR_TYPE "environment"

void getMacAddress(char* buf)
{
  String theAddress = WiFi.macAddress();
  theAddress.replace(":", "");
  strcpy(buf, theAddress.c_str());
}

void setup() 
{
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);

  getMacAddress(g_deviceID);
  bool ok = scd30.start();

  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  publishDataTimer = xTimerCreate("publishDataTimer", pdMS_TO_TICKS(g_sampleIntervalSeconds * 1000), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(publishData));

  makeTopic("meta", "status", topicMetaStatus);
  makeTopic("meta", "firmware", topicMetaFirmware);
  makeTopic("data", "temperature", "SCD30", topicDataTemperatureSCD30);
  makeTopic("data", "humidity", "SCD30", topicDataHumiditySCD30);
  makeTopic("data", "co2", "SCD30", topicDataCO2SCD30);
  // TODO: Add Command topics

  WiFi.onEvent(WiFiEvent);

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setWill(topicMetaStatus, 2, 1, "OFFLINE", 7);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);

  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "Device ID:");
  Heltec.display -> drawString(COL2, 0, g_deviceID);
  Heltec.display -> drawString(0, 10, "Firmware:");
  Heltec.display -> drawString(COL2, 10, g_firmwareVersion);
  Heltec.display -> drawString(0, 20, "Sensor:");
  Heltec.display -> drawString(COL2, 20, ok ? "OK" : "-");
  Heltec.display -> display();

  Serial.printf("------------------%s sensor------------------\n", SENSOR_TYPE);
  Serial.printf("Firmware: %s\n", g_firmwareVersion);
  Serial.printf("Device ID: %s\n", g_deviceID);
  Serial.printf("MQTT Temperature : %s\n", topicDataTemperatureSCD30);
  Serial.printf("MQTT Humidity    : %s\n", topicDataHumiditySCD30);
  Serial.printf("MQTT CO2         : %s\n", topicDataCO2SCD30);
  Serial.printf("MQTT Status      : %s\n", topicMetaStatus);
  Serial.printf("MQTT Firmware    : %s\n", topicMetaFirmware);

  connectToWifi();
}

void loop() 
{

}