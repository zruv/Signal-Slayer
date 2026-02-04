#ifndef ESP_NOW_COMMON_H
#define ESP_NOW_COMMON_H

#include <stdint.h>
#include "types.h" // Includes the OperationMode enum

// This is the MAC address of this slave device. The master needs this.
// You can find the MAC address by uploading a simple sketch that prints
// WiFi.macAddress() to the Serial Monitor.
// The master's code should use this address in its slaveAddress[] array.
uint8_t slaveAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Define a structure to hold the command data received over ESP-NOW
typedef struct struct_message {
    OperationMode mode;
    bool active;
} struct_message;

#endif // ESP_NOW_COMMON_H
