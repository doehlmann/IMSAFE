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
