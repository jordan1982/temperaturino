#ifndef WIFIMANAGERCONFIG_h
#define WIFIMANAGERCONFIG_h

#include "Arduino.h"
#include <WiFiManager.h> 

typedef struct
{
    char mqtt_server[40];
	char mqtt_port[6];
	char mqtt_topic[50];
	char bot_token[150];
	char telegram_chat_id[40];
	char send_telegram_notification[5];
	char read_value_delay_in_minutes[3];
	char temperature_offset[4];
	char mqtt_user[30];
	char mqtt_pwd[50];
}
WifiManagerConfiguredValues;


class WIFIMANAGERCONFIG
{
   private:
	  


   public:
      WifiManagerConfiguredValues configValues;
      void setupWifiManagerAP(void);
	  WifiManagerConfiguredValues getConfiguredValues(void);
};

#endif
