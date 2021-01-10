#include <BME680CONFIG.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h> 


#define uS_TO_S_FACTOR 10000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20     /* Time ESP32 will go to sleep (in seconds) */
#define MSG_BUFFER_SIZE  (50)

BME680CONFIG bme680Config;
// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);

//define your default values here, if there are different values in config.json, they are overwritten.
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

WiFiClientSecure espclientSecure;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;
String output;

StaticJsonDocument<800> jsonDocument;
char buffer[800];

UniversalTelegramBot bot(bot_token, espclientSecure);


int i;

void connectToWiFi() {
  Serial.print("Connecting to WIFI ");
  //Serial.println(SSID);
  
  WiFi.begin(); 
  while (WiFi.status() != WL_CONNECTED) {
    if ( i == 15 ){ WiFi.reconnect();} 
    if (i > 30){
      break;
    }
    delay(500); i++;
  } 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

}


void setupWifiManagerAP(){
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "MQTT SERVER IP", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT SERVER PORT", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT USER", mqtt_user, 30);
  WiFiManagerParameter custom_mqtt_pwd("mqtt_pwd", "MQTT PWD", mqtt_pwd, 50);
  WiFiManagerParameter custom_bot_token("telegram_bot_token", "Telegram Bot Token", bot_token, 150);
  WiFiManagerParameter custom_telegram_chat_id("telegram_chat_id", "Telegram Chat ID", telegram_chat_id, 40);
  WiFiManagerParameter custom_send_telegram_notification("send_telegram_notification", "Send telegram notification (true/false)", send_telegram_notification, 5);
  WiFiManagerParameter custom_read_value_delay_in_minutes("read_value_delay_in_minutes", "Read Value Delay in Minutes", read_value_delay_in_minutes, 3);

 
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
  wm.addParameter(&custom_bot_token);
  wm.addParameter(&custom_telegram_chat_id);
  wm.addParameter(&custom_send_telegram_notification);
  wm.addParameter(&custom_read_value_delay_in_minutes);

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
      strcpy(mqtt_server, custom_mqtt_server.getValue());
      strcpy(mqtt_port, custom_mqtt_port.getValue());
      strcpy(mqtt_user, custom_mqtt_user.getValue());
      strcpy(mqtt_pwd, custom_mqtt_pwd.getValue());
      strcpy(bot_token, custom_bot_token.getValue());
      strcpy(telegram_chat_id, custom_telegram_chat_id.getValue());
      strcpy(send_telegram_notification, custom_send_telegram_notification.getValue());
      strcpy(read_value_delay_in_minutes, custom_read_value_delay_in_minutes.getValue());
     
      Serial.print(F("server --> "));
      Serial.println(mqtt_server);
      Serial.print(F("port --> "));
      Serial.println(mqtt_port);
      Serial.print(F("mqtt_user --> "));
      Serial.println(mqtt_user);
      Serial.print(F("mqtt_pwd --> "));
      Serial.println(mqtt_pwd);
      Serial.print(F("bot_token --> "));
      Serial.println(bot_token);
      Serial.print(F("telegram_chat_id --> "));
      Serial.println(telegram_chat_id);
      Serial.print(F("send_telegram_notification --> "));
      Serial.println(send_telegram_notification);
      Serial.print(F("read_value_delay_in_minutes --> "));
      Serial.println(read_value_delay_in_minutes);
      
      
     
  }
}



void setup() {
  Serial.begin(115200);
  #ifdef ESP8266
  espclientSecure.setInsecure();
  #endif

  setupWifiManagerAP();
  bme680Config.setupBME680();
  //connectToWiFi();
  bot.updateToken(bot_token);
  mqttUser = mqtt_user;
  mqttPwd = mqtt_pwd;
  Serial.println("MqttUser:"+String(mqttUser));
  Serial.println("MqttPwd:"+String(mqttPwd));
  client.setServer(mqtt_server, atoi(mqtt_port)); 
  
}

void loop() {
  unsigned long now = millis();
  String delayString = String(read_value_delay_in_minutes);
  int delayValue = delayString.toInt();
  bmeValues bmeValues = bme680Config.getValues();
  if (now - lastMsg >(delayValue * uS_TO_S_FACTOR)) {
    Serial.print("Delay:");
    Serial.println(delayValue*uS_TO_S_FACTOR);
    lastMsg = now;
    Serial.print("LastMsg:");
    Serial.println(lastMsg);
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "New Data,ready yo send - count: #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    if (!client.connected()) {
      reconnect();
    }
    setJsonValues(bmeValues.temperature, bmeValues.humidity, bmeValues.gasResistance, bmeValues.pressure, bmeValues.iaq, bmeValues.iaqAccuracy, bmeValues.staticIaq, bmeValues.co2Equivalent, bmeValues.breathVocEquivalent);
    size_t n = serializeJson(jsonDocument, buffer);
    client.publish("esp32/cameretta/sensor", buffer, n);
    String messageValue= "IaqAccuracy: "+(String)bmeValues.iaqAccuracy + " - Temperature: " + (String)bmeValues.temperature ; 
    //sendTelegramMessage(messageValue);
    Serial.print(messageValue);
    delay(3000);
    client.disconnect();
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Cameretta-";
    clientId += String(random(0xffff), HEX);
    if(WiFi.status() != WL_CONNECTED){
      connectToWiFi();
      delay(2000);
    }
    // Attempt to connect
    client.connect(clientId.c_str(),mqttUser, mqttPwd);
    if (client.connected()) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      String errorConnectMqtt = "failed to connect to MQTT Server try again in 5 seconds";
      sendTelegramMessage(errorConnectMqtt);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendTelegramMessage(String message){
  if (String(send_telegram_notification) =="true"){
    bot.sendMessage(telegram_chat_id, message, "");
  } 
}

void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}


void setJsonValues(float temperature, float humidity, float gasResistance, float pressure, float iaq, int iaqAccuracy, int staticIaq, float co2Equivalent, float breathVocEquivalent  ) {
  jsonDocument.clear();
  add_json_object("temperature", temperature, "°C");
  add_json_object("humidity", humidity, "%");
  add_json_object("gas", gasResistance, "ohm");
  add_json_object("pressure", pressure, "mBar");
  add_json_object("iaq", iaq, "IAQ");
  add_json_object("iaqAccuracy", iaqAccuracy, "IAQ");
  add_json_object("staticIaq", staticIaq, "IAQ");
  add_json_object("co2Equivalent", co2Equivalent, "ppm");
  add_json_object("breathVocEquivalent", breathVocEquivalent, "ppm");
}
