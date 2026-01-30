# BGT24LTR11 RF Shield Board V3.0 — Development Summary (STM32H7 Interface)

This document describes the Infineon Distance2GoL RF Shield design, adapted for use as a remote radar front-end connected to an STM32H7 main MCU via shielded cable.

Reference schematic set:
:contentReference[oaicite:7]{index=7}

---

## 1. Functional Blocks

The shield contains:

1. BGT24LTR11 radar MMIC (U1)
2. Power gating MOSFETs (Q1, Q2)
3. V_TUNE filtering + optional sample/hold
4. IF baseband amplifier block (dual TLV9002 op-amps)
5. EEPROM (I2C)
6. Low-noise LDO regulator (LP5907-3.3V)
7. Shield can + absorber placement

Block diagram:
:contentReference[oaicite:8]{index=8}

---

## 2. Core Radar Section Passives

### Radar IC (U1)
- U1 = BGT24LTR11N16

Decoupling:
- C1 = 10 µF
- C8/C9/C10 = 100 nF

Pull/series resistors:
- R9 = 16 kΩ (R_TUNE band select)

Radar schematic sheet:
:contentReference[oaicite:9]{index=9}

---

## 3. Control Signals (From MCU)

These digital enables must be provided:

| Signal | Function |
|--------|----------|
| VCC_BGT_EN | Enables main radar VCC rail via MOSFET |
| TX_EN | Controls TX_ON pin |
| VCC_DIV_EN | Enables divider rail |
| VCC_PTAT | Enables PTAT block |
| IF_S&H_EN | Enables IF sample/hold switches (optional) |
| V_PTAT_S&H_EN | Enables PTAT tuning hold |

Control bus names shown in schematic:
:contentReference[oaicite:10]{index=10}

---

## 4. Analog Outputs to STM32H7

### Primary radar baseband outputs:
- IFI (in-phase)
- IFQ (quadrature)

These feed the IF AMP block.

MCU must sample these using ADC channels.

IF outputs are high impedance (~1kΩ) and require buffering.

Receiver characteristics:
:contentReference[oaicite:11]{index=11}

---

## 5. IF Amplifier + Filtering Stage

Op-amps:
- U4/U5 = TLV9002 dual RRIO amplifiers

Passive filter network includes:
- 220 pF, 330 pF caps
- 8.2 nF, 22 nF caps
- Gain resistors: 150 kΩ feedback

Full IF AMP schematic:
:contentReference[oaicite:12]{index=12}

---

## 6. Shield Board Passive BOM Highlights

Capacitors:
- 10 µF bulk
- Multiple 100 nF decouplers
- RF/IF shaping caps: 5.6 pF, 120 pF, 220 pF

Resistors:
- 16 kΩ (R_TUNE)
- Many 0Ω jumpers (configurable routing)
- 100Ω series resistors on IF lines

BOM summary:
:contentReference[oaicite:13]{index=13}

---

## 7. Shielding + Absorber Requirement

Metal shield:
- MP1/MP2 Laird frame + cover

Absorber:
- Eccosorb MMI 17×10×0.5 mm
- Must be placed off-center inside lid:

Placement:
- x = 2.5 mm right from inner left edge
- y = 2.5 mm up from inner bottom edge

Absorber drawing:
:contentReference[oaicite:14]{index=14}

---

## 8. PCB Stackup Constraints

Board is a hybrid RF stack:

- Top dielectric: Rogers RO4350B (0.254 mm)
- Inner layers: FR-4
- Total thickness: 1.058 mm

Stackup:
:contentReference[oaicite:15]{index=15}

---

## 9. STM32H7 Shielded Cable Interface Recommendation

Signals to export over shielded harness:

### Analog (coax recommended)
- IFI_OUT
- IFQ_OUT

### Control (twisted pair)
- TX_EN
- VCC_BGT_EN
- VCC_DIV_EN

### Power
- 3.3V_RF rail
- Ground reference

### Optional tuning DAC
- V_TUNE_IN (STM32 DAC output)

---

## 10. Minimum Bring-Up Wiring for STM32H7

Required connections:

- Supply 3.3 V low-noise
- Enable radar via VCC_BGT_EN
- Set TX_EN high
- Feed ADC from IFI + IFQ
- Provide tuning voltage (DAC or PTAT loop)

---

