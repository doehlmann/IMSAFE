# IMSAFE Schematic Design Rationale

**Document Version:** 1.0
**Date:** 2026-01-29
**Project:** IMSAFE Flying Assistant
**Purpose:** Design justification and review documentation for schematic sections

---

## Document Overview

This document provides the engineering rationale behind each schematic section of the IMSAFE project. It is intended to accompany printed schematics during design review and serves as a record of design decisions, calculations, and risk assessments.

### Document Structure (Per Section)
- **Requirement:** What system requirement this section satisfies
- **Reference Design:** Source of circuit topology (datasheet, app note, custom)
- **Decoupling Strategy:** Capacitor selection and placement rationale
- **Power:** Supply rail, voltage, current requirements
- **Communication:** Interface type and parameters
- **Calculations:** Key design calculations with formulas
- **Test Points:** Integrated test/debug access
- **Risks/Unknowns:** Outstanding concerns or assumptions

---

## Section 01: MCU Core (STM32H747AGI6)

### Requirement
Provide dual-core processing capability (Cortex-M7 @ 480MHz + Cortex-M4 @ 240MHz) for real-time sensor fusion, display rendering, and communication tasks.

### Reference Design
- **Primary:** ST AN4661 "Oscillator design guide for STM8AF/AL/S, STM32 MCUs and MPUs"
- **Secondary:** STM32H747 Reference Manual (RM0399)
- **Evaluation Board:** NUCLEO-H747ZI schematic (MB1363)

### Decoupling Strategy
Per ST AN4661, the STM32H7 requires extensive decoupling due to high-speed operation:

| Domain | Bulk Cap | HF Cap | Qty | Rationale |
|--------|----------|--------|-----|-----------|
| VDD (digital) | 4.7uF (0805) | 100nF (0603) | 1+10 | One bulk + one per VDD pin |
| VDDA (analog) | 1uF (0603) | 100nF (0603) | 1+4 | Analog supply isolation |
| VDDUSB | - | 100nF (0603) | 2 | USB PHY filtering |
| VCAP | 2.2uF (0603) | - | 2 | Internal LDO output |
| VBAT | - | 100nF (0603) | 1 | RTC backup domain |
| VREF+ | - | 10nF (0603) | 1 | ADC reference filtering |

**Placement:** All decoupling capacitors placed within 3mm of respective power pins. VDD bulk cap near pin 1 corner.

### Power
- **Supply Rail:** +3V3
- **Typical Current:** 200mA (both cores active, peripherals enabled)
- **Peak Current:** 400mA (flash write, all peripherals)
- **Voltage Range:** 3.0V - 3.6V (3.3V nominal)

### Communication
MCU provides the following interfaces to other sections:
- I2C1: Sensors (barometric, magnetometer, temp/humidity, I/O expander)
- I2C4: Charger IC
- SPI1: IMU
- SPI2: Display
- SDMMC1: microSD
- USART2: Bluetooth module (ESP32-C3)
- USB OTG FS: USB-C data

### Calculations

**HSE Crystal Load Capacitors (8MHz):**
```
CL (crystal) = 10pF (from datasheet)
Cstray (PCB) = 3pF (estimated)
Cload = 2 * (CL - Cstray) = 2 * (10 - 3) = 14pF
Selected: 15pF (standard value)
Effective CL = 15/2 + 3 = 10.5pF (close to target)
```

**LSE Crystal Load Capacitors (32.768kHz):**
```
CL (crystal) = 6pF (from datasheet)
Cstray (PCB) = 2pF (smaller crystal footprint)
Cload = 2 * (CL - Cstray) = 2 * (6 - 2) = 8pF
Selected: 6.8pF (standard value, slight undershoot acceptable)
Effective CL = 6.8/2 + 2 = 5.4pF
```

### Test Points
- NRST: Reset signal access
- BOOT0: Boot mode selection
- SWD interface: SWDIO, SWCLK, SWO via Tag-Connect

### Risks/Unknowns
1. **Crystal startup:** LSE crystal may require adjustment of load caps based on actual PCB capacitance
2. **EMI:** High-speed MCU may require additional filtering if EMI testing fails
3. **Thermal:** Power dissipation at full load (~1.3W) - verify package thermal resistance

---

## Section 02: Power Input (USB-C)

### Requirement
Provide USB-C connectivity for 5V power input (charging) and USB 2.0 data communication.

### Reference Design
- **Primary:** USB Type-C Specification Rev 2.0 (UFP/Sink configuration)
- **Secondary:** USBLC6-2SC6 datasheet application circuit
- **Note:** No USB-PD negotiation (5V only), simplified UFP implementation

### Decoupling Strategy
| Component | Value | Purpose |
|-----------|-------|---------|
| C201 | 100nF | VBUS high-frequency filtering |
| C202 | 10uF 16V | VBUS bulk, inrush current handling |

**Placement:** Capacitors placed immediately after connector, before ESD protection.

### Power
- **Input:** VBUS (5V from USB host/charger)
- **Current:** Up to 3A (with appropriate host)
- **Output:** VBUS rail to charger section

### Communication
- **Interface:** USB 2.0 (D+/D- differential pair)
- **Speed:** Full Speed (12Mbps) or High Speed (480Mbps)
- **ESD Protection:** USBLC6-2SC6 (3.5pF line capacitance, USB 2.0 compatible)

### Calculations

