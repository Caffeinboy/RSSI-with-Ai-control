# RSSI-Based Smart Relay Control System with AI Crowd Monitoring

An ESP32-based dual-node automation system using WiFi RSSI, Blynk IoT, OLED monitoring, and relay control with AI-based people detection support.

This project uses two ESP32 modules:

- **Publisher (Transmitter)** → Monitors WiFi RSSI, manages system activation, sends relay selection, AI status, and people count to Blynk
- **Receiver** → Receives Blynk virtual pin updates, controls 5 relays, and displays live system status on OLED

The system automatically enables/disables relay control based on WiFi signal strength (RSSI threshold) and supports AI crowd monitoring integration using Virtual Pins V5 and V6.

---

# Features

## Publisher (Transmitter)

- WiFi RSSI monitoring
- Automatic system activation using RSSI threshold
- Blynk Virtual Pin control
- OLED live status display
- NTP real-time clock display
- Boot animation on OLED
- WiFi connection animation
- Non-blocking WiFi + Blynk reconnect logic
- AI status publishing
- Headcount publishing
- Serial Monitor debugging

## Receiver

- Receives Blynk virtual pin updates
- Controls 5 relay outputs
- OLED live monitoring display
- NTP real-time clock
- AI status display
- Headcount display
- WiFi + Blynk connection monitoring
- Auto-sync after reconnect
- Non-blocking reconnect logic
- Serial Monitor debugging

---

# Virtual Pins Used

| Virtual Pin | Function |
|---|---|
| V3 | Relay selection (1–5) |
| V4 | System Active / Inactive |
| V5 | AI Detection Status |
| V6 | Head Count from AI |

---

# RSSI Logic

System activation depends on WiFi signal strength:

RSSI > -60 dBm  → System ACTIVE  
RSSI <= -60 dBm → System INACTIVE + Slider Reset

When the signal becomes weak:

- System deactivates
- Slider resets to 0
- Receiver turns OFF all relays

This prevents operation outside the defined WiFi range.

---

# Hardware Used

- ESP32 Dev Board × 2
- 0.96" I2C OLED Display (SSD1306)
- 5-Channel Relay Module
- WiFi Router
- Blynk IoT Platform
- Optional AI Camera / Python Detection System

---

# OLED Pin Connections

## OLED → ESP32

| OLED | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

# Relay Connections

## Receiver ESP32 Relay Pins

| Relay | GPIO |
|---|---|
| Relay 1 | GPIO 5 |
| Relay 2 | GPIO 18 |
| Relay 3 | GPIO 19 |
| Relay 4 | GPIO 23 |
| Relay 5 | GPIO 25 |

---

# Libraries Required

Install from Arduino Library Manager:

- WiFi
- Blynk
- Adafruit GFX Library
- Adafruit SSD1306
- Wire
- time.h (built-in)

---

# Blynk Setup

Create a Blynk Template with:

Template Name: `publisher`

Create the following widgets:

- Slider → V3
- Switch / Display → V4
- AI Status → V5
- Head Count → V6

Use the generated:

- `BLYNK_TEMPLATE_ID`
- `BLYNK_TEMPLATE_NAME`
- `BLYNK_AUTH_TOKEN`

inside both ESP32 codes.

---

# Important Notes

## Use Same Auth Token

Both Publisher and Receiver must use the **same Blynk Auth Token** for real-time sync.

Using separate tokens causes delayed updates and unstable synchronization.

---

## NTP Time

Both ESP32 boards use NTP:

configTime(19800, 0, "pool.ntp.org");

This provides real-time clock display on OLED.

---

## Recommended Upload Settings

For stable upload:

- Board: ESP32 Dev Module
- Upload Speed: 115200
- Flash Frequency: 80MHz
- Partition Scheme: Default

Avoid high upload speeds like:

921600

This may cause:

A fatal error occurred: The chip stopped responding

---

# Project Flow

Publisher:  
WiFi RSSI → V4 Logic → V3/V5/V6 → Blynk Cloud

↓

Receiver:  
Blynk Cloud → Receive V3/V4/V5/V6 → Relay Control + OLED Display

---

# Serial Monitor Output Example

WiFi Connected  
Blynk Connected  
V4 ACTIVE  
V3 Updated: 3  
AI: ON  
People Count: 12  
Relays updated  
System Ready

---

# Future Improvements

- MQTT instead of Blynk
- Local server fallback
- ESP-NOW backup communication
- Database logging
- Telegram alerts
- AI camera direct integration
- Mobile dashboard upgrade

---

# Author

ESP32 + Blynk + RSSI + AI Automation Project  
Smart Relay Control with Crowd Monitoring System
