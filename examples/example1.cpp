#include <Arduino.h>
#include "espota.hpp"
#include <WiFi.h>


const char* cert_pem = \
"-----BEGIN CERTIFICATE-----\n" \
"...\n" \
"-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  WiFi.begin("SSID", "PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFiClient cliente; 
  // Update by http
  ESPOTA ota(cliente,"example.com",80,"/firmware.bin");
  ota.doUpdate();
  // Update by https
  ESPOTA ota2("https://example.com/firmware.bin",cert_pem);
  ota2.doUpdate();
}

void loop() {
    Serial.println("...");
    delay(3000);
}