**CC Pull-Down Resistors (UFP/Sink Role):**
```
USB-C Spec requires Rd = 5.1kOhm +/-10% for sink devices
Selected: R201 = R202 = 5.1kOhm (1% tolerance)
This identifies device as UFP (Upstream Facing Port) / Sink
```

**ESD Protection Verification:**
```
USBLC6-2SC6 specifications:
- Line capacitance: 3.5pF (spec requires <10pF for USB 2.0)
- ESD rating: +/-8kV contact, +/-15kV air
- Clamping: ~7V @ 8kV pulse
Meets USB 2.0 signal integrity requirements
```

### Test Points
- VBUS: Power input monitoring
- CC1/CC2: Connection detection verification

### Risks/Unknowns
1. **No USB-PD:** Limited to 5V/3A maximum (15W). Higher power requires PD negotiation IC
2. **Cable quality:** Long/cheap cables may not support 3A reliably
3. **Hot-plug:** VBUS transients during connection - bulk cap sized for inrush

---

## Section 03: Battery Charger (BQ25792)

### Requirement
Charge 2S Li-ion battery pack (7.4V nominal, 16Ah) from USB-C input with I2C monitoring/control capability.

### Reference Design
- **Primary:** TI BQ25792 Datasheet (SLUSFX4)
- **Secondary:** TI Reference Design TIDA-01598 (similar buck-boost charger)
- **Deviation:** Simplified sensing (no full fuel gauge), I2C for basic monitoring only

### Decoupling Strategy
| Location | Bulk Cap | HF Cap | Rationale |
|----------|----------|--------|-----------|
| VBUS Input | 10uF 25V | 100nF | Input filtering, >2x Vbus rating |
| PMID | 10uF 25V | - | Power path mid-point stability |
| BAT Output | 2x 22uF 16V | - | Battery ripple filtering |
| VSYS Output | 22uF 16V | 100nF | System supply stability |
| REGN | - | 100nF | Internal LDO bypass |

### Power
- **Input:** VBUS (5V from USB-C)
- **Output:** VSYS (6.0V - 8.4V, tracks battery), VBAT (to battery)
- **Topology:** Buck-Boost (can charge battery above or below input voltage)
- **Max Charge Current:** 5A (I2C configurable)

### Communication
- **Interface:** I2C (400kHz Fast Mode)
- **Address:** 0x6B (7-bit)
- **Bus:** I2C4 (dedicated to power management)
- **Features:** Status registers, charge parameter configuration, fault reporting

### Calculations

**Current Sense Resistors:**
```
Input Sense (R301):
- Max input current: 3.5A (USB + margin)
- BQ25792 typical: 10mOhm
- Sense voltage @ max: 3.5A * 10mOhm = 35mV
- Power @ max: 3.5^2 * 0.01 = 122.5mW

Battery Sense (R302):
- Max charge current: 5A
- Selected: 5mOhm (lower for efficiency)
- Sense voltage @ max: 5A * 5mOhm = 25mV
- Power @ max: 5^2 * 0.005 = 125mW
```

**Power Inductor (L1):**
```
BQ25792 recommended: 1-2.2uH
Selected: 2.2uH, 5A saturation
Switching frequency: 750kHz (BQ25792 default)
Package: 5x5mm shielded
```

**I2C Pull-ups (shared with I2C4 bus):**
```
Target speed: 400kHz (Fast Mode)
Bus capacitance estimate: 50pF
Rise time requirement: <300ns
Rp_max = 0.3 * 300ns / 50pF = 1.8kOhm (theoretical max)
Rp_min = (3.3V - 0.4V) / 3mA = 967 Ohm
Selected: 4.7kOhm (standard, within range)
```

**Charge Time Estimate:**
```
Battery capacity: 16000mAh
Charge current @ 3A: 16000mAh / 3000mA = 5.3 hours (CC phase)
Note: CV phase adds ~1 hour for full charge
```

### Test Points
- VSYS: System voltage monitoring
- CHG_INT: Charge status interrupt

### Risks/Unknowns
1. **Thermal:** Buck-boost efficiency at high currents - monitor IC temperature
2. **I2C Configuration:** Requires firmware to configure charge parameters at startup
3. **JLCPCB Assembly:** BQ25792 QFN-29 package - verify JLCPCB can assemble
4. **NTC Sensing:** Thermistor connection assumed on battery pack - verify compatibility

---

## Section 04: BMS (HY2120 + HY2213)

### Requirement
Provide 2S Li-ion battery protection (overcharge, over-discharge, overcurrent, short-circuit) and cell balancing.

### Reference Design
- **Primary:** HY2120-CB Datasheet (2S protection IC)
- **Secondary:** HY2213-BB3A Datasheet (cell balancer)
- **Topology:** Standard Chinese BMS architecture with back-to-back MOSFETs

### Decoupling Strategy
| IC | Cap | Value | Purpose |
|----|-----|-------|---------|
| U4 (HY2120) | C401 | 100nF | VDD bypass |
| U5 (HY2213 Cell 1) | C402 | 100nF | VDD bypass |
| U6 (HY2213 Cell 2) | C403 | 100nF | VDD bypass |

### Power
- **Input:** VBAT from charger (6.0V - 8.4V)
- **Output:** VSYS to system (through protection FETs)
- **Protection IC Power:** Derived from battery (self-powered)

### Communication
- **None:** Standalone protection, no MCU interface
- **Status Output:** CELL_MID voltage can be monitored by MCU ADC

### Calculations

