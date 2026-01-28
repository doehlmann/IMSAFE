# Functional Specification
## IMSAFE Flying Assistant

| Document Info | |
|---------------|---|
| Project Code | IMSAFE |
| Revision | A |
| Date | 2026-01-27 |
| Author | Dean Oehlmann |
| Status | Draft |

---

## 1. Executive Summary

### 1.1 Product Description

The IMSAFE Flying Assistant is a safety enhancement device for GA pilots and Light Sport Aircraft pilots. It provides electronic monitoring of equipment and procedures through a tactile checklist interface with visual feedback.

The fundamental design consists of 8 toggle switches acting as a physical checklist for takeoff (all up) and landing (all down) procedures. Programmable LEDs provide visual acknowledgment. A central 3.5" TFT display provides additional flight information.

### 1.2 Key Features

| # | Feature | Description |
|---|---------|-------------|
| 1 | Toggle Checklist | 8x SPDT toggle switches for takeoff/landing checklist |
| 2 | LED Feedback | 16x WS2812 programmable LEDs (above/below each switch) |
| 3 | Altitude Sensing | BMP390 barometric pressure sensor |
| 4 | IMU/Compass | MPU9250/6500 + HMC5983 for synthetic horizon |
| 5 | TFT Display | 3.5" ST7796 320x480 SPI display |
| 6 | RTC/Timer | Real-time clock with flight timer |
| 7 | Rotary Encoders | 2x PEC12R-4130F-S0012 with pushbutton |
| 8 | MicroSD Storage | Flight data logging |
| 9 | Gas Sensing | MICS-4514 for CO and NO2 detection |
| 10 | Vibration Analysis | 2x external IMUs for engine/gearbox monitoring |
| 11 | Bluetooth | Wireless connectivity |
| 12 | Battery Backup | 7.4V 16000mAh Li-ion |
| 13 | USB-C | Data, programming, and charging |
| 14 | Ruggedized | Commercial grade components |
| 15 | Landing Radar | Infineon BGT24LTR11 (0-30m range) |
| 16 | Power Management | Wake on USB/button, 3hr battery timeout |
| 17 | Temperature | Cabin temperature monitoring |

### 1.3 Target Market

- General Aviation pilots
- Light Sport Aircraft pilots
- Experimental aircraft operators

### 1.4 Project Goals

| Goal | Metric | Target |
|------|--------|--------|
| Unit cost | BOM cost | < $250 |
| Development time | Weeks | 4 weeks |
| Power consumption | Active current | TBD |
| Size | PCB dimensions | As required |

---

## 2. Requirements

### 2.1 Functional Requirements

| ID | Requirement | Priority | Verification |
|----|-------------|----------|--------------|
| FR-001 | 8 toggle switches indicate checklist state | Must | Test |
| FR-002 | LEDs provide visual feedback for each switch | Must | Test |
| FR-003 | All switches UP indicates takeoff checklist complete | Must | Test |
| FR-004 | All switches DOWN indicates landing checklist complete | Must | Test |
| FR-005 | Display shows altitude, attitude, heading | Must | Test |
| FR-006 | CO alarm when threshold exceeded | Must | Test |
| FR-007 | Landing radar shows height during approach | Should | Test |
| FR-008 | Log flight data to SD card | Should | Test |
| FR-009 | Bluetooth connectivity for data access | Could | Demo |
| FR-010 | Vibration analysis alerts for engine anomalies | Could | Test |

### 2.2 Performance Requirements

| Parameter | Min | Typical | Max | Unit | Notes |
|-----------|-----|---------|-----|------|-------|
| Supply Voltage (internal) | 3.0 | 3.3 | 3.6 | V | Logic rails |
| Battery Voltage | 6.0 | 7.4 | 8.4 | V | 2S Li-ion |
| Active Current | - | TBD | TBD | mA | All peripherals on |
| Sleep Current | - | TBD | TBD | uA | RTC running |
| Operating Temp | -20 | 25 | 70 | C | |
| Storage Temp | -40 | - | 85 | C | |
| Altitude Accuracy | - | ±1 | ±3 | m | Relative |
| Radar Range | 0.3 | - | 30 | m | |
| CO Detection | 1 | - | 1000 | ppm | |

### 2.3 Physical Requirements

| Parameter | Specification | Notes |
|-----------|---------------|-------|
| Dimensions | TBD | Panel mount |
| Weight | TBD | Including battery |
| Enclosure | IP42 minimum | Cockpit environment |
| Mounting | Panel / Freestanding | |
| Connectors | USB-C, external IMU | |

---

## 3. System Architecture

### 3.1 Block Diagram

