#include <Arduino.h>
#include "ArduinoJson.h"
#include "OTAManager.h"

// Objeto global para manejar OTA y WiFi
OTAManager ota;

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

  // Conectar a WiFi con IP estatica usando la libreria
  ota.connect(ssid, password, kLocalIp, kGatewayIp, kSubnetMask, kPrimaryDns);

  // Activa OTA: permite cargar nuevo firmware por WiFi sin cable USB.
  ota.begin();
  Serial.println("OTA inicializado. IP: " + ota.getLocalIP().toString());

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
  ota.handle();
  
}
