# IMSAFE Schematic Sections Breakdown

**Date:** 2026-01-28
**Purpose:** Break schematic into logical sections for systematic development
**Rules:**
- Minimum component size: 0603 (unless no alternative)
- MCU package: LQFP-176 (hand-solderable)

---

## Section Index

| # | Section | Main IC(s) | Status |
|---|---------|------------|--------|
| 1 | MCU Core | STM32H747AG | Pending |
| 2 | Power Input (USB-C) | - | Pending |
| 3 | Battery Charger | BQ25792 | Pending |
| 4 | BMS | HY2120 + HY2213 | Pending |
| 5 | 3.3V Regulator | TPS62133 | Pending |
| 6 | 5V Regulator | TPS62130 | Pending |
| 7 | Barometric Sensor | BMP390 | Pending |
| 8 | IMU | ICM-42688-P | Pending |
| 9 | Magnetometer | MMC5983MA | Pending |
| 10 | Gas Sensor | MICS-4514 | Pending |
| 11 | Temp/Humidity | Si7021 | Pending |
| 12 | Display Interface | ST7789 FPC | Pending |
| 13 | microSD | MEM2067 | Pending |
| 14 | I/O Expander & Switches | MCP23017 | Pending |
| 15 | Status LEDs | WS2812B-2020 | Pending |
| 16 | Rotary Encoders | EC11 | Pending |
| 17 | Bluetooth Module | ESP32-C3-MINI-1 | Pending |
| 18 | Vibration Sensor Interface | M8 connectors | Pending |
| 19 | Audio Alarm | MLT-8530 | Pending |
| 20 | Debug & Test Points | Tag-Connect | Pending |

---

## Section 1: MCU Core (STM32H747AG)

### Main Component
- **U1:** STM32H747AGI6 (LQFP-176)

### Power Supply Decoupling
Per ST AN4661, STM32H7 requires extensive decoupling.

| Ref | Value | Package | Pin Group | Notes |
|-----|-------|---------|-----------|-------|
| C101 | 4.7µF | 0805 | VDD bulk | Near pin 1 corner |
| C102-C111 | 100nF | 0603 | Each VDD pin | 10× VDD pins |
| C112-C115 | 100nF | 0603 | Each VDDA pin | 4× analog |
| C116 | 1µF | 0603 | VDDA bulk | Analog supply |
| C117-C118 | 100nF | 0603 | VDDUSB | USB PHY |
| C119-C120 | 2.2µF | 0603 | VCAP1, VCAP2 | Internal regulator |
| C121 | 100nF | 0603 | VBAT | RTC backup |
| C122 | 10nF | 0603 | VREF+ | Reference filter |

**Total decoupling:** ~22 capacitors

### HSE Crystal (8 MHz)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Y1 | 8 MHz | 3225 | 20ppm, 10pF load |
| C123 | 15pF | 0603 | OSC_IN load cap |
| C124 | 15pF | 0603 | OSC_OUT load cap |
| R101 | 1MΩ | 0603 | Feedback resistor (optional) |

### LSE Crystal (32.768 kHz)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Y2 | 32.768kHz | 3215 | 20ppm, 6pF load |
| C125 | 6.8pF | 0603 | OSC32_IN load cap |
| C126 | 6.8pF | 0603 | OSC32_OUT load cap |
| R102 | 10MΩ | 0603 | Feedback resistor (optional) |

### Reset Circuit
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R103 | 10kΩ | 0603 | NRST pull-up |
| C127 | 100nF | 0603 | NRST filter cap |

### Boot Configuration
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R104 | 10kΩ | 0603 | BOOT0 pull-down (boot from flash) |

### Section 1 Passive Count: ~28 components

---

## Section 2: Power Input (USB-C)

### Main Components
- **J1:** USB-C receptacle 16-pin (U262-161N-4BVC11)
- **U2:** ESD protection (USBLC6-2SC6)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R201 | 5.1kΩ | 0603 | CC1 pull-down (UFP) |
| R202 | 5.1kΩ | 0603 | CC2 pull-down (UFP) |
| C201 | 100nF | 0603 | VBUS filter |
| C202 | 10µF | 0805 | VBUS bulk |

### Section 2 Passive Count: ~4 components

---

## Section 3: Battery Charger (BQ25792)

### Main Component
- **U3:** BQ25792RQMR (QFN-29 4×4mm)

