#include <Arduino.h>
#include "ArduinoJson.h"
#include "UbidotsEsp32Mqtt.h"

//Credenciales de red
const char *WIFI_SSID = "Delga1213";
const char *WIFI_PASS = "kike4325";

//Credenciales Ubidots
const char *UBIDOTS_TOKEN = "BBUS-k2DesIYqrGRk5133NrNl748KpgD6Nv";
const char *DEVICE_LABEL = "clase-iot";
const char *VARIABLE_ADC = "adc";
const char *VARIABLE_LED = "led";

Ubidots ubidots(UBIDOTS_TOKEN);

#define SENSOR_PIN 34
#define LED_PIN 2

void reconectarUbidots();
void publishADC();
void callback(char *topic, byte *payload, unsigned int length);
void controlLED(String msj);

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    ubidots.setDebug(true);
    ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
    ubidots.setup();
    ubidots.setCallback(callback);
}

void loop() {
    if(!ubidots.connected()){
        reconectarUbidots();
    }
    publishADC();


    ubidots.loop();
}

void reconectarUbidots(){
    Serial.println("Reconectando Ubidots...");
    ubidots.reconnect();

    bool suscribe = ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LED);
    Serial.println(suscribe ? "Ok" : "Error");
    delay(500);
}

void publishADC(){
    static unsigned long timerToPublish = millis();
    static int publishFrequency = 15000;

    if(millis() > timerToPublish + publishFrequency){
        Serial.println("Enviando valores a la variable ADC");
        ubidots.add(VARIABLE_ADC, analogRead(SENSOR_PIN));
        
        bool publish = ubidots.publish(DEVICE_LABEL);

        Serial.println("Verificación del mensaje enviado:");

        if(publish){
            Serial.println("Ok");
        }
        else{
            Serial.println("Error");
        }

        bool suscribe = ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LED);
        Serial.println(suscribe ? "Ok" : "Error");

        timerToPublish = millis();
        
    }
}

void callback(char *topic, byte *payload, unsigned int length){
    String mensaje = "";

    for (unsigned int i = 0; i < length; i++){
        mensaje += (char)payload[i];
    }
    mensaje.trim();

    Serial.println("Topic: " + String(topic));
    Serial.println("Mensaje: " + mensaje);

    controlLED(mensaje);
}

void controlLED(String msj){
    int control = msj.toInt();

    if(control == 1){
        Serial.println("LED: ON");
        digitalWrite(LED_PIN, HIGH);
    }
    else{
        Serial.println("LED: OFF");
        digitalWrite(LED_PIN, LOW);
    }
}