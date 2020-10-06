#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int moistureSensorPin = PIN_A0;
int pumpPin = 1;

long lastMessage = 0;
int moisture = 0;
int soilMoistureThreshold = SOIL_MOISTURE_THRESHOLD;

// Define the topic using the chipId.
String baseTopic = String("Garden/" + ESP.getChipId());
String moistureTopic = String(baseTopic + "/Moisture");
String thresholdTopic = String(baseTopic + "/Config/Treshold");
String subTopic = String(baseTopic + '/*');

/**
 * The setup connects to the WiFi and sets the server for the MQTT client.
 * Additionally it sets the pinmode for our soil moisture sensor
 */
void setup() {
  Serial.begin(9600);
  pinMode(moistureSensorPin, INPUT);
  connectWifi();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(callback);
}

/**
 * Ensures the wifi and mqtt remain connected. Then reads the sensor values.
 * If the value is lower than the threshold, calls to water the plant.
 * Every 5 minutes, publishes the latest moisture reading.
 */ 
void loop() {
  if(WiFi.isConnected() == false) {
    connectWifi();
  }
  if(mqttClient.connected() == false) {
    connectMqtt();
  }

  moisture = analogRead(moistureSensorPin);
  if(moisture < soilMoistureThreshold) {
    // The plant is too dry, give it water.
  }

  long timeElapsed = millis();
  // Check if 5 minutes have passed since the last message was sent so we don't spam the server with readings.
  if(timeElapsed - lastMessage > 300000) {
    lastMessage = timeElapsed;
    mqttClient.publish(moistureTopic.c_str(), String(moisture).c_str());
  }
}

/**
 * Connects the MCU to wireless using the configured secret SSID and pasword.
 * Will retry until a connection can be established, at which point it will print the current IP adress.
 */
void connectWifi() {
  Serial.println();
  Serial.printf("Connecting to %s.\n" , SSID);

  WiFi.begin(SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.printf("IP address: %s.\n", WiFi.localIP());
}

/**
 * Establishes a connection with the MQTT broker. If connecting fails it will wait 2 seconds and try again.
 */ 
void connectMqtt() {
  while(!mqttClient.connected() == false){
    Serial.print("Attempting MQTT connection.");
    if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker.");
      mqttClient.subscribe(baseTopic.c_str());
    } else {
      Serial.printf("Failed, rc=%s.Retrying in 2 seconds.\n", mqttClient.state());
      delay(2000);
    }
  }
}

/**
 * The callback is called whenever a message is recieved on any subscribed topic. Garden/ESPID/*
 * Depending on the specific topic, it will either forcefully water the plant 
 * or overwrite the moistureThreshold at which it will be automatically watered.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if(topic == thresholdTopic.c_str())
  {
    overWriteThreshold(payload, length);
  }
}

/**
 * Converts the given payload to an int and sets it as the new threshold.
 */
void overWriteThreshold(byte* payload, unsigned int length){
  payload[length] = '\0';
  int newThreshold = atoi((char*)payload);
  soilMoistureThreshold = newThreshold;
}