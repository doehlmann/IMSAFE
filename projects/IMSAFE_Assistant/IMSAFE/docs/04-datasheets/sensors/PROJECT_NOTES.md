# Aircraft Landing Radar Project

## Overview

Development of a radar-based landing aid for light sport aircraft using the Infineon Distance2Go L (D2GL) 24GHz FMCW radar module. The system provides accurate height-above-ground data to assist with landing flare timing.

**Note: This is a pilot aid, not a safety-critical system.**

## Project Goals

1. **Immediate:** Collect radar data during approaches using Raspberry Pi logging
2. **Short-term:** Develop flare indication algorithm with LED feedback
3. **Long-term:** Integrated avionics box with:
   - Radar altimeter (D2GL)
   - 9-axis IMU
   - Compass
   - BMP380 barometric sensor
   - LED landing light array (lights converge to center at flare point)
   - Standalone operation on XMC4700 (no Pi required)

---

## Hardware

### Infineon Distance2Go L (D2GL)

| Component | Details |
|-----------|---------|
| Radar IC | BGT24LTR11 |
| Frequency | 24.025 GHz |
| Bandwidth | 200 MHz |
| Modulation | FMCW (Software Controlled) |
| MCU | XMC4700 (ARM Cortex-M4F @ 144MHz) |
| Antennas | 1 TX, 2 RX |
| Interface | USB CDC (Virtual Serial Port) |
| Firmware | v1.0.0 |

### Current Test Setup

- D2GL board connected via USB
- Raspberry Pi (doehlmann@192.168.10.102)
- Serial port: `/dev/ttyACM1`

### Planned Sensor Additions

| Sensor | Interface | Purpose |
|--------|-----------|---------|
| 9-axis IMU (e.g., ICM-20948) | I2C/SPI | Attitude reference |
| Compass (or integrated in IMU) | I2C | Heading |
| BMP380 | I2C | Barometric altitude, descent rate backup |
| LED Array | PWM/WS2812 | Visual flare indicator |

---

## Communication Protocol

### Message Format

All communication over USB CDC virtual serial port. Little-endian byte order.

#### Payload Message (bidirectional)
```
Byte 0:     0x5A (start byte)
Byte 1:     Endpoint number
Bytes 2-3:  Payload length (16-bit LE)
Bytes 4-N:  Payload data
Last 2:     0xDB 0xE0 (end marker)
```

#### Status Message (device to host only)
```
Byte 0:     0x5B (start byte)
Byte 1:     Endpoint number
Bytes 2-3:  Status code (0x0000 = OK)
```

### Endpoint 0 Commands (Protocol Control)

| Code | Command |
|------|---------|
| 0x00 | Query endpoint info |
| 0x01 | Query firmware info |
| 0x02 | Firmware reset |

### D2GL Endpoint Commands (Type 0x544A5346 "TJSF")

| Code | Name | Direction | Description |
|------|------|-----------|-------------|
| 0x50 | PARA_VALUES_REQ | Host→Dev | Request current parameters |
| 0x51 | PARA_VALUES_CNF | Dev→Host | Parameter values response |
| 0x52 | PARA_VALUES_DEF_REQ | Host→Dev | Request parameter limits |
| 0x53 | PARA_VALUES_DEF_CNF | Dev→Host | Parameter limits response |
| 0x54 | PARA_SET_VALUES_REQ | Host→Dev | Set parameters |
| 0x55 | RESULT_REQ | Host→Dev | Request distance result |
| 0x56 | RESULT_CNF | Dev→Host | Distance result |
| 0x57 | GET_SHIELD_INFO_REQ | Host→Dev | Request board info |
| 0x58 | GET_SHIELD_INFO_CNF | Dev→Host | Board info response |
| 0x59 | GET_RAW_DATA_REQ | Host→Dev | Request raw I/Q data |
| 0x6A | GET_RAW_DATA_CNF | Dev→Host | Raw I/Q data response |
| 0x61 | ADV_RESULT_REQ | Host→Dev | Request advanced result |
| 0x62 | ADV_RESULT_CNF | Dev→Host | Advanced result response |

### Result Message Structure (0x56)

```
Offset  Type      Field
0       uint8     Command (0x56)
1       uint32    result_count
5       uint32    frame_number
9       float32   range_m
13      float32   strength
17      float32   velocity_mps
21      uint8     target_status (0=none, 1=static, 2=transit, 3=moving)
22      uint8     direction_of_movement
```

### Parameter Structure (0x54 Set / 0x51 Get Response)

```
Offset  Type      Field                       Default
0       uint8     Command
1       float32   max_distance_m              16.0
5       float32   min_distance_m              1.0
9       float32   frame_time_sec              0.1 (10Hz)
13      uint16    number_of_samples           64
15      uint32    hold_threshold              75
19      uint32    activation_threshold        150
23      uint8     chirps_per_frame            10
24      uint8     continuous_mode             0
25      uint8     baseband_gain_stage         1
26      float32   pulse_repetition_rate_sec   0.0
30      uint8     activate_cfar               0
```

---

## Software

### Python Script: d2gl_radar.py

Location: `G:\my drive\electronics\landing height\infineon\d2gl_radar.py`

Deployed to Pi: `~/d2gl_radar.py`

#### Usage

```bash
# Basic test (100 samples)
python d2gl_radar.py /dev/ttyACM1

# Extended logging to CSV
python d2gl_radar.py /dev/ttyACM1 -n 1000 -o flight_data.csv

# Verbose debug mode
python d2gl_radar.py /dev/ttyACM1 -v
```

