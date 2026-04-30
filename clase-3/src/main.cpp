#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "ArduinoJson.h"

#define LED_PIN 2

const char* ssid = "Delga_2.4";
const char* password = "Delga1213";

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

const char* topic_to_publish = "test-topic/from-esp32";
const char* topic_to_subscribe = "test-topic/to-esp32";

WiFiClient client;
PubSubClient mqttClient(client);


void conectarWiFi();
void conectarMQTT();
void publicar();
void callbackMQTT(char* topic, byte* payload, unsigned int length);


void setup() {
pinMode(LED_PIN, OUTPUT);
Serial.begin(115200);
conectarWiFi();

mqttClient.setServer(mqtt_server, mqtt_port);
mqttClient.setCallback(callbackMQTT);
}

void loop() {
if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();


  static unsigned long lastPublishTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= 5000) { // Publicar cada 5 segundos
    publicar();
    lastPublishTime = currentTime;
  }
}


void conectarWiFi(){
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT(){

  // Obtener MAC del ESP32 y crear un cliente único
  String mac = WiFi.macAddress();
  mac.replace(":", "");  // Eliminar los dos puntos
  String clientId = "ESP32_" + mac;

  Serial.print("MAC del ESP32: ");
  Serial.println(mac);
  Serial.print("Client ID: ");
  Serial.println(clientId);
  
  while (!mqttClient.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Conectado al broker MQTT");
      // Suscribirse al topic
      if (mqttClient.subscribe(topic_to_subscribe)) {
        Serial.print("Suscrito al topic: ");
        Serial.println(topic_to_subscribe);
      } else {
        Serial.print("Error al suscribirse al topic: ");
        Serial.println(topic_to_subscribe);
      }
    } else {
      Serial.print("Error de conexión, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void publicar(){
  JsonDocument doc;

  doc["Estado"] = "Activo desde ESP32";

  String payload;
  serializeJson(doc, payload);

  if (mqttClient.publish(topic_to_publish, payload.c_str())) {
    Serial.println("Mensaje publicado correctamente");
  } else {
    Serial.println("Error al publicar el mensaje");
  }
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el topic: ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if(String(topic) == topic_to_subscribe){
    Serial.print("Mensaje recibido en el topic suscrito: ");
    Serial.println(message);


    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      return;
    }

    String estado = doc["estado"];
    Serial.print("Estado recibido: ");
    Serial.println(estado);

    if(estado == "LED_ON") {
      Serial.println("Encendiendo LED...");
      digitalWrite(LED_PIN, HIGH);
    } else if(estado == "LED_OFF") {
      Serial.println("Apagando LED...");
      digitalWrite(LED_PIN, LOW);
    }
  }
}