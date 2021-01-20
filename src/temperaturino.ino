#include <BME680CONFIG.h>
#include <WIFIMANAGERCONFIG.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>


#define uS_TO_S_FACTOR 10000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20     /* Time ESP32 will go to sleep (in seconds) */
#define MSG_BUFFER_SIZE  (50)

WIFIMANAGERCONFIG wifiManagerConfig;
WifiManagerConfiguredValues wifiManagerConfiguredValues;
BME680CONFIG bme680Config;
// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);

char *mqttUser;
char *mqttPwd;
char *mqttTopic;
WiFiClientSecure espclientSecure;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;
String output;

StaticJsonDocument<800> jsonDocument;
char buffer[800];

UniversalTelegramBot bot(wifiManagerConfiguredValues.bot_token, espclientSecure);


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

void setup() {
  Serial.begin(115200);
  #ifdef ESP8266
  espclientSecure.setInsecure();
  #endif
  wifiManagerConfig.setupWifiManagerAP();
  wifiManagerConfiguredValues = wifiManagerConfig.getConfiguredValues();
  bme680Config.setupBME680(atof(wifiManagerConfiguredValues.temperature_offset));
  //connectToWiFi();
  bot.updateToken(wifiManagerConfiguredValues.bot_token);
  mqttUser = wifiManagerConfiguredValues.mqtt_user;
  mqttPwd = wifiManagerConfiguredValues.mqtt_pwd;
  mqttTopic = wifiManagerConfiguredValues.mqtt_topic;
  Serial.println("MqttUser:"+String(mqttUser));
  Serial.println("MqttPwd:"+String(mqttPwd));
  Serial.println("MqttTopic:"+String(mqttTopic));
  client.setServer(wifiManagerConfiguredValues.mqtt_server, atoi(wifiManagerConfiguredValues.mqtt_port)); 
  
}

void loop() {
  unsigned long now = millis();
  String delayString = String(wifiManagerConfiguredValues.read_value_delay_in_minutes);
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
    client.publish(mqttTopic, buffer, n);
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
  if (String(wifiManagerConfiguredValues.send_telegram_notification) =="true"){
    bot.sendMessage(wifiManagerConfiguredValues.telegram_chat_id, message, "");
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
