# Functional Specification
## Electronic Gadget: Smart Bike Rear/Helmet Light (XIAO nRF52840 Sense Plus + WS2812)

| Document Info | |
|---------------|---|
| Project Code | GAD-BIKELIGHT |
| Revision | A |
| Date | 2026-01-12 |
| Author | User + ChatGPT |
| Status | Draft |

---

## 1. Executive Summary

### 1.1 Product Description
A bicycle rear light (initially) / helmet-mounted light (future) built around a Seeed Studio XIAO nRF52840 Sense Plus driving a 24‑LED WS2812 (NeoPixel) strip. The system provides high-visibility turn signaling, automatic brake indication using the onboard 6‑axis IMU, and night running light modes controlled by a handlebar “mode” button and an ambient light sensor. The MVP is a wired system consisting of the main light unit plus a handlebar switch pod.

### 1.2 Key Features
- Turn signaling: Orange center-out cascade left/right, 8 s duration, opposite-button cancel logic.
- Brake indication: IMU-based “deceleration / dynamics” detection triggers full-bright red brake display with dwell + hold.
- Night running light: Red steady or red flashing; quick-tap UI on mode button; optional ambient light AUTO enable/disable.
- Inactivity idle mode: After configurable inactivity, LEDs off and polling reduced (≈99% reduction); wakes on movement or button.
- Modular hardware: Separate 5 V LED power board (1S + USB-C OR-ing) and separate handlebar switch board with indicator LEDs.

### 1.3 Target Market / Application
Cyclists seeking a compact, conspicuous rear/helmet light with turn signals and brake sensing. MVP use is a rear light with wired handlebar controls; later revisions may migrate to helmet mounting and/or wireless controls.

### 1.4 Project Goals
| Goal | Metric | Target |
|------|--------|--------|
| Development time | MVP firmware + wired prototype | Rapid iteration |
| Power consumption | Idle polling rate | ~2 s tick (≈99% reduction) |
| Visibility | Turn + brake patterns | High conspicuity, clear priority behavior |
| Modularity | PCB partition | 2 auxiliary PCBs (5 V power + handlebar pod) |

---

## 2. Requirements

### 2.1 Functional Requirements

| ID | Requirement | Priority | Verification |
|----|-------------|----------|--------------|
| FR-001 | Device shall drive a 24‑LED WS2812 strip with correct timing | Must | Test |
| FR-002 | Device shall provide LEFT turn indication: orange center→left cascade | Must | Demo |
| FR-003 | Device shall provide RIGHT turn indication: orange center→right cascade | Must | Demo |
| FR-004 | Turn indication shall auto-timeout after 8 s and return to idle-ready state | Must | Test |
| FR-005 | Opposite turn button press shall cancel current turn; second press starts new turn | Must | Test |
| FR-006 | Turn mode shall optionally add “funky” opposite-side sparkle without affecting main direction | Should | Demo |
| FR-007 | Device shall provide brake indication using IMU dynamics with full-bright red output | Must | Test |
| FR-008 | Brake logic shall include minimum dwell time above threshold to reject bumps | Must | Test |
| FR-009 | Brake indication shall latch for configurable hold time | Must | Test |
| FR-010 | Device shall provide night running light steady red | Must | Test |
| FR-011 | Device shall provide night running light flashing red | Must | Test |
| FR-012 | Mode button shall support 2-tap / 3-tap UI: 2 taps toggles steady on/off; 3 taps forces flash on | Must | Test |
| FR-013 | 2 taps shall turn OFF when in steady or flash; if OFF, 2 taps shall turn ON steady | Must | Test |
| FR-014 | 3 taps shall turn ON flash even if system is OFF (active state) | Must | Test |
| FR-015 | Ambient light sensor shall support AUTO night enable/disable with hysteresis + dwell | Should | Test |
| FR-016 | Manual tap actions shall not permanently disable AUTO behavior (AUTO must remain available) | Must | Test |
| FR-017 | System shall implement display priority: TURN > BRAKE > NIGHT | Must | Test |
| FR-018 | System shall enter idle mode after configurable inactivity (default 5 min) | Must | Test |
| FR-019 | In idle mode, LEDs shall be off and polling shall reduce to ~2 s tick | Must | Test |
| FR-020 | In idle mode, ambient light sensor shall not wake/enable lights | Must | Test |
| FR-021 | System shall wake from idle via sustained movement (≈5 s) or mode button hold | Must | Test |
| FR-022 | System shall include a 5 V LED power board accepting 1S battery and USB-C input with OR-ing | Must | Review/Test |
| FR-023 | System shall include a handlebar switch board with 3 momentary switches: Left/Right/Mode | Must | Test |
| FR-024 | Handlebar board shall include 2 indicator LEDs that flash during left/right turn operation | Should | Demo |

