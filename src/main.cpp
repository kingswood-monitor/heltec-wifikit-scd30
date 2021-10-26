#include <Arduino.h>
#include "globals.h"
#include "heltec.h"
#include <kwWiFi.h>
#include <kwSCD30.h>

// Required to avoid conflict with OLED
// See: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
#define PIN_SDA 4
#define PIN_SCL 15

#define TEMPERATURE_OFFSET -2.5 // degC
kwSCD30 scd30;

void setup() {
  pinMode(LED,OUTPUT);
	digitalWrite(LED,LOW);
  
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);

  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  connectToWifi();

  bool ok = scd30.start();
  Serial.printf("SCD30 sensor: %s\n", ok ? "OK" : "Not OK");

}

void loop() {
  if (scd30.dataAvilable())
  {
    Serial.printf("temp(degC) %.1f humidity(%%) %d co2(ppm)  %d\n", scd30.temperature(), scd30.humidity(), scd30.co2());
  }
}