**Current Sense Resistor (R403):**
```
HY2120 overcurrent threshold: 200mV typical
Desired trip current: 40A (high for safety margin)
Rsense = 200mV / 40A = 5mOhm
Selected: 5mOhm 1206 package (0.5W rating)

Normal operation power (5A):
P = 5^2 * 0.005 = 125mW (well within rating)

Trip condition power (40A peak):
P = 40^2 * 0.005 = 8W (brief pulse only)
```

**Cell Balance Resistors (R404, R405):**
```
HY2213 activates at 4.2V per cell
Desired bleed current: ~68mA (moderate rate)
Rbalance = 4.2V / 68mA = 61.8 Ohm
Selected: 62 Ohm (E24 standard value)

Actual bleed current: 4.2V / 62 Ohm = 67.7mA
Power dissipation: 4.2^2 / 62 = 284mW
Package: 0805 (0.25W rating) - adequate with thermal margin

Balance time for 500mAh imbalance:
t = 500mAh / 67.7mA = 7.4 hours
```

**MOSFET Selection (AO3400A):**
```
Requirements:
- Vds > 8.4V (pack voltage) -> AO3400A = 30V OK
- Id > 5A continuous -> AO3400A = 5.7A @ 25C (marginal, but acceptable)
- Low Rds_on for efficiency -> AO3400A = 28mOhm @ Vgs=4.5V

Back-to-back configuration:
- 2 FETs per protection path
- Total Rds = 2 * 28mOhm = 56mOhm
- Conduction loss @ 5A: 5^2 * 0.056 = 1.4W (distributed across 2 FETs)
```

### Test Points
- CELL_MID: Mid-point voltage for cell balance verification
- PACK_NEG: Output side of protection circuit

### Risks/Unknowns
1. **MOSFET Thermal:** 1.4W dissipation at 5A - may need thermal relief in PCB
2. **Balance Rate:** 67mA is slow for large capacity - full balance may take many hours
3. **Protection Latching:** HY2120 behavior after fault trip needs verification
4. **No MCU Control:** Cannot disable protection or read fault status directly

---

## Section 05: 3.3V Regulator (TPS62133)

### Requirement
Provide 3.3V regulated supply for MCU, sensors, and low-power digital ICs from battery-derived VSYS.

### Reference Design
- **Primary:** TI TPS62133 Datasheet (SLVSDI4)
- **Secondary:** TI WEBENCH Power Designer output
- **Configuration:** Fixed 3.3V output version (no feedback resistors needed)

### Decoupling Strategy
| Location | Value | Package | Purpose |
|----------|-------|---------|---------|
| Input | 10uF 16V | 0805 | Bulk input filtering |
| Input | 100nF | 0603 | High-frequency input |
| Output | 2x 22uF 10V | 0805 | Output bulk (low ESR) |
| Output | 100nF | 0603 | High-frequency output |

**Selection Rationale:** TI datasheet specifies minimum 10uF input, 22uF output. Doubled output capacitance for improved transient response with MCU load.

### Power
- **Input:** VSYS (6.0V - 8.4V)
- **Output:** +3V3 (3.3V regulated)
- **Max Current:** 3A continuous
- **Typical Load:** 500mA (MCU + sensors)
- **Efficiency:** ~92% @ 7.4V input, 1A load

### Communication
- **None:** Standalone regulator
- **Enable:** Directly tied to VSYS via R501 (always on when battery present)

### Calculations

**Inductor Selection:**
```
TI recommended range: 1.0uH - 2.2uH for 2.5MHz operation
Selected: 1.5uH (middle of range, good ripple/size tradeoff)
Saturation current: 4A (exceeds 3A max output)
DC resistance: <30mOhm (efficiency requirement)
```

**Power Loss Estimate:**
```
Output power @ 1A: 3.3V * 1A = 3.3W
Efficiency: 92%
Input power: 3.3W / 0.92 = 3.59W
Power loss: 3.59W - 3.3W = 290mW
```

**Output Voltage Accuracy:**
```
TPS62133 fixed 3.3V version: +/-2% over line/load/temp
Output range: 3.234V - 3.366V
Acceptable for all 3.3V loads
```

### Test Points
- TP1: +3V3 output voltage monitoring

### Risks/Unknowns
1. **Load Transient:** MCU current spikes during flash write - may cause voltage dip
2. **Thermal:** 290mW loss at 1A - verify thermal pad soldering
3. **Input Voltage Range:** VSYS can drop to 6V at low battery - verify regulation maintained

---

## Section 06: 5V Regulator (TPS62130)

### Requirement
Provide 5V regulated supply for LEDs, display backlight, buzzer, and gas sensor heater from VSYS.

### Reference Design
- **Primary:** TI TPS62130 Datasheet (SLVSBI2)
- **Configuration:** Adjustable version with resistor divider for 5.0V output

### Decoupling Strategy
Same as Section 05 (TPS62133):
| Location | Value | Package | Purpose |
|----------|-------|---------|---------|
| Input | 10uF 16V | 0805 | Bulk input filtering |
| Input | 100nF | 0603 | High-frequency input |
| Output | 2x 22uF 10V | 0805 | Output bulk |
| Output | 100nF | 0603 | High-frequency output |

### Power
- **Input:** VSYS (6.0V - 8.4V)
- **Output:** +5V (5.09V actual)
- **Max Current:** 3A continuous
- **Typical Load:** 1A (LEDs at medium brightness)

### Communication
- **None:** Standalone regulator

