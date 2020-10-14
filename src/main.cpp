#include <Arduino.h>
#include <PubSubClient.h>
#include <credentials.h>
#include <ESP8266WiFi.h>

WiFiClient wifiClient;
PubSubClient pubsubClient;
String ESP_ID = String(ESP.getChipId());
const char* moistureTopic;
const char* thresholdTopic;
const char* subscriptionTopic;

void setup() { 
  initializeTopics();
  pubsubClient = configureMQTTClient(pubsubClient,wifiClient,MQTT_HOST,MQTT_PORT);  
}

void loop() {
  if(!wifiClient.connected())
  {
    connectWifi(SSID,WIFI_PASSWORD);
  }  

  if(!isMQTTConnected())
  {
    connectMQTT(ESP_ID.c_str(), MQTT_USER,MQTT_PASSWORD);
  }



  delay(100);
}

void connectWifi(char* ssid, char* wifiPassword)
{
  WiFi.begin(ssid,wifiPassword);
}

bool isWifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

PubSubClient configureMQTTClient(PubSubClient pubSubClient,WiFiClient wifiClient, char* mqttHost, uint16_t mqttPort)
{
  PubSubClient pubsubClient(wifiClient);
  pubsubClient.setServer(mqttHost, mqttPort);  
  return pubsubClient;
}

void connectMQTT(const char* mqttId, char* mqttUser, char* mqttPassword)
{
  pubsubClient.connect(mqttId,mqttUser,mqttPassword);
  if(pubsubClient.connected())
  {
    pubsubClient.subscribe(subscriptionTopic);
  }

  pubsubClient.setCallback(callback);
}

bool isMQTTConnected()
{
  return pubsubClient.connected();
}

/**
 * The callback is called whenever a message is recieved on any subscribed topic. Garden/ESP_ID/*
 * Depending on the specific topic, it will either forcefully water the plant 
 * or overwrite the moistureThreshold at which it will be automatically watered.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if(topic == thresholdTopic)
  {
    // Overwrite the threshold.
  }
}

/**
 * Use strings to set the topics for the pubSubClient and copies them to character arrays so the String objects
 * Don't have to be kept in memory.
 */
void initializeTopics()
{
  String baseTopic = String("Garden/" + ESP.getChipId());
  moistureTopic = String(baseTopic + "/Moisture").c_str();
  thresholdTopic = String(baseTopic + "/Config/Treshold").c_str();
  subscriptionTopic = String(baseTopic + '/*').c_str();
}