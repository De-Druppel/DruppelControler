#include <Arduino.h>
#include <PubSubClient.h>
#include <credentials.h>
#include <ESP8266WiFi.h>

WiFiClient wifiClient;

PubSubClient pubsubClient;

String ESP_ID = String(ESP.getChipId());

/// Setup is called once when the ESP8266 is starting.
/// Used for configuration.
void setup() { 
  pubsubClient = configureMQTTClient(pubsubClient,wifiClient,MQTT_HOST,MQTT_PORT);  
}

/// Loop is called every cycle of the ESP8266.
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

/// Connect to wifi
/// @ssid SSID of the wifi network you want to connect to.
/// @wifiPassword Password of the wifi network.
void connectWifi(char* ssid, char* wifiPassword)
{
  WiFi.begin(ssid,wifiPassword);
}

/// Checks if the wifi is connected.
/// returns: boolean that shows if the Wifi is connected.
bool isWifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

/// Configure the MQTT client.
/// @pubSubClient client that is configured.
/// @wifiClient wifi client that is passed to the pubSubClient.
/// @mqttHost Hostname or ip of the MQTT server you want to connect to.
/// @mqttPort Port that is used to connect to the MQTT server.
/// returns: Configured PubSubClient object.
PubSubClient configureMQTTClient(PubSubClient pubSubClient,WiFiClient wifiClient, char* mqttHost, uint16_t mqttPort)
{
  PubSubClient pubsubClient(wifiClient);
  pubsubClient.setServer(mqttHost, mqttPort);  
  return pubsubClient;
}

/// Connect to the MQTT client.
/// pubSubClient needs the be configured prior to running this function.
/// @mqttId Unique ID that is used for connecting to the MQTT server.
/// @mqttUser Username for the MQTT server
/// @mqttPassword Password for the MQTT server.
void connectMQTT(const char* mqttId, char* mqttUser, char* mqttPassword)
{
  pubsubClient.connect(mqttId,mqttUser,mqttPassword);
}

/// Check if the MQTT server is connected.
/// returns: Boolean that shows if the MQTT client is connected.
bool isMQTTConnected()
{
  return pubsubClient.connected();
}

