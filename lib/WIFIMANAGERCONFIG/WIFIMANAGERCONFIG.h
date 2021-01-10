#ifndef WIFIMANAGERCONFIG_h
#define WIFIMANAGERCONFIG_h

#include "Arduino.h"
#include <WiFiManager.h> 

typedef struct
{
    char mqtt_server[40];
	char mqtt_port[6];
	char bot_token[150];
	char telegram_chat_id[40];
	char send_telegram_notification[5];
	char read_value_delay_in_minutes[3];
	char mqtt_user[30];
	char mqtt_pwd[50];
	char *mqttUser;
	char *mqttPwd;
}
configValues;


class WIFIMANAGERCONFIG
{
   private:
	  configValues configValues;


   public:
      void setupWifiManagerAP(void);
	  configValues getConfiguredValues(void);
};

#endif
