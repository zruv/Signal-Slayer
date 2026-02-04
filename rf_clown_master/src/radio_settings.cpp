/* 
 * Radio Settings for the Master ESP32
 */

#include "radio_settings.h"
#include "config.h"
#include <Arduino.h>

// Define Radio Object
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

bool configureNrf() {
  if (radio.begin()) {
     Serial.println("NRF24 Init Success");
     radio.setAutoAck(false);
     radio.stopListening();
     radio.setRetries(0, 0);
     radio.setPALevel(RF24_PA_MAX, true);
     radio.setDataRate(RF24_2MBPS);
     radio.setCRCLength(RF24_CRC_DISABLED);
     return true;
  } else {
     Serial.println("NRF24 Init FAILED! Check Wiring.");
     return false;
  }
}

bool setupRadio() {
  Serial.print("Initializing Radio... ");
  return configureNrf();
}
