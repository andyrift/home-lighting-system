# Home Lighting System
Simple esp32-based lighting system for a home

## Master module

**Components:**

- ESP32 board

- HC-06 bluetooth module

**Features:**

- Persistent configuration *(power loss does not erase configs)*

- Bluetoth communication

  A user can:

  - Set WiFi credentials
  - Set hot and cold channel intensity
  - Request current configuration

- Connect to home WiFi *(in parallel with being paired to a smartphone with bluetooth)*

- Sync time on startup and every day after that, using [http://worldtimeapi.org/]()

- Multicast UDP packets with commands

## Slave module

**Components:**

- ESP32 board

- Lighting control circuit *(custom)*

- Light switch

- Mode switch

**Features**

- Persistent configuration

- Can boot in one of two modes, depending on the *mode switch* position:

  - Setup mode
  - Working mode

- **Setup Mode**

  - Host a WiFi access point
  - Host HTTP server with a configuration web page
  - Recieve and save confiuration

- **Working Mode**

  - Connect to home WiFi network
  - Recieve multicast UDP packets with commands and change configuration depending on the commands
  - Smooth light channel intensity change
  - Smooth light switch on and off

## Pictures

[Pictures markdown](/pictures.md)