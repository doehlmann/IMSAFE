# IMSAFE MainBoard - Circuit Mind Handoff Package

**Project:** IMSAFE Flying Assistant
**Version:** 1.0
**Date:** 2026-01-30
**MCU:** STM32H747IIT6 (LQFP-176)
**Board:** 4-layer, 100×80mm target

---

## Table of Contents

1. [System Overview](#1-system-overview)
2. [Power Distribution](#2-power-distribution)
3. [STM32H747 Pin Allocation](#3-stm32h747-pin-allocation)
4. [Section Netlists](#4-section-netlists)
5. [Component Specifications](#5-component-specifications)
6. [Inter-Section Connections](#6-inter-section-connections)
7. [Design Rules](#7-design-rules)

---

## 1. System Overview

### Block Diagram

```
                                    ┌─────────────────────────────────────────┐
                                    │           STM32H747IIT6                 │
                                    │         (LQFP-176 Dual-Core)            │
    USB-C ──────────────────────────┤ USB_OTG_FS                              │
      │                             │                                         │
      ▼                             │ FDCAN1 ──────► TCAN1042 ──► M8 CAN Bus │
  ┌───────────┐    VSYS            │                                         │
  │ BQ25792   │──────────┐         │ SPI4 ─────────► Display (ST7789)        │
  │ Charger   │          │         │                                         │
  └───────────┘          │         │ SDMMC1 ────────► microSD                │
      │                  │         │                                         │
      ▼                  ▼         │ I2C1 ──────────► Sensors (BMP390,       │
  ┌───────────┐    ┌─────────┐    │                   ICM-42688-P, MMC5983MA,│
  │ BMS       │    │ TPS62133│    │                   Si7021, MICS-4514)     │
  │ HY2120+   │    │ 3.3V    │────┤                                         │
  │ HY2213    │    └─────────┘    │ I2C4 ──────────► MCP23017 (Switches)    │
  └───────────┘          │         │                                         │
      │                  │         │ USART2 ─────────► ESP32-C3 (BLE)        │
      ▼                  ▼         │                                         │
  ┌───────────┐    ┌─────────┐    │ TIM2_CH1 ───────► WS2812B LEDs (16)     │
  │ Battery   │    │ TPS62130│    │                                         │
  │ 2S Li-ion │    │ 5V      │────┤ TIM3/4 ──────────► Encoders (2)         │
  │ 7.4V      │    └─────────┘    │                                         │
  └───────────┘                   │ GPIO ────────────► Buzzer (MLT-8530)    │
                                   │                                         │
                                   │ SWD ─────────────► Debug (Tag-Connect) │
                                   └─────────────────────────────────────────┘
```

### Board Architecture

| Board | Function | Interface to Main |
|-------|----------|-------------------|
| MainBoard | MCU, power, sensors, storage | — |
| SwitchLED Board | 8 switches, 16 LEDs, MCP23017 | FFC 10-pin (I2C + data) |
| Radar Interface | BGT24LTR11 or USB fallback | 10-pin header (SPI/UART) |
| SensorPod ×2 | ADXL355/372 vibration | M8 4-pin CAN bus |

---

## 2. Power Distribution

### Power Tree

```
USB-C VBUS (5V)                     Battery (6.0-8.4V)
      │                                    │
      ▼                                    ▼
┌─────────────┐                    ┌─────────────┐
│  USBLC6-2SC6│◄── ESD             │   TVS +     │◄── Protection
│             │                    │   PTC Fuse  │
└──────┬──────┘                    └──────┬──────┘
       │                                  │
       ▼                                  ▼
┌──────────────────────────────────────────────────┐
│                   BQ25792                         │
│               Buck-Boost Charger                  │
│  VBUS──►PMID──►BAT (to battery via BMS)          │
│                    │                              │
│                    └──►VSYS (6.0-8.4V)           │
└─────────────────────────┬────────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
    ┌───────────┐   ┌───────────┐   ┌───────────┐
    │ TPS62133  │   │ TPS62130  │   │  Direct   │
    │  3.3V/3A  │   │   5V/3A   │   │  to BMS   │
    └─────┬─────┘   └─────┬─────┘   └───────────┘
          │               │
          ▼               ▼
    ┌───────────┐   ┌───────────┐
    │   +3V3    │   │    +5V    │
    │           │   │           │
    │ • MCU     │   │ • WS2812B │
    │ • Sensors │   │ • CAN bus │
    │ • BLE     │   │ • Display │
    │ • microSD │   │   BL      │
    └───────────┘   └───────────┘
```

### Power Rails Summary

| Rail | Voltage | Source | Max Current | Consumers |
|------|---------|--------|-------------|-----------|
| VBUS | 5V | USB-C | 3A (PD) | Charger input |
| VBAT | 6.0-8.4V | Battery | 5A | BMS, VSYS |
| VSYS | 6.0-8.4V | BQ25792 | 3A | Regulators |
| +3V3 | 3.3V | TPS62133 | 3A | MCU, sensors, BLE |
| +5V | 5.0V | TPS62130 | 3A | LEDs, CAN, display BL |
| VBAT_SENSE | 6.0-8.4V | Battery tap | <1mA | BQ25792 monitoring |

---

## 3. STM32H747 Pin Allocation

### Complete Pin Assignment Table

| Pin | Port | Function | Net Name | Section | Notes |
|-----|------|----------|----------|---------|-------|
| 1 | PE2 | GPIO | NC | — | Reserved |
| 2 | PE3 | GPIO | NC | — | Reserved |
| 3 | PE4 | GPIO | NC | — | Reserved |
| 4 | PE5 | GPIO | NC | — | Reserved |
| 5 | PE6 | GPIO | NC | — | Reserved |
| 6 | VBAT | VBAT | VBAT | 01 | RTC backup |
| 7 | PC13 | GPIO | WAKEUP | 01 | Wakeup button |
| 8 | PC14 | OSC32_IN | LSE_IN | 01 | 32.768kHz |
| 9 | PC15 | OSC32_OUT | LSE_OUT | 01 | 32.768kHz |
| 10 | VSS | VSS | GND | 01 | Ground |
| 11 | VDD | VDD | +3V3 | 01 | Power |
| 12 | PH0 | OSC_IN | HSE_IN | 01 | 8MHz |
| 13 | PH1 | OSC_OUT | HSE_OUT | 01 | 8MHz |
| 14 | NRST | NRST | NRST | 01 | Reset |
| 15 | PC0 | GPIO | MCP23017_INTA | 14 | IO expander INT |
| 16 | PC1 | GPIO | ESP32_EN | 17 | BLE module enable |
| 17 | PC2 | GPIO | ESP32_IO9 | 17 | BLE boot mode |
| 18 | PC3 | ADC3_INP1 | MICS_RED | 10 | Gas sensor reducing |
| 19 | VDD | VDD | +3V3 | 01 | Power |
| 20 | VSSA | VSSA | GND | 01 | Analog ground |
| 21 | VREF+ | VREF+ | +3V3 | 01 | ADC reference |
| 22 | VDDA | VDDA | +3V3 | 01 | Analog power |
| 23 | PA0 | TIM2_CH1 | LED_DATA | 15 | WS2812B data |
| 24 | PA1 | ADC1_INP1 | MICS_OX | 10 | Gas sensor oxidizing |
| 25 | PA2 | USART2_TX | ESP32_RX | 17 | BLE UART TX |
| 26 | PA3 | USART2_RX | ESP32_TX | 17 | BLE UART RX |
| 27 | VSS | VSS | GND | 01 | Ground |
| 28 | VDD | VDD | +3V3 | 01 | Power |
| 29 | PA4 | GPIO | DISP_DC | 12 | Display D/C |
| 30 | PA5 | SPI1_SCK | DISP_SCK | 12 | Display SPI clock |
| 31 | PA6 | TIM3_CH1 | ENC1_A | 16 | Encoder 1 A |
| 32 | PA7 | TIM3_CH2 | ENC1_B | 16 | Encoder 1 B |
| 33 | PC4 | ADC1_INP4 | VBAT_SENSE | 03 | Battery voltage |
| 34 | PC5 | GPIO | MICS_HEATER | 10 | Gas heater control |
| 35 | PB0 | GPIO | DISP_RST | 12 | Display reset |
| 36 | PB1 | GPIO | DISP_CS | 12 | Display chip select |
| 37 | PB2 | GPIO | DISP_BL | 12 | Display backlight |
| 38 | PE7 | GPIO | NC | — | Reserved |
| 39 | PE8 | GPIO | NC | — | Reserved |
| 40 | PE9 | GPIO | NC | — | Reserved |
| 41 | PE10 | GPIO | NC | — | Reserved |
| 42 | PE11 | GPIO | NC | — | Reserved |
| 43 | PE12 | GPIO | NC | — | Reserved |
| 44 | PE13 | GPIO | NC | — | Reserved |
| 45 | PE14 | GPIO | NC | — | Reserved |
| 46 | PE15 | GPIO | NC | — | Reserved |
| 47 | PB10 | I2C2_SCL | I2C2_SCL | — | Spare I2C |
| 48 | PB11 | I2C2_SDA | I2C2_SDA | — | Spare I2C |
| 49 | VCAP1 | VCAP1 | VCAP1 | 01 | 2.2µF to GND |
| 50 | VDD | VDD | +3V3 | 01 | Power |
| 51 | PB12 | GPIO | BQ_INT | 03 | Charger interrupt |
| 52 | PB13 | GPIO | BQ_CE | 03 | Charger enable |
| 53 | PB14 | SPI2_MISO | SD_MISO | 13 | SD card data out |
| 54 | PB15 | SPI2_MOSI | SD_MOSI | 13 | SD card data in |
| 55 | PD8 | GPIO | NC | — | Reserved |
| 56 | PD9 | GPIO | NC | — | Reserved |
| 57 | PD10 | GPIO | NC | — | Reserved |
| 58 | PD11 | GPIO | NC | — | Reserved |
| 59 | PD12 | GPIO | SD_CD | 13 | SD card detect |
| 60 | PD13 | GPIO | SD_CS | 13 | SD card CS |
| 61 | PD14 | GPIO | BUZZER | 19 | Buzzer control |
| 62 | PD15 | GPIO | NC | — | Reserved |
| 63 | PC6 | GPIO | ENC1_SW | 16 | Encoder 1 switch |
| 64 | PC7 | GPIO | ENC2_SW | 16 | Encoder 2 switch |
| 65 | PC8 | SDMMC1_D0 | SD_D0 | 13 | SD data 0 |
| 66 | PC9 | SDMMC1_D1 | SD_D1 | 13 | SD data 1 |
| 67 | PA8 | GPIO | ENC1_SW_ALT | 16 | Alt encoder SW |
| 68 | PA9 | USB_OTG_FS_VBUS | USB_VBUS | 02 | USB VBUS sense |
| 69 | PA10 | USB_OTG_FS_ID | USB_ID | 02 | USB ID |
| 70 | PA11 | USB_OTG_FS_DM | USB_DM | 02 | USB D- |
| 71 | PA12 | USB_OTG_FS_DP | USB_DP | 02 | USB D+ |
| 72 | PA13 | SWDIO | SWDIO | 20 | Debug |
| 73 | VCAP2 | VCAP2 | VCAP2 | 01 | 2.2µF to GND |
| 74 | VSS | VSS | GND | 01 | Ground |
| 75 | VDD | VDD | +3V3 | 01 | Power |
| 76 | PA14 | SWCLK | SWCLK | 20 | Debug |
| 77 | PA15 | GPIO | NC | — | Reserved |
| 78 | PC10 | SDMMC1_D2 | SD_D2 | 13 | SD data 2 |
| 79 | PC11 | SDMMC1_D3 | SD_D3 | 13 | SD data 3 |
| 80 | PC12 | SDMMC1_CK | SD_CLK | 13 | SD clock |
| 81 | PD0 | FDCAN1_RX | FDCAN1_RX | 18 | CAN receive |
| 82 | PD1 | FDCAN1_TX | FDCAN1_TX | 18 | CAN transmit |
| 83 | PD2 | SDMMC1_CMD | SD_CMD | 13 | SD command |
| 84 | PD3 | SPI2_SCK | SD_SCK | 13 | SD clock (SPI mode) |
| 85 | PD4 | GPIO | NC | — | Reserved |
| 86 | PD5 | GPIO | NC | — | Reserved |
| 87 | PD6 | GPIO | NC | — | Reserved |
| 88 | PD7 | GPIO | NC | — | Reserved |
| 89 | PB3 | SWO | SWO | 20 | Debug trace |
| 90 | PB4 | SPI1_MISO | DISP_MISO | 12 | Display MISO |
| 91 | PB5 | SPI1_MOSI | DISP_MOSI | 12 | Display MOSI |
| 92 | PB6 | TIM4_CH1 | ENC2_A | 16 | Encoder 2 A |
| 93 | PB7 | TIM4_CH2 | ENC2_B | 16 | Encoder 2 B |
| 94 | BOOT0 | BOOT0 | BOOT0 | 01 | Boot config |
| 95 | PB8 | I2C1_SCL | I2C1_SCL | 07-11 | Sensor I2C |
| 96 | PB9 | I2C1_SDA | I2C1_SDA | 07-11 | Sensor I2C |
| 97 | PE0 | GPIO | NC | — | Reserved |
| 98 | PE1 | GPIO | NC | — | Reserved |
| 99 | PDR_ON | PDR_ON | +3V3 | 01 | Power-on reset |
| 100 | VDD | VDD | +3V3 | 01 | Power |
| — | — | — | — | — | Pins 101-176 reserved |

### I2C Bus Assignments

| Bus | Pins | Speed | Devices |
|-----|------|-------|---------|
| I2C1 | PB8/PB9 | 400 kHz | BMP390, ICM-42688-P, MMC5983MA, Si7021, BQ25792 |
| I2C4 | PD12/PD13 | 400 kHz | MCP23017 (via SwitchLED board FFC) |

### SPI Bus Assignments

| Bus | Pins | Speed | Devices |
|-----|------|-------|---------|
| SPI1 | PA5/PB4/PB5 | 20 MHz | Display (ST7789) |
| SPI2 | PD3/PB14/PB15 | 25 MHz | microSD (fallback from SDMMC) |

---

## 4. Section Netlists

### Section 01: MCU Core

```
COMPONENT: U1 - STM32H747IIT6 (LQFP-176)
COMPONENT: Y1 - 8MHz Crystal (3225)
COMPONENT: Y2 - 32.768kHz Crystal (3215)

NETLIST:
+3V3        → U1.VDD (all), U1.VDDA, U1.VREF+
GND         → U1.VSS (all), U1.VSSA
HSE_IN      → U1.PH0, Y1.pin1
HSE_OUT     → U1.PH1, Y1.pin2
LSE_IN      → U1.PC14, Y2.pin1
LSE_OUT     → U1.PC15, Y2.pin2
VCAP1       → U1.VCAP1, C_VCAP1.pin1
VCAP2       → U1.VCAP2, C_VCAP2.pin1
NRST        → U1.NRST, R_NRST.pin1, C_NRST.pin1
BOOT0       → U1.BOOT0, R_BOOT0.pin1

PASSIVES:
C_VCAP1     2.2µF   0603    VCAP1 → GND
C_VCAP2     2.2µF   0603    VCAP2 → GND
C_HSE1      15pF    0603    HSE_IN → GND
C_HSE2      15pF    0603    HSE_OUT → GND
C_LSE1      6.8pF   0603    LSE_IN → GND
C_LSE2      6.8pF   0603    LSE_OUT → GND
R_NRST      10kΩ    0603    +3V3 → NRST
C_NRST      100nF   0603    NRST → GND
R_BOOT0     10kΩ    0603    BOOT0 → GND
C_VDD×18    100nF   0603    Each VDD pin → GND (place closest)
C_VDDA      1µF     0603    VDDA → GND
C_VREF      10nF    0603    VREF+ → GND
C_BULK      4.7µF   0805    +3V3 → GND (near pin 1 corner)
```

### Section 02: USB-C

```
COMPONENT: J1 - USB-C 16-pin (U262-161N-4BVC11)
COMPONENT: U2 - USBLC6-2SC6 (SOT-23-6)

NETLIST:
VBUS        → J1.VBUS, U2.VBUS, C_VBUS_BULK.pin1
USB_DP      → J1.D+, U2.I/O1, U1.PA12
USB_DM      → J1.D-, U2.I/O2, U1.PA11
CC1         → J1.CC1, R_CC1.pin1
CC2         → J1.CC2, R_CC2.pin1
GND         → J1.GND, U2.GND, R_CC1.pin2, R_CC2.pin2

PASSIVES:
R_CC1       5.1kΩ   0603    CC1 → GND
R_CC2       5.1kΩ   0603    CC2 → GND
C_VBUS_HF   100nF   0603    VBUS → GND
C_VBUS_BULK 10µF    0805    VBUS → GND
```

### Section 03: Battery Charger (BQ25792)

```
COMPONENT: U3 - BQ25792RQMR (QFN-29)
COMPONENT: L1 - 2.2µH 5A Inductor (5×5mm)

NETLIST:
VBUS        → U3.VBUS
PMID        → U3.PMID, C_PMID.pin1
BAT         → U3.BAT, L1.pin2, C_BAT1.pin1, C_BAT2.pin1 → To BMS
VSYS        → U3.SYS, C_VSYS1.pin1, C_VSYS2.pin1
SW          → U3.SW, L1.pin1
I2C1_SDA    → U3.SDA
I2C1_SCL    → U3.SCL
BQ_INT      → U3.INT, U1.PB12
BQ_CE       → U3.CE, U1.PB13
GND         → U3.GND, U3.PGND (thermal pad)

PASSIVES:
C_VBUS1     10µF/25V    0805    VBUS → GND
C_VBUS2     10µF/25V    0805    VBUS → GND
C_PMID      10µF/25V    0805    PMID → GND
C_BAT1      22µF/16V    0805    BAT → GND
C_BAT2      22µF/16V    0805    BAT → GND
C_VSYS1     22µF/16V    0805    VSYS → GND
C_VSYS2     100nF       0603    VSYS → GND
C_REGN      100nF       0603    REGN → GND
R_ILIM      10mΩ        0805    Current sense (per datasheet)
R_NTC       10kΩ        0603    NTC bias
```

### Section 04: BMS (HY2120 + HY2213)

```
COMPONENT: U4 - HY2120-CB (SOT-23-6) - Protection
COMPONENT: U5 - HY2213-BB3A (SOT-23-6) - Cell 1 Balance
COMPONENT: U6 - HY2213-BB3A (SOT-23-6) - Cell 2 Balance
COMPONENT: Q1-Q4 - AO3400A (SOT-23) - MOSFETs

NETLIST:
CELL1+      → Battery cell 1 positive
CELL_MID    → Battery cell junction
CELL2-      → Battery cell 2 negative (BAT-)
BAT+        → From BQ25792 BAT output
BAT-        → To system GND via MOSFETs

PASSIVES:
R_SENSE     5mΩ     1206    Current sense (BAT- path)
R_BAL1      62Ω     0805    Cell 1 balance resistor
R_BAL2      62Ω     0805    Cell 2 balance resistor
C_VDD       100nF   0603    U4.VDD → GND
C_BAL1      100nF   0603    U5.VDD → GND
C_BAL2      100nF   0603    U6.VDD → GND
C_BULK      10µF    0805    VDD → GND (missing - add)
R_OD        1kΩ     0603    Overdischarge gate
R_OC        1kΩ     0603    Overcurrent gate
```

### Section 05: 3.3V Regulator (TPS62133)

```
COMPONENT: U7 - TPS62133RGTR (QFN-16 3×3mm) - Fixed 3.3V
COMPONENT: L2 - 1.5µH 4A Inductor (4×4mm)

NETLIST:
VSYS        → U7.VIN, C_VIN1.pin1, C_VIN2.pin1
+3V3        → U7.VOUT, L2.pin2, C_VOUT1.pin1, C_VOUT2.pin1
SW          → U7.SW, L2.pin1
EN          → U7.EN (tie to VIN or add enable control)
GND         → U7.GND, U7.PGND

PASSIVES:
C_VIN1      10µF/16V    0805    VIN → GND
C_VIN2      100nF       0603    VIN → GND (close to pin)
C_VOUT1     22µF/10V    0805    VOUT → GND
C_VOUT2     22µF/10V    0805    VOUT → GND
C_VOUT3     100nF       0603    VOUT → GND (close to pin)
```

### Section 06: 5V Regulator (TPS62130)

```
COMPONENT: U8 - TPS62130RGTR (QFN-16 3×3mm) - Adjustable
COMPONENT: L3 - 1.5µH 4A Inductor (4×4mm)

NETLIST:
VSYS        → U8.VIN, C_VIN1.pin1, C_VIN2.pin1
+5V         → U8.VOUT, L3.pin2, C_VOUT1.pin1, C_VOUT2.pin1
SW          → U8.SW, L3.pin1
FB          → U8.FB, R_FB_TOP.pin2, R_FB_BOT.pin1
EN          → U8.EN (tie to VIN)
GND         → U8.GND, U8.PGND, R_FB_BOT.pin2

PASSIVES:
C_VIN1      10µF/16V    0805    VIN → GND
C_VIN2      100nF       0603    VIN → GND
C_VOUT1     22µF/10V    0805    VOUT → GND
C_VOUT2     22µF/10V    0805    VOUT → GND
C_VOUT3     100nF       0603    VOUT → GND
R_FB_TOP    100kΩ       0603    VOUT → FB
R_FB_BOT    24.3kΩ      0603    FB → GND

CALCULATION: VOUT = 0.8V × (1 + 100k/24.3k) = 5.09V
```

### Section 07: Barometric Sensor (BMP390)

```
COMPONENT: U9 - BMP390 (LGA-10 2×2mm)

NETLIST:
+3V3        → U9.VDD, U9.VDDIO
I2C1_SDA    → U9.SDI (with pull-up at bus level)
I2C1_SCL    → U9.SCK (with pull-up at bus level)
BMP_INT     → U9.INT (optional, to MCU GPIO)
GND         → U9.GND, U9.SDO (I2C addr = 0x77)

PASSIVES:
C_VDD       100nF   0603    VDD → GND (close)
C_VDDIO     100nF   0603    VDDIO → GND (close)
C_BULK      10µF    0805    VDD → GND (add)
R_INT       10kΩ    0603    +3V3 → INT (add pull-up)
```

### Section 08: IMU (ICM-42688-P)

```
COMPONENT: U10 - ICM-42688-P (LGA-14 2.5×3mm)

NETLIST:
+3V3        → U10.VDD, U10.VDDIO
I2C1_SDA    → U10.SDA
I2C1_SCL    → U10.SCL
IMU_INT1    → U10.INT1 → MCU GPIO (optional)
IMU_INT2    → U10.INT2 → MCU GPIO (optional)
GND         → U10.GND, U10.RESV (tie to GND)

PASSIVES:
C_VDD       100nF   0603    VDD → GND
C_VDDIO     100nF   0603    VDDIO → GND
C_RESV      10nF    0603    RESV → GND (per datasheet)
C_BULK      10µF    0805    VDD → GND (add)
```

### Section 09: Magnetometer (MMC5983MA)

```
COMPONENT: U11 - MMC5983MA (LGA-16 3×3mm)

NETLIST:
+3V3        → U11.VDD, U11.VDDIO
I2C1_SDA    → U11.SDA
I2C1_SCL    → U11.SCL
MAG_INT     → U11.INT → MCU GPIO (optional)
GND         → U11.GND

PASSIVES:
C_VDD       100nF   0603    VDD → GND
C_VDDIO     100nF   0603    VDDIO → GND
C_C1        4.7µF   0603    C1 pin → GND (SET/RESET driver)
```

### Section 10: Gas Sensor (MICS-4514)

```
COMPONENT: U12 - MICS-4514 (SMD)
COMPONENT: Q5 - AO3400A (SOT-23) - Heater driver

NETLIST:
+5V         → U12.HEATER+ (via Q5)
MICS_HEATER → Q5.GATE, U1.PC5
MICS_RED    → U12.RED_OUT, R_LOAD_RED.pin1 → U1.PC3 (ADC)
MICS_OX     → U12.OX_OUT, R_LOAD_OX.pin1 → U1.PA1 (ADC)
GND         → U12.GND, Q5.SOURCE

PASSIVES:
R_LOAD_RED  10kΩ    0603    +3V3 → RED_OUT (load resistor)
R_LOAD_OX   10kΩ    0603    +3V3 → OX_OUT (load resistor)
C_ADC_RED   100nF   0603    MICS_RED → GND (ADC filter)
C_ADC_OX    100nF   0603    MICS_OX → GND (ADC filter)
R_GATE      10kΩ    0603    MICS_HEATER → GND (gate pull-down)
R_GATE_S    100Ω    0603    MCU → Q5.GATE (series)
C_HEATER    100nF   0603    HEATER+ → GND (add)
C_HEATER_B  10µF    0805    HEATER+ → GND (add bulk)
```

### Section 11: Temp/Humidity (Si7021)

```
COMPONENT: U13 - Si7021-A20-GM (DFN-6 3×3mm)

NETLIST:
+3V3        → U13.VDD
I2C1_SDA    → U13.SDA
I2C1_SCL    → U13.SCL
GND         → U13.GND

PASSIVES:
C_VDD       100nF   0603    VDD → GND
C_BULK      10µF    0805    VDD → GND (add)
```

### Section 12: Display Interface

```
COMPONENT: J2 - FPC Connector (40-pin for display module)

NETLIST:
+5V         → J2.VCC (backlight power)
+3V3        → J2.VDD (logic)
DISP_SCK    → U1.PA5 (SPI1_SCK), J2.SCL
DISP_MOSI   → U1.PB5 (SPI1_MOSI), J2.SDA
DISP_MISO   → U1.PB4 (SPI1_MISO), J2.SDO
DISP_CS     → U1.PB1, J2.CS
DISP_DC     → U1.PA4, J2.DC
DISP_RST    → U1.PB0, J2.RST
DISP_BL     → U1.PB2 → Q6 → J2.LED (PWM backlight)
GND         → J2.GND

PASSIVES:
R_SCK       33Ω     0603    MCU → DISP_SCK (series, optional)
R_MOSI      33Ω     0603    MCU → DISP_MOSI (series, optional)
C_VDD       100nF   0603    Display VDD → GND
C_BULK      10µF    0805    Display VDD → GND
Q6          AO3400A SOT-23  Backlight driver
R_BL_GATE   10kΩ    0603    DISP_BL → GND (pull-down)
```

### Section 13: microSD

```
COMPONENT: J3 - MEM2067-02-180-00-A (hinged socket)
COMPONENT: U14, U15 - USBLC6-2SC6 (ESD protection)

NETLIST (SDMMC 4-bit mode):
+3V3        → J3.VDD
SD_D0       → U1.PC8 (SDMMC1_D0), J3.DAT0
SD_D1       → U1.PC9 (SDMMC1_D1), J3.DAT1
SD_D2       → U1.PC10 (SDMMC1_D2), J3.DAT2
SD_D3       → U1.PC11 (SDMMC1_D3), J3.DAT3
SD_CLK      → U1.PC12 (SDMMC1_CK), J3.CLK
SD_CMD      → U1.PD2 (SDMMC1_CMD), J3.CMD
SD_CD       → U1.PD12, J3.CD (card detect)
GND         → J3.GND

PASSIVES:
C_VDD       100nF   0603    VDD → GND
C_BULK      10µF    0805    VDD → GND
R_DAT0      33Ω     0603    Series on DAT0
R_DAT1      33Ω     0603    Series on DAT1
R_DAT2      33Ω     0603    Series on DAT2
R_DAT3      33Ω     0603    Series on DAT3
R_CMD       33Ω     0603    Series on CMD
R_CLK       33Ω     0603    Series on CLK
R_CD        10kΩ    0603    +3V3 → CD (pull-up)
```

### Section 14: I/O Expander (MCP23017)

```
COMPONENT: U16 - MCP23017-E/SO (SOIC-28)

NETLIST:
+3V3        → U16.VDD
I2C4_SDA    → U16.SDA → FFC connector
I2C4_SCL    → U16.SCL → FFC connector
MCP_INTA    → U16.INTA, U1.PC0
MCP_INTB    → U16.INTB (optional)
A0          → U16.A0, R_A0.pin1 → GND (addr = 0x20)
A1          → U16.A1, R_A1.pin1 → GND
A2          → U16.A2, R_A2.pin1 → GND
RESET       → U16.RESET, R_RST.pin1 → +3V3
GND         → U16.GND

MCP PINS → FFC to SwitchLED Board:
GPA0-GPA7   → 8 toggle switches (to GND)
GPB0-GPB7   → Spare / encoder switches

PASSIVES:
C_VDD       100nF   0603    VDD → GND
C_BULK      10µF    0805    VDD → GND (add)
R_A0        10kΩ    0603    A0 → GND
R_A1        10kΩ    0603    A1 → GND
R_A2        10kΩ    0603    A2 → GND
R_RST       10kΩ    0603    +3V3 → RESET
R_I2C_SDA   4.7kΩ   0603    +3V3 → I2C4_SDA
R_I2C_SCL   4.7kΩ   0603    +3V3 → I2C4_SCL
```

### Section 15: Status LEDs (WS2812B-2020)

```
COMPONENT: LED1-LED16 - WS2812B-2020 (16 total)

NETLIST (Daisy chain):
+5V         → LED1.VDD ... LED16.VDD
LED_DATA    → U1.PA0 (TIM2_CH1), R_DATA.pin1
R_DATA.pin2 → LED1.DIN
LED1.DOUT   → LED2.DIN
LED2.DOUT   → LED3.DIN
...
LED15.DOUT  → LED16.DIN
LED16.DOUT  → NC
GND         → LED1.GND ... LED16.GND

PASSIVES:
R_DATA      330Ω    0603    MCU → LED1.DIN (series)
C_LED1-16   100nF   0603    VDD → GND per LED (16 total)
```

### Section 16: Rotary Encoders

```
COMPONENT: ENC1, ENC2 - EC11 (with push switch)

NETLIST:
ENC1_A      → U1.PA6 (TIM3_CH1)
ENC1_B      → U1.PA7 (TIM3_CH2)
ENC1_SW     → U1.PC6
ENC2_A      → U1.PB6 (TIM4_CH1)
ENC2_B      → U1.PB7 (TIM4_CH2)
ENC2_SW     → U1.PC7
ENC_COM     → GND

PASSIVES:
C_ENC1_A    10nF    0603    ENC1_A → GND (debounce)
C_ENC1_B    10nF    0603    ENC1_B → GND
C_ENC1_SW   100nF   0603    ENC1_SW → GND
C_ENC2_A    10nF    0603    ENC2_A → GND
C_ENC2_B    10nF    0603    ENC2_B → GND
C_ENC2_SW   100nF   0603    ENC2_SW → GND
R_ENC1_A    10kΩ    0603    +3V3 → ENC1_A (pull-up, add)
R_ENC1_B    10kΩ    0603    +3V3 → ENC1_B (pull-up, add)
R_ENC1_SW   10kΩ    0603    +3V3 → ENC1_SW (pull-up, add)
R_ENC2_A    10kΩ    0603    +3V3 → ENC2_A (pull-up, add)
R_ENC2_B    10kΩ    0603    +3V3 → ENC2_B (pull-up, add)
R_ENC2_SW   10kΩ    0603    +3V3 → ENC2_SW (pull-up, add)
```

### Section 17: Bluetooth (ESP32-C3-MINI-1)

```
COMPONENT: U17 - ESP32-C3-MINI-1 (Module)

NETLIST:
+3V3        → U17.3V3
ESP32_TX    → U17.TXD0, U1.PA3 (USART2_RX)
ESP32_RX    → U17.RXD0, U1.PA2 (USART2_TX)
ESP32_EN    → U17.EN, U1.PC1
ESP32_IO9   → U17.IO9, U1.PC2 (boot mode)
GND         → U17.GND

PASSIVES:
C_3V3       10µF    0805    3V3 → GND
C_3V3_HF    100nF   0603    3V3 → GND (close to module)
R_EN        10kΩ    0603    +3V3 → EN (pull-up)
R_IO9       10kΩ    0603    +3V3 → IO9 (pull-up for normal boot)
C_EN        1µF     0603    EN → GND (RC delay for stable boot)
```

### Section 18: Vibration Interface (CAN-FD)

```
COMPONENT: U1801 - TCAN1042-Q1 (SOIC-8)
COMPONENT: D1801 - PESD2CAN (SOT-23)
COMPONENT: J1801 - M8 4-pin A-coded connector

NETLIST:
+3V3        → U1801.VCC, C1801.pin1
FDCAN1_TX   → U1.PD1, U1801.TXD
FDCAN1_RX   → U1.PD0, U1801.RXD
CANH        → U1801.CANH, R1801.pin2, D1801.A1, J1801.pin2
CANL        → U1801.CANL, R1802.pin2, D1801.A2, J1801.pin4
STB         → U1801.STB → GND (normal mode) or MCU GPIO
+5V         → J1801.pin1 (pod power)
GND         → U1801.GND, D1801.C, J1801.pin3, C1802.pin2

SPLIT TERMINATION:
R1801.pin1  → C1802.pin1 (junction)
R1802.pin1  → C1802.pin1 (junction)

PASSIVES:
C1801       100nF   0603    VCC → GND
R1801       60Ω     0603    CANH → split junction
R1802       60Ω     0603    CANL → split junction
C1802       4.7nF   0603    Split junction → GND
D1801       PESD2CAN SOT-23 CAN ESD protection
```

### Section 19: Audio (MLT-8530)

```
COMPONENT: BZ1 - MLT-8530 (Active buzzer)
COMPONENT: Q7 - AO3400A (SOT-23) - Driver
COMPONENT: D2 - 1N4148W (SOD-123) - Flyback

NETLIST:
+3V3        → BZ1.+ (via D2 cathode)
BUZZER      → U1.PD14, R1902.pin1
R1902.pin2  → Q7.GATE
Q7.DRAIN    → BZ1.-, D2.anode
Q7.SOURCE   → GND
D2.cathode  → BZ1.+

PASSIVES:
R1901       10kΩ    0603    Q7.GATE → GND (pull-down)
R1902       100Ω    0603    MCU → Q7.GATE (series)
D2          1N4148W SOD-123 Flyback protection
C_VDD       100nF   0603    BZ1 VDD → GND (add)
C_BULK      10µF    0805    BZ1 VDD → GND (add)
```

### Section 20: Debug (Tag-Connect + Header)

```
COMPONENT: J6 - Tag-Connect TC2030-IDC-NL footprint (pads only)
COMPONENT: J7 - 6-pin header (2.54mm, optional)

NETLIST:
+3V3        → J6.pin1, J7.pin1
SWDIO       → U1.PA13, J6.pin2, J7.pin2
GND         → J6.pin3, J7.pin3
SWCLK       → U1.PA14, J6.pin4, J7.pin4
NRST        → U1.NRST, J6.pin5, J7.pin5
SWO         → U1.PB3, J6.pin6, J7.pin6

(No additional passives - NRST pull-up in Section 01)
```

---

## 5. Component Specifications

### Master BOM with JLCPCB Part Numbers

| Ref | Description | Part Number | Package | LCSC | Qty |
|-----|-------------|-------------|---------|------|-----|
| U1 | MCU Dual-Core | STM32H747IIT6 | LQFP-176 | C2062629 | 1 |
| U3 | Buck-Boost Charger | BQ25792RQMR | QFN-29 | C5446540 | 1 |
| U4 | BMS Protection | HY2120-CB | SOT-23-6 | C113632 | 1 |
| U5, U6 | Cell Balancer | HY2213-BB3A | SOT-23-6 | C113633 | 2 |
| U7 | 3.3V Buck | TPS62133RGTR | QFN-16 | C129780 | 1 |
| U8 | 5V Buck | TPS62130RGTR | QFN-16 | C129779 | 1 |
| U9 | Barometric | BMP390 | LGA-10 | C2682150 | 1 |
| U10 | IMU | ICM-42688-P | LGA-14 | C2839277 | 1 |
| U11 | Magnetometer | MMC5983MA | LGA-16 | C2842878 | 1 |
| U12 | Gas Sensor | MICS-4514 | SMD | C3659364 | 1 |
| U13 | Temp/Humidity | Si7021-A20-GM | DFN-6 | C82463 | 1 |
| U14, U15 | SD ESD | USBLC6-2SC6 | SOT-23-6 | C7519 | 2 |
| U16 | I/O Expander | MCP23017-E/SO | SOIC-28 | C47023 | 1 |
| U17 | BLE Module | ESP32-C3-MINI-1 | Module | C2838502 | 1 |
| U1801 | CAN Transceiver | TCAN1042-Q1 | SOIC-8 | C129966 | 1 |
| U2 | USB ESD | USBLC6-2SC6 | SOT-23-6 | C7519 | 1 |
| Q1-Q4, Q5-Q7 | N-ch MOSFET | AO3400A | SOT-23 | C20917 | 7 |
| D1801 | CAN TVS | PESD2CAN | SOT-23 | C85391 | 1 |
| D1 | Input TVS | SMBJ12A | SMB | C64944 | 1 |
| D2 | Flyback | 1N4148W | SOD-123 | C81598 | 1 |
| F1 | PTC Fuse | 1812L200DR | 1812 | C145565 | 1 |
| Y1 | 8MHz Crystal | X322508MSB4SI | 3225 | C9002 | 1 |
| Y2 | 32.768kHz Crystal | X322532768MSB4GI | 3215 | C32346 | 1 |
| L1 | 2.2µH 5A | MWSA0503S-2R2MT | 5×5mm | C408356 | 1 |
| L2, L3 | 1.5µH 4A | XAL4020-152MEB | 4×4mm | C191761 | 2 |
| J1 | USB-C 16-pin | U262-161N-4BVC11 | SMD | C319148 | 1 |
| J3 | microSD | MEM2067-02-180-00-A | SMD | C393941 | 1 |
| J1801 | M8 4-pin | Panel mount | — | External | 1 |
| BZ1 | Buzzer | MLT-8530 | SMD | C94599 | 1 |
| LED1-16 | RGB LED | WS2812B-2020 | 2020 | C965555 | 16 |
| ENC1, ENC2 | Encoder | EC11 | TH | C470754 | 2 |
| SW1-8 | Toggle | MTS-102 | TH | C431540 | 8 |

### Passive Components Summary

| Value | Package | LCSC | Qty | Usage |
|-------|---------|------|-----|-------|
| 100nF | 0603 | C14663 | 45 | Decoupling |
| 10µF | 0805 | C19702 | 15 | Bulk caps |
| 22µF | 0805 | C45783 | 8 | Output caps |
| 4.7µF | 0805 | C1779 | 3 | Bulk caps |
| 2.2µF | 0603 | C23630 | 2 | VCAP |
| 1µF | 0603 | C15849 | 6 | Filtering |
| 15pF | 0603 | C1644 | 2 | HSE load |
| 6.8pF | 0603 | C105620 | 2 | LSE load |
| 10nF | 0603 | C57112 | 8 | HF filter |
| 4.7nF | 0603 | C1590 | 2 | CAN term |
| 10kΩ | 0603 | C25804 | 25 | Pull-up/down |
| 4.7kΩ | 0603 | C23162 | 4 | I2C pull-up |
| 5.1kΩ | 0603 | C23186 | 2 | USB CC |
| 100kΩ | 0603 | C25803 | 4 | Feedback |
| 24.3kΩ | 0603 | C23351 | 1 | FB divider |
| 1kΩ | 0603 | C21190 | 4 | Gate series |
| 330Ω | 0603 | C23138 | 1 | LED series |
| 33Ω | 0603 | C23140 | 10 | Signal series |
| 100Ω | 0603 | C22775 | 3 | Gate series |
| 62Ω | 0805 | C23220 | 2 | Balance |
| 60Ω | 0603 | — | 4 | CAN term |
| 10mΩ | 0805 | C22859 | 1 | Current sense |
| 5mΩ | 1206 | C318841 | 1 | BMS sense |

---

## 6. Inter-Section Connections

### Power Net Distribution

```
VSYS (Section 03 BQ25792)
  ├──► Section 05 (TPS62133 VIN)
  ├──► Section 06 (TPS62130 VIN)
  └──► Section 04 (BMS if needed)

+3V3 (Section 05 TPS62133)
  ├──► Section 01 (MCU all VDD)
  ├──► Section 07 (BMP390)
  ├──► Section 08 (ICM-42688-P)
  ├──► Section 09 (MMC5983MA)
  ├──► Section 11 (Si7021)
  ├──► Section 12 (Display logic)
  ├──► Section 13 (microSD)
  ├──► Section 14 (MCP23017)
  ├──► Section 17 (ESP32-C3)
  ├──► Section 18 (CAN transceiver)
  └──► Section 20 (Debug header)

+5V (Section 06 TPS62130)
  ├──► Section 10 (MICS-4514 heater)
  ├──► Section 12 (Display backlight)
  ├──► Section 15 (WS2812B LEDs)
  └──► Section 18 (M8 connector pin 1 → pods)
```

### I2C Bus Topology

```
I2C1 (400 kHz) - PB8/PB9
  │
  ├─[4.7k pull-ups to +3V3]
  │
  ├──► BMP390 (0x77)
  ├──► ICM-42688-P (0x68)
  ├──► MMC5983MA (0x30)
  ├──► Si7021 (0x40)
  └──► BQ25792 (0x6B)

I2C4 (400 kHz) - via FFC to SwitchLED Board
  │
  ├─[4.7k pull-ups at MCP23017]
  │
  └──► MCP23017 (0x20)
```

### FFC Connector Pinout (MainBoard ↔ SwitchLED Board)

| Pin | Signal | Direction | Notes |
|-----|--------|-----------|-------|
| 1 | +5V | Out | LED power |
| 2 | +3V3 | Out | Logic power |
| 3 | GND | — | Ground |
| 4 | I2C4_SDA | Bidir | To MCP23017 |
| 5 | I2C4_SCL | Out | To MCP23017 |
| 6 | LED_DATA | Out | WS2812B chain |
| 7 | MCP_INTA | In | Interrupt |
| 8 | ENC1_A | In | Encoder 1 |
| 9 | ENC1_B | In | Encoder 1 |
| 10 | GND | — | Ground |

---

## 7. Design Rules

### PCB Specifications

| Parameter | Value |
|-----------|-------|
| Layers | 4 |
| Thickness | 1.6mm |
| Min trace | 0.127mm (5 mil) |
| Min space | 0.127mm (5 mil) |
| Min drill | 0.3mm |
| Copper weight | 1oz outer, 0.5oz inner |
| Finish | ENIG |
| Solder mask | Green |
| Silkscreen | White |

### Layer Stack

| Layer | Function |
|-------|----------|
| L1 (Top) | Components, signals |
| L2 | GND plane |
| L3 | Power planes (+3V3, +5V, VSYS) |
| L4 (Bottom) | Components, signals |

### Critical Layout Guidelines

1. **MCU (U1):** Place decoupling caps within 3mm of VDD pins. Keep HSE/LSE traces short (<10mm).

2. **Power (U3, U7, U8):** Place input caps at VIN pins. Minimize SW node area. Use thermal vias under QFN pads.

3. **Sensors (U9-U13):** Keep away from power inductors and switching nodes. Provide local ground plane.

4. **CAN (U1801):** Route CANH/CANL as differential pair. Place TVS close to connector.

5. **USB (J1):** Route D+/D- as 90Ω differential pair. Place ESD close to connector.

6. **microSD (J3):** Keep SDMMC traces matched length (±5mm). Place ESD at connector.

---

## Appendix A: Net Name Reference

| Net Name | Type | Voltage | Section |
|----------|------|---------|---------|
| GND | Power | 0V | All |
| +3V3 | Power | 3.3V | 05→All |
| +5V | Power | 5.0V | 06→10,12,15,18 |
| VSYS | Power | 6-8.4V | 03→05,06 |
| VBUS | Power | 5V | USB→03 |
| VBAT | Power | 6-8.4V | Battery |
| I2C1_SDA | Signal | 3.3V | 01,07-11 |
| I2C1_SCL | Signal | 3.3V | 01,07-11 |
| I2C4_SDA | Signal | 3.3V | 01,14 |
| I2C4_SCL | Signal | 3.3V | 01,14 |
| FDCAN1_TX | Signal | 3.3V | 01,18 |
| FDCAN1_RX | Signal | 3.3V | 01,18 |
| CANH | CAN | Diff | 18 |
| CANL | CAN | Diff | 18 |
| USB_DP | USB | Diff | 02 |
| USB_DM | USB | Diff | 02 |
| LED_DATA | Signal | 5V | 01,15 |
| SD_CLK | Signal | 3.3V | 01,13 |
| SD_CMD | Signal | 3.3V | 01,13 |
| SD_D0-D3 | Signal | 3.3V | 01,13 |
| DISP_SCK | Signal | 3.3V | 01,12 |
| DISP_MOSI | Signal | 3.3V | 01,12 |
| SWDIO | Debug | 3.3V | 01,20 |
| SWCLK | Debug | 3.3V | 01,20 |
| NRST | Reset | 3.3V | 01,20 |

---

## Appendix B: I2C Address Map

| Address | Device | Section |
|---------|--------|---------|
| 0x20 | MCP23017 | 14 |
| 0x30 | MMC5983MA | 09 |
| 0x40 | Si7021 | 11 |
| 0x68 | ICM-42688-P | 08 |
| 0x6B | BQ25792 | 03 |
| 0x77 | BMP390 | 07 |

---

## Document Control

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-01-30 | Claude Code | Initial handoff package |

---

*End of Circuit Mind Handoff Package*
