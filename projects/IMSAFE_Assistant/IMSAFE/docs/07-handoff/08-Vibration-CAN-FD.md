# Remote Vibration Sensor Pods (ADXL355 + ADXL372) — Schematic & Connection Strategy

**Status:** ARCHITECTURE SELECTED
**Date:** 2026-01-30
**Cable length target:** ~2.5 m (sensor pod on gearbox + sensor pod on engine)

---

## Selected Architecture: Smart Pod with CAN-FD

**Decision:** Option 2 (Smart Pod + CAN-FD) is selected based on:
- STM32H747IIT6 has **FDCAN1 available** on pins **PD0 (RX) / PD1 (TX)**
- 2.5 m cable in engine bay environment requires robust differential signaling
- CAN-FD provides deterministic timing, error handling, and multi-drop capability

This document retains Option 1 (Direct SPI) for reference/prototyping, but **Option 2 is the production design**.

---

## Architecture Options Summary

| Option | Interface | Complexity | EMC Robustness | Recommendation |
|--------|-----------|------------|----------------|----------------|
| 1 | Direct SPI over 2.5 m | Low | Poor | Prototyping only |
| **2** | **Smart Pod + CAN-FD** | **Medium** | **Excellent** | **SELECTED** |

---

## A) Architecture Option 1 — Direct SPI over 2.5 m cable (simple, validate carefully)
### When this is acceptable
- Development / prototyping where you can tolerate rework.
- You can run **slow SPI** (start 100–500 kHz) and validate with scope margins.
- Cable is **shielded**, has a dedicated ground reference, and routing is controlled.

### Pod schematic blocks (both ADXL355 and ADXL372)
**At the sensor pod PCB:**
- Connector → ESD protection (optional but recommended for field) → series damping resistors (driver side) → sensor pins
- Local power filtering + LDO (if needed) + decoupling caps per datasheet

### Recommended “minimum viable” SPI signals per pod
- SCLK
- MOSI
- MISO
- CS (one CS per sensor pod)
- GND (dedicated return)
- +V power (5 V or 3.3 V, see below)
Optional but recommended:
- DRDY/INT line to MCU (for timestamping + clean sampling)

### Power distribution suggestions
- Run **+5 V** and **GND** to the pod, then regulate locally:
  - ADXL372: LDO to **3.3 V** (or 2.5 V / 1.8 V) for VS + VDDIO
  - ADXL355: can run **VSUPPLY = 3.3 V** and **VDDIO = 3.3 V** (or 1.8 V), with local decoupling
- Add input TVS + bulk cap at pod (e.g., 4.7–10 µF) + 0.1 µF ceramics at regulators/sensor pins.

### Signal integrity “musts” at 2.5 m
- Use **shielded twisted pairs** where possible:
  - Pair 1: SCLK + GND
  - Pair 2: MOSI + GND
  - Pair 3: MISO + GND
  - Pair 4: CS + GND (or DRDY/INT + GND)
- Add **series resistors** at the MCU end (start 22–47 Ω) for SCLK/MOSI/CS.
- Keep **MISO** clean: series resistor only if needed; don’t over‑slow edges excessively.
- Tie shield to chassis/ground at **one end** (typically main board end) to avoid ground loops.

### Recommended connector (Option 1)
- **M12 A‑coded 8‑pin** (robust, common, IP‑rated) or **M8 8‑pin** if space constrained.
- 8 conductors gives you: power pair + 4 SPI lines + DRDY + spare.
  - If you use M8 4‑pin you will be forced to drop DRDY and/or share grounds; not recommended at 2.5 m.

### Example pinout for each pod (M12 A‑coded 8‑pin)
| Pin | Signal |
|---:|---|
| 1 | +5V_IN |
| 2 | GND |
| 3 | SCLK |
| 4 | MOSI |
| 5 | MISO |
| 6 | CS |
| 7 | DRDY (or INT1) |
| 8 | Shield/drain or spare GPIO |

---