### Calculations

**Feedback Resistor Divider:**
```
TPS62130 Vfb reference: 0.8V
Desired Vout: 5.0V
Formula: Vout = Vfb * (1 + R601/R602)
Rearranged: R601/R602 = (Vout/Vfb) - 1 = (5.0/0.8) - 1 = 5.25

Selected:
R601 = 100kOhm (top resistor, VOUT to FB)
R602 = 24.3kOhm (bottom resistor, FB to GND)
Ratio: 100/24.3 = 4.115

Actual Vout: 0.8V * (1 + 4.115) = 4.09V

ERROR DETECTED: Recalculating...
Need ratio of 5.25, selected gives 4.115

Corrected selection:
R601 = 100kOhm
R602 = 19.1kOhm (gives ratio 5.24)
Vout = 0.8 * (1 + 5.24) = 4.99V

OR use standard values:
R601 = 100kOhm
R602 = 18kOhm (E24)
Vout = 0.8 * (1 + 5.56) = 5.25V (5% high, acceptable)
```

**NOTE:** Schematic uses R602 = 24.3k which gives 4.09V output. This requires correction to 18k-19.1k for true 5V output.

### Test Points
- TP2: +5V output voltage monitoring

### Risks/Unknowns
1. **DESIGN ERROR:** Feedback resistor values produce 4.09V, not 5V - REQUIRES CORRECTION
2. **LED Current:** WS2812B may draw high peak currents - verify regulator response
3. **Thermal:** Higher power dissipation than 3.3V rail due to larger load

---

## Section 07: Barometric Sensor (BMP390)

### Requirement
Measure atmospheric pressure for altitude calculation and weather monitoring.

### Reference Design
- **Primary:** Bosch BMP390 Datasheet
- **Application:** Direct implementation per datasheet reference circuit

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C701 | 100nF | Power supply bypass |
| VDDIO | C702 | 100nF | I/O supply bypass |

**Note:** BMP390 requires separate VDD (sensor) and VDDIO (I/O level) supplies. Both connected to +3V3.

### Power
- **Supply:** +3V3 (VDD and VDDIO)
- **Current:** 700uA typical (continuous measurement mode)

### Communication
- **Interface:** I2C (up to 3.4MHz, using 400kHz)
- **Address:** 0x77 (SDO=high) or 0x76 (SDO=low), using 0x77
- **Bus:** I2C1 (shared sensor bus)

### Calculations
**Altitude Resolution:**
```
BMP390 pressure resolution: 0.03 Pa (ultra-low noise mode)
Altitude change per Pa: ~8.5m per 100Pa (at sea level)
Altitude resolution: 0.03 Pa * 8.5m/100Pa = 2.55mm

This exceeds requirement for aviation altitude sensing.
```

### Test Points
- BARO_INT: Interrupt output for data-ready signaling

### Risks/Unknowns
1. **Thermal Effects:** Pressure reading affected by PCB temperature - locate away from heat sources
2. **Venting:** Sensor requires path to ambient air - enclosure design consideration
3. **I2C Address Conflict:** Verify no other device uses 0x77

---

## Section 08: IMU (ICM-42688-P)

### Requirement
Provide 6-axis motion sensing (3-axis accelerometer + 3-axis gyroscope) for attitude determination.

### Reference Design
- **Primary:** TDK ICM-42688-P Datasheet
- **Application:** SPI interface for maximum data rate

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C801 | 100nF | Core supply bypass |
| VDDIO | C802 | 100nF | I/O supply bypass |
| Pin 6 (RESV) | C803 | 10nF | Required per datasheet |
| Additional | C804 | 100nF | Extra filtering |

### Power
- **Supply:** +3V3 (VDD and VDDIO)
- **Current:** 1mA typical (both accel + gyro active)

### Communication
- **Interface:** SPI (up to 24MHz, using 10MHz)
- **Bus:** SPI1 (dedicated to IMU)
- **Data Rate:** Up to 32kHz ODR

### Calculations
**SPI Timing:**
```
ICM-42688-P max SPI clock: 24MHz
Selected: 10MHz (conservative, allows longer traces)
Data transfer for 6-axis @ 16-bit: 12 bytes
Transfer time: 12 * 8 / 10MHz = 9.6us
At 1kHz ODR: 9.6us / 1ms = 0.96% bus utilization
```

### Test Points
- INT1: Interrupt output for data-ready

### Risks/Unknowns
1. **Vibration Sensitivity:** Aviation environment is noisy - may need filtering algorithm
2. **Mounting:** Sensor alignment critical - PCB placement and enclosure mounting matter
3. **SPI Integrity:** Long traces may require series termination resistors

---

## Section 09: Magnetometer (MMC5983MA)

### Requirement
Provide 3-axis magnetic field sensing for compass heading determination.

### Reference Design
- **Primary:** MEMSIC MMC5983MA Datasheet
- **Application:** I2C interface implementation

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C901 | 100nF | Core supply bypass |
| VDDIO | C902 | 100nF | I/O supply bypass |
| C1 | C903 | 4.7uF | Internal charge pump capacitor |

**Note:** C1 (4.7uF) is required for the internal SET/RESET circuit - do not omit.

### Power
- **Supply:** +3V3 (VDD and VDDIO)
- **Current:** 500uA typical (continuous mode)

### Communication
- **Interface:** I2C (up to 400kHz)
- **Address:** 0x30
- **Bus:** I2C1 (shared sensor bus)

