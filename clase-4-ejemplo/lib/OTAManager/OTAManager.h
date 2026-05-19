#ifndef OTAManager_h
#define OTAManager_h

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAManager {
public:
    OTAManager();

    // Iniciar OTA
    void begin(const char* hostname = nullptr);

    // Mantener OTA activo en el loop
    void handle();

    // Conectar usando DHCP normal
    bool connectDHCP(const char* ssid, const char* password);

    // Conectar usando IP estatica simple
    // El DNS se toma automaticamente como el gateway
    bool connectStatic(const char* ssid,
                       const char* password,
                       IPAddress localIp,
                       IPAddress gateway,
                       IPAddress subnet);

    // Estado de conexion
    bool isConnected();

    // Mostrar datos de red
    void printNetworkInfo();

    IPAddress getLocalIP();

private:
    bool waitForWiFi(unsigned long timeoutMs = 20000);
    bool testDNS(const char* host);
};

#endif