## B) Architecture Option 2 — “Smart Pod” (recommended for 2.5 m): Local MCU + robust link
Direct SPI over 2.5 m can work, but it’s **borderline** in noisy environments (motor/gearbox), and you typically end up adding a lot of signal‑integrity band‑aids.
A more production‑robust approach is to:
- put a small MCU in each pod (or one MCU handling both sensors if colocated),
- read the accelerometer locally over short SPI,
- stream data back over **CAN‑FD** (preferred) or **RS‑485**.

### Why this is better
- Short, clean SPI traces on the pod → full sensor performance.
- Only **one differential pair** (CANH/CANL or RS‑485 A/B) over the long cable.
- Much better EMC / noise immunity.
- Cable/connector becomes simpler.

### Smart pod schematic blocks (per pod)
1) **Sensor** (ADXL355 or ADXL372) with datasheet decoupling and optional discharge resistors (ADXL355).
2) **Local MCU** (small STM32G0 / STM32C0 / RP2040 / etc.)
   - SPI master for sensor
   - Timestamping using DRDY
   - Optional buffering / compression / windowing (FFT pre-processing if desired later)
3) **Transceiver**
   - **CAN‑FD transceiver** (recommended if main MCU supports CAN‑FD)  
   - or **RS‑485 transceiver** (UART link) if CAN not available
4) **Power**
   - +12 V or +5 V from main board, local buck/LDO to required rails
   - Reverse protection + TVS at input
5) **Connector**: **M8 4‑pin** becomes feasible if using CAN/RS‑485

### Recommended connector (Option 2)
- **M8 4‑pin, A‑coded** (very common in industrial sensors), per pod:
  - Pin 1: +V (e.g., +12 V or +5 V)
  - Pin 3: 0V/GND
  - Pin 2: CANH (or RS‑485 A)
  - Pin 4: CANL (or RS‑485 B)
- Cable: shielded twisted pair for the bus + power pair.

### Main board integration (Option 2)
- One bus shared by both pods (daisy chain) or home‑run to a hub.
- Address pods by CAN ID (or by node ID in RS‑485 protocol).

---

## C) Per‑sensor “proposed schematic approach” (what to draw in KiCad/Altium)
### 1) ADXL355 pod schematic (digital precision, +125 °C capable)
**Core net names (recommended):**
- Power: `VSUPPLY_3V3`, `VDDIO_3V3` (or `VDDIO_1V8`), `VSS`, `VSSIO`, `V1P8ANA`, `V1P8DIG`
- SPI: `ADXL355_CS`, `ADXL355_SCLK`, `ADXL355_MOSI`, `ADXL355_MISO`
- Interrupts: `ADXL355_DRDY`, optionally `ADXL355_INT1`, `ADXL355_INT2`

**Passives (place close):**
- `C_VSUPPLY1 = 0.1uF`, `C_VSUPPLY2 = 1uF` from VSUPPLY→GND
- `C_VDDIO1 = 0.1uF`, `C_VDDIO2 = 1uF` from VDDIO→GND
- `C_V1P8ANA = 0.1uF` from V1P8ANA→GND
- `C_V1P8DIG = 0.1uF` from V1P8DIG→GND
- `R_DISCH_ANA = 100k` from V1P8ANA→GND
- `R_DISCH_DIG = 100k` from V1P8DIG→GND
- Optional SI: `R_SCLK = 33R`, `R_MOSI = 33R`, `R_CS = 33R` (place at driver end)

**Mode strapping:**
- Ensure SPI mode by **NOT** shorting Pin2 (SCLK/VSSIO) to Pin6 (VSSIO). That short enables I²C mode.
- RESERVED pin: tie to GND or leave open per datasheet.

### 2) ADXL372 pod schematic (gearbox/motor, cost‑optimized, +105 °C target)
**Core net names (recommended):**
- Power: `VS_3V3`, `VDDIO_3V3`, `GND`
- SPI: `ADXL372_CS`, `ADXL372_SCLK`, `ADXL372_MOSI`, `ADXL372_MISO`
- Interrupts: `ADXL372_INT1`, `ADXL372_INT2` (optional but useful)

