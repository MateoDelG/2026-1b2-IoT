#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ===============================
// CONFIGURACIÓN WIFI
// ===============================

const char* ssid = "Delga_2.4";
const char* password = "Delga1213";

// ===============================
// CONFIGURACIÓN MQTT
// ===============================

// Cambia esta IP por la IP del computador donde está Mosquitto
const char* mqtt_server = "192.168.1.4";
const int mqtt_port = 1883;

// Topic para publicar datos del sensor
const char* TOPIC_SENSOR = "test-topic/sensor";

// Topic para recibir mensajes desde el maestro
const char* TOPIC_MASTER = "test-topic/master";

// ===============================
// OBJETOS WIFI Y MQTT
// ===============================

WiFiClient espClient;
PubSubClient client(espClient);

// ===============================
// CONFIGURACIÓN GENERAL
// ===============================

unsigned long tiempoAnterior = 0;
const unsigned long intervaloPublicacion = 5000;

const int ledPin = 2;

// ===============================
// CONEXIÓN WIFI
// ===============================

void conectarWiFi() {
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

// ===============================
// FUNCIÓN PARA PUBLICAR DATOS
// ===============================

/*
Datos a publicar en formato JSON:
{
  "dispositivo": "esp32_01",
  "temperatura": 28.4,
  "humedad": 63.2,
  "unidad_temperatura": "C"
}
*/
void publicarDatosSensor() {
  float temperatura = random(200, 350) / 10.0;
  float humedad = random(400, 800) / 10.0;

  JsonDocument doc;

  doc["dispositivo"] = "esp32_01";
  doc["temperatura"] = temperatura;
  doc["humedad"] = humedad;
  doc["unidad_temperatura"] = "C";

  char mensaje[256];
  serializeJson(doc, mensaje);

  bool publicado = client.publish(TOPIC_SENSOR, mensaje);

  if (publicado) {
    Serial.print("Mensaje publicado en ");
    Serial.print(TOPIC_SENSOR);
    Serial.print(": ");
    Serial.println(mensaje);
  } else {
    Serial.println("Error al publicar el mensaje MQTT");
  }
}

// ===============================
// FUNCIÓN PARA SUSCRIBIRSE
// ===============================

void suscribirseMaster() {
  bool suscrito = client.subscribe(TOPIC_MASTER);

  if (suscrito) {
    Serial.print("Suscrito correctamente al topic: ");
    Serial.println(TOPIC_MASTER);
  } else {
    Serial.print("Error al suscribirse al topic: ");
    Serial.println(TOPIC_MASTER);
  }
}

// ===============================
// FUNCIÓN CALLBACK PARA RECIBIR MENSAJES
// ===============================
/*
Recibe mensajes en formato JSON con el siguiente formato:
{
  "comando": "LED_ON"
}
*/
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Mensaje recibido en topic: ");
  Serial.println(topic);

  String mensaje = "";

  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.print("Payload recibido: ");
  Serial.println(mensaje);

  // Procesar mensajes recibidos en topic definido
  if (String(topic) == TOPIC_MASTER) {

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, mensaje);

    if (error) {
      Serial.println("El mensaje recibido no es un JSON válido");
      return;
    }

    String comando = doc["comando"];

    if (comando == nullptr) {
      Serial.println("El JSON no contiene el campo 'comando'");
      return;
    }

    if (String(comando) == "LED_ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("Comando ejecutado: LED encendido");
    } 
    else if (String(comando) == "LED_OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("Comando ejecutado: LED apagado");
    } 
    else {
      Serial.print("Comando no reconocido: ");
      Serial.println(comando);
    }
  }
}

// ===============================
// CONEXIÓN AL BROKER MQTT
// ===============================

void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT... ");

    String clientId = "ESP32_Cliente_";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");

      // Al conectarse, se suscribe al topic maestro
      suscribirseMaster();

    } else {
      Serial.print("falló. Código: ");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos...");
      delay(5000);
    }
  }
}

// ===============================
// SETUP
// ===============================

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  conectarWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callbackMQTT);
}

// ===============================
// LOOP PRINCIPAL
// ===============================

void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervaloPublicacion) {
    tiempoAnterior = tiempoActual;

    publicarDatosSensor();
  }
}