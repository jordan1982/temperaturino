# Temperaturino Sensor
Firmware for temperature sensor made with d1 mini and bme680

## Prerequisites
Add a copy of the "BME680Config" and "WiFiManagerConfig" folder libraries into your arduino library folder. 
For example "E: \ mypath \ Arduino \ libraries ".
Install wifimanager library:
https://github.com/tzapu/WiFiManager

## Startup and Configuration
Connect to the access point "TemperaturinoAPConfig" to configure your sensor (WiFi settings, MQTT settings, Telegram Notification and read delay value).
Open your browser and type the address http://192.168.4.1, and fill in the required settings field.

#####  SSID: TemperaturinoAPConfig
#####  PASSWORD: temperaturino2021


If you need a box to print in 3d you can find the project files, created by Lillifee, on
https://www.prusaprinters.org/prints/50229-air-quality-sensor-case-d1-mini
