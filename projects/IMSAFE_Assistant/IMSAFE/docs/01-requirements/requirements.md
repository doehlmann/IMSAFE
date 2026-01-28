# IMSAFE Requirements Document

## 1. Functional Requirements

### 1.1 Checklist System

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-CL-001 | System shall have 8 SPDT toggle switches | Must | Open |
| FR-CL-002 | System shall detect all switches in UP position (takeoff) | Must | Open |
| FR-CL-003 | System shall detect all switches in DOWN position (landing) | Must | Open |
| FR-CL-004 | System shall use I2C expander (MCP23017) for switch inputs | Should | Open |
| FR-CL-005 | LEDs shall indicate switch state (green = correct) | Must | Open |
| FR-CL-006 | LEDs shall flash red for alarm conditions | Must | Open |

### 1.2 Sensor System

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-SN-001 | System shall measure barometric altitude via BMP390 | Must | Open |
| FR-SN-002 | System shall provide attitude data via IMU | Must | Open |
| FR-SN-003 | System shall provide heading via compass | Must | Open |
| FR-SN-004 | System shall detect CO levels via MICS-4514 | Must | Open |
| FR-SN-005 | System shall detect NO2 levels via MICS-4514 | Should | Open |
| FR-SN-006 | System shall measure cabin temperature | Should | Open |

### 1.3 Display System

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-DP-001 | System shall drive 3.5" TFT via SPI | Must | Open |
| FR-DP-002 | Display shall show synthetic horizon | Must | Open |
| FR-DP-003 | Display shall show altitude | Must | Open |
| FR-DP-004 | Display shall show heading | Must | Open |
| FR-DP-005 | Display shall show flight timer | Should | Open |
| FR-DP-006 | Display shall show radar height during approach | Should | Open |

### 1.4 Landing Radar

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-RD-001 | Radar shall measure height 0.3m to 30m | Should | Open |
| FR-RD-002 | Radar shall use BGT24LTR11 24GHz module | Should | Open |
| FR-RD-003 | Radar data shall be processed by XMC4700 | Should | Open |

### 1.5 Data Logging

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-DL-001 | System shall log flight data to microSD | Should | Open |
| FR-DL-002 | Log shall include timestamp, position, sensors | Should | Open |
| FR-DL-003 | Log shall record incident/alarm events | Should | Open |

### 1.6 Connectivity

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-CN-001 | System shall support USB-C for data/charging | Must | Open |
| FR-CN-002 | System shall support Bluetooth connectivity | Could | Open |

### 1.7 Vibration Analysis

| ID | Requirement | Priority | Status |
|----|-------------|----------|--------|
| FR-VB-001 | System shall support 2 external IMUs | Could | Open |
| FR-VB-002 | IMUs shall attach to engine and gearbox | Could | Open |
| FR-VB-003 | System shall perform vibration FFT analysis | Could | Open |

---

## 2. Performance Requirements

| ID | Parameter | Min | Typ | Max | Unit |
|----|-----------|-----|-----|-----|------|
| PR-001 | Internal supply voltage | 3.0 | 3.3 | 3.6 | V |
| PR-002 | Battery voltage | 6.0 | 7.4 | 8.4 | V |
| PR-003 | Operating temperature | -20 | 25 | 70 | C |
| PR-004 | Storage temperature | -40 | - | 85 | C |
| PR-005 | Altitude accuracy | - | 1 | 3 | m |
| PR-006 | Heading accuracy | - | 2 | 5 | deg |
| PR-007 | Radar range min | - | - | 0.3 | m |
| PR-008 | Radar range max | 30 | - | - | m |
| PR-009 | Display update rate | 10 | 30 | - | Hz |
| PR-010 | Sensor sample rate | 10 | 50 | - | Hz |

---

## 3. Environmental Requirements

| ID | Parameter | Specification |
|----|-----------|---------------|
| ER-001 | Operating humidity | 10-90% RH non-condensing |
| ER-002 | Vibration | Aircraft cockpit environment |
| ER-003 | ESD protection | IEC 61000-4-2 Level 2 |
| ER-004 | EMC | FCC Part 15 Class B |
| ER-005 | RoHS | Compliant |

---

## 4. Traceability Matrix

| Requirement | Test ID | Verification Method |
|-------------|---------|---------------------|
| FR-CL-001 | TC-001 | Inspection |
| FR-CL-002 | TC-002 | Functional Test |
| FR-CL-003 | TC-003 | Functional Test |
| FR-SN-001 | TC-010 | Calibration Test |
| FR-DP-001 | TC-020 | Functional Test |
| FR-RD-001 | TC-030 | Range Test |
