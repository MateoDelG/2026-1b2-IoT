#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

 //Wifi
const char* WiFiSSID = "Delga1213";
const char* WiFiPassword = "kike4325";

//LED
#define LED_PIN 2

//Gmini
const char* API_KEY = "AIzaSyDtec7YxNiWlF5GSUBp7sXtgXZr0qTqYis";
const char* GEMINI_MODEL = "gemini-2.5-flash";

String GEMINI_URL = String("https://generativelanguage.googleapis.com/v1beta/models/") 
                  + GEMINI_MODEL
                  + ":generateContent?key=" 
                  + API_KEY;

const char* SYSTEM_PROMPT =
  R"(Eres un clasificador de comandos para un ESP32.

  Debes responder exactamente una de estas tres etiquetas:
  LED_ON
  LED_OFF
  NONE

  Reglas:
  - Si el usuario quiere prender, encender, activar o poner en ON un LED o una luz, responde LED_ON.
  - Si el usuario quiere apagar, desactivar o poner en OFF un LED o una luz, responde LED_OFF.
  - Si el mensaje no es un comando claro sobre el LED, responde NONE.

  No escribas explicaciones.
  No uses comillas.
  No uses puntos.
  No agregues texto adicional.)";


void conectar_wifi();
String createGeminiRequest(String userText);
String askGemini (String userText);


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  conectar_wifi();
}

void loop() {
  if(Serial.available() > 0) {
    String userText = Serial.readStringUntil('\n');
    String command = askGemini(userText);

    if (command == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED encendido.");
    } else if (command == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED apagado.");
    } else if (command == "NONE") {
      Serial.println("Comando no reconocido.");
    } else {
      Serial.println("Error al procesar el comando.");
    }
  }
}

void conectar_wifi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WiFiSSID, WiFiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("¡Conectado!");
}


/*
{
  "systemInstruction": {
    "role": "system",
    "parts": [
      {
        "text": "CONTENIDO_DE_SYSTEM_PROMPT"
      }
    ]
  },
  "contents": [
    {
      "role": "user",
      "parts": [
        {
          "text": "TEXTO_DEL_USUARIO"
        }
      ]
    }
  ],
  "generationConfig": {
    "maxOutputTokens": 30,
    "temperature": 0,
    "responseMimeType": "text/x.enum",
    "responseSchema": {
      "type": "STRING",
      "enum": [
        "LED_ON",
        "LED_OFF",
        "NONE"
      ]
    },
    "thinkingConfig": {
      "thinkingLevel": "MINIMAL",
      "includeThoughts": false
    }
  }
}
*/
String createGeminiRequest(String userText) {
  JsonDocument doc;

  JsonObject systemInstruction = doc["systemInstruction"].to<JsonObject>();
  systemInstruction["role"] = "system";

  JsonArray systemParts = systemInstruction["parts"].to<JsonArray>();
  JsonObject systemText = systemParts.add<JsonObject>();
  systemText["text"] = SYSTEM_PROMPT;

  JsonArray contents = doc["contents"].to<JsonArray>();
  JsonObject userMessage = contents.add<JsonObject>();
  userMessage["role"] = "user";

  JsonArray userParts = userMessage["parts"].to<JsonArray>();
  JsonObject userPart = userParts.add<JsonObject>();
  userPart["text"] = userText;

  JsonObject generationConfig = doc["generationConfig"].to<JsonObject>();
  generationConfig["maxOutputTokens"] = 50;
  generationConfig["temperature"] = 0;
  generationConfig["responseMimeType"] = "text/x.enum";

  JsonObject responseSchema = generationConfig["responseSchema"].to<JsonObject>();
  responseSchema["type"] = "STRING";

  JsonArray options = responseSchema["enum"].to<JsonArray>();
  options.add("LED_ON");
  options.add("LED_OFF");
  options.add("NONE");

  //valido para Gemini 2.5
  JsonObject thinkingConfig = generationConfig["thinkingConfig"].to<JsonObject>();
  thinkingConfig["thinkingBudget"] = 0;
  
  //valido para Gemini 3.5
  // JsonObject thinkingConfig = generationConfig["thinkingConfig"].to<JsonObject>();
  // thinkingConfig["thinkingLevel"] = "MINIMAL";
  // thinkingConfig["includeThoughts"] = false;

  String jsonText;
  serializeJson(doc, jsonText);

  return jsonText;
}

String askGemini (String userText) {
  WiFiClientSecure client;

  client.setInsecure(); // No validar el certificado SSL, aceptar cualquier certificado (¡INSEGURO!)

  HTTPClient https;
  // Iniciar la conexión HTTP con el servidor de Gemini
  if(!https.begin(client, GEMINI_URL)) {
    Serial.println("Error al iniciar la conexión HTTP");
    return "ERROR";
  }

  // Configurar la solicitud HTTP POST
  https.addHeader("Content-Type", "application/json");

  // Crear el cuerpo de la solicitud con el mensaje del usuario
  String requestBody = createGeminiRequest(userText);

  // Enviar la solicitud POST y obtener el código de respuesta
  int httpResponseCode = https.POST(requestBody);

  // Verificar la respuesta del servidor
  String response = https.getString();

  // Cerrar la conexión HTTP
  https.end();

  Serial.println("Código de respuesta HTTP: " + String(httpResponseCode));

  // Si la respuesta es exitosa (código 200), imprimir la respuesta de Gemini
  if (httpResponseCode == 200) {
    Serial.println("Respuesta de Gemini: " + response);
    } 
  else {
    return "NONE";
  }

  JsonDocument responseDoc;
  DeserializationError error = deserializeJson(responseDoc, response);
  if (error) {
    Serial.print("Error al parsear la respuesta JSON: ");
    Serial.println(error.c_str());
    return "ERROR";
  }

  const char* generatedText = responseDoc["candidates"][0]["content"]["parts"][0]["text"];

  // Verificamos que realmente se haya encontrado texto.
  // Si Gemini no devuelve texto, mostramos la respuesta completa para revisar qué pasó.
  if (!generatedText) {
    Serial.println("Gemini no devolvió texto.");
    Serial.println(response);
    return "NONE";
  }

  // Convertimos el texto recibido a un objeto String de Arduino.
  // Esto facilita limpiarlo y modificarlo.
  String answer = String(generatedText);

  // Limpiamos la respuesta para quedarnos solo con la etiqueta.
  answer.trim();
  // Convertimos la respuesta a mayúsculas para evitar problemas de comparación.
  answer.toUpperCase();
  // Eliminamos caracteres no deseados que podrían venir en la respuesta.
  answer.replace("\n", "");
  answer.replace("\r", "");
  answer.replace("\"", "");
  answer.replace(".", "");
  answer.replace(" ", "");

  Serial.print("Respuesta de Gemini: ");
  Serial.println(answer);

  if (answer != "LED_ON" && answer != "LED_OFF" && answer != "NONE") {
    Serial.print("Respuesta inválida de Gemini: ");
    Serial.println(answer);
    return "NONE";
  }

  return answer;
}