#include "OTAManager.h"

OTAManager::OTAManager() : _hostname(nullptr), _connected(false) {
}

void OTAManager::begin(const char* hostname) {
    _hostname = hostname;
    
    if (_hostname != nullptr) {
        ArduinoOTA.setHostname(_hostname);
    }
    
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });
    
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        switch (error) {
            case OTA_AUTH_ERROR:
                Serial.println("Auth Failed");
                break;
            case OTA_BEGIN_ERROR:
                Serial.println("Begin Failed");
                break;
            case OTA_CONNECT_ERROR:
                Serial.println("Connect Failed");
                break;
            case OTA_RECEIVE_ERROR:
                Serial.println("Receive Failed");
                break;
            case OTA_END_ERROR:
                Serial.println("End Failed");
                break;
        }
    });
    
    ArduinoOTA.begin();
    Serial.println("OTA inicializado");
}

void OTAManager::handle() {
    ArduinoOTA.handle();
}

// Configuracion WiFi con IP estatica
void OTAManager::connect(const char* ssid, const char* password,
                         IPAddress localIp, IPAddress gateway, 
                         IPAddress subnet, IPAddress dns) {
    WiFi.mode(WIFI_STA);
    WiFi.config(localIp, gateway, subnet, dns);
    WiFi.begin(ssid, password);
    
    Serial.println("Conectando a WiFi...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Conexion fallida. Reintentando...");
        delay(5000);
        ESP.restart();
    }
    
    _connected = true;
    Serial.println("WiFi conectado. IP: " + WiFi.localIP().toString());
}

// Configuracion WiFi con IP dinamica (DHCP)
void OTAManager::connect(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.println("Conectando a WiFi...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Conexion fallida. Reintentando...");
        delay(5000);
        ESP.restart();
    }
    
    _connected = true;
    Serial.println("WiFi conectado. IP: " + WiFi.localIP().toString());
}