**Passives:**
- `C_VS = 0.1uF` from VS→GND (close)
- `C_VDDIO = 0.1uF` from VDDIO→GND (close)
- If supply noise present: series **ferrite bead / ≤100Ω resistor** + add `C_VS_BULK = 1uF` in parallel with 0.1uF

**Reserved / NIC pins:**
- NIC pins: leave floating.
- RESERVED pins: leave or tie to GND as allowed (pick one rule and keep consistent).

---

## D) Main Board Interface (Section 18: Vibration Interface)

### FDCAN1 Pin Assignment (CONFIRMED)

| Signal | STM32H747 Pin | AF | Direction |
|--------|---------------|-----|-----------|
| FDCAN1_RX | **PD0** | AF9 | Input |
| FDCAN1_TX | **PD1** | AF9 | Output |

### Main Board Schematic Requirements

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  Section 18: Vibration Interface (Main Board)                               │
│                                                                             │
│  STM32H747                                                                  │
│  ┌──────────┐         ┌──────────────┐         ┌──────────────┐            │
│  │          │  PD1    │              │  CANH   │    M8 4-pin  │            │
│  │  FDCAN1  │────────►│ TCAN1042-Q1  │────────►│   Connector  │──► To Pods │
│  │          │  PD0    │  (or equiv)  │  CANL   │   (J1801)    │            │
│  │          │◄────────│              │◄────────│              │            │
│  └──────────┘         └──────────────┘         └──────────────┘            │
│                              │                                              │
│                         120Ω term                                           │
│                        (split or                                            │
│                         direct)                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Recommended CAN-FD Transceiver

| Parameter | TCAN1042-Q1 | SN65HVD230 | Notes |
|-----------|-------------|------------|-------|
| Voltage | 3.3V / 5V | 3.3V | TCAN more flexible |
| Data Rate | 8 Mbps (FD) | 1 Mbps | TCAN required for FD |
| AEC-Q100 | Yes | No | Automotive qualified |
| Standby | Yes | No | Low-power mode |
| Package | SOIC-8 | SOIC-8 | Same footprint |
| **Selection** | **TCAN1042-Q1** | — | **Recommended** |

**JLCPCB Part:** C129966 (TCAN1042VDRQ1) or equivalent

### Main Board BOM (Section 18)

| Ref | Part | Value | Package | LCSC | Notes |
|-----|------|-------|---------|------|-------|
| U1801 | TCAN1042-Q1 | — | SOIC-8 | C129966 | CAN-FD transceiver |
| C1801 | Capacitor | 100nF | 0603 | C14663 | VCC bypass |
| C1802 | Capacitor | 100nF | 0603 | C14663 | VIO bypass (if split supply) |
| R1801 | Resistor | 60Ω | 0603 | — | Split termination (to C1803) |
| R1802 | Resistor | 60Ω | 0603 | — | Split termination (to C1803) |
| C1803 | Capacitor | 4.7nF | 0603 | C1590 | Split term filter |
| D1801 | TVS | PESD2CAN | SOT-23 | C85391 | ESD protection CANH/CANL |
| J1801 | Connector | M8 4-pin | Panel | — | IP67 bulkhead |

**Note:** Split termination (2×60Ω + 4.7nF) preferred over single 120Ω for improved EMC.

### Connector Pinout (M8 4-pin A-coded)

| Pin | Signal | Wire Color (suggested) |
|-----|--------|------------------------|
| 1 | +5V | Red |
| 2 | CANH | White |
| 3 | GND | Black |
| 4 | CANL | Blue |

Shield: Connect to chassis/GND at main board end only.

---

## E) Smart Pod Design (Engine + Gearbox)

### Pod MCU Selection

| Parameter | STM32G0B1 | STM32C011 | ESP32-C3 |
|-----------|-----------|-----------|----------|
| FDCAN | Yes (1x) | No | No (TWAI only) |
| SPI | Yes | Yes | Yes |
| Package | LQFP-32 | TSSOP-20 | Module |
| Flash | 512 KB | 32 KB | 4 MB |
| Price | ~$2.50 | ~$0.80 | ~$1.50 |
| **Selection** | **STM32G0B1** | — | — |

