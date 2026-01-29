# Manual Schematic Completion Guide

## Overview

This guide helps you complete the missing schematics in KiCad. After you place the main ICs and create the basic wiring, Claude Code can add the passive components (resistors, capacitors, etc.).

---

## Current Status Summary

| Section | Status | What You Need to Do |
|---------|--------|---------------------|
| 01-06 | ✅ Complete | ERC cleanup only |
| 07 Barometric | ⚠️ IC Missing | Place BMP390 + wire I2C |
| 08 IMU | ⚠️ IC Missing | Place ICM-42688-P + wire I2C/INT |
| 09 Magnetometer | ⚠️ IC Missing | Place MMC5983MA + wire I2C/INT |
| 10 Gas Sensor | ⚠️ IC Missing | Place MICS-4514 + wire analog |
| 11 Temp/Humidity | ⚠️ IC Missing | Place Si7021-A20 + wire I2C |
| 12 Display | ❌ Empty | Place FPC connector + backlight circuit |
| 13 microSD | ❌ Empty | Place SD socket + ESD protection |
| 14 I/O Expander | ✅ Complete | ERC cleanup only |
| 15 Status LEDs | ⚠️ Partial | Wire WS2812B chain |
| 16 Rotary Encoders | ⚠️ Partial | Verify encoder wiring |
| 17 Bluetooth | ❌ Empty | Place ESP32-C3-MINI-1 + antenna |
| 18 Vibration | ❌ Empty | Place M8 connectors |
| 19 Audio | ✅ Complete | ERC cleanup only |
| 20 Debug | ❌ Empty | Place Tag-Connect + test points |

---

## Phase 1: Sensor ICs (Sections 07-11)

These sections have bypass capacitors placed. You just need to add the main IC and wire it.

### Section 07: Barometric Sensor (BMP390)

**Symbol to use:** Create custom or use generic IC symbol
- Package: LGA-10 (2.0 x 2.0 mm)
- JLCPCB: C1930854

**Connections to make:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | VDD | +3V3 |
| 2 | GND | GND |
| 3 | SDI/SDA | I2C1_SDA (hierarchical label) |
| 4 | SDO/ADR | GND (I2C address = 0x76) |
| 5 | SCK/SCL | I2C1_SCL (hierarchical label) |
| 6 | CSB | +3V3 (I2C mode) |
| 7 | INT | BARO_INT (hierarchical label, optional) |
| 8-10 | NC | No connect |

**Steps:**
1. Open Section_07_Barometric.kicad_sch
2. Add symbol for BMP390 (or create from datasheet)
3. Place at approximately (127, 115)
4. Add hierarchical labels: I2C1_SDA, I2C1_SCL, BARO_INT
5. Wire VDD to +3V3 power symbol
6. Wire GND to ground
7. Wire SDO/ADR and CSB to set I2C mode
8. Add no-connect flags to NC pins (press Q)
9. Connect bypass cap C701 between VDD and GND

---

### Section 08: IMU (ICM-42688-P)

**Symbol to use:** Create custom or use generic IC symbol
- Package: LGA-14 (2.5 x 3.0 mm)
- JLCPCB: C1850418

**Connections to make:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | AP_SDO/AP_AD0 | GND (I2C addr 0x68) |
| 2 | RESV | 10nF cap to GND (C803) |
| 3 | RESV | No connect |
| 4 | INT1 | IMU_INT (hierarchical label) |
| 5 | VDDIO | +3V3 |
| 6 | GND | GND |
| 7 | RESV | No connect |
| 8 | VDD | +3V3 |
| 9 | GND | GND |
| 10 | RESV | No connect |
| 11 | INT2 | No connect (or IMU_INT2) |
| 12 | AP_CS | +3V3 (I2C mode) |
| 13 | AP_SCL/AP_SCLK | I2C1_SCL |
| 14 | AP_SDA/AP_SDIO | I2C1_SDA |

**Steps:**
1. Open Section_08_IMU.kicad_sch
2. Add ICM-42688-P symbol
3. Wire per table above
4. Ensure C803 (10nF) connects between RESV pin 2 and GND
5. Add no-connect flags to unused RESV pins

---

### Section 09: Magnetometer (MMC5983MA)

