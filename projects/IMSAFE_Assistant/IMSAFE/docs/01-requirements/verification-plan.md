# IMSAFE Verification Plan

## 1. Test Categories

### 1.1 Unit Tests
- Individual component function verification
- Performed during development

### 1.2 Integration Tests
- Subsystem interaction verification
- Performed after board assembly

### 1.3 System Tests
- Full system function verification
- Performed on assembled unit

### 1.4 Environmental Tests
- Temperature, vibration, EMC
- Performed on production-representative units

---

## 2. Test Procedures

### TC-001: Toggle Switch Physical Verification
**Requirement:** FR-CL-001
**Method:** Inspection
**Procedure:**
1. Verify 8 toggle switches installed
2. Verify SPDT operation (3 terminals each)
3. Verify mechanical feel acceptable

### TC-002: Takeoff Checklist Detection
**Requirement:** FR-CL-002
**Method:** Functional Test
**Procedure:**
1. Power on unit
2. Set all switches to DOWN position
3. Move each switch to UP position
4. Verify LED indication for each switch
5. Verify "takeoff complete" when all UP

### TC-003: Landing Checklist Detection
**Requirement:** FR-CL-003
**Method:** Functional Test
**Procedure:**
1. Power on unit
2. Set all switches to UP position
3. Move each switch to DOWN position
4. Verify LED indication for each switch
5. Verify "landing complete" when all DOWN

### TC-010: Altitude Sensor Calibration
**Requirement:** FR-SN-001
**Method:** Calibration Test
**Procedure:**
1. Connect reference pressure source
2. Apply known pressures corresponding to altitudes
3. Record indicated vs actual altitude
4. Verify within ±3m accuracy

### TC-020: Display Function Test
**Requirement:** FR-DP-001
**Method:** Functional Test
**Procedure:**
1. Power on unit
2. Verify display initializes
3. Verify all display modes accessible
4. Verify no dead pixels or artifacts

### TC-030: Radar Range Test
**Requirement:** FR-RD-001
**Method:** Range Test
**Procedure:**
1. Set up known targets at 0.5m, 1m, 5m, 10m, 20m, 30m
2. Record radar indicated distance
3. Verify accuracy at each range
4. Verify minimum detectable range

---

## 3. Test Equipment Required

| Equipment | Purpose | Calibration |
|-----------|---------|-------------|
| Pressure chamber | Altitude sensor | Annual |
| Precision power supply | Power testing | Annual |
| Digital multimeter | Current/voltage | Annual |
| Radar target reflector | Range testing | N/A |
| Temperature chamber | Environmental | Annual |

---

## 4. Pass/Fail Criteria

| Test | Pass Criteria |
|------|---------------|
| TC-001 | 8 switches present, correct type |
| TC-002 | Correct detection within 50ms |
| TC-003 | Correct detection within 50ms |
| TC-010 | Altitude within ±3m of reference |
| TC-020 | Display functional, no defects |
| TC-030 | Range within ±0.1m or ±5% |

---

## 5. Integration Tests

### IT-001: USB Enumeration
**Requirement:** FR-CN-001
**Method:** Functional Test
**Procedure:**
1. Connect USB-C cable to powered host
2. Verify device enumerates within 5 seconds
3. Verify correct USB device descriptor
4. Verify data transfer capability

### IT-002: Flight Data Logging
**Requirement:** FR-DL-001, FR-DL-002
**Method:** Functional Test
**Procedure:**
1. Insert formatted microSD card
2. Power on unit and enable logging
3. Generate sensor data for 60 seconds
4. Power off and remove SD card
5. Verify log file created with timestamp, sensor data

### IT-003: Low Battery Warning
**Requirement:** Power Management
**Method:** Functional Test
**Procedure:**
1. Simulate battery voltage at warning threshold (6.5V)
2. Verify warning indication on display
3. Simulate battery voltage at shutdown threshold (6.0V)
4. Verify graceful shutdown sequence

### IT-004: Charging Operation
**Requirement:** FR-CN-001, Power Management
**Method:** Functional Test
**Procedure:**
1. Connect USB-C with discharged battery
2. Verify charging indicator active
3. Verify system operates while charging
4. Verify charge termination at full

### IT-005: Factory Reset
**Requirement:** System
**Method:** Functional Test
**Procedure:**
1. Configure non-default settings
2. Initiate factory reset via menu
3. Verify all settings restored to defaults
4. Verify user data preserved (optional) or cleared

---

### IT-006: IMU / Attitude Display
**Requirement:** FR-SN-002
**Method:** Functional Test
**Procedure:**
1. Power on unit and enter synthetic horizon display mode
2. Place unit level - verify horizon indicates level (±2°)
3. Tilt unit 30° pitch up - verify display shows 30° pitch
4. Tilt unit 30° roll left - verify display shows 30° roll
5. Verify update rate is smooth (no visible lag)

### IT-007: Compass / Heading Display
**Requirement:** FR-SN-003
**Method:** Functional Test
**Procedure:**
1. Power on unit away from magnetic interference
2. Point unit North - verify heading reads 0° (±5°)
3. Rotate 90° clockwise - verify heading reads 90° (±5°)
4. Rotate full 360° - verify continuous heading tracking
5. Verify heading displayed on main display

