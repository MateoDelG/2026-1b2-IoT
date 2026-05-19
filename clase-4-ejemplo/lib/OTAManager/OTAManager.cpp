#include "OTAManager.h"

OTAManager::OTAManager() {
}

void OTAManager::begin(const char* hostname) {
    if (hostname != nullptr) {
        ArduinoOTA.setHostname(hostname);
    }

    ArduinoOTA.onStart([]() {
        Serial.println("Iniciando actualizacion OTA...");
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nActualizacion OTA finalizada");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progreso OTA: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error OTA [%u]: ", error);

        if (error == OTA_AUTH_ERROR) {
            Serial.println("Error de autenticacion");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Error al iniciar");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Error de conexion");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Error de recepcion");
        } else if (error == OTA_END_ERROR) {
            Serial.println("Error al finalizar");
        }
    });

    ArduinoOTA.begin();

    Serial.println("OTA inicializado");
}

void OTAManager::handle() {
    ArduinoOTA.handle();
}

bool OTAManager::connectDHCP(const char* ssid, const char* password) {
    Serial.println("Conectando WiFi con DHCP...");

    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setSleep(false);

    WiFi.begin(ssid, password);

    if (!waitForWiFi()) {
        return false;
    }

    printNetworkInfo();

    if (!testDNS("broker.emqx.io")) {
        Serial.println("WiFi conectado, pero DNS no funciona.");
        return false;
    }

    return true;
}

bool OTAManager::connectStatic(const char* ssid,
                               const char* password,
                               IPAddress localIp,
                               IPAddress gateway,
                               IPAddress subnet) {
    Serial.println("Conectando WiFi con IP estatica simplificada...");

    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setSleep(false);

    // En esta version simplificada, el DNS se toma como el gateway.
    IPAddress dns = gateway;

    Serial.print("IP estatica: ");
    Serial.println(localIp);

    Serial.print("Gateway: ");
    Serial.println(gateway);

    Serial.print("Mascara: ");
    Serial.println(subnet);

    Serial.print("DNS automatico usado: ");
    Serial.println(dns);

    if (!WiFi.config(localIp, gateway, subnet, dns)) {
        Serial.println("Error configurando IP estatica");
        return false;
    }

    WiFi.begin(ssid, password);

    if (!waitForWiFi()) {
        return false;
    }

    printNetworkInfo();

    if (!testDNS("broker.emqx.io")) {
        Serial.println("WiFi conectado, pero DNS no funciona.");
        return false;
    }

    return true;
}

bool OTAManager::waitForWiFi(unsigned long timeoutMs) {
    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeoutMs) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi conectado correctamente");
        return true;
    }

    Serial.println("No se pudo conectar a WiFi");
    return false;
}

bool OTAManager::testDNS(const char* host) {
    IPAddress resolvedIp;

    Serial.print("Probando DNS con: ");
    Serial.println(host);

    if (WiFi.hostByName(host, resolvedIp)) {
        Serial.print("DNS OK. ");
        Serial.print(host);
        Serial.print(" = ");
        Serial.println(resolvedIp);
        return true;
    }

    Serial.println("DNS FALLÓ");
    return false;
}

bool OTAManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void OTAManager::printNetworkInfo() {
    Serial.print("IP local: ");
    Serial.println(WiFi.localIP());

    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("Mascara: ");
    Serial.println(WiFi.subnetMask());

    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());

    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
}

IPAddress OTAManager::getLocalIP() {
    return WiFi.localIP();
}