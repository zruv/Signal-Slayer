#include <Arduino.h> // Good practice for PlatformIO
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA); // Ensure WiFi is initialized to get MAC
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Nothing needed in loop for this example
}
