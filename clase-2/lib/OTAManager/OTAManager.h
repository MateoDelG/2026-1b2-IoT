#ifndef OTAManager_h
#define OTAManager_h

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAManager {
public:
    OTAManager();
    void begin(const char* hostname = nullptr);
    void handle();

    // Configuracion WiFi con IP estatica
    void connect(const char* ssid, const char* password,
                 IPAddress localIp, IPAddress gateway, 
                 IPAddress subnet, IPAddress dns);

    // Configuracion WiFi con IP dinamica (DHCP)
    void connect(const char* ssid, const char* password);

    IPAddress getLocalIP() { return WiFi.localIP(); }

private:
    const char* _hostname;
    bool _connected;
};

#endif