**Symbol to use:** Create custom
- Package: LGA-16 (3.0 x 3.0 mm)
- JLCPCB: C404329

**Connections to make:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | VDD | +3V3 |
| 2 | VDDIO | +3V3 |
| 3 | GND | GND |
| 4-5 | SA0-SA1 | GND (addr 0x30) |
| 6 | SDA/SDI | I2C1_SDA |
| 7 | SCL/SCK | I2C1_SCL |
| 8 | CS | +3V3 (I2C mode) |
| 9 | C1 | 4.7µF cap to GND (C903) |
| 10 | INT | MAG_INT (hierarchical label) |
| 11-14 | NC | No connect |
| 15-16 | GND | GND |

---

### Section 10: Gas Sensor (MICS-4514)

**Symbol to use:** Create custom
- Package: Through-hole/SMD hybrid
- JLCPCB: C3659364

**This is an analog sensor with heater:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | HGND | GND |
| 2 | OX_OUT | ADC_GAS_OX (to MCU ADC) |
| 3 | PRE_HEAT | GPIO for heater control (via MOSFET) |
| 4 | RED_OUT | ADC_GAS_RED (to MCU ADC) |
| 5 | H+ | +5V (heater power) |
| 6 | RGND | GND |

**Note:** Heater draws ~50mA. Use N-channel MOSFET (AO3400A) for PWM control.

---

### Section 11: Temp/Humidity (Si7021-A20)

**Symbol to use:** Create custom
- Package: DFN-6 (3.0 x 3.0 mm)
- JLCPCB: C132603

**Connections to make:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | SDA | I2C1_SDA |
| 2 | GND | GND |
| 3 | NC | No connect |
| 4 | SCL | I2C1_SCL |
| 5 | VDD | +3V3 |
| 6 | GND | GND |

**Simplest sensor - fixed I2C address 0x40**

---

## Phase 2: Interface Circuits (Sections 12, 13, 17, 18, 20)

### Section 12: Display Interface (ST7789)

**Components needed:**
1. **FPC Connector** - 24-pin 0.5mm pitch (for typical ST7789 module)
2. **Backlight MOSFET** - AO3400A for PWM brightness control
3. **Series resistor** - For backlight current limiting

**Typical ST7789 module pinout (varies by module):**
| Pin | Name | Connect To |
|-----|------|------------|
| 1-2 | GND | GND |
| 3 | LEDK | Backlight cathode (via MOSFET to GND) |
| 4-7 | LEDA | +3V3 (via current limit resistor) |
| 8 | RESET | GPIO or tied to +3V3 via RC |
| 9 | RS/DC | SPI_DC (data/command select) |
| 10 | SDA | SPI1_MOSI |
| 11 | SCL | SPI1_SCK |
| 12 | CS | SPI1_CS_DISP |
| 13-14 | GND | GND |
| 15 | VDD | +3V3 |
| ... | ... | ... |

**Steps:**
1. Find your specific display module pinout
2. Add FPC connector symbol
3. Add AO3400A MOSFET for backlight
4. Wire SPI signals to hierarchical labels
5. Add backlight PWM control (DISP_BL)

---

### Section 13: microSD (MEM2067)

**Components needed:**
1. **MEM2067-02-180-01-A** - microSD socket (JLCPCB: C727177)
2. **USBLC6-2SC6** - ESD protection for data lines (optional)

**Connections:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | DAT2 | SD_DAT2 (or NC for SPI mode) |
| 2 | CD/DAT3 | SD_CS (SPI chip select) |
| 3 | CMD/DI | SPI_MOSI |
| 4 | VDD | +3V3 |
| 5 | CLK | SPI_SCK |
| 6 | VSS | GND |
| 7 | DAT0/DO | SPI_MISO |
| 8 | DAT1 | SD_DAT1 (or NC for SPI mode) |
| CD | Card Detect | GPIO with pull-up |

---

### Section 17: Bluetooth (ESP32-C3-MINI-1)

**Symbol to use:** ESP32-C3-MINI-1 (available in KiCad libraries)
- Package: Module with castellated pads
- JLCPCB: C2838502

