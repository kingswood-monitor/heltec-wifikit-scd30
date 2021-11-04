# WIFI_CO2_SENSOR

This was an attempt to use the Heltec Wifi Kit 32 with an SCD30. I was able to detect the SCD30, but couldn't read any data. Possible an issue with SDA/SCD pullups.

See [this discussion](hhttps://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62#issuecomment-449690785) on Github about the OLED initialising I2C to (SCL) 15 / (SDA) 4 instead of  (SCL) 22 / (SDA) 21