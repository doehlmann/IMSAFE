# IMSAFE Schematic Design Rationale

**Document Version:** 2.0
**Date:** 2026-01-29
**Project:** IMSAFE Flying Assistant
**Purpose:** Design justification and review documentation for schematic sections

---

## Document Overview

This document provides the engineering rationale behind each schematic section of the IMSAFE project. It is intended to accompany printed schematics during design review and serves as a record of design decisions, calculations, and risk assessments.

### Document Structure (Per Section)
- **Requirements:** Formal requirement IDs satisfied (req-XXX-NNN format)
- **Component IDs:** All schematic components with designators
- **Reference Design:** Source of circuit topology (datasheet, app note, custom)
- **Pinout Verification:** Datasheet table/page reference for IC pinouts
- **Decoupling Strategy:** Capacitor selection and placement rationale
- **Power:** Supply rail, voltage, current requirements
- **Expected Voltages/Signals:** Nominal values for test verification
- **Communication:** Interface type and parameters
- **Calculations:** Key design calculations with formulas
- **Thermal Considerations:** Components >0.25W with mitigation strategy
- **Test Points:** Integrated test/debug access
- **ERC Status:** Electrical rules check results and waived warnings
- **DRC Constraints:** PCB layout rules assumed
- **Safe Failure Behavior:** For power/storage sections
- **Risks/Unknowns:** Outstanding concerns or assumptions

---

## ERC Summary (All Sections)

**ERC Run Date:** 2026-01-29
**Tool:** KiCad 9.0 kicad-cli

| Category | Count | Status |
|----------|-------|--------|
| Errors | 174 | Under review |
| Warnings | 233 | Mostly waived |

### Waived Warnings (Global)
- **lib_symbol_mismatch (158):** Custom symbols created for JLCPCB parts - functionally correct
- **lib_symbol_issues (15):** Symbol properties differ from library - intentional customization
- **footprint_link_issues (9):** Custom footprints for JLCPCB assembly

### Errors Requiring Resolution
- **pin_not_connected (105):** NC pins to be explicitly marked
- **hier_label_mismatch (1):** Remaining hierarchical label cleanup
- **power_pin_not_driven (5):** PWR_FLAG symbols added for VSYS/VBAT nets

---

## Section 01: MCU Core (STM32H747AGI6)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-mcu-001 | Dual-core processing for sensor fusion and UI |
| req-mcu-002 | 480MHz Cortex-M7 for display rendering |
| req-mcu-003 | Multiple SPI/I2C peripherals for sensor interfaces |
| req-mcu-004 | USB OTG for data transfer |
| req-mcu-005 | SDMMC for flight data logging |
| req-pwr-010 | RTC backup domain for timekeeping |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U1 | STM32H747AGI6 | UFBGA-169 | C2766695 | Main MCU |
| Y1 | 8MHz | HC49 | C12674 | HSE crystal |
| Y2 | 32.768kHz | 3.2x1.5mm | C32346 | LSE crystal |
| C101-C110 | 100nF | 0603 | C14663 | VDD decoupling |
| C111 | 4.7uF | 0805 | C1779 | VDD bulk |
| C112-C115 | 100nF | 0603 | C14663 | VDDA decoupling |
| C116 | 1uF | 0603 | C15849 | VDDA bulk |
| C117-C118 | 2.2uF | 0603 | C23630 | VCAP |
| C119 | 100nF | 0603 | C14663 | VBAT |
| C120 | 10nF | 0603 | C1588 | VREF+ |
| C121-C122 | 15pF | 0603 | C1644 | HSE load caps |
| C123-C124 | 6.8pF | 0603 | C1633 | LSE load caps |
| R101 | 10k | 0603 | C25804 | NRST pull-up |

### Reference Design
- **Primary:** ST AN4661 "Oscillator design guide for STM8AF/AL/S, STM32 MCUs and MPUs"
- **Secondary:** STM32H747 Reference Manual (RM0399)
- **Evaluation Board:** NUCLEO-H747ZI schematic (MB1363)

### Pinout Verification
- **Verified:** STM32H747AGI6 Datasheet DS13187 Rev 9, Table 11 (Pin Definitions), Pages 75-92
- **Ball Map:** Table 10, Page 74 (UFBGA169 package)

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | All VDD pins |
| VCAP1/2 | 1.2V | +/-10% | Internal regulator output |
| VBAT | 3.30V | - | Tied to VDD (no backup battery) |
| NRST | 3.30V | - | High when not in reset |
| BOOT0 | 0V | - | Boot from flash |
| HSE_OUT | 8MHz | +/-50ppm | Crystal oscillation |
| LSE_OUT | 32.768kHz | +/-20ppm | RTC crystal |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U1 (STM32H747) | 1.3W max (typical 0.7W) | Thermal vias under package, large GND pour on top/bottom layers, 4-layer PCB required |

**Thermal Calculation:**
```
Package: UFBGA-169
Theta_JA (junction to ambient): 34 C/W
At 1.3W: Tj = Ta + (1.3 * 34) = 25 + 44 = 69C (acceptable, <125C max)
Thermal vias: 9 vias minimum in exposed pad area
```

### Test Points
- NRST: Reset signal access
- BOOT0: Boot mode selection
- SWD interface: SWDIO, SWCLK, SWO via Tag-Connect

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 38 | pin_not_connected - NC pins need marking |
| Warnings | 1 | lib_symbol_mismatch - waived |

### DRC Constraints (Assumed)
- Min trace width: 0.15mm (6mil) for signals
- Min spacing: 0.15mm
- Crystal traces: Max 5mm length, guard ring with GND
- Decoupling caps: Within 3mm of power pins
- VCAP traces: Short, wide (0.5mm minimum)

### Risks/Unknowns
1. **Crystal startup:** LSE crystal may require adjustment of load caps based on actual PCB capacitance
2. **EMI:** High-speed MCU may require additional filtering if EMI testing fails
3. **Thermal:** Power dissipation at full load (~1.3W) - thermal vias critical

---

## Section 02: Power Input (USB-C)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-pwr-001 | USB-C power input for charging |
| req-pwr-002 | 5V/3A input capability |
| req-com-001 | USB 2.0 data interface |
| req-esd-001 | ESD protection on external interfaces |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| J1 | USB-C Receptacle | USB-C-16P | C2765186 | USB connector |
| U2 | USBLC6-2SC6 | SOT-23-6 | C2827654 | ESD protection |
| R201 | 5.1k | 0603 | C23186 | CC1 pull-down |
| R202 | 5.1k | 0603 | C23186 | CC2 pull-down |
| C201 | 100nF | 0603 | C14663 | VBUS HF filter |
| C202 | 10uF 16V | 0805 | C15850 | VBUS bulk |

### Reference Design
- **Primary:** USB Type-C Specification Rev 2.0 (UFP/Sink configuration)
- **Secondary:** USBLC6-2SC6 datasheet application circuit
- **Note:** No USB-PD negotiation (5V only), simplified UFP implementation

### Pinout Verification
- **USB-C Connector:** USB-IF Type-C Spec Rev 2.0, Table 3-1 (Receptacle Pin Assignments)
- **USBLC6-2SC6:** ST Datasheet, Table 1 Pin Description, Page 3

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VBUS | 5.0V | +/-5% | USB power input |
| CC1 | 0.41V | - | With Rd=5.1k, source Rp |
| CC2 | 0.41V | - | With Rd=5.1k, source Rp |
| D+ | 0-3.3V | - | USB data, idle low |
| D- | 0-3.3V | - | USB data, idle low |

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

### Thermal Considerations
No components exceed 0.25W under normal operation.

