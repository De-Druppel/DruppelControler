#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int moistureSensorPin = 8;

long lastMessage = 0;
int moisture = 0;
int soilMoistureThreshold = SOIL_MOISTURE_THRESHOLD;

// Define the topic using the chipId.
String baseTopic = String("Garden/" + ESP.getChipId());
String moistureTopic = String(baseTopic + "/Moisture");
String thresholdTopic = String(baseTopic + "/Config/Treshold");

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

void loop() {
  if(WiFi.isConnected() == false) {
    connectWifi();
  }
  if(mqttClient.connected() == false) {
    connectMqtt();
  }

  moisture = digitalRead(moistureSensorPin);
  if(moisture < soilMoistureThreshold) {
    // The plant is too dry, give it water.
  }

  long timeElapsed = millis();
  // Check if 5 minutes have passed since the last message was sent so we don't spam the server with readings.
  if(timeElapsed - lastMessage > 300000) {
    lastMessage = timeElapsed;
    // Send message to the right topic.
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
      mqttClient.subscribe(thresholdTopic.c_str());
    } else {
      Serial.printf("Failed, rc=%s.Retrying in 2 seconds.\n", mqttClient.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

}