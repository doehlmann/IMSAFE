# IMSAFE Flying Assistant - Project Summary

**Version:** 2.0 (Consolidated)
**Date:** 2026-01-30
**Status:** Schematic Development Ready

---

## 1. Product Overview

The IMSAFE Flying Assistant is a safety enhancement device for GA/LSA pilots providing:
- Physical checklist interface (8 toggle switches with LED feedback)
- Flight instruments (altitude, attitude, heading)
- Gas detection (CO/NO2 alarms)
- Vibration analysis (engine/gearbox health monitoring)
- Landing radar assistance
- Flight data logging

---

## 2. System Architecture (Current)

### 2.1 Block Diagram

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                POWER SYSTEM                                      │
│                                                                                  │
│  USB-C ──► BQ25792 ──► 2S Li-ion ──► TPS62133 (3.3V)                            │
│  (5V)     (Buck-Boost)  (7.4V)      TPS62130 (5V)                               │
│                │                                                                 │
│                └── BMS: HY2120-CB + HY2213-BB3A (Protection + Balancing)        │
└─────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────┐
│                          MAIN MCU: STM32H747IIT6                                 │
│                          (Dual-Core M7+M4, LQFP-176)                            │
│                                                                                  │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐       │
│  │  SPI1   │ │  I2C1   │ │  I2C4   │ │ SDMMC1  │ │ USART2  │ │ FDCAN1  │       │
│  │ Display │ │ Sensors │ │ I/O Exp │ │ microSD │ │   BLE   │ │Vibration│       │
│  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘       │
└───────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────────────┘
        │          │          │          │          │          │
        ▼          │          │          ▼          ▼          ▼
   ┌─────────┐     │          │     ┌─────────┐ ┌─────────┐ ┌─────────────────┐
   │ ST7789  │     │          │     │ MEM2067 │ │ESP32-C3 │ │ TCAN1042-Q1     │
   │  3.5"   │     │          │     │ SD Slot │ │  MINI-1 │ │     ▼           │
   └─────────┘     │          │     └─────────┘ └─────────┘ │ ┌─────────────┐ │
                   │          │                              │ │ Engine Pod  │ │
   ┌───────────────┴───┐      │                              │ │ ADXL355     │ │
   │     I2C1 Bus      │      │                              │ │ STM32G0B1   │ │
   │                   │      │                              │ └─────────────┘ │
   │ ┌──────┐ ┌──────┐ │ ┌────┴────┐                        │ ┌─────────────┐ │
   │ │BMP390│ │ICM-  │ │ │MCP23017 │                        │ │ Gearbox Pod │ │
   │ │ Baro │ │42688 │ │ │ + 8 SW  │                        │ │ ADXL372     │ │
   │ └──────┘ │-P IMU│ │ │ + 16LED │                        │ │ STM32G0B1   │ │
   │          └──────┘ │ └─────────┘                        │ └─────────────┘ │
   │ ┌──────┐ ┌──────┐ │                                    └─────────────────┘
   │ │MMC   │ │Si7021│ │                                     CAN-FD over M8
   │ │5983MA│ │ Temp │ │
   │ │ Mag  │ │ Humi │ │
   │ └──────┘ └──────┘ │
   │                   │
   │ ┌──────┐ ┌──────┐ │
   │ │MICS  │ │BQ257 │ │
   │ │4514  │ │92    │ │
   │ │ Gas  │ │Chrgr │ │
   │ └──────┘ └──────┘ │
   └───────────────────┘