### IT-008: Vibration Analysis
**Requirement:** FR-VB-001, FR-VB-002, FR-VB-003
**Method:** Functional Test
**Procedure:**
1. Connect external IMU to engine mount point
2. Connect external IMU to gearbox mount point
3. Power on unit and verify both IMUs detected
4. Apply known vibration source (calibrated shaker or running engine)
5. Verify FFT display shows frequency spectrum
6. Verify baseline comparison detects anomaly when introduced

---

### IT-009: CO / NO2 Gas Detection
**Requirement:** FR-SN-004, FR-SN-005
**Method:** Functional Test
**Procedure:**
1. Power on unit and enter gas monitor display mode
2. Verify MICS-4514 sensor heater warmup completes (~30s)
3. Verify baseline CO and NO2 readings displayed
4. Apply calibration gas (known CO concentration) if available
5. Verify CO alarm triggers at threshold (50ppm default)
6. Verify NO2 reading responds to ambient changes

### IT-010: Cabin Temperature
**Requirement:** FR-SN-006
**Method:** Functional Test
**Procedure:**
1. Power on unit
2. Verify temperature displayed on screen
3. Compare reading to reference thermometer (±2°C)
4. Apply heat source nearby - verify reading increases
5. Verify temperature logged in flight data

### IT-011: Rotary Encoders
**Requirement:** FR-UI-001, FR-UI-002, FR-UI-003, FR-UI-004
**Method:** Functional Test
**Procedure:**
1. Power on unit
2. Rotate left encoder CW - verify menu moves down/right
3. Rotate left encoder CCW - verify menu moves up/left
4. Press left encoder - verify selection confirmed
5. Rotate right encoder - verify value adjustment
6. Press right encoder - verify secondary action
7. Verify no missed steps or bouncing

### IT-012: RTC / Flight Timer
**Requirement:** FR-TK-001, FR-TK-002, FR-TK-003, FR-TK-004
**Method:** Functional Test
**Procedure:**
1. Set RTC to known time via settings menu
2. Power off unit for 1 hour
3. Power on - verify RTC maintained correct time (±2s drift)
4. Start flight timer - verify display shows 00:00
5. Wait 60 seconds - verify timer shows ~01:00
6. Stop/reset timer - verify reset to 00:00

### IT-013: LED Indicators
**Requirement:** FR-CL-005, FR-CL-006
**Method:** Functional Test
**Procedure:**
1. Power on unit with all switches in neutral
2. Move switch 1 to UP - verify LED turns green
3. Move all switches UP - verify all LEDs green
4. Trigger alarm condition (e.g., CO threshold)
5. Verify affected LED flashes red
6. Clear alarm - verify LED returns to normal
7. Test brightness adjustment (day/night modes)

### IT-014: Bluetooth Connectivity
**Requirement:** FR-CN-002, FR-CN-003, FR-CN-004, FR-CN-005, FR-CN-006, FR-CN-007
**Method:** Functional Test
**Procedure:**
1. Enable Bluetooth in settings menu
2. Scan for device from smartphone/tablet
3. Verify device advertises as BLE 5.0 compatible
4. Pair with device using secure bonding - verify pairing successful
5. Verify pairing persists after power cycle (bonding stored)
6. Request flight log download - verify data received correctly
7. Modify configuration via companion app - verify settings applied
8. Test range: move smartphone to 10m distance, verify connection stable
9. Test reconnection: disable/enable phone BT, verify auto-reconnect
10. Disconnect and verify graceful handling
11. Verify BLE does not interfere with other functions (display, sensors)

### IT-015: Bluetooth Data Throughput
**Requirement:** FR-CN-004
**Method:** Performance Test
**Procedure:**
1. Connect via BLE to smartphone
2. Initiate flight log download (1 MB test file)
3. Measure transfer time
4. Calculate throughput (target: >50 KB/s for practical log download)
5. Verify no data corruption (checksum validation)

---

### Integration Tests Pass/Fail Criteria

| Test | Pass Criteria |
|------|---------------|
| IT-001 | USB enumeration < 5s, data transfer OK |
| IT-002 | Log file created, data valid |
| IT-003 | Warning at 6.5V, shutdown at 6.0V |
| IT-004 | Charges and operates simultaneously |
| IT-005 | Settings reset, predictable behavior |
| IT-006 | Attitude within ±2° level, ±5° at 30° tilt |
| IT-007 | Heading within ±5° of reference |
| IT-008 | Both IMUs detected, FFT spectrum visible |
| IT-009 | CO alarm at threshold, NO2 responsive |
| IT-010 | Temperature within ±2°C of reference |
| IT-011 | Encoders responsive, no missed steps |
| IT-012 | RTC drift <2s/hr, timer accurate |
| IT-013 | LEDs indicate state, alarm flashes red |
| IT-014 | BT pairs, data transfers, 10m range, secure bonding |
| IT-015 | Log download >50 KB/s, no corruption |

---
