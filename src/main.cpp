/** 
 * This file is part of De-Druppel/DruppelController 
 * For license information, please view the LICENSE file that was distributed with this source code. 
 */ 

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <credentials.h>

WiFiClient wifiClient;
PubSubClient pubsubClient;
String ESP_ID = String(ESP.getChipId());
const char* MOISTURE_TOPIC;
const char* TRESHOLD_TOPIC;
const char* SUBSCRIPTION_TOPIC;
const int MOISTURE_SENSOR_PIN = A0;
float moisturePercentage;

/**
 * Use strings to set the topics for the pubSubClient and copies them to character arrays so the String objects
 * Don't have to be kept in memory.
 */
void initializeTopics() {
  String BASE_TOPIC = String("Garden/" + ESP.getChipId());
  MOISTURE_TOPIC = String(BASE_TOPIC + "/Moisture").c_str();
  TRESHOLD_TOPIC = String(BASE_TOPIC + "/Config/Treshold").c_str();
  SUBSCRIPTION_TOPIC = String(BASE_TOPIC + '/*').c_str();
}

/// Connect to wifi
/// @ssid SSID of the wifi network you want to connect to.
/// @wifiPassword Password of the wifi network.
void connectWifi(char* ssid, char* wifiPassword) {
  Serial.printf("Attempting to connect to wifi %s \n", SSID);
  WiFi.begin(ssid,wifiPassword);

 while (WiFi.status() != WL_CONNECTED)
 {
       delay(500);
    Serial.print(".");
 }
 
    Serial.printf("Connected to WIFI %s \n", SSID);  
  
}

/// Configure the MQTT client.
/// @pubSubClient client that is configured.
/// @wifiClient wifi client that is passed to the pubSubClient.
/// @mqttHost Hostname or ip of the MQTT server you want to connect to.
/// @mqttPort Port that is used to connect to the MQTT server.
/// returns: Configured PubSubClient object.
PubSubClient configureMQTTClient(PubSubClient pubSubClient,WiFiClient wifiClient, char* mqttHost, uint16_t mqttPort) {
  PubSubClient pubsubClient(wifiClient);
  pubsubClient.setServer(mqttHost, mqttPort);  
  return pubsubClient;
}

/**
 * The callback is called whenever a message is recieved on any subscribed topic.
 * Depending on the specific topic, it will either forcefully water the plant 
 * or overwrite the moistureThreshold at which it will be automatically watered.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if(topic == TRESHOLD_TOPIC) {
    // Overwrite the threshold.
  }
}

/// Connect to the MQTT client.
/// pubSubClient needs the be configured prior to running this function.
/// @mqttId Unique ID that is used for connecting to the MQTT server.
/// @mqttUser Username for the MQTT server
/// @mqttPassword Password for the MQTT server.
void connectMQTT(const char* mqttId, char* mqttUser, char* mqttPassword) {
  Serial.printf("Attempting to connect to host %s \n", MQTT_HOST); 
  pubsubClient.connect(mqttId,mqttUser,mqttPassword);
  if(pubsubClient.connected()) {
    Serial.printf("Connected to MQTT HOST %s \n", MQTT_HOST);
    pubsubClient.subscribe(SUBSCRIPTION_TOPIC);
  }

  pubsubClient.setCallback(callback);
}


//method used for publishing last measurements to mqtt broker
void publishMeasurements() {
//publish last moisture percentage
	pubsubClient.publish(MOISTURE_TOPIC, String(moisturePercentage).c_str());
}

/*
*get the currrent mositure percentage by making a measurement with the sensor
*/
void makeMoistureMeasurement() {
  //read analog signal and create a mositure percentage
  float sensor0 = analogRead(MOISTURE_SENSOR_PIN);
  //set the current percentage
  moisturePercentage = 100.00 - ( ( 100.00 * sensor0 ) / 1023.00 );
}

/// Setup is called once when the ESP8266 is starting.
/// Used for configuration.
void setup() { 
  Serial.begin(9600);  
  initializeTopics();
  pubsubClient = configureMQTTClient(pubsubClient,wifiClient,MQTT_HOST,MQTT_PORT);  
}

/// Loop is called every cycle of the ESP8266.
void loop() {
  makeMoistureMeasurement();

  if (WiFi.status() == WL_CONNECTED) {
    //connect to the mqtt server only when wifi is connected and there is no mqtt connection
    if (!pubsubClient.connected()) {
      connectMQTT(ESP_ID.c_str(), MQTT_USER, MQTT_PASSWORD);
    } else {
		//publish last measurements
		publishMeasurements();
	}
  } else { //if not connected: connect
    connectWifi(SSID,WIFI_PASSWORD);
  }
  delay(100);
}