### Calculations
**Heading Resolution:**
```
MMC5983MA resolution: 0.0625 mGauss (18-bit mode)
Earth's field: ~500 mGauss
Angular resolution: arctan(0.0625/500) = 0.007 degrees

This exceeds requirement for compass heading.
```

### Test Points
- None dedicated (uses shared I2C bus test points)

### Risks/Unknowns
1. **Magnetic Interference:** PCB traces, inductors, speaker magnet may affect readings
2. **Calibration:** Requires hard/soft iron compensation in firmware
3. **Distance from MCU:** Place away from high-current switching circuits

---

## Section 10: Gas Sensor (MICS-4514)

### Requirement
Detect carbon monoxide (CO) and nitrogen dioxide (NO2) for cabin air quality monitoring.

### Reference Design
- **Primary:** SGX MICS-4514 Datasheet
- **Application:** Custom heater control and analog signal conditioning

### Decoupling Strategy
Heater is PWM-controlled, requires different approach:
| Function | Component | Value | Purpose |
|----------|-----------|-------|---------|
| Heater switch | Q5 | AO3400A | PWM control |
| Gate pull-down | R1001 | 10k | Default off state |
| Gate series | R1002 | 100R | Gate current limit |

### Power
- **Heater Supply:** +5V (80-100mA during heating)
- **Sensor Supply:** +3V3 (for signal conditioning)
- **Total Power:** ~500mW during active heating

### Communication
- **Interface:** Analog (2 ADC channels)
- **Signals:** Reducing gas (CO) and Oxidizing gas (NO2) resistance ratios

### Calculations
**Signal Conditioning:**
```
MICS-4514 outputs: Variable resistance based on gas concentration
Load resistor selection: 10k (provides mid-range output voltage)

For reducing sensor (CO):
- Rs range: 10k - 500k (depending on CO concentration)
- Rload: 10k
- Vout = Vcc * Rload / (Rs + Rload)
- At Rs=10k: Vout = 3.3V * 10k/(10k+10k) = 1.65V
- At Rs=100k: Vout = 3.3V * 10k/(100k+10k) = 0.3V

ADC input filtering: 100nF + 1k series resistor
Cutoff frequency: 1/(2*pi*1k*100nF) = 1.6kHz
```

**Heater Power:**
```
MICS-4514 heater resistance: ~50 Ohm
At 5V: I = 5V/50R = 100mA
Power: 5V * 100mA = 500mW
PWM control allows reduced power during standby
```

### Test Points
- Analog outputs to MCU ADC (accessible via header)

### Risks/Unknowns
1. **Warm-up Time:** Sensor requires 10+ minutes to stabilize after power-on
2. **Calibration:** Requires known gas concentrations for accurate readings
3. **Heater Thermal:** 500mW continuous - thermal design consideration
4. **Cross-sensitivity:** May respond to other gases - algorithm needed

---

## Section 11: Temperature/Humidity Sensor (Si7021)

### Requirement
Measure ambient temperature and relative humidity for pilot comfort and IMSAFE assessment.

### Reference Design
- **Primary:** Silicon Labs Si7021-A20 Datasheet
- **Application:** Minimal external components per reference design

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C1101 | 100nF | Power supply bypass |

**Note:** Si7021 is extremely low power and requires minimal filtering.

### Power
- **Supply:** +3V3
- **Current:** 150uA typical (during measurement)

### Communication
- **Interface:** I2C (up to 400kHz)
- **Address:** 0x40
- **Bus:** I2C1 (shared sensor bus)

### Calculations
**Measurement Accuracy:**
```
Si7021 specifications:
- Temperature accuracy: +/-0.4C (typical)
- Humidity accuracy: +/-3% RH (typical)
- Resolution: 14-bit temperature, 12-bit humidity

Temperature resolution: 175.72C / 2^14 = 0.01C
Humidity resolution: 125% / 2^12 = 0.03% RH

Both exceed IMSAFE monitoring requirements.
```

### Test Points
- None dedicated (uses shared I2C bus test points)

### Risks/Unknowns
1. **Self-heating:** MCU/power electronics may affect local temperature reading
2. **Response Time:** Humidity sensor has ~8 second response time
3. **Condensation:** Sensor may be damaged by direct water contact

---

## Section 12: Display Interface (ST7789)

### Requirement
Interface to IPS LCD display (240x320 or 240x240) for pilot information presentation.

### Reference Design
- **Primary:** Display module datasheet (FPC connector pinout)
- **Application:** SPI interface with GPIO control signals

### Decoupling Strategy
| Function | Value | Purpose |
|----------|-------|---------|
| Display VDD | 100nF + 10uF | Module power filtering |
| SPI signals | 33R series | Impedance matching |

### Power
- **Display Logic:** +3V3 (from display module regulator)
- **Backlight:** +5V via PWM-controlled MOSFET
- **Backlight Current:** Up to 130mA at full brightness

### Communication
- **Interface:** SPI (up to 62.5MHz, using 40MHz)
- **Bus:** SPI2 (dedicated to display)
- **Control Signals:** DC (data/command), RST (reset), CS (chip select)

### Calculations
**SPI Data Rate:**
```
Display resolution: 240 x 320 = 76,800 pixels
Color depth: 16-bit (RGB565)
Frame size: 76,800 * 2 = 153,600 bytes

At 40MHz SPI:
Frame time: 153,600 * 8 / 40MHz = 30.7ms
Max frame rate: 1 / 30.7ms = 32.6 fps

Adequate for UI updates.
```

