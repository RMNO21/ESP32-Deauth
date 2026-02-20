# ESP32-Deauth (Educational Project)

This repository contains an ESP32-based deauth tool for **educational and testing use only**. It combines:

- An OLED-based menu controlled by buttons on the ESP32
- A built-in web interface for triggering attacks from a browser
- Support for deauthing one, some, or all visible Wi-Fi networks using the same core logic

![Circuit Diagram](image/circuit%20(1).png)

Read the [DISCLAIMER](DISCLAIMER.md) carefully before using this project.

## Features

- Scans for nearby Wi-Fi networks (SSIDs, BSSIDs, channel, RSSI, encryption)
- Deauth modes:
  - Single network (Deauth one)
  - Multiple selected networks (Deauth some)
  - All detected networks (Deauth all)
- Two control interfaces:
  - OLED + buttons on the ESP32
  - Web interface (HTTP) via ESP32 access point
- Basic battery level display on the OLED

## Hardware Overview

See [MODULES.md](MODULES.md) for a short list of required modules.

Typical setup:

- ESP32 DevKit V1
- SSD1306 128x64 I2C OLED display
- 4 push buttons (UP, DOWN, OK, BACK)
- Optional Li-ion battery with power switch

## Getting Started

### Clone the repository

```bash
git clone https://github.com/RMNO21/ESP32-Deauth.git
cd ESP32-Deauth
```

### Arduino IDE (recommended)

1. Install the **ESP32** board support in Arduino IDE.
2. Install libraries:
   - Adafruit SSD1306
   - Adafruit GFX
3. Open `src/main.cpp` as an Arduino sketch or create a sketch and copy the project files.
4. Select board: **DOIT ESP32 DEVKIT V1** (or matching ESP32 board).
5. Select the correct COM port for your ESP32.
6. Compile and upload.

By default the ESP32 starts an AP:

- SSID: `ESP32-Deauth`
- Password: `git-RMNO21`

Connect to this AP and open `http://192.168.4.1/` in a browser to access the web interface.

### Using the OLED menu

- On boot, the OLED shows a menu:
  - Deauth one
  - Deauth some
  - Deauth All
  - Battery
- Use the buttons:
  - UP / DOWN: move between menu items
  - OK: select the current item
  - BACK: stop an attack or go back

**Deauth one**

- Scans networks
- Lets you scroll and pick a single target
- Runs a deauth attack on that network

**Deauth some**

- Scans networks
- Use OK to toggle selection (`*` next to SSID)
- Press BACK to start deauthing selected networks one by one

**Deauth All**

- Starts deauthing all detected networks across channels

**Battery**

- Displays estimated battery percentage based on analog battery input

### Using the Web Interface

When connected to the `ESP32-Deauth` AP, open `http://192.168.4.1/`:

- See a table of detected Wi-Fi networks
- Launch deauth on a single network by index
- Launch deauth on all networks
- Use the "Deauth Some Networks" section:
  - Select multiple networks via checkboxes
  - Provide a reason code
  - Deauth each selected network sequentially

## Legal and Ethical Use

- This project is for **educational and defensive** purposes only.
- Do not use against networks you do not own or have explicit permission to test.
- You are solely responsible for ensuring compliance with all applicable laws.

See [DISCLAIMER.md](DISCLAIMER.md) for more detail.

## License

This project is licensed under the GNU General Public License v3.0 or later. See [LICENSE](LICENSE) for details.
