# RF-Clown ESP-NOW PlatformIO

**DISCLAIMER:** This project is for educational and research purposes only. The use of this software for jamming or interfering with wireless communications may be illegal in your country. I am not responsible for any misuse of this software. Please use it responsibly and in accordance with your local laws.

## Description

RF-Clown is a master-slave signal jamming system that uses ESP32 microcontrollers, ESP-NOW for communication, and NRF24L01 modules for RF signal generation. The system consists of a master controller and one or more slave devices that can be remotely controlled to perform jamming on various frequency bands.

This project is an implementation of the RF-Clown concept using the PlatformIO IDE.

## Features

*   Master-slave architecture for synchronized, multi-point jamming.
*   Remote control of slave devices via ESP-NOW.
*   Serial monitor-based menu for controlling the master device.
*   Multiple jamming modes for different frequency bands (WiFi, Bluetooth, RC, etc.).
*   "Aggressive" jamming mode for WiFi.

## Directory Structure

*   `esp32-mac-address/`: A simple PlatformIO project to get the MAC address of your ESP32 board.
*   `rf_clown_master/`: The PlatformIO project for the master controller device.
*   `rf_clown_slave/`: The PlatformIO project for the slave device.

## Hardware Requirements

*   2 x ESP32 development boards
*   2 x NRF24L01 modules
*   Jumper wires

### Wiring (for both master and slave)

Connect the NRF24L01 module to your ESP32 as follows:

| NRF24L01 | ESP32 |
| :--- | :--- |
| VCC | 3.3V |
| GND | GND |
| CE | GPIO5 |
| CSN | GPIO17 |
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |

## Software Requirements

*   [PlatformIO](https://platformio.org/)
*   [nrf24/RF24 library](https://github.com/nRF24/RF24) (will be installed automatically by PlatformIO)

## Setup and Usage

1.  **Get Slave MAC Address:**
    *   Open the `esp32-mac-address/` project in PlatformIO.
    *   Build and upload the project to your **slave** ESP32.
    *   Open the Serial Monitor (speed 115200) and copy the MAC address.

2.  **Configure Master:**
    *   Open the `rf_clown_master/` project in PlatformIO.
    *   Open the `src/esp_now_common.h` file.
    *   Replace the placeholder MAC address in the `slaveAddress` array with the MAC address of your slave device.
    *   Save the file.

3.  **Upload Firmware:**
    *   Build and upload the `rf_clown_master/` project to your **master** ESP32.
    *   Build and upload the `rf_clown_slave/` project to your **slave** ESP32.

4.  **Operate the System:**
    *   Open the Serial Monitor for the **master** device (speed 115200).
    *   You will see a menu with options to control the jamming modes.
    *   Use the following keys to control the master:
        *   `n`: Next mode
        *   `p`: Previous mode
        *   `t`: Toggle jamming (active/deactive)

The slave device will automatically receive commands from the master and start/stop jamming accordingly. You can monitor the slave's status by opening a Serial Monitor for the slave device.
