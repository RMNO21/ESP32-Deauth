# ESP Deauth (Educational Project)
<img width="1280" height="757" alt="image" src="https://github.com/user-attachments/assets/e7bfb0f2-5364-456a-94c7-93c8dbbdab42" />



This repository provides a minimal ESP32-based hardware and software scaffold intended for education and experimentation in a controlled environment. It is designed for ethical, lawful, and responsible learning only.

Important: This project does not implement or demonstrate any real-world attack or unauthorized network access. It is a learning placeholder for hardware wiring, firmware structure, and basic ESP32 programming.

## Clone This Repository
To get started, clone this repository (RMNO21/ESP32-Deauth):
```
git clone https://github.com/RMNO21/ESP32-Deauth
```

## How to Use
This project is a hardware-software scaffold for ESP32. Below are common workflows to develop and upload firmware to an ESP32 Dev Kit V1 board.
![Circuit Diagram](image/circuit%20(1).png)

### PlatformIO (recommended)
Prerequisites: PlatformIO Core or PlatformIO IDE extension installed.
1. Create or integrate into a PlatformIO project with an ESP32 environment.
```
pio project init --board esp32dev
```
3. Create or update a root PlatformIO config (platformio.ini):
```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```
4. Build and upload:
```
pio run -e esp32dev
pio run -e esp32dev -t upload
```
5. Serial monitor (optional):
```
pio device monitor -p /dev/ttyUSB0
```

### ESP-IDF (alternative)
Prerequisites: ESP-IDF installed and configured.
1. Create/enter a project and build with ESP-IDF:
```
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

### Arduino IDE (alternative)
Prerequisites: ESP32 board support added to Arduino IDE.
1. Select board: ESP32 Dev Module; connect your ESP32 to the PC; upload via the IDE.

## What This Project Does
- Scan for nearby Wi-Fi access points (beacons) and deauthenticate a selected target.
- Optionally, deauthenticate all nearby Wi-Fi networks to disrupt all connections in range.
- Control the device via physical buttons or a web interface by connecting to the Wi-Fi network "ESP32-Deauth" (password: git-RMNO21) and navigating to the ESP32’s IP address: 192.168.4.1.

## Licensing
This project is licensed under the GNU General Public License v3.0 or later. See LICENSE for details.
See the MODULES.md for hardware details.
