/*
  WIFIMANAGERCONFIG.cpp - Config Library for WiFiManager
*/

#include "WIFIMANAGERCONFIG.h"  // dichiarazione della classe


/* funzione di inizializzazione */
void WIFIMANAGERCONFIG::setupWifiManagerAP(void){
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "MQTT SERVER IP", configValues.mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT SERVER PORT", configValues.mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT USER", configValues.mqtt_user, 30);
  WiFiManagerParameter custom_mqtt_pwd("mqtt_pwd", "MQTT PWD", configValues.mqtt_pwd, 50);
  WiFiManagerParameter custom_mqtt_topic("mqtt_topic", "MQTT TOPIC", configValues.mqtt_topic, 50);
  WiFiManagerParameter custom_bot_token("telegram_bot_token", "Telegram Bot Token", configValues.bot_token, 150);
  WiFiManagerParameter custom_telegram_chat_id("telegram_chat_id", "Telegram Chat ID", configValues.telegram_chat_id, 40);
  WiFiManagerParameter custom_send_telegram_notification("send_telegram_notification", "Send telegram notification (true/false)", configValues.send_telegram_notification, 5);
  WiFiManagerParameter custom_read_value_delay_in_minutes("read_value_delay_in_minutes", "Read Value Delay in Minutes", configValues.read_value_delay_in_minutes, 3);
  WiFiManagerParameter custom_temperature_offset("temperature_offset", "Temperature Offset", configValues.temperature_offset, 4);
 
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  
  // WiFi.mode(WiFi_STA); // it is a good practice to make sure your code sets wifi mode how you want it.
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  
  //reset settings - wipe credentials for testing
  wm.resetSettings();

  //add all your parameters here
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_pwd);
  wm.addParameter(&custom_mqtt_topic);
  wm.addParameter(&custom_bot_token);
  wm.addParameter(&custom_telegram_chat_id);
  wm.addParameter(&custom_send_telegram_notification);
  wm.addParameter(&custom_read_value_delay_in_minutes);
  wm.addParameter(&custom_temperature_offset);

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  res = wm.autoConnect("TemperaturinoAPConfig","temperature2021"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  }else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...:)");
       //read updated parameters
      strcpy(configValues.mqtt_server, custom_mqtt_server.getValue());
      strcpy(configValues.mqtt_port, custom_mqtt_port.getValue());
      strcpy(configValues.mqtt_user, custom_mqtt_user.getValue());
      strcpy(configValues.mqtt_pwd, custom_mqtt_pwd.getValue());
	  strcpy(configValues.mqtt_topic, custom_mqtt_topic.getValue());
      strcpy(configValues.bot_token, custom_bot_token.getValue());
      strcpy(configValues.telegram_chat_id, custom_telegram_chat_id.getValue());
      strcpy(configValues.send_telegram_notification, custom_send_telegram_notification.getValue());
      strcpy(configValues.read_value_delay_in_minutes, custom_read_value_delay_in_minutes.getValue());
      strcpy(configValues.temperature_offset, custom_temperature_offset.getValue());
      
	  Serial.print(F("server --> "));
      Serial.println(configValues.mqtt_server);
      Serial.print(F("port --> "));
      Serial.println(configValues.mqtt_port);
      Serial.print(F("mqtt_user --> "));
      Serial.println(configValues.mqtt_user);
      Serial.print(F("mqtt_pwd --> "));
      Serial.println(configValues.mqtt_pwd);
	  Serial.print(F("mqtt_topic --> "));
      Serial.println(configValues.mqtt_topic);
      Serial.print(F("bot_token --> "));
      Serial.println(configValues.bot_token);
      Serial.print(F("telegram_chat_id --> "));
      Serial.println(configValues.telegram_chat_id);
      Serial.print(F("send_telegram_notification --> "));
      Serial.println(configValues.send_telegram_notification);
      Serial.print(F("read_value_delay_in_minutes --> "));
      Serial.println(configValues.read_value_delay_in_minutes);
      Serial.print(F("temperature_offset --> "));
      Serial.println(configValues.temperature_offset);
      
     
  }



}

WifiManagerConfiguredValues WIFIMANAGERCONFIG::getConfiguredValues(void){
	return configValues;
}




