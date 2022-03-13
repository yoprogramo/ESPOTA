
/* espota.cpp
 * Support class to OTA operation on ESP
 * Biblioeteca Technologies SL 2022
 * author: Jose Antonio Espinosa (yoprogramo@gmail.com)
 */
#ifdef ARDUINO_ARCH_ESP32

#include "espota.hpp"
#include <Update.h>
#include <HttpsOTAUpdate.h>

void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

ESPOTA::ESPOTA(WiFiClient wifi, const char *host, int port, const char *firmware_path) {
  this->firmware_path = firmware_path;
  this->port = port;
  this->host = host;
  this->wifi = wifi;
  this->https_mode = false;
}

ESPOTA::ESPOTA(const char *url, const char *cert) {
  this->https_mode = true;
  this->url = url;
  this->cert = cert;
  this->https_mode = true;
}

void ESPOTA::setFirmwarePath(const char *firmware_url) {
  this->firmware_path = firmware_path;
}

void ESPOTA::setHost(const char *host) {
  this->host = host;
}

void ESPOTA::setPort(int port) {
  this->port = port;
}

void ESPOTA::setUrl(const char *url) {
  this->url = url;
}

void ESPOTA::setCert(const char *cert) {
  this->cert = cert;
}

void ESPOTA::doUpdate() {
  if (https_mode) {
    doHttpsUpdate();
  } else {
    doHttpUpdate();
  }
}

void ESPOTA::doHttpsUpdate() {
    HttpsOTAStatus_t otastatus;
    Serial.println("Starting HTTPS update");
    HttpsOTA.onHttpEvent(HttpEvent);
    Serial.println("Starting OTA");
    HttpsOTA.begin(url, cert); 
    Serial.println("Please Wait it takes some time ...");
    do {
        otastatus = HttpsOTA.status();
        if(otastatus == HTTPS_OTA_FAIL) { 
                Serial.println("Firmware Upgrade Fail");
                return;
            }
    } while (otastatus != HTTPS_OTA_SUCCESS);
    ESP.restart();
}

void ESPOTA::doHttpUpdate() {
    bool isValidContentType = false;
    int contentLength = 0;

    Serial.print("Connecting to: ");
    Serial.println(this->host);
    if (wifi.connect(this->host,this->port)) {
        Serial.println("Connected to server");
        Serial.print("Requesting: ");
        Serial.println(this->firmware_path);
        wifi.print(String("GET ") + this->firmware_path + " HTTP/1.1\r\n" +
                   "Host: " + this->host + "\r\n" +
                   "Cache-Control: no-cache\r\n" +
                   "Connection: close\r\n\r\n");
        String line = "";
        while (wifi.connected()) {
            yield();
            line = wifi.readStringUntil('\n');
            Serial.println(line);
            if (line.startsWith("Content-Type:")) {
                isValidContentType = line.indexOf("application/octet-stream") > 0;
            }
            if (line.startsWith("Content-Length:")) {
                contentLength = line.substring(16).toInt();
            }
            if (line == "\r") {
                break;
            }
        }
        if (isValidContentType) {
            Serial.println ("Starting OTA process");
            if (Update.begin(contentLength)) {
                Update.writeStream(wifi);
                if (Update.end()) {
                    if (Update.isFinished()) {
                        Serial.println("OTA process finished");
                        Serial.println("Rebooting...");
                        ESP.restart();
                    } else {
                        Serial.println("OTA process failed");
                    }
                } else {
                    Serial.println("Error during OTA update");
                }
                Serial.printf("    ERROR: %d", Update.getError());
            } else {
                 Serial.println("Not enough space available");
            }

        } else {
            Serial.println("Invalid content type");
        }
    } else {
        Serial.println("Connection failed");
    }
    wifi.flush();
}

#endif