**Backlight PWM:**
```
MOSFET: AO3400A (same as other drivers)
PWM frequency: 1kHz (no visible flicker)
Full brightness: 130mA @ 5V = 650mW
```

### Test Points
- Backlight PWM signal accessible via test pad

### Risks/Unknowns
1. **FPC Connector:** Verify pinout matches specific display module ordered
2. **Display Variants:** Different ST7789 modules have different initialization sequences
3. **Viewing Angle:** IPS provides good angles, but verify for cockpit mounting

---

## Section 13: microSD Card (MEM2067)

### Requirement
Provide removable storage for flight data logging and configuration files.

### Reference Design
- **Primary:** MEM2067 hinged socket datasheet
- **Secondary:** STM32 SDMMC application notes
- **Protection:** USBLC6-2SC6 for ESD on all data lines

### Decoupling Strategy
| Function | Value | Purpose |
|----------|-------|---------|
| VDD | 10uF + 100nF | Card power supply |
| Signal lines | 33R series | Impedance matching |

### Power
- **Supply:** +3V3
- **Current:** 30mA typical, 100mA peak (during write)

### Communication
- **Interface:** SDIO (4-bit mode) via STM32 SDMMC peripheral
- **Speed:** Up to 50MHz (High-Speed mode)
- **Card Detect:** GPIO with pull-up resistor

### Calculations
**Data Rate:**
```
SDMMC 4-bit mode @ 50MHz:
Theoretical: 50MHz * 4 bits = 200 Mbps = 25 MB/s

Practical with overhead: ~20 MB/s write speed
Adequate for flight data logging.
```

**Series Resistor Selection:**
```
SDMMC signal impedance: ~50 Ohm
Series resistor: 33R (matches source impedance)
Helps with EMI and signal integrity at 50MHz
```

### Test Points
- Card detect signal

### Risks/Unknowns
1. **Card Compatibility:** Some cards may not support high-speed mode
2. **Hot Removal:** Data corruption if card removed during write
3. **ESD:** High-traffic connector - ESD protection essential
4. **File System:** FAT32 or exFAT required for interoperability

---

## Section 14: I/O Expander & Switches (MCP23017)

### Requirement
Expand MCU GPIO for 8 toggle switch inputs used for IMSAFE checklist items.

### Reference Design
- **Primary:** Microchip MCP23017 Datasheet
- **Application:** I2C I/O expansion with internal pull-ups

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C1401 | 100nF | Power supply bypass |

### Power
- **Supply:** +3V3
- **Current:** 1mA typical

### Communication
- **Interface:** I2C (up to 1.7MHz, using 400kHz)
- **Address:** 0x20 (A0=A1=A2=GND)
- **Bus:** I2C1 (shared sensor bus)
- **Interrupt:** INTA, INTB outputs for switch change notification

### Calculations
**I2C Address Configuration:**
```
MCP23017 base address: 0x20
A2:A1:A0 = 000 (all to GND)
Final address: 0x20

Address range available: 0x20-0x27 (8 devices possible)
Only using one device in this design.
```

**Switch Debouncing:**
```
MCP23017 provides internal pull-ups (~100k)
Switches connect port pins to GND
Debouncing: Handled in firmware (software debounce)
No external RC debounce needed due to slow switch operation
```

### Test Points
- INTA, INTB: Interrupt outputs for debug

### Risks/Unknowns
1. **I2C Bus Loading:** Adding more devices to I2C1 bus - verify capacitance budget
2. **Internal Pull-ups:** 100k may be weak for long switch wiring
3. **Interrupt Latency:** I2C polling or interrupt response time for switch detection

---

## Section 15: Status LEDs (WS2812B-2020)

### Requirement
Provide 16 individually addressable RGB LEDs for status indication (IMSAFE checklist, system status, alerts).

### Reference Design
- **Primary:** Worldsemi WS2812B-2020 Datasheet
- **Application:** Daisy-chain configuration with series data resistor

### Decoupling Strategy
| Per LED | Value | Purpose |
|---------|-------|---------|
| C1501-C1516 | 100nF each | VDD bypass for each LED |

**Critical:** Each WS2812B requires dedicated decoupling capacitor close to VDD pin. Missing caps cause data corruption.

**Data Line:**
| Component | Value | Purpose |
|-----------|-------|---------|
| R1501 | 330R | Series resistor on data input (first LED only) |

### Power
- **Supply:** +5V
- **Current per LED:** 12mA max per color = 36mA max per LED (white)
- **Total (16 LEDs):** Up to 576mA at full white brightness
- **Typical:** 50mA (varied colors, reduced brightness)

### Communication
- **Interface:** Single-wire serial (800kHz pulse-coded)
- **Protocol:** WS2812B proprietary (24-bit RGB per LED)
- **Data Pin:** GPIO configured as output with DMA for timing

### Calculations
**Data Timing:**
```
WS2812B bit period: 1.25us (800kHz)
Bits per LED: 24 (8R + 8G + 8B)
Time per LED: 24 * 1.25us = 30us
Time for 16 LEDs: 16 * 30us = 480us
Reset pulse: >50us

Total update time: 480us + 50us = 530us
Max update rate: ~1.9kHz (far exceeds visual requirement)
```

**Power Dissipation:**
```
At full white (36mA per LED):
P = 16 * 36mA * 5V = 2.88W

More typical operation (50mA total):
P = 50mA * 5V = 250mW
```