**Priority Key:**
- Must: Required for MVP
- Should: Important but not blocking
- Could: Nice to have

### 2.2 Performance Requirements

| Parameter | Min | Typical | Max | Unit | Notes |
|-----------|-----|---------|-----|------|-------|
| Battery Input (1S) | 3.0 | 3.7 | 4.2 | V | Li‑ion/LiPo |
| USB Input | 4.75 | 5.0 | 5.25 | V | USB‑C VBUS |
| LED Rail Output | 4.8 | 5.0 | 5.2 | V | WS2812 supply |
| LED Rail Current | - | - | 60 | mA | MVP target (power board) |
| WS2812 Count | - | 24 | - | LED | Straight line strip |
| Turn Duration | - | 8 | - | s | Auto-timeout |
| IMU Sampling (active) | - | 100 | - | Hz | Firmware-configurable |
| Light Sensor Sampling (active) | - | 20 | - | Hz | Firmware-configurable |
| Idle Tick | - | 2 | - | s | Reduced processing rate |
| Idle Entry Time | 10 | 300 | 3600 | s | Default 300 s (5 min), adjustable |
| Brake Detect Dwell | 10 | 250 | 2000 | ms | Reject bumps |
| Brake Hold Time | 50 | 800 | 10000 | ms | Latch/visibility |
| Serial Diagnostics Rate | - | 1 | - | Hz | Human-readable debug |
| Operating Temp | -10 | 25 | 50 | °C | Prototype target |

### 2.3 Physical Requirements

| Parameter | Specification | Notes |
|-----------|---------------|-------|
| Main Controller | Seeed Studio XIAO nRF52840 Sense Plus | nRF52840 + IMU onboard |
| LED Strip | 24× WS2812 | Rear/helmet lighting |
| Auxiliary PCB #1 | 1S+USB‑C → 5 V LED rail | OR-ing/diodes; decoupling |
| Auxiliary PCB #2 | Handlebar switch pod | 3 momentary switches + 2 indicator LEDs |
| Wiring | Multi-core cable harness | MVP is wired back to main |
| Mounting | Rear light first; helmet later | Mechanical TBD |
| Connector(s) | TBD (JST / headers) | Defined during PCB layout |

### 2.4 Environmental Requirements

| Parameter | Specification | Standard |
|-----------|---------------|----------|
| Operating Humidity | 10-90% RH non-condensing | Practical target |
| Vibration | Bicycle vibration tolerant | Practical test |
| Outdoor Use | Splash resistant (future enclosure) | Target for next phase |

---

## 3. System Architecture

### 3.1 Block Diagram

```
                 ┌──────────────────────────────────────────────────┐
                 │                 AUX PCB #1                        │
                 │   1S LiPo (3.0–4.2V) + USB-C 5V OR-ing/diodes     │
                 │          + Boost/Regulator to 5V @ ~60mA          │
                 └───────────────┬──────────────────────────────────┘
                                 │ 5V_LED_RAIL
                                 ▼
                        ┌──────────────────┐
                        │ 24× WS2812 Strip │
                        │ (DATA from MCU)  │
                        └───────┬──────────┘
                                │ DATA
                                ▼
┌──────────────────────────────────────────────────────────────────────────┐
│                 MAIN LIGHT UNIT (XIAO nRF52840 Sense Plus)               │
│                                                                          │
│   ┌──────────────┐   ┌───────────────┐   ┌──────────────────────────┐   │
│   │ GPIO Inputs  │◄──┤ AUX PCB #2    │   │ Ambient Light Sensor     │   │
│   │ Left/Right/  │   │ Handlebar Pod │   │ (OPT101 analog)          │   │
│   │ Mode Buttons │   │ + 2 LEDs      │   │ → ADC (D0 used as AIN)   │   │
│   └──────┬───────┘   └───────────────┘   └───────────────┬──────────┘   │
│          │                                                   │           │
│          │                                                   ▼           │
│   ┌──────▼───────────────────────────────────────────────────────────┐  │
│   │ nRF52840 MCU + IMU (LSM6DS3TR-C via I2C on Sense Plus)           │  │
│   │ - IMU filtering + brake detection + riding detection             │  │
│   │ - LED pattern rendering with priority                            │  │
│   │ - USB serial diagnostics, optional BLE UART status               │  │
│   └──────────────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────────────────┘
```

