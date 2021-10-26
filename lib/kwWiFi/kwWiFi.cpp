#include "kwWiFi.h"


void connectToWifi() 
{  
  Serial.print("Connecting");

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  byte count = 0;
	while(WiFi.status() != WL_CONNECTED && count < 10)
	{
		count ++;
		delay(500);
    Serial.print("...");
	}

  if(WiFi.status() == WL_CONNECTED)
	{
    digitalWrite(LED,HIGH);
    Serial.println("OK");
	}
  else
  {
    Serial.println("Failed");
  }
  
  Serial.println("WiFi setup done.");
}