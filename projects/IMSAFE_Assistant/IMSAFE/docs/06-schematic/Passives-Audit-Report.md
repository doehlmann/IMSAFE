# IMSAFE MainBoard - Passives Audit Report

**Date:** 2026-01-29
**Schematic Version:** Post-manual IC placement
**Auditor:** Claude Code

---

## Executive Summary

| Metric | Count |
|--------|-------|
| Total Capacitors Found | 79 |
| Total Resistors Found | 31 |
| Sections Audited | 20 |
| Critical Issues | 4 sections |
| High Priority Issues | 5 sections |

---

## Section-by-Section Analysis

### Section 01: MCU Core (STM32H747IIT6) - GOOD

**Capacitors Present (27):**
| Type | Value | Qty | Purpose |
|------|-------|-----|---------|
| Bypass | 100nF | 18 | VDD pin decoupling |
| Crystal load | 6.8pF | 2 | HSE oscillator |
| Crystal load | 15pF | 2 | LSE oscillator |
| Bulk | 2.2uF | 2 | Power filtering |
| Filter | 10nF | 1 | High-speed filtering |
| Bulk | 4.7uF | 1 | Power filtering |
| Filter | 1uF | 1 | Filtering |

**Resistors Present (4):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10k | 2 | NRST pull-up, wake-up |
| 1M | 1 | High impedance |
| 10M | 1 | High impedance |

**Status:** Adequate coverage for STM32H747

**Recommendations:**
- [ ] Add 22uF bulk capacitor on main 3.3V rail near MCU
- [ ] Consider RC filter on SWDIO for high-speed debug

---

### Section 02: USB-C - ADEQUATE

**Capacitors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 1 | VBUS bypass |
| 10uF | 1 | VBUS bulk |

**Resistors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 5.1k | 2 | CC1/CC2 pull-down (UFP sink) |

**Recommendations:**
- [ ] Add 1nF on CC1-GND and CC2-GND for ESD margin
- [ ] Add 22nF on D+/D- for ESD chip filtering

---

### Section 03: Charger (BQ25792) - GOOD

**Capacitors Present (20):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10uF/25V | 10 | Bulk capacitors |
| 100nF | 3 | Bypass/filtering |
| 10uF | 3 | Power filtering |
| 47nF | 2 | Control signal filtering |
| 4.7uF | 1 | Filtering |
| 1nF | 1 | Filtering |

**Resistors Present (3):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100R | 1 | Current sensing |

**Recommendations:**
- [ ] Add 10k pull-down on CHG_INT pin
- [ ] Add 10k pull-down on ACOK and PGOOD signals
- [ ] Verify CE pin has proper pull-up per datasheet
- [ ] Add 100nF across battery sense lines

---

### Section 04: BMS - CRITICAL: NEEDS WORK

**Capacitors Present (3):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 3 | VDD bypass |

**Resistors Present (5):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 1k | 2 | Signal conditioning |
| Various | 3 | Pull-up/down |

**MISSING - CRITICAL:**
- [ ] **10uF bulk capacitor on BMS IC VDD**
- [ ] **100nF bypass on each additional VDD pin**
- [ ] **10k pull-ups on interrupt lines**
- [ ] **RC filters (10k + 100nF) on cell voltage sense lines**

---

### Section 05: 3.3V Regulator - GOOD

**Capacitors Present (6):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 22uF/10V | 2 | Input bulk |
| 100nF | 2 | Input/output bypass |
| 3.3nF | 1 | Compensation |
| 10uF/16V | 1 | Output bulk |

**Resistors Present (1):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100k | 1 | Feedback divider |

**Recommendations:**
- [ ] Verify second feedback resistor present
- [ ] Consider 47uF parallel output cap for heavy loads

---

### Section 06: 5V Regulator - GOOD

**Capacitors Present (6):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 22uF/10V | 2 | Input bulk |
| 100nF | 2 | Bypass |
| 3.3nF | 1 | Compensation |
| 10uF/16V | 1 | Output bulk |

**Resistors Present (3):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100k | 2 | Feedback dividers |

**Status:** Properly configured

---

### Section 07: Barometric (BMP390) - MINIMAL

**Capacitors Present (1):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 1 | VDD bypass |

**Resistors Present (2):**
Pull-up values present

**MISSING - HIGH PRIORITY:**
- [ ] **Second 100nF on SDO/ADR pin**
- [ ] **10k pull-up on INT pin**
- [ ] **10uF bulk capacitor on VDD**
- [ ] Verify I2C pull-ups elsewhere

---

### Section 08: IMU (ICM-42688-P) - MINIMAL

**Capacitors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 2 | VDD bypass |

**Resistors Present (0)**

**MISSING - HIGH PRIORITY:**
- [ ] **10uF bulk capacitor on VDD**
- [ ] **10k pull-ups on INT1, INT2 pins**
- [ ] **10nF on SDA/SCL lines**
- [ ] 100pF on CLK if using SPI

---

### Section 09: Magnetometer (MMC5983MA) - ADEQUATE

**Capacitors Present (3):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 2 | VDD bypass |
| 4.7uF | 1 | Bulk cap |

**Resistors Present (0)**

**Recommendations:**
- [ ] Add 10k pull-ups on INT pins
- [ ] Consider 10nF on I2C lines for long traces

---

### Section 10: Gas Sensor (MICS-4514) - CRITICAL: NEEDS WORK