### Input Stage
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C301 | 10µF 25V | 0805 | VBUS input |
| C302 | 100nF | 0603 | VBUS HF filter |
| C303 | 10µF 25V | 0805 | PMID cap |

### Output Stage (to battery)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| L1 | 2.2µH 5A | 5×5mm | Power inductor |
| C304 | 22µF 16V | 0805 | BAT output |
| C305 | 22µF 16V | 0805 | BAT output |

### System Output (VSYS)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C306 | 22µF 16V | 0805 | VSYS output |
| C307 | 100nF | 0603 | VSYS HF |

### Sensing & Config
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R301 | 10mΩ | 0805 | Input current sense |
| R302 | 5mΩ | 1206 | Battery current sense |
| R303 | 10kΩ | 0603 | NTC thermistor bias |
| R304 | 100kΩ | 0603 | ILIM set (if used) |
| C308 | 100nF | 0603 | REGN bypass |
| C309 | 1µF | 0603 | PMID LDO |

### I2C Pull-ups (shared bus)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R305 | 4.7kΩ | 0603 | SDA pull-up |
| R306 | 4.7kΩ | 0603 | SCL pull-up |

### Section 3 Passive Count: ~16 components

---

## Section 4: BMS (HY2120 + HY2213)

### Main Components
- **U4:** HY2120-CB (SOT-23-6) - Protection
- **U5, U6:** HY2213-BB3A (SOT-23-6) - Balancers ×2
- **Q1-Q4:** AO3400A (SOT-23) - MOSFETs ×4

### Protection IC (HY2120)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C401 | 100nF | 0603 | VDD bypass |
| R401 | 1kΩ | 0603 | OD gate resistor |
| R402 | 1kΩ | 0603 | OC gate resistor |

### Current Sensing
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R403 | 5mΩ | 1206 | Overcurrent sense |

### Cell Balancing (per cell)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R404 | 62Ω 1/4W | 0805 | Cell 1 balance load |
| R405 | 62Ω 1/4W | 0805 | Cell 2 balance load |
| C402 | 100nF | 0603 | U5 bypass |
| C403 | 100nF | 0603 | U6 bypass |

### Section 4 Passive Count: ~9 components

---

## Section 5: 3.3V Regulator (TPS62133)