### Test Points
- VBUS: Power input monitoring
- CC1/CC2: Connection detection verification

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- USB D+/D-: 90 Ohm differential impedance
- Trace length matching: Within 0.5mm
- VBUS trace: 0.5mm minimum (2A+ capability)
- ESD component placement: Within 10mm of connector

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Overvoltage on VBUS | ESD diodes clamp to ~7V | Downstream charger IC has OVP |
| Short circuit | USB host current limiting | Fuse or PTC recommended for production |
| Reverse polarity | USB-C connector prevents reversal | N/A |
| ESD event | USBLC6 absorbs discharge | 8kV contact / 15kV air rated |

### Risks/Unknowns
1. **No USB-PD:** Limited to 5V/3A maximum (15W). Higher power requires PD negotiation IC
2. **Cable quality:** Long/cheap cables may not support 3A reliably
3. **Hot-plug:** VBUS transients during connection - bulk cap sized for inrush

---

## Section 03: Battery Charger (BQ25792)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-pwr-003 | 2S Li-ion charging capability |
| req-pwr-004 | Buck-boost topology for Vbat > Vin |
| req-pwr-005 | I2C charge monitoring |
| req-pwr-006 | 5A max charge current |
| req-safe-001 | Battery temperature monitoring |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U3 | BQ25792 | QFN-29 | C5378953 | Charger IC |
| L1 | 2.2uH 5A | 5x5mm | C408335 | Buck-boost inductor |
| R301 | 10mOhm | 1206 | C22270 | Input sense |
| R302 | 5mOhm | 2512 | C208668 | Battery sense |
| C301 | 10uF 25V | 0805 | C15850 | VBUS input bulk |
| C302 | 100nF | 0603 | C14663 | VBUS input HF |
| C303 | 10uF 25V | 0805 | C15850 | PMID |
| C304-C305 | 22uF 16V | 0805 | C45783 | BAT output |
| C306 | 22uF 16V | 0805 | C45783 | VSYS output |
| C307 | 100nF | 0603 | C14663 | VSYS HF |
| C308 | 100nF | 0603 | C14663 | REGN bypass |
| R303-R304 | 4.7k | 0603 | C23162 | I2C4 pull-ups |

### Reference Design
- **Primary:** TI BQ25792 Datasheet (SLUSFX4) Rev A, Application Circuit Fig 32
- **Secondary:** TI Reference Design TIDA-01598 (similar buck-boost charger)
- **Deviation:** Simplified sensing (no full fuel gauge), I2C for basic monitoring only

### Pinout Verification
- **Verified:** BQ25792 Datasheet SLUSFX4 Rev A, Table 1 (Pin Functions), Pages 4-5
- **QFN-29 Package:** Figure 2, Page 3

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VBUS | 5.0V | +/-5% | When USB connected |
| VSYS | 6.0-8.4V | - | Tracks battery, min 6V |
| VBAT | 6.0-8.4V | - | Battery terminal voltage |
| PMID | ~5V | - | Power path midpoint |
| REGN | 5.1V | - | Internal LDO output |
| CHG_INT | 3.3V/0V | - | Active low interrupt |
| SDA/SCL | 3.3V | - | I2C idle high |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U3 (BQ25792) | 0.5W typical @ 3A charge | Thermal pad with 9 vias to inner GND plane |
| L1 (Inductor) | 0.3W DCR loss | Component rated for continuous operation |
| R302 (Sense) | 125mW @ 5A | 2512 package for thermal mass |

**Charger IC Thermal:**
```
QFN-29 Theta_JA: ~32 C/W
At 0.5W: Tj rise = 0.5 * 32 = 16C above ambient
Max junction: 25 + 16 = 41C (well below 125C limit)
```

### Test Points
- VSYS: System voltage monitoring
- CHG_INT: Charge status interrupt

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 6 | pin_not_connected - NC pins |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Inductor to IC: <5mm, wide traces (1mm+)
- Sense resistor traces: Kelvin connection (4-wire)
- VSYS/VBAT traces: 1mm minimum (5A capability)
- Thermal pad: Full coverage with via array

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Input overvoltage | BQ25792 OVP trips at 22V | Input disconnected |
| Battery overvoltage | Charge stops at 8.4V (2S) | Hardware OVP in BQ25792 |
| Battery overcurrent | 5A limit in BQ25792 | Configurable via I2C |
| Battery over-temp | NTC threshold trip | Charge suspended |
| IC over-temp | 120C shutdown | Charge suspended, resumes when cool |
| Short circuit | Hardware protection | Fast current limit |
| I2C failure | Default safe config | 1A charge, conservative settings |

### Risks/Unknowns
1. **Thermal:** Buck-boost efficiency at high currents - monitor IC temperature
2. **I2C Configuration:** Requires firmware to configure charge parameters at startup
3. **JLCPCB Assembly:** BQ25792 QFN-29 package - verify JLCPCB can assemble
4. **NTC Sensing:** Thermistor connection assumed on battery pack - verify compatibility

---

## Section 04: BMS (HY2120 + HY2213)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-pwr-007 | 2S battery protection |
| req-pwr-008 | Overcharge protection (>4.25V/cell) |
| req-pwr-009 | Over-discharge protection (<2.5V/cell) |
| req-safe-002 | Overcurrent/short-circuit protection |
| req-pwr-011 | Cell balancing |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U4 | HY2120-CB | SOT-23-6 | C113632 | 2S protection IC |
| U5 | HY2213-BB3A | SOT-23-3 | C113637 | Cell 1 balancer |
| U6 | HY2213-BB3A | SOT-23-3 | C113637 | Cell 2 balancer |
| Q1-Q2 | AO3400A | SOT-23 | C20917 | Discharge FETs |
| Q3-Q4 | AO3400A | SOT-23 | C20917 | Charge FETs |
| R401 | 330 | 0603 | C23138 | OV delay |
| R402 | 1k | 0603 | C21190 | CS filter |
| R403 | 5mOhm | 1206 | C208668 | Current sense |
| R404-R405 | 62 | 0805 | C17415 | Balance resistors |
| C401-C403 | 100nF | 0603 | C14663 | IC bypass |

### Reference Design
- **Primary:** HY2120-CB Datasheet (2S protection IC)
- **Secondary:** HY2213-BB3A Datasheet (cell balancer)
- **Topology:** Standard Chinese BMS architecture with back-to-back MOSFETs

### Pinout Verification
- **HY2120-CB:** Datasheet Table 1, Pin Configuration (SOT-23-6)
- **HY2213-BB3A:** Datasheet Table 1, Pin Configuration (SOT-23-3)

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VBAT+ | 6.0-8.4V | - | Full pack voltage |
| CELL_MID | 3.0-4.2V | - | Cell 1 positive / Cell 2 negative |
| VBAT- | 0V | - | Pack ground reference |
| VSYS | 6.0-8.4V | - | Protected output (=VBAT when OK) |
| DO (HY2120) | >3V | - | Discharge enable, high=ON |
| CO (HY2120) | >3V | - | Charge enable, high=ON |

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
Package: 0805 (0.25W rating) - marginal, see thermal notes
```

**MOSFET Selection (AO3400A):**
```
Requirements:
- Vds > 8.4V (pack voltage) -> AO3400A = 30V OK
- Id > 5A continuous -> AO3400A = 5.7A @ 25C (marginal)
- Low Rds_on for efficiency -> AO3400A = 28mOhm @ Vgs=4.5V