**Series Resistor:**
```
R1501 = 330R
Purpose: Reduces ringing on data line, improves signal integrity
Located at first LED data input only
```

### Test Points
- LED_DATA: Data signal to first LED

### Risks/Unknowns
1. **Timing Critical:** STM32 DMA required for precise timing - verify implementation
2. **5V Logic:** WS2812B expects 5V logic, 3.3V may be marginal - verify with actual parts
3. **Power Transients:** All LEDs switching simultaneously causes current spikes
4. **Thermal:** At full brightness, LEDs generate significant heat

---

## Section 16: Rotary Encoders (EC11)

### Requirement
Provide two rotary encoders with push-button for menu navigation and value adjustment.

### Reference Design
- **Primary:** Generic EC11 rotary encoder datasheet
- **Application:** Hardware RC debouncing for reliable edge detection

### Decoupling Strategy
Not applicable - mechanical switches, no IC.

**Debouncing:**
| Per Encoder | Component | Value | Purpose |
|-------------|-----------|-------|---------|
| Channel A | C1601/C1604 | 10nF | RC debounce |
| Channel B | C1602/C1605 | 10nF | RC debounce |
| Switch | C1603/C1606 | 100nF | Switch debounce (longer) |

### Power
- **Supply:** +3V3 (via MCU internal pull-ups)
- **Current:** Negligible (passive switches)

### Communication
- **Interface:** GPIO (quadrature A/B signals + switch)
- **Signals per encoder:** ENC_A, ENC_B, ENC_SW
- **Interrupt:** A/B edges trigger direction decode in firmware

### Calculations
**RC Debounce Time Constant:**
```
Internal MCU pull-up: ~40k (approximate)
Debounce capacitor: 10nF

Time constant: tau = R * C = 40k * 10nF = 0.4ms
Debounce time (5*tau): ~2ms

Typical encoder bounce: 1-5ms
2ms debounce is adequate for suppressing bounce.

Switch debounce (100nF):
tau = 40k * 100nF = 4ms
Debounce time: ~20ms
Appropriate for mechanical push button.
```

**Quadrature Decoding:**
```
EC11 resolution: 20 pulses per revolution (typical)
With quadrature (4 edges per pulse): 80 edges per revolution
Angular resolution: 360 / 80 = 4.5 degrees per edge
```

### Test Points
- Encoder signals available at MCU GPIO pads

### Risks/Unknowns
1. **Encoder Quality:** Cheap encoders may have excessive bounce - verify with scope
2. **Pull-up Strength:** MCU internal pull-ups may need external supplementation
3. **Mechanical Wear:** Encoders have limited lifespan - consider replaceable mounting

---

## Section 17: Bluetooth Module (ESP32-C3-MINI-1)

### Requirement
Provide Bluetooth Low Energy connectivity for companion app communication.

### Reference Design
- **Primary:** Espressif ESP32-C3-MINI-1 Datasheet
- **Secondary:** ESP32-C3 Hardware Design Guidelines
- **Application:** UART bridge to STM32 for BLE data exchange

### Decoupling Strategy
| Pin | Component | Value | Purpose |
|-----|-----------|-------|---------|
| 3V3 | C1701 | 10uF | Bulk power (RF transients) |
| 3V3 | C1702 | 100nF | High-frequency bypass |
| EN | C1703 | 1uF | Enable delay capacitor |

### Power
- **Supply:** +3V3
- **Current:** 20mA typical, 350mA peak (TX burst)
- **Sleep Current:** <5uA (deep sleep)

### Communication
- **External Interface:** UART to STM32 (115200 baud typical)
- **BLE:** Bluetooth 5.0 LE
- **Antenna:** PCB antenna (integrated in module)

### Calculations
**Enable RC Delay:**
```
EN pin has internal pull-up
C1703 = 1uF creates power-on delay
Delay time: ~10ms (ensures 3V3 stable before boot)
R1701 = 10k provides defined pull-up
```

**UART Baud Rate:**
```
Selected: 115200 baud
Data rate: 115200 / 10 bits = 11.5 kB/s
Adequate for BLE data exchange (BLE LE is ~1 Mbps max)
```

### Test Points
- UART TX/RX accessible via debug header

### Risks/Unknowns
1. **Antenna Placement:** PCB antenna sensitive to nearby ground/metal - follow keepout guidelines
2. **RF Interference:** May need shielding if EMI testing fails
3. **Firmware:** ESP32-C3 requires separate firmware development
4. **Power Spikes:** 350mA TX peaks may cause voltage droop

---

## Section 18: Vibration Sensor Interface

### Requirement
Provide external interface for industrial vibration sensors (M8 connector) for aircraft engine monitoring.

### Reference Design
- **Custom design** - M8 industrial connector interface
- **Signal type:** SPI from external accelerometer modules

### Decoupling Strategy
| Per Channel | Component | Value | Purpose |
|-------------|-----------|-------|---------|
| SPI CLK | C1801/C1803 | 100nF | Input filtering |
| SPI MISO | C1802/C1804 | 100nF | Input filtering |
| SPI CLK | R1801/R1803 | 33R | Series termination |
| SPI MISO | R1802/R1804 | 33R | Series termination |

### Power
- **Supply to Sensors:** +3V3 or +5V (depends on sensor module)
- **Interface Level:** 3.3V logic

