#include <Arduino.h>
#include "ArduinoJson.h"
#include "OTAManager.h"
#include "PubSubClient.h"

// Objeto global para manejar OTA y WiFi
OTAManager ota;

// Credenciales de la red WiFi a la que se conectara el ESP32.
const char* ssid = "Delga1213";
const char* password = "kike4325";


// Credenciales del broker MQTT al que se conectara el ESP32.
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

String clientId = "";
const char* topic_to_publish = "test-topic";
const char* topic_to_subscribe = "test-topic";

// String willMessage = "ESP32 " + clientId + " desconectado inesperadamente";

WiFiClient client;
PubSubClient mqttClient(client);

void connectToMQTT();
void publishMessageQoS0();
void publishMessageRetain();
void callbackMQTT(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callbackMQTT);
  connectToMQTT();
  publishMessageRetain();


}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  // Deben ejecutarse continuamente para atender solicitudes OTA y MQTT
  // ota.handle();
  mqttClient.loop();

  static unsigned long lastPublishTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= 5000) { // Publicar cada 5 segundos
    publishMessageQoS0();
    lastPublishTime = currentTime;
  }
}


void connectToMQTT(){

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Obtener MAC del ESP32 y crear un cliente único
  String mac = WiFi.macAddress();
  mac.replace(":", "");  // Eliminar los dos puntos
  clientId = "ESP32_" + mac;

  Serial.print("MAC del ESP32: ");
  Serial.println(mac);
  Serial.print("Client ID: ");
  Serial.println(clientId);
  
  while (!mqttClient.connected()) {
    Serial.println("Conectando al broker MQTT...");

    // Intentar conectar sin autenticación ni mensaje de última voluntad
    // if (mqttClient.connect(clientId.c_str())) {

    // Intentar conectar con mensaje de última voluntad
    String willMessage = "ESP32 " + clientId + " desconectado inesperadamente";
    if(mqttClient.connect(clientId.c_str(), NULL, NULL, topic_to_publish, 0, true, willMessage.c_str())) {


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

void publishMessageQoS0(){
JsonDocument doc;

doc["id"] = clientId;
doc["Estado"] = true;
doc["timestamp"] = millis();

JsonObject data = doc["data"].to<JsonObject>();
data["temperatura"] = random(20, 40);
data["humedad"] = random(30, 70);

  String payload;
  serializeJson(doc, payload);

  if (mqttClient.publish(topic_to_publish, payload.c_str())) {
    Serial.println("Mensaje publicado correctamente");
  } else {
    Serial.println("Error al publicar el mensaje");
  }
}

void publishMessageRetain() {
  JsonDocument doc;

  doc["tipo de mensaje"] = "retenido en setup";

  String payload;
  serializeJson(doc, payload);

  bool retain = true;

  if (mqttClient.publish(topic_to_publish, payload.c_str(), retain)) {
    Serial.println("Mensaje retenido publicado correctamente");
  } else {
    Serial.println("Error al publicar el mensaje retenido");
  }
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Mensaje recibido en el topic: ");
  // Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if(String(topic) == topic_to_subscribe){
    // Serial.print("Mensaje recibido en el topic suscrito: ");
    // Serial.println(message);


    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      return;
    }

    String id = doc["id"];
    bool estado = doc["Estado"];
    long timestamp = doc["timestamp"];
   
    JsonObject data = doc["data"].as<JsonObject>();
    float temperatura = data["temperatura"];
    int humedad = data["humedad"];

    if(id != clientId){
      Serial.println("Contenido del mensaje:");

      Serial.print("ID: ");
      Serial.println(id);

      Serial.print("Estado: ");
      Serial.println(estado ? "true" : "false");

      Serial.print("Timestamp: ");
      Serial.println(timestamp);

      Serial.print("Temperatura: ");
      Serial.println(temperatura);

      Serial.print("Humedad: ");
      Serial.println(humedad);
    }
      else {
        Serial.println("Mensaje recibido desde el mismo cliente, ignorando...");
      }
  }
}

