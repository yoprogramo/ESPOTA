/* espota.hpp
 * Support class to OTA operation on ESP
 * Biblioeteca Technologies SL 2022
 * author: Jose Antonio Espinosa (yoprogramo@gmail.com)
 */

#ifndef ESP_OTA_H
#define ESP_OTA_H
#ifdef ARDUINO_ARCH_ESP32
#include <Arduino.h>
#include <WiFiClient.h>

class ESPOTA {
    public:
        ESPOTA(WiFiClient wifi, const char *host, int port, const char *firmware_path);
        ESPOTA(const char *url, const char *cert);
        void doUpdate();
        void setFirmwarePath(const char *firmware_path);
        void setPort(int port);
        void setHost(const char *host);
        void setUrl(const char *url);
        void setCert(const char *cert);

    private:
        bool https_mode;
        const char *url;
        const char *cert;
        const char *firmware_path;
        int port;
        const char *host;
        WiFiClient wifi;
        void doHttpUpdate();
        void doHttpsUpdate();
};

#endif
#endif