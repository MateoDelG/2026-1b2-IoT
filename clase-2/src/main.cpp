#include <Arduino.h>
#include "ArduinoJson.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

// Credenciales de la red WiFi a la que se conectara el ESP32.
const char* ssid = "Delga_2.4";
const char* password = "Delga1213";

// Configuracion de red estatica del ESP32.
// Esto evita que la IP cambie cada vez que reinicia el equipo.
const IPAddress kLocalIp(192, 168, 1, 17);
const IPAddress kGatewayIp(192, 168, 1, 254);
const IPAddress kSubnetMask(255, 255, 255, 0);
const IPAddress kPrimaryDns(192, 168, 1, 254);

void setup() {
  // Inicia el puerto serial para ver mensajes en el monitor serial.
  Serial.begin(115200);
  // Pequeña espera para que el monitor serial alcance a conectarse.
  delay(1000);

  // Modo estacion: el ESP32 se conecta a un router WiFi.
  WiFi.mode(WIFI_STA);
  // Aplica la IP estatica definida arriba.
  WiFi.config(kLocalIp, kGatewayIp, kSubnetMask, kPrimaryDns);
  // Intenta conectarse a la red WiFi.
  WiFi.begin(ssid, password);

  // Espera hasta que haya conexion. Si falla, reinicia y vuelve a intentar.
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  // Activa OTA: permite cargar nuevo firmware por WiFi sin cable USB.
  ArduinoOTA.begin();
  Serial.println("OTA inicializado. IP: " + WiFi.localIP().toString());

  // Cadena JSON de ejemplo que vamos a leer.
  String inputJson = "{\"device\": \"Nodo001\", \"status\": \"active\"}";
  /*
    Estructura esperada del JSON:
    {
    "device": "ESP32",
    "status": "active"
    }
  */
  
  // Objeto donde se guarda el JSON ya interpretado.
  JsonDocument doc;
  // Convierte texto JSON a estructura de datos utilizable.
  DeserializationError error = deserializeJson(doc, inputJson);
  if (error) {
    Serial.print("Error al deserializar JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Muestra el JSON original y tambien con formato legible.
  Serial.println("Json crudo: ");
  Serial.println(inputJson);
  Serial.println("Json entrante:");
  serializeJsonPretty(doc, Serial);
  Serial.println("");
  
  // Extrae campos del JSON por su clave.
  String device = doc["device"];
  String status = doc["status"];
  
  // Logica de ejemplo: construye una respuesta segun el estado recibido.
  String response;
  if(status == "active") {
    response = "Device status active";
    Serial.println("El dispositivo está activo.");
  } else {
    Serial.println("El dispositivo no está activo.");
    response = "Device status inactive";
  }
  
  //Respuesta en JSON
  // Crea un nuevo JSON para enviar/mostrar una respuesta.
  JsonDocument responseDoc;
  responseDoc["device"] = device;
  responseDoc["response"] = response;
  String outputJson;
  serializeJsonPretty(responseDoc, outputJson);
  Serial.println(outputJson);
}

void loop() {
  // Debe ejecutarse continuamente para atender solicitudes OTA.
  ArduinoOTA.handle();
  
}