**Rationale:** STM32G0B1 has native FDCAN, same toolchain as main MCU (STM32CubeIDE), adequate flash for buffering.

### Pod Schematic Block Diagram

```
┌──────────────────────────────────────────────────────────────────────┐
│  Vibration Sensor Pod (Engine or Gearbox variant)                    │
│                                                                      │
│  ┌──────────┐    ┌──────────────┐    ┌──────────────┐               │
│  │ M8 4-pin │    │   LDO        │    │  STM32G0B1   │               │
│  │          │───►│  5V → 3.3V   │───►│              │               │
│  │ +5V/GND  │    │  (AP2112)    │    │   SPI ────────────┐          │
│  │          │    └──────────────┘    │              │    │          │
│  │ CANH ◄───────────────────────────►│   FDCAN     │    │          │
│  │ CANL ◄───────────────────────────►│              │    ▼          │
│  └──────────┘    ┌──────────────┐    └──────────────┘  ┌─────────┐  │
│                  │ TCAN1042-Q1  │                      │ ADXL355 │  │
│                  │ (transceiver)│                      │   or    │  │
│                  └──────────────┘                      │ ADXL372 │  │
│                                                        └─────────┘  │
│  Variant: Engine Pod → ADXL355 (LGA-14, 6×6mm)                      │
│           Gearbox Pod → ADXL372 (LGA-16, 3×3.25mm)                  │
└──────────────────────────────────────────────────────────────────────┘
```

### Pod BOM (Common + Variant)

#### Common Components (Both Pods)

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| U1 | STM32G0B1KET6 | — | LQFP-32 | Pod MCU |
| U2 | TCAN1042-Q1 | — | SOIC-8 | CAN-FD transceiver |
| U3 | AP2112K-3.3 | 3.3V | SOT-23-5 | LDO regulator |
| Y1 | Crystal | 8 MHz | 3215 | HSE (optional, use HSI) |
| C1-C4 | Capacitor | 100nF | 0603 | Decoupling |
| C5 | Capacitor | 10µF | 0805 | Bulk input |
| C6 | Capacitor | 10µF | 0805 | LDO output |
| R1-R2 | Resistor | 60Ω | 0603 | CAN split termination |
| C7 | Capacitor | 4.7nF | 0603 | Split term filter |
| D1 | TVS | PESD2CAN | SOT-23 | CAN ESD |
| J1 | Connector | M8 4-pin | Panel | Cable interface |

#### Engine Pod Variant

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| U4 | ADXL355BEZ-RL | — | LGA-14 (6×6) | Engine vibration sensor |
| C8-C9 | Capacitor | 100nF | 0603 | VSUPPLY bypass |
| C10 | Capacitor | 1µF | 0603 | VSUPPLY bulk |
| C11-C12 | Capacitor | 100nF | 0603 | VDDIO bypass |
| C13 | Capacitor | 1µF | 0603 | VDDIO bulk |
| C14 | Capacitor | 100nF | 0603 | V1P8ANA bypass |
| C15 | Capacitor | 100nF | 0603 | V1P8DIG bypass |
| R3 | Resistor | 100kΩ | 0603 | V1P8ANA discharge |
| R4 | Resistor | 100kΩ | 0603 | V1P8DIG discharge |

#### Gearbox Pod Variant

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| U4 | ADXL372BCCZ-RL7 | — | LGA-16 (3×3.25) | Gearbox vibration sensor |
| C8 | Capacitor | 100nF | 0603 | VS bypass |
| C9 | Capacitor | 1µF | 0603 | VS bulk (optional) |
| C10 | Capacitor | 100nF | 0603 | VDDIO bypass |
| FB1 | Ferrite | 100Ω@100MHz | 0603 | VS filter (optional) |

---

## F) CAN-FD Protocol Design

