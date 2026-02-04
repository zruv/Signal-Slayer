#ifndef ESP_NOW_COMMON_H
#define ESP_NOW_COMMON_H

#include <stdint.h>
#include "types.h" // Includes the OperationMode enum

// IMPORTANT: REPLACE WITH THE MAC ADDRESS OF YOUR SLAVE ESP32
// You can find the MAC address of your slave by uploading a simple sketch
// that prints WiFi.macAddress() to the Serial Monitor.
uint8_t slaveAddress[] = {0xD4, 0xE9, 0xF4, 0xE1, 0xAC, 0xF0};

// Define a structure to hold the command data sent over ESP-NOW
typedef struct struct_message {
    OperationMode mode;
    bool active;
} struct_message;

#endif // ESP_NOW_COMMON_H