**Key connections:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | GND | GND |
| 2 | 3V3 | +3V3 |
| 3 | EN | +3V3 via 10k + 100nF to GND (RC reset) |
| 4-15 | GPIO | Various functions |
| 16 | TXD0 | UART_RX (to MCU) |
| 17 | RXD0 | UART_TX (from MCU) |
| 39-53 | GND | GND (multiple pads) |

**Note:** Module has internal antenna. Keep GND plane clear near antenna area.

---

### Section 18: Vibration Sensor Interface (M8 Connectors)

**Components needed:**
1. **M8 4-pin female connectors** x2 (for external sensor pods)

**Typical M8 wiring (for ADXL355/372 pod):**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | VDD | +3V3 |
| 2 | GND | GND |
| 3 | SDA | I2C2_SDA |
| 4 | SCL | I2C2_SCL |

**Steps:**
1. Add M8 connector symbols (2x)
2. Create hierarchical labels for I2C2 bus
3. Add ESD protection diodes on connector pins

---

### Section 20: Debug & Test Points

**Components needed:**
1. **Tag-Connect TC2030-IDC** footprint (no schematic symbol needed, or use 6-pin header)
2. **Test point** symbols for key signals

**SWD connections:**
| Pin | Name | Connect To |
|-----|------|------------|
| 1 | VCC | +3V3 |
| 2 | SWDIO | PA13 (STM32 SWDIO) |
| 3 | NRST | NRST |
| 4 | SWCLK | PA14 (STM32 SWCLK) |
| 5 | GND | GND |
| 6 | SWO | PB3 (optional trace output) |

**Recommended test points:**
- TP1: +3V3
- TP2: +5V
- TP3: VBAT
- TP4: GND
- TP5: UART_TX (debug)
- TP6: UART_RX (debug)

---

## Phase 3: Cleanup Existing Sections

### Section 15: Status LEDs (WS2812B-2020)

**Verify:**
1. DIN of first LED connected to GPIO
2. DOUT of each LED chains to DIN of next
3. Each LED has 100nF bypass cap between VDD and GND
4. 300-500Ω series resistor on data line (first LED only)

### Section 16: Rotary Encoders

**Verify:**
1. Encoder A/B outputs have debounce capacitors (100nF)
2. Switch (push button) has debounce capacitor
3. Common pin connected to GND
4. Outputs connected to GPIO (directly or via I/O expander)

---

## Handoff Checklist

Before handing off to Claude Code for passive placement:

### For each section, ensure:
- [ ] Main IC symbol is placed
- [ ] IC is wired to power (+3V3, +5V, GND as appropriate)
- [ ] Communication interfaces are wired (I2C, SPI, UART)
- [ ] Interrupt/GPIO lines have hierarchical labels
- [ ] No-connect flags on unused pins (press Q in KiCad)
- [ ] Power symbols placed (+3V3, GND, etc.)

### After your manual work:
1. Save all schematics
2. Run ERC to see remaining errors
3. Commit changes to git
4. Tell Claude Code which sections are ready

### What Claude Code will add:
- Bypass capacitors (100nF on each VDD pin)
- Bulk capacitors (4.7µF-10µF per power domain)
- Pull-up/pull-down resistors (calculated values)
- Series resistors (for LEDs, data lines)
- ESD protection components
- Filter capacitors (for ADC inputs)

---

## Symbol Creation Tips

If KiCad doesn't have a symbol for a component:

1. **Use Symbol Editor** (Tools > Symbol Editor)
2. **Create new symbol** in project library
3. **Add pins** matching the datasheet
4. **Set pin types:**
   - Power input: VDD, VCC
   - Power output: (for regulators)
   - Input: SCL, CS, EN
   - Bidirectional: SDA, data lines
   - Output: INT, data out
   - Passive: For resistors/capacitors
   - No connect: NC pins

5. **Save and use** in schematic

---

## Approximate Time Estimates

| Phase | Sections | Estimated Time |
|-------|----------|----------------|
| Phase 1 | 07-11 (Sensors) | 2-3 hours |
| Phase 2 | 12, 13, 17, 18, 20 | 3-4 hours |
| Phase 3 | 15, 16 cleanup | 30 min |
| ERC Fixes | All sections | 1-2 hours |

**Total: ~8-10 hours of manual KiCad work**

After that, Claude Code can add ~100+ passive components programmatically.
