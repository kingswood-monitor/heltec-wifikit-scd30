// #include <Arduino.h>
// #include "config.h"
// #include <Timemark.h>
// #include <kwSCD30.h>

// kwSCD30 scd30;
// Timemark samplePeriod(SAMPLE_INTERVAL_SECONDS * 1000);

// void setup() {
//   Serial.begin(115200);
//   delay(500);
//   Serial.println("Starting");
//   Wire.begin(21,22,0); // The OLED display interferes with I2C: use Wire1 for the SCD30

//   // Sensors
//   if (scd30.start())
//   {
//       Serial.println("SCD30 sensor found");
//   }
//   else
//   {
//     Serial.println("SCD30 sensor NOT found");
//   }

//   // Sample timer
//   samplePeriod.start();

// }

// void loop() {
//   if (samplePeriod.expired())
//   {
//     payload_t payload = scd30.payload();
//     Serial.printf("Temp: %f Humidity: %d CO2: %d\n", payload.temperature, payload.humidity, payload.co2);
//   }
// }

#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

void setup()
{
  Serial.begin(115200);
  Serial.println("SCD30 Example");
  Wire1.begin(21,22);

  if (airSensor.begin(Wire1) == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  //The SCD30 has data ready every two seconds
}

void loop()
{
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
  else
    Serial.println("Waiting for new data");

  delay(500);
}