Back-to-back configuration:
- 2 FETs per protection path (charge + discharge)
- Total Rds = 2 * 28mOhm = 56mOhm
- Conduction loss @ 5A: 5^2 * 0.056 = 1.4W (distributed)
```

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| R404/R405 (Balance) | 284mW each | 0805 package marginal - add thermal relief copper, consider 1206 upgrade |
| Q1-Q4 (MOSFETs) | 350mW each @ 5A | Parallel GND pour on both layers, thermal vias |
| R403 (Sense) | 125mW @ 5A | 1206 package adequate |

**Balance Resistor Thermal:**
```
0805 package rating: 0.25W
Actual dissipation: 0.284W (13% over rating)
Mitigation: Large copper pours (>25mm2) around each resistor
Alternative: Use 1206 package (0.5W rating)
Duty cycle consideration: Balancing is intermittent, not continuous
```

**MOSFET Thermal:**
```
SOT-23 Theta_JA: ~200 C/W
At 350mW per FET: Tj rise = 0.35 * 200 = 70C
With 25C ambient: Tj = 95C (below 150C max)
Thermal relief pads recommended
```

### Test Points
- CELL_MID: Mid-point voltage for cell balance verification
- PACK_NEG: Output side of protection circuit

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 15 | pin_not_connected on unused FET gates |
| Warnings | 1 | PWR_FLAG added for VBAT net |

### DRC Constraints (Assumed)
- High-current traces (VBAT+, VSYS): 2mm minimum width
- Sense resistor: Kelvin connection required
- MOSFET drain/source: 1mm minimum, short to sense resistor
- Balance resistors: Thermal relief pattern

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Cell overvoltage (>4.25V) | HY2120 disables charging | CO pin goes low |
| Cell undervoltage (<2.5V) | HY2120 disables discharge | DO pin goes low |
| Overcurrent (>40A) | HY2120 trips after delay | ~10ms response |
| Short circuit | HY2120 immediate trip | <100us response |
| Protection IC failure | MOSFETs default OFF | Fail-safe, no current flow |
| MOSFET failure (short) | BQ25792 backup protection | Charger OVP/OCP active |
| Balance resistor open | Cell imbalance over time | Monitoring via CELL_MID ADC |

### Risks/Unknowns
1. **MOSFET Thermal:** 1.4W dissipation at 5A - verify PCB thermal design
2. **Balance Rate:** 67mA is slow for large capacity - full balance may take many hours
3. **Protection Latching:** HY2120 behavior after fault trip needs verification
4. **No MCU Control:** Cannot disable protection or read fault status directly

---

## Section 05: 3.3V Regulator (TPS62133)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-pwr-012 | 3.3V regulated supply for digital ICs |
| req-pwr-013 | 3A capability for MCU + sensors |
| req-pwr-014 | >90% efficiency requirement |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U7 | TPS62133RGTR | QFN-16 | C128068 | 3.3V buck regulator |
| L2 | 1.5uH 4A | 4x4mm | C408335 | Output inductor |
| C501 | 10uF 16V | 0805 | C15850 | Input bulk |
| C502 | 100nF | 0603 | C14663 | Input HF |
| C503-C504 | 22uF 10V | 0805 | C45783 | Output bulk |
| C505 | 100nF | 0603 | C14663 | Output HF |
| R501 | 100k | 0603 | C25803 | EN pull-up to VSYS |

### Reference Design
- **Primary:** TI TPS62133 Datasheet (SLVSDI4) Rev B, Application Circuit Fig 19
- **Secondary:** TI WEBENCH Power Designer output
- **Configuration:** Fixed 3.3V output version (no feedback resistors needed)

### Pinout Verification
- **Verified:** TPS62133 Datasheet SLVSDI4 Rev B, Table 1 (Pin Functions), Page 4
- **QFN-16 Package:** Figure 22, Page 16

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VSYS (input) | 6.0-8.4V | - | Battery-derived |
| +3V3 (output) | 3.30V | +/-2% | Fixed output version |
| EN | VSYS | - | Tied high via R501 |
| SW | Switching | - | ~2.5MHz pulses |
| PG | 3.3V | - | Power good, high when OK |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U7 (TPS62133) | 290mW @ 1A | Thermal pad with 4 vias to GND plane |
| L2 (Inductor) | 45mW DCR loss | Shielded inductor, rated for application |

**Regulator Thermal:**
```
QFN-16 Theta_JA: ~45 C/W (with thermal pad)
At 290mW: Tj rise = 0.29 * 45 = 13C
Max junction: 25 + 13 = 38C (excellent margin)
```

### Test Points
- TP1: +3V3 output voltage monitoring

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 3 | pin_not_connected - NC pins |
| Warnings | 0 | PWR_FLAG added for VSYS |

### DRC Constraints (Assumed)
- Input/output traces: 0.5mm minimum
- Inductor to IC: <5mm, direct connection
- Output caps: Within 3mm of VOUT pin
- Thermal pad: Full solder coverage

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Input overvoltage | TPS62133 rated to 17V | BMS limits to 8.4V max |
| Output short | Current limit ~4A, then hiccup | Thermal shutdown if sustained |
| IC failure | Output drops to 0V | MCU reset, system safe state |
| Inductor saturation | Increased current, thermal | Inductor rated >3A sat |

### Risks/Unknowns
1. **Load Transient:** MCU current spikes during flash write - may cause voltage dip
2. **Thermal:** 290mW loss at 1A - verify thermal pad soldering
3. **Input Voltage Range:** VSYS can drop to 6V at low battery - verify regulation maintained

---

## Section 06: 5V Regulator (TPS62130)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-pwr-015 | 5V regulated supply for LEDs, display, buzzer |
| req-pwr-016 | 3A capability for WS2812B LED array |
| req-pwr-017 | Adjustable output for 5.0V +/-3% |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U8 | TPS62130RGTR | QFN-16 | C128068 | Adjustable buck regulator |
| L3 | 1.5uH 4A | 4x4mm | C408335 | Output inductor |
| R601 | 100k | 0603 | C25803 | FB divider top |
| R602 | 19.1k | 0603 | C25947 | FB divider bottom |
| R603 | 100k | 0603 | C25803 | EN pull-up |
| C601 | 10uF 16V | 0805 | C15850 | Input bulk |
| C602 | 100nF | 0603 | C14663 | Input HF |
| C603-C604 | 22uF 10V | 0805 | C45783 | Output bulk |
| C605 | 100nF | 0603 | C14663 | Output HF |

### Reference Design
- **Primary:** TI TPS62130 Datasheet (SLVSBI2) Rev E, Application Circuit Fig 32
- **Configuration:** Adjustable version with resistor divider for 5.0V output

### Pinout Verification
- **Verified:** TPS62130 Datasheet SLVSBI2 Rev E, Table 1 (Pin Functions), Page 4
- **QFN-16 Package:** Figure 33, Page 26

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
- **Output:** +5V (5.03V actual)
- **Max Current:** 3A continuous
- **Typical Load:** 1A (LEDs at medium brightness)

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VSYS (input) | 6.0-8.4V | - | Battery-derived |
| +5V (output) | 5.03V | +/-3% | Resistor divider set |
| FB | 0.80V | +/-1% | Feedback reference |
| EN | VSYS | - | Tied high via R603 |
| SW | Switching | - | ~2.5MHz pulses |

### Communication
- **None:** Standalone regulator

### Calculations

**Feedback Resistor Divider:**
```
TPS62130 Vfb reference: 0.8V
Desired Vout: 5.0V
Formula: Vout = Vfb * (1 + R601/R602)
Rearranged: R601/R602 = (Vout/Vfb) - 1 = (5.0/0.8) - 1 = 5.25

Selected (CORRECTED):
R601 = 100kOhm (top resistor, VOUT to FB) - JLCPCB C25803
R602 = 19.1kOhm (bottom resistor, FB to GND) - JLCPCB C25947
Ratio: 100/19.1 = 5.24