### Node Addressing

| Node ID | Device | CAN Base ID |
|---------|--------|-------------|
| 0x10 | Engine Pod (ADXL355) | 0x100–0x10F |
| 0x20 | Gearbox Pod (ADXL372) | 0x200–0x20F |
| 0x01 | Main MCU (STM32H747) | 0x010–0x01F |

### Message Format (CAN-FD, 64-byte payload)

```
┌────────────────────────────────────────────────────────────────┐
│  Vibration Data Frame (64 bytes)                               │
├────────┬────────┬────────┬────────┬────────────────────────────┤
│ Byte 0 │ Byte 1 │ Byte 2 │ Byte 3 │ Bytes 4–63                 │
│ SeqNum │ Status │ Temp   │ Resv   │ Accel Data (20 samples)    │
├────────┴────────┴────────┴────────┴────────────────────────────┤
│ Each sample: X(16b) + Y(16b) + Z(16b) = 6 bytes                │
│ 60 bytes / 6 = 10 triaxial samples per frame                   │
│ At 3.2 kHz ODR: 320 frames/sec                                 │
└────────────────────────────────────────────────────────────────┘
```

### Bus Parameters

| Parameter | Value | Notes |
|-----------|-------|-------|
| Nominal bit rate | 500 kbps | Arbitration phase |
| Data bit rate | 2 Mbps | Data phase (FD) |
| Sample point | 75% / 80% | Nom / Data |
| Bus length | 5 m max | Both pods on single bus |
| Termination | 120Ω at each end | Main board + last pod |

---

## G) Cable Assembly BOM

| Item | Description | Length | Qty | Notes |
|------|-------------|--------|-----|-------|
| Cable | 4-core shielded, 22 AWG | 2.5 m | 2 | One per pod |
| Connector | M8 4-pin male, cable mount | — | 2 | Pod end |
| Connector | M8 4-pin female, panel mount | — | 1 | Main board (daisy) |
| Connector | M8 Y-splitter (optional) | — | 1 | If home-run preferred |
| Heat shrink | Adhesive-lined | — | AR | Strain relief |

**Cable spec:** Belden 3084A or equivalent (shielded, twisted pairs, suitable for CAN).

---

## H) Firmware Notes

### Main MCU (STM32H747 — M7 Core)

```c
// CubeMX Configuration:
// FDCAN1: PD0 (RX), PD1 (TX)
// Mode: FD with BRS
// Nominal: 500 kbps, Data: 2 Mbps

// Receive callback
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifoITs) {
    FDCAN_RxHeaderTypeDef header;
    uint8_t data[64];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, data);

    if ((header.Identifier & 0xF00) == 0x100) {
        // Engine pod data
        ProcessEngineVibration(data);
    } else if ((header.Identifier & 0xF00) == 0x200) {
        // Gearbox pod data
        ProcessGearboxVibration(data);
    }
}
```

### Pod MCU (STM32G0B1)

- Sample ADXL at 3.2 kHz using DRDY interrupt
- Buffer 10 samples → pack into 64-byte CAN-FD frame
- Transmit at 320 Hz frame rate
- Include temperature reading every Nth frame for thermal compensation

---

## I) Recommendation Summary

| Aspect | Decision |
|--------|----------|
| Architecture | **Smart Pod with CAN-FD** (Option 2) |
| Main MCU interface | **FDCAN1 on PD0/PD1** |
| Transceiver | **TCAN1042-Q1** (both main board and pods) |
| Pod MCU | **STM32G0B1** |
| Connector | **M8 4-pin A-coded** |
| Engine sensor | **ADXL355** (+125°C, 20 µg/√Hz) |
| Gearbox sensor | **ADXL372** (+105°C, ±200g) |

### Next Steps

1. Update **Section 18** schematic in KiCad with CAN-FD transceiver circuit
2. Create separate **Sensor Pod** KiCad project (or add as second board in project)
3. Assign CAN IDs in firmware architecture doc
4. Procure M8 connectors and cables for prototype
