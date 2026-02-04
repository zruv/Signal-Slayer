#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <esp_now.h>
#include <WiFi.h>

// nRF24-specific Pins (Common SPI)
// MISO: 19
// MOSI: 23
// SCK:  18
// NOTE: Use different CE/CSN pins if on the same breadboard as master
// to avoid signal crosstalk, even though they are separate MCUs.
#define NRF_CE_PIN    5   
#define NRF_CSN_PIN   17 

// Global state variables, controlled by master
extern OperationMode current_Mode;
extern bool current_Active;

// Channel definitions
// These are the channels the SLAVE will jam.
extern const byte bluetooth_channels[];
extern const byte ble_channels[];
extern const byte WiFi_channels[];
extern const byte usbWireless_channels[];
extern const byte videoTransmitter_channels[];
extern const byte rc_channels[];
extern const byte zigbee_channels[];
extern const byte nrf24_channels[];

#endif // CONFIG_H
