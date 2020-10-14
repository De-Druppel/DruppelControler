/** 
 * This file is part of De-Druppel/DruppelController 
 * For license information, please view the LICENSE file that was distributed with this source code. 
 */ 

#include <Arduino.h>
#include <PubSubClient.h>
#include <credentials.h>
#include <ESP8266WiFi.h>

WiFiClient wifiClient;

PubSubClient pubsubClient;

String ESP_ID = String(ESP.getChipId());

void setup() { 
  pubsubClient = configureMQTTClient(pubsubClient,wifiClient,MQTT_HOST,MQTT_PORT);  
}

void loop() {
  if(!isWifiConnected())
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
}

bool isMQTTConnected()
{
  return pubsubClient.connected();
}