### 3.2 MCU Selection

**Selected MCU:** Nordic nRF52840 (on Seeed Studio XIAO nRF52840 Sense Plus)

| Parameter | Requirement | nRF52840 (XIAO) | Status |
|-----------|-------------|------------------|--------|
| BLE | Optional tuning/telemetry | BLE 5.x | ✓ |
| IMU | 6-axis motion | LSM6DS3TR-C onboard | ✓ |
| GPIO | ≥ 4 | Multiple | ✓ |
| ADC | ≥ 1 channel | Available | ✓ |
| USB | USB device (dev/debug) | Native USB | ✓ |
| Performance | Smooth WS2812 patterns + filtering | Sufficient | ✓ |

**Alternatives Considered:**
| Part | Reason Not Selected |
|------|---------------------|
| ESP32 | Higher power, different toolchain, not needed for MVP |
| STM32 family | Would require separate BLE and sensor integration for this prototype |

### 3.3 Power Architecture

```
Input Sources:
─────────────
USB-C (5V) ──┬──────────────┐
             │              │
1S LiPo ─────┴──► OR-ing (diodes/ideal diode) ─► Boost/Reg ─► 5V LED Rail (~60mA)
                                           │
                                           └────────► XIAO power (dev via USB-C)

Power Rails:
────────────
• VUSB: 5.0V USB input (development powering)
• VBAT: 3.0–4.2V battery input (1S)
• VLED: 5.0V WS2812 rail (from AUX PCB #1)
• VDD_3V3: XIAO internal 3.3V domain (on-board regulation)
```

### 3.4 Pin Allocation

| Pin | Function | Direction | Notes |
|-----|----------|-----------|-------|
| D6 | WS2812_DATA | Output | Data to strip |
| D1 | MODE_BUTTON | Input | Pull-up, active low |
| D2 | LEFT_BUTTON | Input | Pull-up, active low |
| D3 | RIGHT_BUTTON | Input | Pull-up, active low |
| D0 (A0) | LIGHT_ADC | Input | OPT101 analog output |
| (TBD) | LEFT_IND_LED | Output | Handlebar LED drive |
| (TBD) | RIGHT_IND_LED | Output | Handlebar LED drive |

---

## 4. Detailed Design

### 4.1 Sensor Interface

**Sensor 1: IMU (LSM6DS3TR-C)**
| Parameter | Value |
|-----------|-------|
| Interface | I2C (on-board) |
| Measurement | 3-axis accel + 3-axis gyro |
| Use | Riding detection, brake detection |
| Processing | Low-pass accel magnitude, “dynamic accel” magnitude, gyro magnitude |

**Sensor 2: Ambient Light (OPT101 analog)**
| Parameter | Value |
|-----------|-------|
| Interface | Analog output to ADC |
| Use | AUTO night enable/disable (hysteresis + dwell) |
| Notes | In idle mode, sensor does not wake system |

### 4.2 User Interface