**Capacitors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 2 | VDD bypass |

**Resistors Present (6):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10k | 3 | Pull-ups |
| 1k | 2 | Signal conditioning |

**MISSING - CRITICAL:**
- [ ] **100nF + 10uF on heater supply (high transient current)**
- [ ] **1uF on sensor analog output lines**
- [ ] **RC filters (10k + 100nF) on analog outputs to ADC**

---

### Section 11: Temp/Humidity (Si7021) - MINIMAL

**Capacitors Present (1):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 1 | VDD bypass |

**Resistors Present (0)**

**MISSING - HIGH PRIORITY:**
- [ ] **10uF bulk capacitor on VDD**
- [ ] **10k pull-up on data lines per datasheet**

---

### Section 12: Display - PENDING

Connector only, no passives audit possible.

---

### Section 13: microSD - PENDING

Connector only, no passives audit possible.

---

### Section 14: I/O Expander (MCP23017) - CRITICAL: NEEDS WORK

**Capacitors Present (1):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 1 | VDD bypass |

**Resistors Present (4):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10k | 4 | I2C pull-ups, address pins |

**MISSING - CRITICAL:**
- [ ] **10uF bulk capacitor on VDD**
- [ ] **Second 100nF bypass capacitor**
- [ ] 10nF on output port lines if driving long traces

---

### Section 15: Status LEDs - MINIMAL

**Capacitors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 2 | Supply filtering |

**Resistors Present (1):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 330R | 1 | LED current limiting |

**Recommendations:**
- [ ] Verify 330R is correct for LED brightness
- [ ] Add 100nF bypass per WS2812 LED if used

---

### Section 16: Rotary Encoders - GOOD

**Capacitors Present (6):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10nF | 4 | Debounce filters |
| 100nF | 2 | General filtering |

**MISSING:**
- [ ] **10k pull-up resistors on encoder A/B/SW lines**
- [ ] Consider 100nF debounce caps instead of 10nF

---

### Section 17: Bluetooth (ESP32-C3) - PENDING

Module placed but not wired, passives TBD.

---

### Section 18: Vibration Interface - ADEQUATE

**Capacitors Present (6):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100nF | 4 | Bypass |
| 1uF | 2 | Filtering |

**Resistors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 100k | 2 | High-impedance pull-ups |

**Recommendations:**
- [ ] Add 10uF bulk on main supply
- [ ] Add 10k pull-ups on motor control lines

---

### Section 19: Audio - CRITICAL: INSUFFICIENT

**Capacitors Present (0)**

**Resistors Present (2):**
| Value | Qty | Purpose |
|-------|-----|---------|
| 10k | 1 | Impedance matching |

**MISSING - CRITICAL:**
- [ ] **100nF on audio IC VDD**
- [ ] **10uF bulk on audio supply**
- [ ] **100nF input coupling capacitors**
- [ ] **10uF output coupling capacitors**

---

### Section 20: Debug - PENDING

Test points only, no passives needed.

---

## I2C Bus Pull-up Summary

| Bus | Location | Pull-ups Present | Value | Status |
|-----|----------|------------------|-------|--------|
| I2C1 | Sensors | Unknown | - | **VERIFY** |
| I2C4 | IO Expander | Yes | 10k | OK (consider 4.7k) |

**Recommendation:** Consolidate all I2C pull-ups at one location (MCU section or IO expander section) with 4.7k values for 400kHz operation.

---

## Priority Action List

### CRITICAL (Fix Before PCB Layout)

1. **Section 04 BMS:** Add 10uF bulk + interrupt pull-ups
2. **Section 10 Gas Sensor:** Add heater supply caps + analog RC filters
3. **Section 14 IO Expander:** Add 10uF bulk + second 100nF bypass
4. **Section 19 Audio:** Add all power and coupling capacitors

### HIGH PRIORITY

5. **Section 07 Barometric:** Add 10uF bulk + INT pull-up
6. **Section 08 IMU:** Add 10uF bulk + INT pull-ups
7. **Section 11 Temp/Humidity:** Add 10uF bulk
8. **Section 16 Encoders:** Add pull-up resistors on A/B/SW

### MEDIUM PRIORITY

9. **Section 01 MCU:** Add 22uF bulk on main 3.3V
10. **Section 02 USB-C:** Add ESD filter caps
11. **Section 03 Charger:** Add status pin pull-downs
12. **All Sensors:** Verify I2C pull-ups are consolidated

---

## Recommended Component Values

| Component | Value | Package | JLCPCB Basic | Purpose |
|-----------|-------|---------|--------------|---------|
| Bypass cap | 100nF | 0603 | C14663 | HF decoupling |
| Bulk cap | 10uF | 0805 | C19702 | LF filtering |
| Bulk cap | 22uF | 0805 | C45783 | Heavy load filtering |
| I2C pull-up | 4.7k | 0603 | C23162 | I2C bus |
| General pull-up | 10k | 0603 | C25804 | Digital signals |
| LED series | 330R | 0603 | C23138 | Current limiting |
| Debounce cap | 100nF | 0603 | C14663 | Switch debounce |
| RC filter R | 10k | 0603 | C25804 | Analog filtering |
| RC filter C | 100nF | 0603 | C14663 | Analog filtering |

---

## Next Steps

1. Review this audit against component datasheets
2. Add missing passives in KiCad
3. Re-run ERC to verify connections
4. Annotate schematic
5. Generate BOM for review