#### As a Library

```python
from d2gl_radar import D2GLRadar, RadarParameters

radar = D2GLRadar('/dev/ttyACM1')
radar.connect()

# Single measurement
result = radar.get_result()
print(f"Height: {result.range_m}m, Descent: {result.velocity_mps}m/s")

# Modify parameters
params = radar.get_parameters()
params.frame_time_sec = 0.02      # 50Hz
params.activation_threshold = 50  # More sensitive
params.min_distance_m = 0.5
params.max_distance_m = 10.0
radar.set_parameters(params)

# Continuous streaming
def callback(r):
    print(f"{r.range_m:.2f}m")
    return True  # False to stop

radar.stream_results(callback, max_samples=500)
radar.disconnect()
```

---

## Test Results

### Initial Test (2026-01-26)

```
Radar Parameters:
  Distance range: 1.0m - 16.0m
  Frame time: 0.1s (10 Hz)
  Samples/chirp: 64
  Chirps/frame: 10
  Thresholds: activation=150, hold=75

Results:
  - Successfully detected moving target at ~3.7m
  - Velocity tracking functional (-2.5 to +0.7 m/s observed)
  - Signal strength: 200-5400 when target present
  - Status transitions: none → moving → transit → moving
```

### Recommended Parameter Adjustments for Ground Detection

```python
params.activation_threshold = 30   # Lower for static/slow ground return
params.hold_threshold = 15
params.frame_time_sec = 0.02       # 50Hz for better flare timing
params.min_distance_m = 0.5        # Detect closer to ground
params.max_distance_m = 10.0       # Don't need full 16m range
```

---

## XMC4700 Headroom Analysis

| Resource | Total | Radar Firmware | Available |
|----------|-------|----------------|-----------|
| CPU | 144MHz M4F | ~30-40% | 60-70% |
| Flash | 2MB | ~200KB | ~1.8MB |
| RAM | 352KB | ~50KB (FFT) | ~300KB |
| I2C | 2 buses | 0 | 2 |
| SPI | 3 buses | 1 (BGT24LTR11) | 2 |
| UART | 3 | 1 (USB CDC) | 2 |
| PWM | 8+ (CCU4/CCU8) | Chirp timing | 6+ |

**Conclusion:** Sufficient resources for IMU, compass, BMP380, and LED control.

---

## Development Roadmap

### Phase 1: Data Collection (Current)
- [x] Reverse engineer D2GL protocol
- [x] Create Python communication library
- [x] Basic Pi logging functional
- [ ] Mount radar in aircraft
- [ ] Collect approach/landing data
- [ ] Characterize ground return signature
- [ ] Determine optimal parameters

### Phase 2: Algorithm Development (Pi Prototype)
- [ ] Add BMP380 support to Pi
- [ ] Add IMU support to Pi
- [ ] Develop sensor fusion algorithm
- [ ] Develop flare indication logic
- [ ] Prototype LED indicator on Pi GPIO
- [ ] Flight test prototype

### Phase 3: PCB Design (KiCad)
- [ ] Schematic: XMC4700 + BGT24LTR11 + sensors
- [ ] Component selection and sourcing
- [ ] PCB layout (antenna considerations critical)
- [ ] Power supply design (aircraft 12-14V input)
- [ ] LED driver circuit
- [ ] Enclosure design

### Phase 4: Firmware Development
- [ ] Study Infineon DAVE project structure
- [ ] Add I2C sensor drivers
- [ ] Port sensor fusion from Python to C
- [ ] Implement LED control
- [ ] Implement flight logic
- [ ] Remove Pi dependency

### Phase 5: Integration & Testing
- [ ] Ground testing (vehicle-mounted)
- [ ] Flight testing
- [ ] Calibration procedures
- [ ] Documentation

---

## File Locations

### This Project
```
G:\my drive\electronics\landing height\infineon\
├── PROJECT_NOTES.md                    (this file)
├── d2gl_radar.py                       (Python library/CLI)
├── Distance2GoL_record_*               (test recordings)
└── infineon-bgt24ltr11-demo-*/         (Infineon SDK packages)
    └── Firmware_Software/
        ├── DAVE project/               (XMC4700 firmware source)
        └── Communication Library/
            ├── ComLib_C_Interface/     (C protocol library)
            └── ComLib_Matlab_Interface/ (MATLAB examples)
```

### Raspberry Pi
```
doehlmann@192.168.10.102:~/
└── d2gl_radar.py
```

---

## Reference Documents

Located in project folder:
- BGT24LTR11 Datasheet
- Demo Distance2GoL Application Notes
- Radar Baseboard MCU4 User Manual (UG100163)
- DAVE IDE project files

---

## Notes

### 24GHz and Fiberglass
24GHz penetrates fiberglass well. Mounting inside aircraft belly fairing should work. Avoid metal obstructions in beam path.

### Update Rate Considerations
At 60kt approach speed: ~31m/s = 3.1m per 100ms (10Hz)
Recommend 50Hz (20ms) for 0.6m resolution during flare.

### Safety
This is a pilot aid only. Not certified avionics. Pilot remains responsible for all landing decisions.

---

## Next Session

- KiCad MCP integration for PCB development
- Schematic capture for integrated board
- Component selection

---

*Last updated: 2026-01-26*