**Handlebar Controls (AUX PCB #2):**
| Control | Type | Function |
|--------|------|----------|
| Left Button | Momentary | Start left turn; cancel right then second press starts left |
| Right Button | Momentary | Start right turn; cancel left then second press starts right |
| Mode Button | Momentary | Night mode UI + idle/power interactions |

**Turn Indicator LEDs (AUX PCB #2):**
| LED | Behavior |
|-----|----------|
| Left Indicator | Flashes while left turn is active |
| Right Indicator | Flashes while right turn is active |

**Mode Button Gestures (current firmware behavior):**
| Action | Timing | Function |
|--------|--------|----------|
| 2 taps | within ~0.9 s window | If night ON (steady/flash) → OFF; if OFF → steady ON |
| 3 taps | within ~0.9 s window | Flash ON (even if off, in active state) |
| Hold | ~2–5 s | Visual countdown then enter idle/off at ~5 s |
| Hold in idle | ~2 s | Wake from idle |

**LED Patterns (main strip):**
| State | Pattern |
|-------|---------|
| Turn Left | Orange centre-out cascade to left, optional opposite sparkle |
| Turn Right | Orange centre-out cascade to right, optional opposite sparkle |
| Brake | Full strip red at full brightness, latched |
| Night Steady | Full strip red at ~50% brightness |
| Night Flash | Full strip red flashing |
| Idle/Off | LEDs off |

### 4.3 Data Storage
**MVP (current):**
- No non-volatile data logging required for MVP.
- Serial diagnostics at 1 Hz provides live values for tuning.

**Planned (future):**
- RAM ring buffer logging + serial/BLE dump.
- Optional external flash/SD in later PCB revision if long captures are needed.

### 4.4 Communication Protocol

**USB CDC (Serial):**
| Baud Rate | 115200 |
|-----------|--------|
| Use | Diagnostics, tuning, development |

**Optional BLE UART (development):**
| Use | Status/telemetry; future parameter tuning |
| Notes | BLE implementation is optional; may be enabled once the board/library stack is finalized |

---

## 5. Firmware Architecture

### 5.1 Module Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                         │
├─────────────────┬─────────────────┬─────────────────────────────┤
│ app_main        │ imu_processing  │ comms                        │
│ - main loop     │ - read accel    │ - USB serial (1 Hz)          │
│ - state/priority│ - read gyro     │ - optional BLE UART          │
│ - idle manager  │ - filters       │                              │
├─────────────────┼─────────────────┼─────────────────────────────┤
│ buttons         │ light_sensor    │ led_renderer                 │
│ - debounce      │ - ADC filter    │ - TURN > BRAKE > NIGHT        │
│ - 2/3 tap UI    │ - hyst+dwell    │ - WS2812 patterns             │
│ - hold idle/off │ - auto night    │ - countdown bars              │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 State Machine

```
                 ┌─────────────┐
                 │  POWER_ON   │
                 │ (init hw)   │
                 └─────┬───────┘
                       │
                       ▼
                 ┌─────────────┐
                 │   ACTIVE    │  LED priority: TURN > BRAKE > NIGHT
                 └─────┬───────┘
                       │ inactivity timeout (default 5 min)
                       ▼
                 ┌─────────────┐
                 │  IDLE/OFF   │  LEDs off, polling reduced (~2 s)
                 └─────┬───────┘
                       │ wake: movement sustain (~5 s) OR mode hold (~2 s)
                       ▼
                 ┌─────────────┐
                 │   ACTIVE    │
                 └─────────────┘
```

### 5.3 Task Scheduling (Super Loop)

```c
while (1) {
  // A) Process button debounce + gesture recognition (tap/hold)
  // B) Read IMU at configured period (fast in active, slow in idle)
  // C) Read ambient light ADC at configured period (fast in active, slow in idle)
  // D) Update riding/brake state machines
  // E) Resolve LED output by priority (TURN > BRAKE > NIGHT)
  // F) Update strip output
  // G) Emit serial diagnostics at 1 Hz; optional BLE telemetry
  // H) Evaluate inactivity for idle entry; evaluate wake conditions for exit
}
```

### 5.4 Interrupt Priority
MVP firmware uses a cooperative loop with timing based on `millis()`. No custom IRQ priority plan is required for the MVP beyond what the platform uses for USB/BLE internally.

### 5.5 Memory Budget
Budgeting is prototype-stage; expected to fit within XIAO nRF52840 resources (flash/RAM). If extended logging is added, memory use will be revisited.

---

## 6. Power Management

### 6.1 Power Modes

| Mode | Behavior | Wake Sources | Entry |
|------|----------|--------------|-------|
| Active | Normal sampling + LED patterns | N/A | Default |
| Idle/Off | LEDs off; polling reduced (~2 s); no auto-light wake | Movement sustain (~5 s) or mode hold (~2 s) | Inactivity timeout or user hold |

### 6.2 Power Budget

**MVP constraints:**
- LED rail limited to ~60 mA (AUX PCB #1 target).
- Actual WS2812 current depends strongly on brightness, color, and number of LEDs lit.

**Worst-case (conceptual) contributors:**
| Component | Notes |
|-----------|------|
| XIAO nRF52840 | Varies with CPU/BLE activity |
| IMU | Continuous in active; sampled slower in idle |
| WS2812 rail | Dominant load during bright patterns |
| Handlebar LEDs | Small compared with WS2812 |

### 6.3 Battery Life Estimation
Battery life depends strongly on LED duty cycle and brightness. Estimation will be performed once the LED rail current is measured in each mode (turn/brake/night/idle) and battery capacity is finalized.

---

## 7. Testing

### 7.1 Unit Tests
| Module | Test | Method |
|--------|------|--------|
| Buttons | Debounce + 2/3 tap + hold | Bench test |
| IMU | Read + filtering + thresholds | Bench + ride test |
| Light sensor | ADC range + thresholds + dwell | Bench test |
| LED renderer | Pattern correctness | Visual inspection |

### 7.2 Integration Tests

| Test ID | Description | Pass Criteria |
|---------|-------------|---------------|
| IT-001 | WS2812 stable operation | No flicker/gremlins under normal wiring/decoupling |
| IT-002 | Left turn function | Correct pattern, cancel logic, 8 s timeout |
| IT-003 | Right turn function | Correct pattern, cancel logic, 8 s timeout |
| IT-004 | Priority behavior | Turn overrides brake/night; brake overrides night |
| IT-005 | Brake detection robustness | No false brake on bumps; brake triggers on real slowing |
| IT-006 | Night UI | 2 taps steady toggle; 3 taps flash on; 2 taps off from either |
| IT-007 | AUTO night | Dark enables and bright disables with hysteresis + dwell |
| IT-008 | Manual vs AUTO interaction | Manual does not permanently break AUTO availability |
| IT-009 | Idle entry/exit | Enters after timeout; wakes on movement sustain or mode hold |
| IT-010 | Handlebar indicator LEDs | Blink correctly during turn state |

### 7.3 Environmental Tests
Prototype-level outdoor checks: vibration exposure on bike, splash exposure (no enclosure spec yet), temperature sanity.

### 7.4 Production Tests (Automated)
Not defined for MVP.

---

## 8. Manufacturing

### 8.1 Bill of Materials (Key Components)

| Ref | Qty | Part Number | Description | Supplier | Unit Cost |
|-----|-----|-------------|-------------|----------|-----------|
| U1 | 1 | XIAO nRF52840 Sense Plus | Main controller + IMU | Seeed | TBD |
| LED1 | 1 | WS2812 strip (24) | Addressable LED strip | TBD | TBD |
| PCB-A | 1 | AUX PCB #1 | 1S/USB-C → 5V LED rail | Custom | TBD |
| PCB-B | 1 | AUX PCB #2 | Handlebar pod: 3 switches + 2 LEDs | Custom | TBD |
| LS1 | 1 | OPT101 | Analog light sensor | TI / TBD | TBD |
| D* | 2+ | Diodes | Power OR-ing | TBD | TBD |
| C* | 1+ | Bulk electrolytic | LED rail decoupling | TBD | TBD |

### 8.2 PCB Specifications
MVP auxiliary boards are expected to be 2‑layer boards. Final thickness, copper weight, and connector footprints to be defined during schematic/layout.

### 8.3 Assembly Notes
- Provide adequate bulk capacitance on WS2812 5 V rail.
- Maintain robust mechanical strain relief on WS2812 data/power wiring.
- Handlebar pod should be mechanically rugged and weather tolerant.

---

## 9. Regulatory Compliance
Not targeted for MVP prototype. If commercialized, typical requirements include EMC/ESD and product safety considerations for battery-powered outdoor electronics.

---

## 10. Documentation Deliverables

| Document | Format | Status |
|----------|--------|--------|
| Functional Specification | Markdown | This document |
| Firmware Source | Arduino sketch | In progress |
| Wiring Diagram | PDF/MD | Pending |
| AUX PCB #1 Schematic + Layout | KiCad/PDF | Pending |
| AUX PCB #2 Schematic + Layout | KiCad/PDF | Pending |
| Test Notes / Tuning Guide | MD | Pending |

---

## Revision History

| Rev | Date | Author | Description |
|-----|------|--------|-------------|
| A | 2026-01-12 | User + ChatGPT | Initial draft aligned to template |