Actual Vout: 0.8V * (1 + 5.24) = 4.99V (~5.0V)
```

**Note:** Original design had R602 = 24.3k (4.09V output). Corrected to 19.1k (E96 value) for proper 5V output.

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U8 (TPS62130) | 400mW @ 2A load | Thermal pad with 4 vias to GND plane |
| L3 (Inductor) | 60mW DCR loss | Shielded inductor, rated for application |

**Regulator Thermal:**
```
At 2A load, 7.4V input, 5V output:
Efficiency: ~90%
Input power: 5V * 2A / 0.9 = 11.1W
Power loss: 11.1 - 10 = 1.1W (distributed across IC and inductor)
IC dissipation: ~400mW
Tj rise: 0.4 * 45 = 18C
Max junction: 25 + 18 = 43C (good margin)
```

### Test Points
- TP2: +5V output voltage monitoring

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 3 | pin_not_connected - NC pins |
| Warnings | 1 | PWR_FLAG added for VSYS |

### DRC Constraints (Assumed)
- FB trace: Short, away from switching node
- Feedback resistors: Close to IC, away from noise sources
- Output traces: 0.5mm minimum for 3A

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| FB resistor open (R601) | Output drops to ~0.8V | LEDs dim/off, not harmful |
| FB resistor open (R602) | Output rises to Vin | LEDs may be damaged - add OVP |
| Output short | Current limit, hiccup mode | Thermal shutdown |
| IC failure | Output drops to 0V | LEDs off, safe state |

### Risks/Unknowns
1. **LED Current:** WS2812B may draw high peak currents - verify regulator response
2. **Thermal:** Higher power dissipation than 3.3V rail due to larger load
3. **Tolerance:** 1% resistors used for feedback divider accuracy

---

## Section 07: Barometric Sensor (BMP390)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-001 | Atmospheric pressure measurement |
| req-sens-002 | Altitude calculation (barometric) |
| req-sens-003 | 0.03 Pa resolution for precise altitude |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U9 | BMP390 | LGA-10 2x2mm | C2682138 | Barometric sensor |
| C701 | 100nF | 0603 | C14663 | VDD bypass |
| C702 | 100nF | 0603 | C14663 | VDDIO bypass |

### Reference Design
- **Primary:** Bosch BMP390 Datasheet BST-BMP390-DS002 Rev 1.1
- **Application:** Direct implementation per datasheet reference circuit (Section 6.2)

### Pinout Verification
- **Verified:** BMP390 Datasheet, Table 5 (Pin Description), Page 8
- **LGA Package:** Figure 4, Page 8

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C701 | 100nF | Power supply bypass |
| VDDIO | C702 | 100nF | I/O supply bypass |

**Note:** BMP390 requires separate VDD (sensor) and VDDIO (I/O level) supplies. Both connected to +3V3.

### Power
- **Supply:** +3V3 (VDD and VDDIO)
- **Current:** 700uA typical (continuous measurement mode)

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Sensor core power |
| VDDIO | 3.30V | +/-5% | I/O level reference |
| SDA | 3.3V idle | - | I2C data |
| SCL | 3.3V idle | - | I2C clock |
| INT | 0V/3.3V | - | Data ready interrupt |
| SDO | 3.3V | - | I2C address select (0x77) |

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

### Thermal Considerations
No components exceed 0.25W. BMP390 current draw is <1mA.

### Test Points
- BARO_INT: Interrupt output for data-ready signaling

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 1 | pin_not_connected - INT optional |
| Warnings | 1 | lib_symbol_mismatch - waived |

### DRC Constraints (Assumed)
- Sensor placement: Away from heat sources (MCU, regulators)
- Vent hole: Required in enclosure above sensor

### Risks/Unknowns
1. **Thermal Effects:** Pressure reading affected by PCB temperature - locate away from heat sources
2. **Venting:** Sensor requires path to ambient air - enclosure design consideration
3. **I2C Address Conflict:** Verify no other device uses 0x77

---

## Section 08: IMU (ICM-42688-P)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-004 | 6-axis motion sensing (accel + gyro) |
| req-sens-005 | Attitude determination |
| req-sens-006 | 32kHz ODR capability |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U10 | ICM-42688-P | LGA-14 2.5x3mm | C2839706 | 6-axis IMU |
| C801 | 100nF | 0603 | C14663 | VDD bypass |
| C802 | 100nF | 0603 | C14663 | VDDIO bypass |
| C803 | 10nF | 0603 | C1588 | Pin 6 required cap |
| C804 | 100nF | 0603 | C14663 | Extra filtering |

### Reference Design
- **Primary:** TDK ICM-42688-P Datasheet DS-000347 Rev 1.6
- **Application:** SPI interface for maximum data rate (Section 8.2)

### Pinout Verification
- **Verified:** ICM-42688-P Datasheet, Table 11 (Pin Description), Page 14
- **LGA Package:** Figure 3, Page 6

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Core power |
| VDDIO | 3.30V | +/-5% | I/O level |
| MISO | 0-3.3V | - | SPI data output |
| MOSI | 0-3.3V | - | SPI data input |
| SCLK | 0-3.3V | - | SPI clock (10MHz) |
| CS | 3.3V idle | - | Active low chip select |
| INT1 | 0V/3.3V | - | Data ready interrupt |

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

### Thermal Considerations
No components exceed 0.25W. IMU current draw is ~1mA.

### Test Points
- INT1: Interrupt output for data-ready

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Sensor placement: Aligned to board axes
- SPI traces: Equal length if possible, <50mm total
- Mounting: Avoid flex in sensor area

### Risks/Unknowns
1. **Vibration Sensitivity:** Aviation environment is noisy - may need filtering algorithm
2. **Mounting:** Sensor alignment critical - PCB placement and enclosure mounting matter
3. **SPI Integrity:** Long traces may require series termination resistors

---

## Section 09: Magnetometer (MMC5983MA)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-007 | 3-axis magnetic field sensing |
| req-sens-008 | Compass heading determination |
| req-sens-009 | SET/RESET for offset cancellation |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U11 | MMC5983MA | LGA-16 3x3mm | C2843303 | 3-axis magnetometer |
| C901 | 100nF | 0603 | C14663 | VDD bypass |
| C902 | 100nF | 0603 | C14663 | VDDIO bypass |
| C903 | 4.7uF | 0603 | C19666 | SET/RESET capacitor |

### Reference Design
- **Primary:** MEMSIC MMC5983MA Datasheet Rev C
- **Application:** I2C interface implementation (Section 5)

### Pinout Verification
- **Verified:** MMC5983MA Datasheet, Table 1 (Pin Description), Page 5
- **LGA Package:** Figure 1, Page 3

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Core power |
| VDDIO | 3.30V | +/-5% | I/O level |
| SDA | 3.3V idle | - | I2C data |
| SCL | 3.3V idle | - | I2C clock |
| C1 pin | Varies | - | Charge pump, ~2-3V peaks |

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

### Thermal Considerations
No components exceed 0.25W. Magnetometer current draw is <1mA.

### Test Points
- None dedicated (uses shared I2C bus test points)

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Placement: >20mm from inductors, speaker magnets
- Keep-out: No copper pour directly under sensor
- Orientation: Aligned to board axes

### Risks/Unknowns
1. **Magnetic Interference:** PCB traces, inductors, speaker magnet may affect readings
2. **Calibration:** Requires hard/soft iron compensation in firmware
3. **Distance from MCU:** Place away from high-current switching circuits

---

## Section 10: Gas Sensor (MICS-4514)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-010 | CO detection for cabin air quality |
| req-sens-011 | NO2 detection for cabin air quality |
| req-safe-003 | Carbon monoxide warning system |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U12 | MICS-4514 | SMD | C3029912 | Dual gas sensor |
| Q5 | AO3400A | SOT-23 | C20917 | Heater MOSFET |
| R1001 | 10k | 0603 | C25804 | Gate pull-down |
| R1002 | 100 | 0603 | C22775 | Gate series |
| R1003-R1004 | 10k | 0603 | C25804 | Load resistors |
| C1001-C1002 | 100nF | 0603 | C14663 | ADC input filter |
| R1005-R1006 | 1k | 0603 | C21190 | ADC series resistor |

### Reference Design
- **Primary:** SGX MICS-4514 Datasheet
- **Application:** Custom heater control and analog signal conditioning

### Pinout Verification
- **Verified:** MICS-4514 Datasheet, Pin Configuration Table, Page 2
- **Package:** 8-pin SMD footprint

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| Heater+ | 5.0V | - | When heater on |
| RED_OUT | 0.3-1.65V | - | Reducing gas (CO) |
| OX_OUT | 0.3-1.65V | - | Oxidizing gas (NO2) |
| HEATER_EN | 0/3.3V | - | PWM control signal |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U12 (MICS-4514 heater) | 500mW | Sensor designed for this; allow airflow around sensor |
| Q5 (MOSFET) | 28mW @ 100mA | SOT-23 adequate |

**Heater Thermal:**
```
MICS-4514 is designed to operate with internal heater at 500mW
Operating temperature: ~300C at heater element (internal)
Package surface: ~60C rise above ambient
Keep-out zone: 5mm around sensor recommended
```

### Test Points
- Analog outputs to MCU ADC (accessible via header)

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Sensor placement: Access to ambient air required
- Keep-out: 5mm around sensor (thermal)
- Analog traces: Away from digital switching noise

### Risks/Unknowns
1. **Warm-up Time:** Sensor requires 10+ minutes to stabilize after power-on
2. **Calibration:** Requires known gas concentrations for accurate readings
3. **Heater Thermal:** 500mW continuous - thermal design consideration
4. **Cross-sensitivity:** May respond to other gases - algorithm needed

---

## Section 11: Temperature/Humidity Sensor (Si7021)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-012 | Ambient temperature measurement |
| req-sens-013 | Relative humidity measurement |
| req-imsafe-001 | Environmental monitoring for pilot assessment |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U13 | Si7021-A20 | DFN-6 3x3mm | C82108 | Temp/humidity sensor |
| C1101 | 100nF | 0603 | C14663 | VDD bypass |

### Reference Design
- **Primary:** Silicon Labs Si7021-A20 Datasheet Rev 1.4
- **Application:** Minimal external components per reference design (Section 4)

### Pinout Verification
- **Verified:** Si7021-A20 Datasheet, Table 1 (Pin Description), Page 5
- **DFN Package:** Figure 1, Page 5

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C1101 | 100nF | Power supply bypass |

**Note:** Si7021 is extremely low power and requires minimal filtering.

### Power
- **Supply:** +3V3
- **Current:** 150uA typical (during measurement)

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Supply |
| SDA | 3.3V idle | - | I2C data |
| SCL | 3.3V idle | - | I2C clock |

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

### Thermal Considerations
No components exceed 0.25W. Si7021 current draw is <1mA.

### Test Points
- None dedicated (uses shared I2C bus test points)

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Placement: Away from heat sources
- Ventilation: Exposure to ambient air required

### Risks/Unknowns
1. **Self-heating:** MCU/power electronics may affect local temperature reading
2. **Response Time:** Humidity sensor has ~8 second response time
3. **Condensation:** Sensor may be damaged by direct water contact

---

## Section 12: Display Interface (ST7789)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-ui-001 | Visual display for pilot information |
| req-ui-002 | 240x320 minimum resolution |
| req-ui-003 | >30fps update rate |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| J2 | FPC-24P | 0.5mm pitch | C2856805 | Display connector |
| Q6 | AO3400A | SOT-23 | C20917 | Backlight MOSFET |
| R1201 | 10k | 0603 | C25804 | Gate pull-down |
| R1202 | 100 | 0603 | C22775 | Gate series |
| R1203-R1205 | 33 | 0603 | C23140 | SPI series resistors |
| C1201 | 10uF | 0805 | C15850 | VDD bulk |
| C1202 | 100nF | 0603 | C14663 | VDD HF |

### Reference Design
- **Primary:** Display module datasheet (FPC connector pinout)
- **Application:** SPI interface with GPIO control signals

### Pinout Verification
- **FPC Connector:** Verified against specific display module datasheet (module-dependent)
- **ST7789:** Sitronix ST7789V Datasheet, Pin Description, Page 10

### Decoupling Strategy
| Function | Value | Purpose |
|----------|-------|---------|
| Display VDD | 100nF + 10uF | Module power filtering |
| SPI signals | 33R series | Impedance matching |

### Power
- **Display Logic:** +3V3 (from display module regulator)
- **Backlight:** +5V via PWM-controlled MOSFET
- **Backlight Current:** Up to 130mA at full brightness

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Display logic |
| BL+ | 5.0V | - | Backlight (PWM modulated) |
| MOSI | 0-3.3V | - | SPI data (40MHz) |
| SCLK | 0-3.3V | - | SPI clock |
| DC | 0/3.3V | - | Data/command select |
| RST | 3.3V | - | Active low reset |
| CS | 3.3V idle | - | Active low chip select |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| Backlight LEDs | 650mW max | Display module handles thermal; PWM reduces average power |
| Q6 (MOSFET) | 47mW @ 130mA | SOT-23 adequate |

### Test Points
- Backlight PWM signal accessible via test pad

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- FPC connector: Match display pinout exactly
- SPI traces: <100mm, controlled impedance if possible

### Risks/Unknowns
1. **FPC Connector:** Verify pinout matches specific display module ordered
2. **Display Variants:** Different ST7789 modules have different initialization sequences
3. **Viewing Angle:** IPS provides good angles, but verify for cockpit mounting

---

## Section 13: microSD Card (MEM2067)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-stor-001 | Flight data logging capability |
| req-stor-002 | Removable storage for data retrieval |
| req-stor-003 | FAT32/exFAT compatibility |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| J3 | MEM2067 | microSD hinged | C111196 | SD card socket |
| U14 | USBLC6-2SC6 | SOT-23-6 | C2827654 | ESD protection |
| R1301-R1304 | 33 | 0603 | C23140 | SDMMC series resistors |
| C1301 | 10uF | 0805 | C15850 | VDD bulk |
| C1302 | 100nF | 0603 | C14663 | VDD HF |
| R1305 | 10k | 0603 | C25804 | Card detect pull-up |

### Reference Design
- **Primary:** MEM2067 hinged socket datasheet
- **Secondary:** STM32 SDMMC application notes (AN4031)
- **Protection:** USBLC6-2SC6 for ESD on all data lines

### Pinout Verification
- **MEM2067:** Verified against manufacturer datasheet, pin mapping table
- **SD Card Standard:** SD Physical Layer Spec 4.10, Pin Assignments

### Decoupling Strategy
| Function | Value | Purpose |
|----------|-------|---------|
| VDD | 10uF + 100nF | Card power supply |
| Signal lines | 33R series | Impedance matching |

### Power
- **Supply:** +3V3
- **Current:** 30mA typical, 100mA peak (during write)

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Card power |
| CLK | 0-3.3V | - | 50MHz max |
| CMD | 0-3.3V | - | Command/response |
| DAT0-DAT3 | 0-3.3V | - | 4-bit data |
| CD | 0V card in | - | Card detect |

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

### Thermal Considerations
No components exceed 0.25W during normal operation.

### Test Points
- Card detect signal

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- SDMMC traces: Equal length, 50 Ohm impedance target
- ESD device: Close to connector

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Card removed during write | File system corruption | Software: safe unmount, journaling |
| Card not present | SDMMC init fails | Software detects, continues without logging |
| Card full | Write errors | Software monitors space, warns user |
| ESD event | USBLC6 absorbs | Card protected, data may need re-read |
| Card failure | Read/write errors | Software detects CRC errors, warns user |

### Risks/Unknowns
1. **Card Compatibility:** Some cards may not support high-speed mode
2. **Hot Removal:** Data corruption if card removed during write
3. **ESD:** High-traffic connector - ESD protection essential
4. **File System:** FAT32 or exFAT required for interoperability

---

## Section 14: I/O Expander & Switches (MCP23017)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-ui-004 | 8 toggle switch inputs for IMSAFE checklist |
| req-io-001 | GPIO expansion via I2C |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U15 | MCP23017-E/SO | SOIC-28 | C47023 | 16-bit I/O expander |
| C1401 | 100nF | 0603 | C14663 | VDD bypass |
| SW1-SW8 | Toggle switch | - | - | IMSAFE input switches |

### Reference Design
- **Primary:** Microchip MCP23017 Datasheet DS20001952C
- **Application:** I2C I/O expansion with internal pull-ups (Section 3.5)

### Pinout Verification
- **Verified:** MCP23017 Datasheet, Table 1-1 (Pin Function Table), Page 7
- **SOIC-28 Package:** Figure 1-1, Page 2

### Decoupling Strategy
| Pin | Cap | Value | Purpose |
|-----|-----|-------|---------|
| VDD | C1401 | 100nF | Power supply bypass |

### Power
- **Supply:** +3V3
- **Current:** 1mA typical

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 3.30V | +/-5% | Supply |
| SDA | 3.3V idle | - | I2C data |
| SCL | 3.3V idle | - | I2C clock |
| GPA0-GPA7 | 3.3V or 0V | - | Port A (switches to GND) |
| GPB0-GPB7 | 3.3V or 0V | - | Port B (expansion) |
| INTA | 3.3V/0V | - | Port A interrupt (open-drain) |
| INTB | 3.3V/0V | - | Port B interrupt (open-drain) |
| A0-A2 | 0V | - | Address select = 0x20 |

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

### Thermal Considerations
No components exceed 0.25W. MCP23017 current draw is ~1mA.

### Test Points
- INTA, INTB: Interrupt outputs for debug

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 22 | pin_not_connected - unused GPIO |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Switch traces: Can be long, low speed
- I2C: Standard 400kHz routing

### Risks/Unknowns
1. **I2C Bus Loading:** Adding more devices to I2C1 bus - verify capacitance budget
2. **Internal Pull-ups:** 100k may be weak for long switch wiring
3. **Interrupt Latency:** I2C polling or interrupt response time for switch detection

---

## Section 15: Status LEDs (WS2812B-2020)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-ui-005 | IMSAFE status indication (8 LEDs) |
| req-ui-006 | System status indication (8 LEDs) |
| req-ui-007 | Alert/warning indication |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| LED1-LED16 | WS2812B-2020 | 2020 | C2843785 | Addressable RGB LED |
| C1501-C1516 | 100nF | 0603 | C14663 | Per-LED bypass |
| R1501 | 330 | 0603 | C23138 | Data series resistor |

### Reference Design
- **Primary:** Worldsemi WS2812B-2020 Datasheet
- **Application:** Daisy-chain configuration with series data resistor

### Pinout Verification
- **Verified:** WS2812B-2020 Datasheet, Pin Configuration, Page 2
- **2020 Package:** 2.0x2.0mm QFN-4

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VDD | 5.0V | +/-5% | LED power |
| DIN (first) | 0/3.3V | - | Data from MCU |
| DOUT (each) | 0/5V | - | Regenerated data to next |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| LED1-LED16 | 180mW each at full white | Limit brightness in firmware; typical operation <50mW/LED |

**LED Array Thermal:**
```
At full white all 16 LEDs: 2.88W total
Per-LED dissipation: 180mW
2020 package thermal resistance: ~150 C/W
Temperature rise: 180mW * 150 = 27C per LED
Acceptable if LEDs spaced >5mm apart
Firmware should limit duty cycle for sustained full-white
```

### Test Points
- LED_DATA: Data signal to first LED

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- VDD trace to LEDs: 0.5mm minimum for current
- Decoupling caps: <3mm from each LED VDD pin
- Data trace: Short from MCU to first LED

### Risks/Unknowns
1. **Timing Critical:** STM32 DMA required for precise timing - verify implementation
2. **5V Logic:** WS2812B expects 5V logic, 3.3V may be marginal - verify with actual parts
3. **Power Transients:** All LEDs switching simultaneously causes current spikes
4. **Thermal:** At full brightness, LEDs generate significant heat

---

## Section 16: Rotary Encoders (EC11)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-ui-008 | Menu navigation input |
| req-ui-009 | Value adjustment input |
| req-ui-010 | Push-button selection |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| SW9-SW10 | EC11E | Through-hole | C470752 | Rotary encoder with switch |
| C1601-C1602 | 10nF | 0603 | C1588 | Encoder 1 A/B debounce |
| C1603 | 100nF | 0603 | C14663 | Encoder 1 switch debounce |
| C1604-C1605 | 10nF | 0603 | C1588 | Encoder 2 A/B debounce |
| C1606 | 100nF | 0603 | C14663 | Encoder 2 switch debounce |

### Reference Design
- **Primary:** Generic EC11 rotary encoder datasheet
- **Application:** Hardware RC debouncing for reliable edge detection

### Pinout Verification
- **EC11E:** Standard pinout verified against manufacturer datasheet

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| ENC1_A | 3.3V/0V | - | Quadrature channel A |
| ENC1_B | 3.3V/0V | - | Quadrature channel B |
| ENC1_SW | 3.3V/0V | - | Push switch |
| ENC2_A | 3.3V/0V | - | Quadrature channel A |
| ENC2_B | 3.3V/0V | - | Quadrature channel B |
| ENC2_SW | 3.3V/0V | - | Push switch |

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

### Thermal Considerations
No components exceed 0.25W. Passive switches, no power dissipation.

### Test Points
- Encoder signals available at MCU GPIO pads

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 6 | pin_not_connected - common pins |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Encoder traces: Low speed, can be long
- Mechanical: Sufficient board space for encoder footprint

### Risks/Unknowns
1. **Encoder Quality:** Cheap encoders may have excessive bounce - verify with scope
2. **Pull-up Strength:** MCU internal pull-ups may need external supplementation
3. **Mechanical Wear:** Encoders have limited lifespan - consider replaceable mounting

---

## Section 17: Bluetooth Module (ESP32-C3-MINI-1)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-com-002 | Bluetooth Low Energy connectivity |
| req-com-003 | Companion app communication |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| U16 | ESP32-C3-MINI-1 | Module | C2934560 | BLE module |
| C1701 | 10uF | 0805 | C15850 | Bulk power |
| C1702 | 100nF | 0603 | C14663 | HF bypass |
| C1703 | 1uF | 0603 | C15849 | EN delay cap |
| R1701 | 10k | 0603 | C25804 | EN pull-up |

### Reference Design
- **Primary:** Espressif ESP32-C3-MINI-1 Datasheet v1.4
- **Secondary:** ESP32-C3 Hardware Design Guidelines v1.2
- **Application:** UART bridge to STM32 for BLE data exchange

### Pinout Verification
- **Verified:** ESP32-C3-MINI-1 Datasheet, Table 1 (Pin Description), Page 8
- **Module:** Figure 1, Page 7

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| 3V3 | 3.30V | +/-5% | Module power |
| EN | 3.3V | - | Enable, high = run |
| TX | 0-3.3V | - | UART transmit to STM32 |
| RX | 0-3.3V | - | UART receive from STM32 |
| IO9 | 3.3V | - | Boot mode (high = normal) |

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

### Thermal Considerations
| Component | Power Dissipation | Mitigation |
|-----------|------------------|------------|
| U16 (ESP32-C3) | 1.15W peak (350mA @ 3.3V) | Module has integrated thermal design; TX bursts are brief |

**Module Thermal:**
```
Peak power during TX: 350mA * 3.3V = 1.15W
Duration: Brief bursts (<10ms typical)
Average power: ~66mW (20mA average)
Module designed for continuous operation
No additional thermal mitigation required
```

### Test Points
- UART TX/RX accessible via debug header

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Antenna keep-out: 15mm clear zone around module antenna area
- No copper pour, traces, or components in antenna zone
- Module ground: Multiple vias to ground plane

### Risks/Unknowns
1. **Antenna Placement:** PCB antenna sensitive to nearby ground/metal - follow keepout guidelines
2. **RF Interference:** May need shielding if EMI testing fails
3. **Firmware:** ESP32-C3 requires separate firmware development
4. **Power Spikes:** 350mA TX peaks may cause voltage droop

---

## Section 18: Vibration Sensor Interface

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-sens-014 | External vibration sensor interface |
| req-sens-015 | Engine monitoring capability |
| req-ext-001 | Industrial M8 connector compatibility |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| J4-J5 | M8-4P Female | Panel mount | - | Vibration sensor connectors |
| C1801-C1804 | 100nF | 0603 | C14663 | SPI input filtering |
| R1801-R1804 | 33 | 0603 | C23140 | SPI series termination |

### Reference Design
- **Custom design** - M8 industrial connector interface
- **Signal type:** SPI from external accelerometer modules

### Pinout Verification
- **M8 Connector:** IEC 61076-2-104 standard pinout
- **Pin 1:** VCC (+3.3V or +5V)
- **Pin 2:** GND
- **Pin 3:** SPI CLK
- **Pin 4:** SPI MISO

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| VCC (J4/J5) | 3.3V or 5V | +/-5% | Sensor power |
| GND | 0V | - | Reference |
| CLK | 0-3.3V | - | SPI clock (1-10MHz) |
| MISO | 0-3.3V | - | SPI data from sensor |

### Communication
- **Interface:** SPI (directly to external sensors)
- **Connector:** M8 4-pin circular (or header for development)

### Cable Specifications
| Parameter | Value | Notes |
|-----------|-------|-------|
| **Cable Length** | 1m maximum | Assumed for SPI signal integrity |
| **Connector Type** | M8 4-pin female (A-coded) | Industrial standard, IP67 rated |
| **Cable Type** | Shielded 4-conductor | 22-24 AWG recommended |
| **Shield Grounding** | Drain wire to GND at PCB end only | Prevents ground loops |

### Shielding/Grounding Strategy
```
Cable Shield Connection:
- Shield connected to GND at PCB connector end
- Shield NOT connected at sensor end (floating)
- This prevents ground loop currents through shield

