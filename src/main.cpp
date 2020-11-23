#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <credentials.h>

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);

String ESP_ID = String(ESP.getChipId()); //Get unique ESP id to use for connecting to the broker and topic.

const int TEMPERATURE_SENSOR_PIN = A0;

String BASE_TOPIC = String("Garden/") + String(ESP.getChipId());
String MOISTURE_TOPIC = BASE_TOPIC + String("/Measurement/Moisture");
String STATUS_TOPIC = BASE_TOPIC + String("/Status");
String TEST_TOPIC = "TEST";

const int MOISTURE_SENSOR_PIN = A0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


//Function used for connecting to the MQTT server.
void connectMQTT()
{  

  Serial.println("Connecting to MQTT...");

  //connect to the broker and set last will to publish offline when disconnected.
  if (client.connect(ESP_ID.c_str(), MQTT_USER, MQTT_PASSWORD, STATUS_TOPIC.c_str(), 2, 1, "0"))
  {
    client.subscribe(TEST_TOPIC.c_str());

    //Publish online when connected.
    client.publish(STATUS_TOPIC.c_str(), "1", true);

    Serial.println("connected");
  }
  else
  {

    Serial.print("failed with state ");
    Serial.print(client.state());    
  }
}

/*
  get the currrent mositure percentage by making a measurement with the sensor
*/
float getMoistureMeasurement() {
  //read analog signal and create a mositure percentage
  float sensor0 = analogRead(MOISTURE_SENSOR_PIN);
  //set the current percentage
  return 100.00 - ( ( 100.00 * sensor0 ) / 1023.00 ); 
}

void setup()
{
  Serial.begin(9600);  
  Serial.println(MOISTURE_TOPIC);
  wifiManager.autoConnect("De Druppel"); 
  client.setServer(MQTT_HOST, MQTT_PORT);  
  client.setCallback(callback);
  connectMQTT();
}

void loop()
{
  float moisturePercentage = getMoistureMeasurement();

  if (client.connected())
  {
    client.publish(MOISTURE_TOPIC.c_str(), String(moisturePercentage).c_str());
  }
  else
  {
    connectMQTT();
  }
  delay(5000);
}