### Main Component
- **U7:** TPS62133RGTR (QFN-16 3×3mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| L2 | 1.5µH 4A | 4×4mm | Shielded inductor |
| C501 | 10µF 16V | 0805 | Input cap |
| C502 | 100nF | 0603 | Input HF |
| C503 | 22µF 10V | 0805 | Output cap |
| C504 | 22µF 10V | 0805 | Output cap |
| C505 | 100nF | 0603 | Output HF |
| R501 | 100kΩ | 0603 | EN pull-up (optional) |

### Section 5 Passive Count: ~7 components

---

## Section 6: 5V Regulator (TPS62130)

### Main Component
- **U8:** TPS62130RGTR (QFN-16 3×3mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| L3 | 1.5µH 4A | 4×4mm | Shielded inductor |
| C601 | 10µF 16V | 0805 | Input cap |
| C602 | 100nF | 0603 | Input HF |
| C603 | 22µF 10V | 0805 | Output cap |
| C604 | 22µF 10V | 0805 | Output cap |
| C605 | 100nF | 0603 | Output HF |
| R601 | 100kΩ | 0603 | FB top (sets 5.09V) |
| R602 | 24.3kΩ | 0603 | FB bottom |
| R603 | 100kΩ | 0603 | EN pull-up (optional) |

### Section 6 Passive Count: ~9 components

---

## Section 7: Barometric Sensor (BMP390)

### Main Component
- **U9:** BMP390 (LGA-10 2×2mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C701 | 100nF | 0603 | VDD bypass |
| C702 | 100nF | 0603 | VDDIO bypass |

### Section 7 Passive Count: ~2 components

---

## Section 8: IMU (ICM-42688-P)

### Main Component
- **U10:** ICM-42688-P (LGA-14 2.5×3mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C801 | 100nF | 0603 | VDD bypass |
| C802 | 100nF | 0603 | VDDIO bypass |
| C803 | 10nF | 0603 | RESV cap (pin 6) |
| C804 | 100nF | 0603 | Additional decoupling |

### Section 8 Passive Count: ~4 components

---

## Section 9: Magnetometer (MMC5983MA)

### Main Component
- **U11:** MMC5983MA (LGA-16 3×3mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C901 | 100nF | 0603 | VDD bypass |
| C902 | 100nF | 0603 | VDDIO bypass |
| C903 | 4.7µF | 0603 | C1 cap (internal use) |

### Section 9 Passive Count: ~3 components

---

## Section 10: Gas Sensor (MICS-4514)

### Main Component
- **U12:** MICS-4514 (SMD)

### Heater Control
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Q5 | AO3400A | SOT-23 | Heater switch |
| R1001 | 10kΩ | 0603 | Gate pull-down |
| R1002 | 100Ω | 0603 | Gate series |

### Signal Conditioning (Reducing - CO)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R1003 | 10kΩ | 0603 | Load resistor |
| C1001 | 100nF | 0603 | ADC input filter |
| R1004 | 1kΩ | 0603 | Series protection |

### Signal Conditioning (Oxidizing - NO2)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R1005 | 10kΩ | 0603 | Load resistor |
| C1002 | 100nF | 0603 | ADC input filter |
| R1006 | 1kΩ | 0603 | Series protection |

### Section 10 Passive Count: ~9 components

---

## Section 11: Temp/Humidity (Si7021)

### Main Component
- **U13:** Si7021-A20-GM (DFN-6 3×3mm)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1101 | 100nF | 0603 | VDD bypass |

### Section 11 Passive Count: ~1 component

---

## Section 12: Display Interface (ST7789)

### Main Component
- **J2:** FPC connector (for display module)

### If SPI Interface
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R1201 | 33Ω | 0603 | SPI CLK series |
| R1202 | 33Ω | 0603 | SPI MOSI series |
| C1201 | 100nF | 0603 | Display VDD bypass |
| C1202 | 10µF | 0805 | Display bulk |

### Backlight Control
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Q6 | AO3400A | SOT-23 | Backlight PWM switch |
| R1203 | 10kΩ | 0603 | Gate pull-down |
| R1204 | 100Ω | 0603 | Gate series |

### Section 12 Passive Count: ~7 components

---

## Section 13: microSD (MEM2067)

### Main Components
- **J3:** MEM2067-02-180-00-A (hinged socket)
- **U14, U15:** USBLC6-2SC6 (ESD) ×2

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1301 | 100nF | 0603 | VDD bypass |
| C1302 | 10µF | 0805 | VDD bulk |
| R1301-R1304 | 33Ω | 0603 | DAT0-3 series resistors ×4 |
| R1305 | 33Ω | 0603 | CMD series |
| R1306 | 33Ω | 0603 | CLK series |
| R1307 | 10kΩ | 0603 | Card detect pull-up |

### Section 13 Passive Count: ~9 components

---

## Section 14: I/O Expander & Switches (MCP23017)

### Main Component
- **U16:** MCP23017-E/SO (SOIC-28)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1401 | 100nF | 0603 | VDD bypass |
| R1401-R1403 | 10kΩ | 0603 | A0-A2 address (to GND) ×3 |
| R1404 | 10kΩ | 0603 | RESET pull-up |

### Switch Connections (directly to MCP23017 with internal pull-ups enabled)
- SW1-SW8: Toggle switches to GND (no external passives needed)

### Section 14 Passive Count: ~5 components

---

## Section 15: Status LEDs (WS2812B-2020)

### Main Components
- **LED1-LED16:** WS2812B-2020 ×16

### Passives (per LED)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1501-C1516 | 100nF | 0603 | Decoupling per LED ×16 |

### Data Line
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R1501 | 330Ω | 0603 | Data series resistor (first LED) |

### Section 15 Passive Count: ~17 components

---

## Section 16: Rotary Encoders (EC11)

### Main Components
- **ENC1, ENC2:** EC11 rotary encoder with switch ×2

### Debounce (per encoder)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1601 | 10nF | 0603 | Encoder A debounce |
| C1602 | 10nF | 0603 | Encoder B debounce |
| C1603 | 100nF | 0603 | Switch debounce |
| C1604-C1606 | (same for ENC2) | 0603 | ×3 |

### Section 16 Passive Count: ~6 components

---

## Section 17: Bluetooth Module (ESP32-C3-MINI-1)

### Main Component
- **U17:** ESP32-C3-MINI-1 (module)

### Passives
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1701 | 10µF | 0805 | 3V3 bulk |
| C1702 | 100nF | 0603 | 3V3 bypass |
| R1701 | 10kΩ | 0603 | EN pull-up |
| R1702 | 10kΩ | 0603 | IO9 pull-up (boot mode) |
| C1703 | 1µF | 0603 | EN delay cap |

### Section 17 Passive Count: ~5 components

---

## Section 18: Vibration Sensor Interface

### Main Components
- **J4, J5:** M8 4-pin connectors ×2 (or header for dev)

### ESD & Filtering (per channel)
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| C1801 | 100nF | 0603 | SPI CLK filter |
| C1802 | 100nF | 0603 | SPI MISO filter |
| R1801 | 33Ω | 0603 | SPI CLK series |
| R1802 | 33Ω | 0603 | SPI MISO series |
| (repeat for second channel) | | | |

### Section 18 Passive Count: ~8 components

---

## Section 19: Audio Alarm (MLT-8530)

### Main Component
- **BZ1:** MLT-8530 (active buzzer)

### Driver
| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Q7 | AO3400A | SOT-23 | Buzzer driver |
| R1901 | 10kΩ | 0603 | Gate pull-down |
| R1902 | 100Ω | 0603 | Gate series |
| D2 | 1N4148W | SOD-123 | Flyback diode |

### Section 19 Passive Count: ~4 components

---

## Section 20: Debug & Test Points

### Main Components
- **J6:** Tag-Connect TC2030-IDC-NL footprint
- **J7:** 6-pin debug header (optional)

### Passives
(NRST pull-up already in MCU section)

### Test Points
| Ref | Net | Notes |
|-----|-----|-------|
| TP1 | 3.3V | Power test |
| TP2 | 5V | Power test |
| TP3 | VBAT | Battery test |
| TP4 | GND | Ground test |
| TP5 | I2C_SDA | Debug |
| TP6 | I2C_SCL | Debug |

### Section 20 Passive Count: ~0 components (test points only)

---

## Summary

| Section | Main IC | Passives | Status |
|---------|---------|----------|--------|
| 1. MCU Core | STM32H747AG | 28 | Pending |
| 2. USB-C Input | - | 4 | Pending |
| 3. Charger | BQ25792 | 16 | Pending |
| 4. BMS | HY2120/HY2213 | 9 | Pending |
| 5. 3.3V Reg | TPS62133 | 7 | Pending |
| 6. 5V Reg | TPS62130 | 9 | Pending |
| 7. Barometric | BMP390 | 2 | Pending |
| 8. IMU | ICM-42688-P | 4 | Pending |
| 9. Magnetometer | MMC5983MA | 3 | Pending |
| 10. Gas Sensor | MICS-4514 | 9 | Pending |
| 11. Temp/Humidity | Si7021 | 1 | Pending |
| 12. Display | ST7789 FPC | 7 | Pending |
| 13. microSD | MEM2067 | 9 | Pending |
| 14. I/O Expander | MCP23017 | 5 | Pending |
| 15. Status LEDs | WS2812B ×16 | 17 | Pending |
| 16. Encoders | EC11 ×2 | 6 | Pending |
| 17. Bluetooth | ESP32-C3 | 5 | Pending |
| 18. Vibration I/F | M8 conn | 8 | Pending |
| 19. Audio | MLT-8530 | 4 | Pending |
| 20. Debug | Tag-Connect | 0 | Pending |
| **TOTAL** | | **~153** | |

---

## Schematic Workflow

1. Complete each section individually
2. Verify passives against datasheet recommendations
3. Assign net names consistently
4. Review inter-section connections
5. Run ERC (Electrical Rules Check)
6. Generate netlist
7. Cross-check with BOM

---

## Net Naming Convention

| Prefix | Meaning | Example |
|--------|---------|---------|
| +3V3 | 3.3V rail | +3V3 |
| +5V | 5V rail | +5V |
| VBAT | Battery voltage | VBAT |
| VSYS | System voltage (from charger) | VSYS |
| GND | Ground | GND |
| I2C1_ | I2C bus 1 | I2C1_SDA, I2C1_SCL |
| SPI1_ | SPI bus 1 | SPI1_SCK, SPI1_MOSI |
| USART2_ | UART 2 | USART2_TX, USART2_RX |
| GPIO_ | General purpose | GPIO_LED_DATA |
