#include <Arduino.h>
#include "UbidotsEsp32Mqtt.h"
#include <ArduinoJson.h>


/****************************************************
 * 1. CREDENCIALES DE CONEXIÓN
 ****************************************************/
const char *UBIDOTS_TOKEN = "BBUS-k2DesIYqrGRk5133NrNl748KpgD6Nv";
const char *WIFI_SSID = "Delga1213";
const char *WIFI_PASS = "kike4325";



/************** Labels en Ubidots **************/
const char *DEVICE_LABEL = "clase-iot";
const char *VARIABLE_ADC = "adc";
const char *VARIABLE_LED = "led";

/************** Pines del ESP32 **************/
const uint8_t PIN_SENSOR = 34;  // Entrada analógica
const uint8_t PIN_LED = 2;      // LED integrado o externo

/************** Tiempos de ejecución **************/
const unsigned long PUBLISH_FREQUENCY = 30000;      // Enviar ADC cada 30 s
const unsigned long CHECK_FREQUENCY = 10000;        // Revisar conexión cada 10 s
const unsigned long RESUBSCRIBE_FREQUENCY = 60000;  // Renovar suscripción cada 60 s

unsigned long timerPublish = 0;
unsigned long timerCheck = 0;
unsigned long timerResubscribe = 0;

/************** Estado del LED **************/
float ultimoValorLed = -1.0;

/************** Objeto Ubidots **************/
Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************************
 * Obtiene el valor recibido desde Ubidots.
 * Puede recibir un valor simple: 1.0 o 0.0
 * También puede recibir un JSON con el campo "value".
 ****************************************************/
float obtenerValorLed(String mensaje) {
  mensaje.trim();

  // Caso 1: mensaje simple
  if (!mensaje.startsWith("{")) {
    Serial.println("Mensaje simple");
    return mensaje.toFloat();
  }

  // Caso 2: mensaje en formato JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, mensaje);

  if (error) {
    Serial.print("Error al leer JSON: ");
    Serial.println(error.c_str());
    return -1.0;
  }

  if (!doc.containsKey("value")) {
    Serial.println("El JSON no contiene el campo value.");
    return -1.0;
  }

  return doc["value"];
}

/****************************************************
 * Esta función se ejecuta cuando llega un mensaje
 * desde Ubidots.
 ****************************************************/
void callback(char *topic, byte *payload, unsigned int length) {
  String topicRecibido = String(topic);
  String mensaje = "";

  // Convertir el payload recibido a texto
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  mensaje.trim();

  Serial.println();
  Serial.println("===== MENSAJE RECIBIDO =====");
  Serial.print("Topic: ");
  Serial.println(topicRecibido);
  Serial.print("Payload: ");
  Serial.println(mensaje);

  // Obtener valor del switch
  float valorLed = obtenerValorLed(mensaje);

  if (valorLed < 0) {
    Serial.println("Mensaje no válido.");
    Serial.println("============================");
    return;
  }

  ultimoValorLed = valorLed;

  Serial.print("Valor LED: ");
  Serial.println(valorLed);

  // Control del LED
  if (valorLed >= 0.5) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("LED ENCENDIDO");
  } else {
    digitalWrite(PIN_LED, LOW);
    Serial.println("LED APAGADO");
  }

  Serial.println("============================");
}

/****************************************************
 * Suscribe el ESP32 a la variable led de Ubidots.
 ****************************************************/
void suscribirSwitch() {
  bool suscrito = ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LED);

  Serial.print("Suscripción a ");
  Serial.print(DEVICE_LABEL);
  Serial.print(" / ");
  Serial.print(VARIABLE_LED);
  Serial.print(": ");
  Serial.println(suscrito ? "OK" : "ERROR");
}

/****************************************************
 * Reconecta al broker MQTT y vuelve a suscribirse.
 ****************************************************/
void reconectarUbidots() {
  Serial.println("Conectando o reconectando a Ubidots...");

  ubidots.reconnect();
  delay(500);

  suscribirSwitch();
}

/****************************************************
 * Configuración inicial.
 ****************************************************/
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  Serial.println("Conectando a WiFi y Ubidots...");

  // false evita imprimir información sensible en el monitor serial
  ubidots.setDebug(false);

  // Conexión WiFi
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);

  // Función que recibirá los mensajes MQTT
  ubidots.setCallback(callback);

  // Configuración MQTT
  ubidots.setup();

  // Primera conexión y suscripción
  reconectarUbidots();

  timerPublish = millis();
  timerCheck = millis();
  timerResubscribe = millis();
}

/****************************************************
 * Ciclo principal.
 ****************************************************/
void loop() {
  // Mantiene activa la conexión MQTT
  ubidots.loop();

  // Si se pierde la conexión, se reconecta
  if (!ubidots.connected()) {
    reconectarUbidots();
  }

  // Muestra el estado MQTT cada cierto tiempo
  if (millis() - timerCheck > CHECK_FREQUENCY) {
    Serial.print("Estado MQTT: ");
    Serial.println(ubidots.connected() ? "CONECTADO" : "DESCONECTADO");

    Serial.print("Último valor LED: ");
    Serial.println(ultimoValorLed);

    timerCheck = millis();
  }

  // Renueva la suscripción periódicamente
  if (millis() - timerResubscribe > RESUBSCRIBE_FREQUENCY) {
    Serial.println("Refrescando suscripción...");
    suscribirSwitch();

    timerResubscribe = millis();
  }

  // Envía el valor analógico a Ubidots
  if (millis() - timerPublish > PUBLISH_FREQUENCY) {
    int lecturaADC = analogRead(PIN_SENSOR);

    Serial.print("Enviando ADC: ");
    Serial.println(lecturaADC);

    ubidots.add(VARIABLE_ADC, lecturaADC);
    bool publicado = ubidots.publish(DEVICE_LABEL);

    Serial.print("Publicación ADC: ");
    Serial.println(publicado ? "OK" : "ERROR");

    timerPublish = millis();
  }
}