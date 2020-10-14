
#include <ESP8266WiFi.h>
#include <settings.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient pubsubClient;
String ESP_ID = String(ESP.getChipId());
bool isSubscribedToMQTT = false;

void setup() {
  Serial.begin(9600);
}

void makeMeasurement() {
  //read analog signal and create a mositure percentage
  float sensor0 = analogRead(0);
  float sensor0P = 100.00 - ( ( 100.00 * sensor0 ) / 1023.00 );
  int sensorInt = (int) sensor0P;

  //show percentage in monitor
  Serial.print("Vochtigheid procent: ");
  Serial.println(sensorInt);
}

PubSubClient configureMQTTClient(PubSubClient pubSubClient, WiFiClient wifiClient, char* mqttHost, uint16_t mqttPort) {
  PubSubClient pubsubClient(wifiClient);
  pubsubClient.setServer(mqttHost, mqttPort);
  return pubsubClient;
}

void connectMQTT(const char* mqttId, char* mqttUser, char* mqttPassword) {
  pubsubClient.connect(mqttId, mqttUser, mqttPassword);
}

void connectWifi() {
  //connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");
}

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool isMQTTConnected() {
  return pubsubClient.connected();
}

void loop() {
  if (isWifiConnected()) {
    //show ip address
    Serial.println("IP address:\t");
    Serial.println(WiFi.localIP());

    
    //connect to the mqtt server only when wifi is connected and there is no mqtt connection
    if (!isMQTTConnected) {
      Serial.println("Connecting to MQTT broker...");
      pubsubClient.connect(ESP_ID.c_str(), MQTT_USER, MQTT_PASSWORD);
    } else {
      //MQTT is connected, do things with it...
      makeMeasurement();
    }
  } else { //if not connected: connect
    connectWifi();
  }
  delay(5000);
}
