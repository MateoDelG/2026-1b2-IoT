#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

// ===============================
// 1. DATOS DE WIFI
// ===============================

const char* WIFI_NAME = "Delga1213";
const char* WIFI_PASSWORD = "kike4325";

// ===============================
// 2. DATOS DE GEMINI
// ===============================

const char* API_KEY = "AIzaSyCyTQ19IEx9-0N1tkmCykhJ3ZK8HdCSo-k";
const char* GEMINI_MODEL = "gemini-3.5-flash";
String GEMINI_URL = String("https://generativelanguage.googleapis.com/v1beta/models/") +
                    GEMINI_MODEL +
                    ":generateContent?key=" +
                    API_KEY;

// ===============================
// 3. LED
// ===============================

const int LED_PIN = 2;

// ===============================
// 4. PROMPT PARA GEMINI
// ===============================

// const char* SYSTEM_PROMPT =
//   R"(Eres un clasificador de comandos para un ESP32.

//   Debes responder exactamente una de estas tres etiquetas:
//   LED_ON
//   LED_OFF
//   NONE

//   Reglas:
//   - Si el usuario quiere prender, encender, activar o poner en ON un LED o una luz, responde LED_ON.
//   - Si el usuario quiere apagar, desactivar o poner en OFF un LED o una luz, responde LED_OFF.
//   - Si el mensaje no es un comando claro sobre el LED, responde NONE.

//   No escribas explicaciones.
//   No uses comillas.
//   No uses puntos.
//   No agregues texto adicional.)";

const char* SYSTEM_PROMPT =
R"(Eres un clasificador de comandos para un ESP32 que controla un LED.

Tu única tarea es clasificar la intención del usuario.

Debes responder exactamente una de estas tres etiquetas:
LED_ON
LED_OFF
NONE

Reglas:
- Responde LED_ON si el usuario quiere encender, prender, activar, iluminar o tener más luz.
- Responde LED_OFF si el usuario quiere apagar, quitar luz, reducir luz, tener menos luz o dejar oscuro.
- Responde NONE si el mensaje no tiene una intención clara relacionada con controlar el LED.

Ejemplos:
Usuario: prende el led
Respuesta: LED_ON

Usuario: enciende la luz
Respuesta: LED_ON

Usuario: quiero más luz
Respuesta: LED_ON

Usuario: apaga el led
Respuesta: LED_OFF

Usuario: quiero menos luz
Respuesta: LED_OFF

Usuario: quita la luz
Respuesta: LED_OFF

Usuario: hola
Respuesta: NONE

No expliques nada.
No escribas frases.
No uses comillas.
No agregues puntos.
No agregues texto adicional.)";

// ===============================
// 5. CONECTAR A WIFI
// ===============================

void connectToWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===============================
// 6. CREAR JSON PARA GEMINI
// ===============================
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
  // JsonObject thinkingConfig = generationConfig["thinkingConfig"].to<JsonObject>();
  // thinkingConfig["thinkingBudget"] = 0;
  
  //valido para Gemini 3.5
  JsonObject thinkingConfig = generationConfig["thinkingConfig"].to<JsonObject>();
  thinkingConfig["thinkingLevel"] = "MINIMAL";
  thinkingConfig["includeThoughts"] = false;

  String jsonText;
  serializeJson(doc, jsonText);

  return jsonText;
}

// ===============================
// 7. PREGUNTAR A GEMINI
// ===============================

