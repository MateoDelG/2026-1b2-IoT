/*
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
              Implementación de la librería Neopixel Manager usando NAMESPACE.
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/
// #include <Arduino.h>
// #include "neopixelManagerNamespace.h"

// void setup() {
//   neopixelManagerNamespace::begin();
// }

// void loop() {
//   neopixelManagerNamespace::showColor(255, 0, 0);
//   delay(1000);

//   neopixelManagerNamespace::showColor(0, 255, 0);
//   delay(1000);

//   neopixelManagerNamespace::showColor(0, 0, 255);
//   delay(1000);

//   neopixelManagerNamespace::turnOff();
//   neopixelManagerNamespace::showPixel(0, 255, 255, 0);
//   delay(500);

//   neopixelManagerNamespace::showPixel(1, 255, 0, 255);
//   delay(500);

//   neopixelManagerNamespace::showPixel(2, 0, 255, 255);
//   delay(500);

//   neopixelManagerNamespace::showPixel(3, 255, 255, 255);
//   delay(500);

//   neopixelManagerNamespace::blink(255, 100, 0, 300);
// }


/*
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
                Implementación de la librería Neopixel Manager usando CLASS.
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

*/
// #include <Arduino.h>
// #include "neopixelManagerClass.h"

// #define NEOPIXEL_PIN 14
// #define NUM_PIXELS 4

// neopixelManagerClass strip(NEOPIXEL_PIN, NUM_PIXELS);

// void setup() {
//   strip.begin();
// }

// void loop() {
//   strip.showColor(255, 0, 0);
//   delay(1000);

//   strip.showColor(0, 255, 0);
//   delay(1000);

//   strip.showColor(0, 0, 255);
//   delay(1000);

//   strip.turnOff();
//   strip.showPixel(0, 255, 255, 0);
//   delay(500);

//   strip.showPixel(1, 255, 0, 255);
//   delay(500);

//   strip.showPixel(2, 0, 255, 255);
//   delay(500);

//   strip.showPixel(3, 255, 255, 255);
//   delay(500);

//   strip.blink(255, 100, 0, 300);
// }


/*
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
          Implementación de ArduinoJson para procesamiento de JSON de entrada y salida.
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/
#include <Arduino.h>
#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  // -------- JSON DE ENTRADA --------
  String inputJson = "{\"device\":\"esp32\",\"status\":\"on\"}";
  /*
  Input JSON:
  {
    "device": "esp32",
    "status": "on"
  }
  */

  JsonDocument rxDoc;
  deserializeJson(rxDoc, inputJson);

  Serial.println("Input JSON:");
  serializeJsonPretty(rxDoc, Serial);
  Serial.println();

  // -------- PROCESAMIENTO --------
  String device = rxDoc["device"];
  String status = rxDoc["status"];
  String responseStatus;

  if (status == "on") {
    responseStatus = "device activated";
  } else {
    responseStatus = "device stopped";
  }

  // -------- JSON DE SALIDA --------
  JsonDocument txDoc;
  txDoc["device"] = device;
  txDoc["response"] = responseStatus;

  Serial.println("\nOutput JSON:");
  serializeJsonPretty(txDoc, Serial);
  Serial.println();

/*
    Output JSON:
  {
    "device": "esp32",
    "response": "device activated"
  }
*/
}

void loop() {
}

/*
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
                  Implementación de ArduinoJson Serial Command Parser
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
*/

// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include "neopixelManagerClass.h"

// #define NEOPIXEL_PIN 14
// #define NUM_PIXELS 4


// // Creamos el objeto de la clase
// neopixelManagerClass strip(NEOPIXEL_PIN, NUM_PIXELS);

// // Prototipo de función
// void processJson(String jsonText);

// void setup() {
//   Serial.begin(115200);
//   strip.begin();

//   Serial.println("ESP32 ready");
//   Serial.println("Send JSON like:");
//   Serial.println("{\"command\":\"setColor\",\"r\":255,\"g\":0,\"b\":0}");
//   Serial.println("{\"command\":\"turnOff\"}");
// }
// /*
// >>Toda la cinta de un color:
//   {"command":"setColor",
//   "r":255,
//   "g":0,
//   "b":0}

// >>Un solo píxel:
//   {"command":"pixel",
//   "index":2,
//   "r":0,
//   "g":0,
//   "b":255}

// >>Apagar toda la cinta:
//   {"command":"turnOff"}

// */

// void loop() {
//   while (Serial.available() > 0) {
//     static String inputJson = "";
//     char c = Serial.read();

//     if (c == '\n') {
//       processJson(inputJson);
//       inputJson = "";
//     } else {
//       inputJson += c;
//     }
//   }
// }

// void processJson(String jsonText) {
//   JsonDocument doc;

//   DeserializationError error = deserializeJson(doc, jsonText);

//   if (error) {
//     Serial.print("JSON error: ");
//     Serial.println(error.c_str());
//     return;
//   }

//   const char* command = doc["command"];

//   if (strcmp(command, "setColor") == 0) {
//     int r = doc["r"];
//     int g = doc["g"];
//     int b = doc["b"];

//     strip.showColor(r, g, b);
//     Serial.println("Color updated");
//   }
//   else if (strcmp(command, "turnOff") == 0) {
//     strip.turnOff();
//     Serial.println("Strip off");
//   }
//   else if (strcmp(command, "pixel") == 0) {
//     int index = doc["index"];
//     int r = doc["r"];
//     int g = doc["g"];
//     int b = doc["b"];

//     strip.showPixel(index, r, g, b);
//     Serial.println("Pixel updated");
//   }
//   else {
//     Serial.println("Unknown command");
//   }
// }