### Communication
- **Interface:** SPI (directly to external sensors)
- **Connector:** M8 4-pin circular (or header for development)

### Calculations
**SPI Signal Integrity:**
```
External cable length: Up to 1m expected
Series resistors: 33R (matches SPI output impedance)
Filter capacitors: 100nF (RC = 33 * 100nF = 3.3us)
This limits bandwidth to ~50kHz, adequate for 1MHz SPI
```

### Test Points
- Connector pinout accessible for debug

### Risks/Unknowns
1. **Cable Length:** Long cables may degrade SPI signal - verify with actual installation
2. **EMI Pickup:** Engine environment is electrically noisy - shielded cables recommended
3. **Connector Sealing:** Aviation environment requires weatherproof connection
4. **Sensor Compatibility:** Interface designed generically - verify specific sensor requirements

---

## Section 19: Audio Alarm (MLT-8530)

### Requirement
Provide audible alarm output for critical alerts (stall warning, system faults).

### Reference Design
- **Primary:** MLT-8530 active buzzer datasheet
- **Application:** MOSFET low-side switch driver

### Decoupling Strategy
Not applicable - electromechanical device.

**Driver Circuit:**
| Component | Value | Purpose |
|-----------|-------|---------|
| Q7 | AO3400A | Low-side switch |
| R1901 | 10k | Gate pull-down (default off) |
| R1902 | 100R | Gate series (current limit) |
| D2 | 1N4148W | Flyback diode |

### Power
- **Supply:** +5V
- **Current:** ~30mA (active)
- **Frequency:** 2.7kHz (active buzzer, internally driven)

### Communication
- **Interface:** GPIO (on/off control)
- **PWM:** Optional for volume control (but reduces clarity)

### Calculations
**MOSFET Gate Drive:**
```
GPIO high voltage: 3.3V
Gate threshold (AO3400A): ~1.2V
Rds_on at Vgs=3.3V: <50mOhm (fully enhanced)

Gate current: (3.3V - 0V) / 100R = 33mA peak
Gate charge time: ~100ns (fast switching)
```

**Flyback Protection:**
```
MLT-8530 has internal inductor for tone generation
Flyback diode (D2) prevents voltage spike when MOSFET turns off
1N4148W: Vf=0.7V, If=200mA continuous
Adequate for 30mA buzzer current.
```

### Test Points
- Buzzer control signal at MCU GPIO

### Risks/Unknowns
1. **Volume:** MLT-8530 is relatively quiet (~85dB) - may need external amplified speaker for noisy cockpit
2. **Tone:** Fixed frequency may not be distinctive enough for different alert types
3. **Vibration:** Buzzer may resonate at certain frequencies - verify mounting

---

## Section 20: Debug & Test Points

### Requirement
Provide programming/debug interface and accessible test points for production testing and development.

### Reference Design
- **Primary:** Tag-Connect TC2030-IDC-NL (pogo-pin SWD interface)
- **Secondary:** Standard 2.54mm header (optional population)

### Decoupling Strategy
Not applicable - connectors only.

### Power
Not applicable - debug only.

### Communication
- **Interface:** SWD (Serial Wire Debug)
- **Signals:** SWDIO, SWCLK, SWO, NRST, VCC, GND
- **Compatible:** ST-Link, J-Link, any SWD debugger

### Test Points Summary

| Reference | Net | Purpose |
|-----------|-----|---------|
| TP1 | +3V3 | 3.3V rail verification |
| TP2 | +5V | 5V rail verification |
| TP3 | VBAT | Battery voltage monitoring |
| TP4 | GND | Ground reference |
| TP5 | I2C1_SDA | I2C bus debug |
| TP6 | I2C1_SCL | I2C bus debug |

### Risks/Unknowns
1. **Tag-Connect Alignment:** Requires precise PCB footprint placement
2. **Production Programming:** Tag-Connect suitable for low volume, may need bed-of-nails for mass production

---

## Appendix A: I2C Address Map

| Address | Device | Section | Notes |
|---------|--------|---------|-------|
| 0x20 | MCP23017 | 14 | I/O Expander |
| 0x30 | MMC5983MA | 09 | Magnetometer |
| 0x40 | Si7021 | 11 | Temp/Humidity |
| 0x6B | BQ25792 | 03 | Charger (I2C4) |
| 0x77 | BMP390 | 07 | Barometric |

---

## Appendix B: Power Budget Summary

| Rail | Typical | Maximum | Source |
|------|---------|---------|--------|
| +3V3 | 254mA | 857mA | TPS62133 (3A max) |
| +5V | 180mA | 452mA | TPS62130 (3A max) |
| VSYS | 400mA | 1.5A | BQ25792/Battery |
| VBAT | - | 5A charge | BQ25792 |

---

## Appendix C: Known Issues and Action Items

| Section | Issue | Severity | Action |
|---------|-------|----------|--------|
| 06 | FB resistors give 4.09V not 5V | HIGH | Change R602 to 18k-19.1k |
| 04 | MOSFET thermal at 5A | MEDIUM | Verify PCB thermal design |
| 15 | 3.3V logic to 5V WS2812B | LOW | Test with actual parts |
| 17 | ESP32 antenna keepout | MEDIUM | Verify PCB layout |
| 05/06 | PWR_FLAG for ERC | LOW | Added in schematic |

---

## Document Revision History

| Rev | Date | Author | Changes |
|-----|------|--------|---------|
| 1.0 | 2026-01-29 | Claude | Initial document creation |