```
┌────────────────────────────────────────────────────────────────────────────┐
│                              POWER SYSTEM                                   │
│  USB-C ──► Charger ──► Battery 7.4V ──► DC-DC ──► 3.3V/5V Rails           │
└────────────────────────────────────────────────────────────────────────────┘
                                    │
┌───────────────────────────────────┼────────────────────────────────────────┐
│                           MAIN MCU (TBD)                                    │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐          │
│  │ Display │  │  I2C    │  │  SPI    │  │  GPIO   │  │  UART   │          │
│  │ ST7796  │  │  Bus    │  │  Bus    │  │         │  │         │          │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘          │
└───────┼────────────┼────────────┼────────────┼────────────┼────────────────┘
        │            │            │            │            │
        ▼            │            │            │            ▼
   ┌─────────┐       │            │            │       ┌─────────┐
   │ 3.5"    │       │            │            │       │Bluetooth│
   │ TFT     │       │            │            │       │ Module  │
   └─────────┘       │            │            │       └─────────┘
                     │            │            │
        ┌────────────┴───┐        │            │
        │                │        │            │
   ┌────▼────┐     ┌────▼────┐   │       ┌────▼────┐
   │ BMP390  │     │ IMU     │   │       │ Switch  │
   │Altitude │     │MPU9250  │   │       │ Board   │
   └─────────┘     └─────────┘   │       │MCP23017 │
                                 │       │ +WS2812 │
   ┌─────────┐     ┌─────────┐   │       └─────────┘
   │ MICS    │     │ HMC5983 │   │
   │ 4514    │     │ Compass │   │
   └─────────┘     └─────────┘   │
                                 │
                            ┌────▼────┐
                            │ SD Card │
                            └─────────┘

┌────────────────────────────────────────────────────────────────────────────┐
│                        RADAR MCU (XMC4700)                                  │
│                   Distance2goL Platform                                     │
│  ┌─────────────┐                                                           │
│  │ BGT24LTR11  │◄──► Radar Processing ──► UART/I2C ──► Main MCU            │
│  │ 24GHz Radar │                                                           │
│  └─────────────┘                                                           │
└────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 MCU Selection

**Radar MCU:** Infineon XMC4700
- Used in Distance2goL reference design
- Handles radar signal processing
- Communicates processed distance data to main MCU

**Main MCU:** TBD (Options under evaluation)
- STM32F4/G4 series
- Infineon XMC4xxx (same ecosystem)
- Requires: SPI (display), I2C (sensors), SD card, USB, sufficient GPIO

### 3.3 Interface Summary

| Interface | Components |
|-----------|------------|
| I2C Bus 1 | BMP390, MPU9250, HMC5983, MCP23017 |
| I2C Bus 2 | External vibration IMUs (engine/gearbox) |
| SPI | ST7796 display, SD card |
| GPIO | WS2812 data, encoders, radar UART |
| ADC | MICS-4514 analog outputs |
| USB | Programming, charging, data |

---

## 4. Component Summary

| Component | Part Number | Interface | Notes |
|-----------|-------------|-----------|-------|
| Altitude Sensor | BMP390 | I2C | Bosch |
| IMU | MPU9250 or MPU6500 | I2C | TBD |
| Compass | HMC5983 | I2C | Honeywell |
| Gas Sensor | MICS-4514 | Analog | CO + NO2 |
| Display | ST7796 3.5" | SPI | 320x480 |
| I/O Expander | MCP23017 | I2C | For switches |
| LEDs | WS2812 x16 | GPIO | Addressable RGB |
| Encoders | PEC12R-4130F-S0012 | GPIO | 2x with button |
| Radar | BGT24LTR11 | Digital | 24GHz |
| SD Card | MicroSD | SPI/SDIO | |
| Bluetooth | TBD | UART | |
| Battery | 7.4V 16000mAh | - | 2S Li-ion |

---

## 5. User Interface

### 5.1 Toggle Switches

8 SPDT toggle switches arranged in a row:
- **All UP:** Takeoff checklist complete
- **All DOWN:** Landing checklist complete
- **Mixed:** Checklist incomplete

### 5.2 LED Indicators

16 WS2812 LEDs (2 per switch - above and below):
- **Green:** Switch in correct position
- **Red Flashing:** Alarm condition
- **Amber:** Attention required
- **Brightness:** Adjustable for day/night

### 5.3 Display Modes

1. **Primary:** Synthetic horizon + altitude + heading
2. **Checklist:** Switch status overview
3. **Gas Monitor:** CO/NO2 levels
4. **Radar:** Landing approach height
5. **Vibration:** Engine health status
6. **Settings:** Configuration menu

### 5.4 Rotary Encoders

- Left: Mode selection, menu navigation
- Right: Value adjustment, zoom

---

## 6. Power Management

### 6.1 Power States

| State | Description | Wake Source |
|-------|-------------|-------------|
| Active | Full operation | - |
| Low Power | Reduced update rate | Button, USB |
| Sleep | MCU sleep, RTC only | Button, USB |
| Off | Complete shutdown | Power button |

### 6.2 Power Transitions

- Wake on USB connect
- Wake on power button press
- Sleep after 3 hours on battery
- Shutdown on low battery (< 5%)

---

## 7. Revision History

| Rev | Date | Author | Description |
|-----|------|--------|-------------|
| A | 2026-01-27 | D. Oehlmann | Initial draft |
