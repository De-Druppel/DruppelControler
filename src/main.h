#ifndef MAIN_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

void setup();
void loop();
void makeMoistureMeasurement();
void connectWifi(char* ssid, char* wifiPassword);
PubSubClient configureMQTTClient(PubSubClient pubSubClient,WiFiClient wifiClient, char* mqttHost, uint16_t mqttPort);
void connectMQTT(const char* mqttId, char* mqttUser, char* mqttPassword);
void publishMeasurements();
void callback(char* topic, byte* payload, unsigned int length);
void initializeTopics();
#endif