```

### 2.2 Multi-Board Architecture

| Board | Functions | Interface |
|-------|-----------|-----------|
| **MainBoard** | MCU, power, sensors, storage, BLE, audio, debug | — |
| **SwitchLED Board** | 8 toggles, 16 WS2812B, MCP23017, 2 encoders | FFC 10-pin (I2C4) |
| **Radar Interface** | BGT24LTR11 shield OR XMC4700 fallback | 10-pin header |
| **Vibration Pods** (×2) | ADXL355/ADXL372, STM32G0B1, TCAN1042-Q1 | M8 4-pin CAN-FD |

---

## 3. Component Summary (Approved)

### 3.1 Core Components

| Function | Part Number | Package | Decision |
|----------|-------------|---------|----------|
| MCU | STM32H747IIT6 | LQFP-176 | DEC-002A |
| Charger | BQ25792RQMR | QFN-29 4×4 | DEC-006 |
| 3.3V Regulator | TPS62133RGTR | QFN-16 3×3 | DEC-007 |
| 5V Regulator | TPS62130RGTR | QFN-16 3×3 | DEC-009 |
| BMS Protection | HY2120-CB | SOT-23-6 | DEC-010 |
| BMS Balancer | HY2213-BB3A (×2) | SOT-23-6 | DEC-010 |

### 3.2 Sensors

| Function | Part Number | Interface | Decision |
|----------|-------------|-----------|----------|
| Barometric | BMP390 | I2C (0x77) | DEC-011 |
| IMU (Attitude) | ICM-42688-P | I2C (0x68) | DEC-012 |
| Magnetometer | MMC5983MA | I2C (0x30) | DEC-013 |
| Gas (CO/NO2) | MICS-4514 | Analog | DEC-014 |
| Temp/Humidity | Si7021-A20 | I2C (0x40) | DEC-015 |
| Engine Vibration | ADXL355 | SPI (remote pod) | DEC-003 |
| Gearbox Vibration | ADXL372 | SPI (remote pod) | DEC-003 |

### 3.3 UI & Connectivity

| Function | Part Number | Interface | Decision |
|----------|-------------|-----------|----------|
| Display | ST7789V2 3.5" TFT | SPI1 | DEC-016 |
| Toggle Switches | MTS-102 (×8) | MCP23017 | DEC-017 |
| Status LEDs | WS2812B-2020 (×16) | GPIO (PA0) | DEC-018 |
| Rotary Encoders | EC11 (×2) | TIM3/TIM4 | DEC-019 |
| I/O Expander | MCP23017-E/SO | I2C4 (0x20) | DEC-020 |
| BLE Module | ESP32-C3-MINI-1 | USART2 | DEC-022 |
| Buzzer | MLT-8530 | GPIO | DEC-028 |

### 3.4 Storage & Connectors

| Function | Part Number | Decision |
|----------|-------------|----------|
| microSD Socket | MEM2067-02-180-00-A | DEC-004 |
| USB-C | U262-161N-4BVC11 | DEC-025 |
| Battery | XT30PW + JST-XH 3P | DEC-026 |
| Vibration Sensors | M8 4-pin A-coded | DEC-027 |
| Debug | Tag-Connect TC2030 | DEC-021 |

---

## 4. Vibration Sensor Interface (CAN-FD)

**Key Change from earlier designs:** Vibration sensors use CAN-FD, NOT SPI.

### 4.1 Architecture

- Each sensor pod has local MCU (STM32G0B1) + CAN transceiver (TCAN1042-Q1)
- Main board FDCAN1 on **PD0 (RX) / PD1 (TX)**
- M8 4-pin connector: +5V, GND, CANH, CANL

### 4.2 CAN-FD Parameters

| Parameter | Value |
|-----------|-------|
| Nominal bit rate | 500 kbps |
| Data bit rate | 2 Mbps |
| Engine Pod CAN ID | 0x100-0x10F |
| Gearbox Pod CAN ID | 0x200-0x20F |

---

## 5. Pin Allocation Summary

| Function | STM32H747 Pins | AF |
|----------|----------------|-----|
| USB | PA11/PA12 | AF10 |
| I2C1 (Sensors) | PB8/PB9 | AF4 |
| I2C4 (I/O Expander) | PD12/PD13 | AF4 |
| SPI1 (Display) | PA5/PB4/PB5 | AF5 |
| SDMMC1 | PC8-12, PD2 | AF12 |
| USART2 (BLE) | PA2/PA3 | AF7 |
| FDCAN1 (Vibration) | PD0/PD1 | AF9 |
| TIM2_CH1 (LEDs) | PA0 | AF1 |
| TIM3 (Encoder 1) | PA6/PA7 | AF2 |
| TIM4 (Encoder 2) | PB6/PB7 | AF2 |
| SWD | PA13/PA14, PB3 | — |

**Total used:** 91 pins (52%)

---

## 6. Schematic Sections

| # | Section | Status |
|---|---------|--------|
| 1 | MCU Core (STM32H747) | In Progress |
| 2 | USB-C Input | Complete |
| 3 | Battery Charger (BQ25792) | Complete |
| 4 | BMS (HY2120/HY2213) | Complete |
| 5 | 3.3V Regulator (TPS62133) | Complete |
| 6 | 5V Regulator (TPS62130) | Complete |
| 7 | Barometric (BMP390) | Pending |
| 8 | IMU (ICM-42688-P) | Pending |
| 9 | Magnetometer (MMC5983MA) | Pending |
| 10 | Gas Sensor (MICS-4514) | Pending |
| 11 | Temp/Humidity (Si7021) | Pending |
| 12 | Display Interface | Pending |
| 13 | microSD | Pending |
| 14 | I/O Expander (MCP23017) | Pending |
| 15 | Status LEDs (WS2812B) | Pending |
| 16 | Rotary Encoders | Pending |
| 17 | Bluetooth (ESP32-C3) | Pending |
| 18 | Vibration Interface (CAN-FD) | Pending |
| 19 | Audio (MLT-8530) | Pending |
| 20 | Debug (SWD) | Pending |

---

## 7. Estimated Cost

| Category | Cost |
|----------|------|
| PCB BOM | ~$141 |
| PCB Fabrication + Assembly | ~$50 |
| Display Module | ~$15 |
| Vibration Pods (housing, cable) | ~$100 |
| Battery (2S 16Ah) | ~$75 |
| Enclosure | ~$35 |
| **Total System** | **~$400-450** |

---

## 8. Superseded Information

The following documents contain **outdated** component references and should be ignored:

| Document | Issue |
|----------|-------|
| IMSAFE-Functional-Spec.md | References MPU9250, HMC5983, ST7796, BQ24072 |
| system-architecture.md | References BQ24072, I2C for vibration sensors |
| Older trade studies | May reference rejected options |

**Authoritative sources:**
- `decision-log.md` - All approved decisions
- `schematic-sections.md` - Current section breakdown
- This summary document

---

## 9. Document Index (Handoff Package)

| File | Purpose |
|------|---------|
| `00-IMSAFE-Project-Summary.md` | This file - consolidated overview |
| `01-Decision-Log.md` | All approved component decisions |
| `02-Schematic-Sections.md` | Section breakdown with passives |
| `03-Requirements.md` | Functional requirements |
| `04-BOM-Complete.csv` | Full BOM with LCSC part numbers |
| `05-Netlist.csv` | Net connectivity |
| `06-Pin-Allocation.md` | STM32H747 pin mapping |
| `07-CircuitMind-Package.md` | Detailed handoff for schematic development |
| `08-Vibration-CAN-FD.md` | Vibration sensor pod architecture |
| `09-SensorPod-Design.md` | Sensor pod schematic details |

---

*Document generated for Circuit Mind handoff - 2026-01-30*
