#include <Arduino.h>
#include <esp_wifi.h> // Required for setting the channel
/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/rfclown
   
   ESP-NOW Slave Receiver
   ________________________________________ */
   
#include "config.h"
#include "radio_settings.h"
#include "esp_now_common.h"

// Global Variables
OperationMode current_Mode = WiFi_MODULE;
bool current_Active = false;
volatile bool state_changed = true; // Flag to indicate a change from master
char dummy_payload[32] = "JAMMING";

// Using the FULL channel lists from the original headless version for max effect
const byte bluetooth_channels[] =        {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
const byte ble_channels[]       =        {2, 26, 80};
const byte WiFi_channels[]      =        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const byte usbWireless_channels[] =      {40, 50, 60};
const byte videoTransmitter_channels[] = {70, 75, 80};
const byte rc_channels[]        =        {1, 3, 5, 7};
const byte zigbee_channels[]    =        {11, 15, 20, 25};
const byte nrf24_channels[]     =        {76, 78, 79};

// --- ESP-NOW Callback ---

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  struct_message received_command;
  memcpy(&received_command, incomingData, sizeof(received_command));
  
  if (current_Mode != received_command.mode || current_Active != received_command.active) {
    current_Mode = received_command.mode;
    current_Active = received_command.active;
    state_changed = true; // Set flag that state has changed
  }
}

// --- Radio Functions ---

void initialize_Jammer() {
  if (current_Active) {
    configureNrf();
    radio.openWritingPipe(0xE8E8F0F0E1LL); // Set a dummy address for writing
    Serial.println(">> JAMMING STARTED (Command from Master) [AGGRESSIVE MODE] <<");
  } else {
    radio.powerDown();
    delay(100);
    Serial.println(">> JAMMING STOPPED (Command from Master) <<");
  }
}

// --- Main Setup and Loop ---

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n*** RF-Clown ESP-NOW Edition (SLAVE) ***");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Set a fixed channel to ensure both devices are on the same one
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Slave MAC: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register the callback function
  esp_now_register_recv_cb(OnDataRecv);

  // Radio Setup
  if (!setupRadio()) {
    Serial.println("\nFATAL: NRF24 radio not detected. Halting program.");
    while(1) { delay(100); }
  }
  
  initialize_Jammer();
  Serial.println("Waiting for command from Master...");
}

void loop() {
  if (state_changed) {
    state_changed = false;
    initialize_Jammer();
    Serial.print("New State Received -> Mode: ");
    Serial.print(current_Mode);
    Serial.print(", Active: ");
    Serial.println(current_Active);
  }

  if (current_Active) {
    // WiFi gets a special, more aggressive attack strategy
    if (current_Mode == WiFi_MODULE) {
      // Fast sequential sweep across all WiFi channels
      for (int i = 0; i < sizeof(WiFi_channels); i++) {
        radio.setChannel(WiFi_channels[i]);
        // Transmit a quick burst of packets on each channel
        for (int j = 0; j < 16; j++) {
          radio.write(&dummy_payload, sizeof(dummy_payload));
        }
      }
    } else {
      // Other modes use the original random-hopping strategy
      const byte* channels = nullptr;
      int channel_count = 0;
      
      switch(current_Mode) {
        case BLE_MODULE:          channels = ble_channels;          channel_count = sizeof(ble_channels); break;
        case Bluetooth_MODULE:    channels = bluetooth_channels;    channel_count = sizeof(bluetooth_channels); break;
        // WiFi_MODULE is handled above
        case USB_WIRELESS_MODULE: channels = usbWireless_channels;  channel_count = sizeof(usbWireless_channels); break;
        case VIDEO_TX_MODULE:     channels = videoTransmitter_channels; channel_count = sizeof(videoTransmitter_channels); break;
        case RC_MODULE:           channels = rc_channels;           channel_count = sizeof(rc_channels); break;
        case ZIGBEE_MODULE:       channels = zigbee_channels;       channel_count = sizeof(zigbee_channels); break;
        case NRF24_MODULE:        channels = nrf24_channels;        channel_count = sizeof(nrf24_channels); break;
        default: break;
      }

      if (channels && channel_count > 0) {
        byte channel = channels[random(0, channel_count)];
        radio.setChannel(channel);
        radio.write(&dummy_payload, sizeof(dummy_payload));
      }
    }
  } else {
    delay(100);
  }
}