PCB Grounding:
- M8 connector shell connected to PCB GND via 100nF capacitor
- Provides HF noise shunting without DC ground path
- Separate ground pour for analog sensor signals recommended

EMI Mitigation:
- Shielded cable mandatory for engine bay environment
- Series resistors (33R) provide source termination
- Filter capacitors (100nF) provide low-pass filtering
- RC cutoff: 1/(2*pi*33*100nF) = 48kHz
```

### Calculations
**SPI Signal Integrity:**
```
External cable length: Up to 1m expected
Series resistors: 33R (matches SPI output impedance)
Filter capacitors: 100nF (RC = 33 * 100nF = 3.3us)
This limits bandwidth to ~50kHz, adequate for 1MHz SPI
```

### Thermal Considerations
No components exceed 0.25W. Interface only, sensors external.

### Test Points
- Connector pinout accessible for debug

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- M8 connector: Panel mount footprint
- SPI traces: Standard routing, short before connector

### Safe Failure Behavior
| Failure Mode | Behavior | Mitigation |
|--------------|----------|------------|
| Cable disconnected | SPI reads invalid | Software timeout detection |
| Cable shorted | SPI signals corrupted | Series resistors limit current |
| Sensor failure | Invalid data | Software validation, CRC if available |
| EMI interference | Corrupted readings | Shielded cable, filtering |
| Overvoltage on cable | Potential IC damage | Consider TVS diodes for production |

### Risks/Unknowns
1. **Cable Length:** Long cables may degrade SPI signal - 1m assumed maximum
2. **EMI Pickup:** Engine environment is electrically noisy - shielded cables mandatory
3. **Connector Sealing:** Aviation environment requires weatherproof connection (IP67)
4. **Sensor Compatibility:** Interface designed generically - verify specific sensor requirements

---

## Section 19: Audio Alarm (MLT-8530)

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-ui-011 | Audible alarm for critical alerts |
| req-safe-004 | Stall warning audio |
| req-safe-005 | System fault notification |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| BZ1 | MLT-8530 | SMD | C94599 | Active buzzer |
| Q7 | AO3400A | SOT-23 | C20917 | Low-side switch |
| R1901 | 10k | 0603 | C25804 | Gate pull-down |
| R1902 | 100 | 0603 | C22775 | Gate series |
| D2 | 1N4148W | SOD-123 | C81598 | Flyback diode |

### Reference Design
- **Primary:** MLT-8530 active buzzer datasheet
- **Application:** MOSFET low-side switch driver

### Pinout Verification
- **MLT-8530:** Verified polarity marking (+ terminal)
- **AO3400A:** Datasheet Table 1, SOT-23 pinout

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

### Expected Voltages/Signals
| Net | Expected Value | Tolerance | Notes |
|-----|---------------|-----------|-------|
| BZ+ | 5.0V | - | Buzzer positive |
| BZ- | 0V (on) / 5V (off) | - | MOSFET drain |
| BUZZER_EN | 0/3.3V | - | Gate control from MCU |

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

### Thermal Considerations
No components exceed 0.25W. Buzzer draws 30mA at 5V = 150mW.

### Test Points
- Buzzer control signal at MCU GPIO

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Buzzer placement: Away from sensors (vibration)
- Power trace: 0.3mm minimum

### Risks/Unknowns
1. **Volume:** MLT-8530 is relatively quiet (~85dB) - may need external amplified speaker for noisy cockpit
2. **Tone:** Fixed frequency may not be distinctive enough for different alert types
3. **Vibration:** Buzzer may resonate at certain frequencies - verify mounting

---

## Section 20: Debug & Test Points

### Requirements Satisfied
| Req ID | Description |
|--------|-------------|
| req-test-001 | SWD programming interface |
| req-test-002 | Key voltage test points |
| req-test-003 | I2C bus access for debug |

### Component IDs
| Designator | Value/Part | Package | JLCPCB | Function |
|------------|-----------|---------|--------|----------|
| J6 | Tag-Connect TC2030-IDC-NL | Pogo pin | - | SWD debug |
| TP1 | Test point | 1mm pad | - | +3V3 |
| TP2 | Test point | 1mm pad | - | +5V |
| TP3 | Test point | 1mm pad | - | VBAT |
| TP4 | Test point | 1mm pad | - | GND |
| TP5 | Test point | 1mm pad | - | I2C1_SDA |
| TP6 | Test point | 1mm pad | - | I2C1_SCL |

### Reference Design
- **Primary:** Tag-Connect TC2030-IDC-NL (pogo-pin SWD interface)
- **Secondary:** Standard 2.54mm header (optional population)

### Pinout Verification
- **Tag-Connect:** TC2030-IDC-NL datasheet, standard SWD pinout

### Decoupling Strategy
Not applicable - connectors only.

### Power
Not applicable - debug only.

### Communication
- **Interface:** SWD (Serial Wire Debug)
- **Signals:** SWDIO, SWCLK, SWO, NRST, VCC, GND
- **Compatible:** ST-Link, J-Link, any SWD debugger

### Test Points Summary

| Reference | Net | Expected Value | Purpose |
|-----------|-----|----------------|---------|
| TP1 | +3V3 | 3.30V +/-2% | 3.3V rail verification |
| TP2 | +5V | 5.03V +/-3% | 5V rail verification |
| TP3 | VBAT | 6.0-8.4V | Battery voltage monitoring |
| TP4 | GND | 0V | Ground reference |
| TP5 | I2C1_SDA | 3.3V idle | I2C bus debug |
| TP6 | I2C1_SCL | 3.3V idle | I2C bus debug |

### Thermal Considerations
Not applicable - passive test points.

### ERC Status
| Type | Count | Status |
|------|-------|--------|
| Errors | 0 | Clean |
| Warnings | 0 | Clean |

### DRC Constraints (Assumed)
- Tag-Connect: Precise footprint placement per TC2030-IDC-NL datasheet
- Test points: 1mm pad minimum, accessible for probes

### Risks/Unknowns
1. **Tag-Connect Alignment:** Requires precise PCB footprint placement
2. **Production Programming:** Tag-Connect suitable for low volume, may need bed-of-nails for mass production

---

## Appendix A: I2C Address Map

| Address | Device | Section | Bus | Notes |
|---------|--------|---------|-----|-------|
| 0x20 | MCP23017 | 14 | I2C1 | I/O Expander |
| 0x30 | MMC5983MA | 09 | I2C1 | Magnetometer |
| 0x40 | Si7021 | 11 | I2C1 | Temp/Humidity |
| 0x6B | BQ25792 | 03 | I2C4 | Charger |
| 0x77 | BMP390 | 07 | I2C1 | Barometric |

---

## Appendix B: Power Budget Summary

| Rail | Typical | Maximum | Source | Margin |
|------|---------|---------|--------|--------|
| +3V3 | 254mA | 857mA | TPS62133 (3A max) | 71% |
| +5V | 180mA | 706mA | TPS62130 (3A max) | 76% |
| VSYS | 400mA | 1.5A | BQ25792/Battery | 70% |
| VBAT | - | 5A charge | BQ25792 | - |

---

## Appendix C: Requirement Traceability Matrix

| Req ID | Description | Section(s) | Status |
|--------|-------------|------------|--------|
| req-mcu-001 | Dual-core processing | 01 | Implemented |
| req-mcu-002 | 480MHz Cortex-M7 | 01 | Implemented |
| req-mcu-003 | Multiple SPI/I2C | 01 | Implemented |
| req-mcu-004 | USB OTG | 01, 02 | Implemented |
| req-mcu-005 | SDMMC | 01, 13 | Implemented |
| req-pwr-001 | USB-C power input | 02 | Implemented |
| req-pwr-002 | 5V/3A input | 02 | Implemented |
| req-pwr-003 | 2S Li-ion charging | 03 | Implemented |
| req-pwr-004 | Buck-boost topology | 03 | Implemented |
| req-pwr-005 | I2C charge monitoring | 03 | Implemented |
| req-pwr-006 | 5A max charge | 03 | Implemented |
| req-pwr-007 | 2S battery protection | 04 | Implemented |
| req-pwr-008 | Overcharge protection | 04 | Implemented |
| req-pwr-009 | Over-discharge protection | 04 | Implemented |
| req-pwr-010 | RTC backup | 01 | Implemented |
| req-pwr-011 | Cell balancing | 04 | Implemented |
| req-pwr-012 | 3.3V supply | 05 | Implemented |
| req-pwr-013 | 3A MCU supply | 05 | Implemented |
| req-pwr-014 | >90% efficiency | 05 | Implemented |
| req-pwr-015 | 5V LED supply | 06 | Implemented |
| req-pwr-016 | 3A LED capability | 06 | Implemented |
| req-pwr-017 | 5V +/-3% | 06 | Implemented |
| req-sens-001 | Pressure measurement | 07 | Implemented |
| req-sens-002 | Altitude calculation | 07 | Implemented |
| req-sens-003 | 0.03 Pa resolution | 07 | Implemented |
| req-sens-004 | 6-axis motion | 08 | Implemented |
| req-sens-005 | Attitude determination | 08 | Implemented |
| req-sens-006 | 32kHz ODR | 08 | Implemented |
| req-sens-007 | Magnetic sensing | 09 | Implemented |
| req-sens-008 | Compass heading | 09 | Implemented |
| req-sens-009 | SET/RESET capability | 09 | Implemented |
| req-sens-010 | CO detection | 10 | Implemented |
| req-sens-011 | NO2 detection | 10 | Implemented |
| req-sens-012 | Temperature sensing | 11 | Implemented |
| req-sens-013 | Humidity sensing | 11 | Implemented |
| req-sens-014 | Vibration interface | 18 | Implemented |
| req-sens-015 | Engine monitoring | 18 | Implemented |
| req-ui-001 | Visual display | 12 | Implemented |
| req-ui-002 | 240x320 resolution | 12 | Implemented |
| req-ui-003 | >30fps update | 12 | Implemented |
| req-ui-004 | 8 toggle switches | 14 | Implemented |
| req-ui-005 | IMSAFE LED status | 15 | Implemented |
| req-ui-006 | System status LEDs | 15 | Implemented |
| req-ui-007 | Alert indication | 15 | Implemented |
| req-ui-008 | Menu navigation | 16 | Implemented |
| req-ui-009 | Value adjustment | 16 | Implemented |
| req-ui-010 | Push-button select | 16 | Implemented |
| req-ui-011 | Audio alarm | 19 | Implemented |
| req-com-001 | USB 2.0 data | 02 | Implemented |
| req-com-002 | BLE connectivity | 17 | Implemented |
| req-com-003 | App communication | 17 | Implemented |
| req-stor-001 | Flight logging | 13 | Implemented |
| req-stor-002 | Removable storage | 13 | Implemented |
| req-stor-003 | FAT32/exFAT | 13 | Implemented |
| req-safe-001 | Battery temp monitoring | 03 | Implemented |
| req-safe-002 | Overcurrent protection | 04 | Implemented |
| req-safe-003 | CO warning | 10 | Implemented |
| req-safe-004 | Stall warning audio | 19 | Implemented |
| req-safe-005 | Fault notification | 19 | Implemented |
| req-esd-001 | ESD protection | 02, 13 | Implemented |
| req-io-001 | GPIO expansion | 14 | Implemented |
| req-ext-001 | M8 connector | 18 | Implemented |
| req-test-001 | SWD programming | 20 | Implemented |
| req-test-002 | Voltage test points | 20 | Implemented |
| req-test-003 | I2C debug access | 20 | Implemented |
| req-imsafe-001 | Environmental monitoring | 11 | Implemented |

---

## Appendix D: Known Issues and Action Items

| Section | Issue | Severity | Status |
|---------|-------|----------|--------|
| 06 | FB resistors corrected | HIGH | **FIXED** - R602 changed to 19.1k |
| 04 | Balance resistors marginal thermal | MEDIUM | Monitor - 0.284W in 0.25W package |
| 04 | MOSFET thermal at 5A | MEDIUM | Verify PCB thermal design |
| 15 | 3.3V logic to 5V WS2812B | LOW | Test with actual parts |
| 17 | ESP32 antenna keepout | MEDIUM | Verify PCB layout |
| 05/06 | PWR_FLAG for ERC | LOW | **FIXED** - Added in schematic |
| 01 | 174 ERC errors | MEDIUM | pin_not_connected - mark NC pins |

---

## Appendix E: Global DRC Constraints

| Parameter | Value | Notes |
|-----------|-------|-------|
| Min trace width (signal) | 0.15mm (6mil) | Standard signals |
| Min trace width (power) | 0.5mm | 1A+ currents |
| Min trace spacing | 0.15mm (6mil) | Standard |
| Min via drill | 0.3mm | Standard |
| Min via annular ring | 0.15mm | Standard |
| Board thickness | 1.6mm | JLCPCB standard |
| Copper weight | 1oz (35um) | Standard |
| Layer count | 4 | Required for thermal/EMI |
| Impedance (USB) | 90 Ohm differential | USB 2.0 spec |
| Impedance (SPI) | 50 Ohm single-ended | Best practice |

---

## Document Revision History

| Rev | Date | Author | Changes |
|-----|------|--------|---------|
| 1.0 | 2026-01-29 | Claude | Initial document creation |
| 2.0 | 2026-01-29 | Claude | Added: requirement IDs, component tables, ERC status, pinout verification, safe failure behavior, vibration sensor cable specs, thermal notes, expected voltages |

