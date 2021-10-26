#include <Arduino.h>

#include "heltec.h"
#include <kwWiFi.h>

void setup() {
  pinMode(LED,OUTPUT);
	digitalWrite(LED,LOW);
  
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);

  WIFISetUp();

}

void loop() {
  ;
}