String askGemini(String userText) {
  // Creamos un cliente seguro para conectarnos a servidores HTTPS.
  // Gemini usa una URL que empieza por https://, por eso necesitamos WiFiClientSecure.
  WiFiClientSecure client;

  // Esta línea permite hacer la conexión HTTPS sin validar el certificado del servidor.
  // Es útil para pruebas y clases, porque simplifica la conexión.
  // En proyectos reales lo ideal es validar el certificado correctamente.
  client.setInsecure();

  // Creamos un objeto HTTPClient.
  // Este objeto nos permite hacer solicitudes HTTP o HTTPS,
  // por ejemplo enviar datos con POST.
  HTTPClient https;

  // Iniciamos la conexión con la URL de Gemini.
  // GEMINI_URL contiene la dirección del modelo y la API Key.
  // Si no se puede iniciar la conexión, devolvemos "NONE"
  // para evitar que el ESP32 haga una acción incorrecta.
  if (!https.begin(client, GEMINI_URL)) {
    Serial.println("No se pudo conectar con Gemini.");
    return "NONE";
  }

  // Indicamos que el contenido que vamos a enviar está en formato JSON.
  // Esto es necesario porque la API de Gemini espera recibir los datos así.
  https.addHeader("Content-Type", "application/json");

  // Creamos el cuerpo de la solicitud.
  // Esta función convierte el texto del usuario en un JSON válido para Gemini.
  // Por ejemplo, si el usuario escribe "enciende el led",
  // aquí se construye la solicitud completa para enviarla a la IA.
  String requestBody = createGeminiRequest(userText);

  // Enviamos la solicitud a Gemini usando el método POST.
  // El resultado se guarda en httpCode.
  // Si todo sale bien, normalmente httpCode debe ser 200.
  int httpCode = https.POST(requestBody);

  // Leemos la respuesta completa que envía Gemini.
  // Esta respuesta viene en formato JSON.
  String response = https.getString();

  // Cerramos la conexión HTTPS.
  // Esto libera recursos del ESP32.
  https.end();

  // Mostramos el código HTTP en el monitor serial.
  // Sirve para saber si la solicitud fue exitosa o si ocurrió un error.
  Serial.print("Código HTTP: ");
  Serial.println(httpCode);

  // Si el código HTTP no es 200, significa que la solicitud falló.
  // En ese caso mostramos la respuesta del servidor para depurar.
  if (httpCode != 200) {
    Serial.println("Error en la solicitud:");
    Serial.println(response);
    return "NONE";
  }

  // Creamos un documento JSON para guardar la respuesta de Gemini
  // de una forma que Arduino pueda leer fácilmente.
  JsonDocument responseDoc;

  // Convertimos el texto JSON recibido en un objeto JSON.
  // Si la respuesta no tiene formato JSON válido, se genera un error.
  DeserializationError error = deserializeJson(responseDoc, response);

  // Verificamos si hubo un error al leer el JSON.
  // Si hubo error, mostramos el problema y devolvemos "NONE".
  if (error) {
    Serial.print("Error leyendo JSON: ");
    Serial.println(error.c_str());
    return "NONE";
  }

  // Extraemos el texto generado por Gemini.
  // La respuesta normalmente viene dentro de esta estructura:
  //
  // candidates[0] -> primera respuesta generada
  // content       -> contenido de esa respuesta
  // parts[0]      -> primera parte del contenido
  // text          -> texto generado por Gemini
  //
  // Esperamos que ese texto sea algo como:
  // LED_ON, LED_OFF o NONE.
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

answer.trim();
answer.toUpperCase();

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

// ===============================
// 8. EJECUTAR RESPUESTA
// ===============================

void executeCommand(String command) {
  if (command == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED encendido.");
  }
  else if (command == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED apagado.");
  }
  else {
    Serial.println("Ninguna acción tomada.");
  }
}

// ===============================
// 9. SETUP
// ===============================

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  connectToWiFi();

  Serial.println();
  Serial.println("Sistema listo.");
  Serial.println("Escribe un comando en el monitor serial:");
  Serial.println("Ejemplo: encender led");
  Serial.println("Ejemplo: apagar led");
}

// ===============================
// 10. LOOP
// ===============================

void loop() {
  if (Serial.available()) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();

    if (userInput.length() == 0) {
      return;
    }

    Serial.println();
    Serial.print("Usuario escribió: ");
    Serial.println(userInput);

    String command = askGemini(userInput);

    executeCommand(command);
  }
}