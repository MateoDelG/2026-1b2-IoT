#include <Arduino.h>
#include "WiFi.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"

const char* ssid = "Delga1213";
const char* password = "kike4325";

const char* mqtt_server = "192.168.1.2";
const int mqtt_port = 1883;

const char* data_topic = "iot/nodes";

String clientId = "ESP32Client-002";


WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);


void connectToMQTT();
void dataSuscriber();
void callbackMQTT(char* topic, byte* payload, unsigned int length);
void publishData();


void setup() {
  Serial.begin(115200);
  MQTTclient.setServer(mqtt_server, mqtt_port);
  MQTTclient.setCallback(callbackMQTT);
  connectToMQTT();
}

void loop() {
  if (!MQTTclient.connected()) {
    connectToMQTT();
  }
  MQTTclient.loop();


  // Publish data every 5 seconds
  static unsigned long lastPublishTime = 30000;
  if (millis() - lastPublishTime > 10000) {
    publishData();
    lastPublishTime = millis();
  }
}

void connectToMQTT(){
WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");


  while(!MQTTclient.connected()) {
    Serial.println("Connecting to MQTT...");

    // if (MQTTclient.connect(clientId.c_str())) 
    String desconexion = clientId + " desconectado abruptamente";
    if(MQTTclient.connect(clientId.c_str(), NULL, NULL, data_topic, 0, true, desconexion.c_str()))
    {
      Serial.println("Connected to MQTT");
      dataSuscriber(); // Subscribe to the topic after connecting
    } 
    else {
      Serial.print("Failed with state ");
      Serial.print(MQTTclient.state());
      delay(2000);
    }
  }
}

void dataSuscriber(){
  Serial.println("Subscribing to topic...");
    if(MQTTclient.subscribe(data_topic)){
      Serial.println("Subscribed to topic");
    }
    else{
      Serial.println("Failed to subscribe to topic");
    }
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived in topic: ");
  // Serial.println(topic);
  String message;
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    message += (char) payload[i];
  }

  if(String(topic) == data_topic){
    Serial.println("Data received from topic: " + String(topic));

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }



  String id = doc["node"].as<String>();
  long timestamp = doc["timestamp"].as<long>();
  
  JsonObject data = doc["data"].as<JsonObject>();
  float temperature = data["temperature"].as<float>();
  float humidity = data["humidity"].as<float>();


  if(id != clientId){
    Serial.println("Node ID: " + id);
    Serial.println("Timestamp: " + String(timestamp));
    Serial.println("Temperature: " + String(temperature));
    Serial.println("Humidity: " + String(humidity));
    Serial.println("-----------------------");
  }
  else{
    Serial.println("Received own message, ignoring...");
  }
}
}


void publishData(){

  JsonDocument doc;
  doc["node"] = clientId;
  doc["timestamp"] = millis();

  JsonObject data = doc["data"].to<JsonObject>();
  data["temperature"] = random(20, 30);
  data["humidity"] = random(40, 60);

  String payload;
  serializeJsonPretty(doc, payload);

  if(MQTTclient.publish(data_topic, payload.c_str(), true)){
    Serial.println("Message published successfully");
  }
  else{
    Serial.println("Failed to publish message");
  }

}
