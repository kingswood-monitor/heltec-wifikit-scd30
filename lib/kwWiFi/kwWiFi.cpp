#include "kwWiFi.h"

void WIFISetUp(void)
{
  WiFi.disconnect(true);
	delay(1000);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoConnect(true);
	WiFi.begin("kingswoodpn","wbtc0rar");
	delay(100);

  Serial.print("Connecting");
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