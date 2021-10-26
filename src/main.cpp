#include <Arduino.h>

#include "heltec.h"
#include "SparkFun_SCD30_Arduino_Library.h"
#include <kwWiFi.h>

// Required to avoid conflict with OLED
// See: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
#define PIN_SDA 4
#define PIN_SCL 15

SCD30 airSensor;

void setup() {
  pinMode(LED,OUTPUT);
	digitalWrite(LED,LOW);
  
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);

  WIFISetUp();

  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

}

void loop() {
  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }
  delay(500);
}
