# Infineon BGT24LTR11N16 — Core Chip Summary (24GHz FMCW Radar)

## 1. Device Overview
The Infineon BGT24LTR11N16 is a fully integrated 24.0–24.25 GHz radar transceiver MMIC.

Key features:
- Integrated low phase-noise VCO
- Built-in PTAT temperature compensation voltage source
- Quadrature homodyne receiver (I/Q outputs)
- Integrated 1:16 frequency divider for PLL reference output
- Single 3.3 V supply operation
- Packaged in TSNP-16-9

Source: Infineon Datasheet Rev 1.32 (2025-06-25)
:contentReference[oaicite:0]{index=0}

---

## 2. Power Requirements

### Main Supply
- VCC = 3.3 V nominal (3.2–3.4 V)
- ICC ≈ 45 mA typ (55 mA max)

:contentReference[oaicite:1]{index=1}

### Additional Rails
- VCC_DIV: prescaler supply (3.3 V)
- VCC_PTAT: PTAT source supply (3.3 V)

Absolute max:
- VCC ≤ 3.6 V
- VCC_PTAT ≤ 2.6 V max rating

:contentReference[oaicite:2]{index=2}

---

## 3. Pinout (Top View TSNP-16-9)

| Pin | Name       | Function |
|-----|------------|----------|
| 1   | VCC        | Main supply (3.3V) |
| 2   | VEE        | Ground |
| 3   | RX         | 24GHz RF receiver input (50Ω) |
| 4   | VEE        | Ground |
| 5   | TX_ON      | TX enable control (internally pulled up via ~98kΩ) |
| 6   | IFQ        | Quadrature IF output (baseband Q) |
| 7   | IFI        | In-phase IF output (baseband I) |
| 8   | DIV_OUT    | Frequency divider output |
| 9   | VCC_DIV    | Divider supply rail |
| 10  | VEE        | Ground |
| 11  | TX         | 24GHz RF transmitter output (50Ω) |
| 12  | VEE        | Ground |
| 13  | R_TUNE     | Band select resistor (typ 16kΩ to GND) |
| 14  | V_TUNE     | VCO tuning voltage input (0.7–2.5 V) |
| 15  | V_PTAT     | PTAT voltage output (~0.7–2.0 V) |
| 16  | VCC_PTAT   | PTAT supply |

Pin definition table:
:contentReference[oaicite:3]{index=3}

---

## 4. Key Analog Characteristics

### Transmitter
- Output power: 2–10 dBm (typ 6 dBm)
- TX_ON logic:
  - OFF: <0.8 V
  - ON: >2.0 V

:contentReference[oaicite:4]{index=4}

### Receiver
- Gain: 20 dB typ
- Noise figure: 10 dB typ
- IF output impedance: ~1 kΩ single-ended

:contentReference[oaicite:5]{index=5}

---

## 5. Required External Passives (Minimal Radar Bring-Up)

### VCO Stabilization
- R_TUNE requires 16 kΩ to GND for correct band operation

:contentReference[oaicite:6]{index=6}

### Decoupling (Recommended)
- VCC: 10 µF + 100 nF close to pin
- VCC_DIV: 100 nF local
- VCC_PTAT: 100 nF local

### Tuning Filtering
- V_TUNE should be low-noise filtered (RC/LC)

---

## 6. Outputs to MCU / ADC
The chip does NOT provide digital radar data.

MCU must sample:
- IFI (Pin 7)
- IFQ (Pin 6)

These go into:
- Analog baseband amplifier/filter stage
- ADC channels on STM32H7

---

## 7. Critical Layout Notes
- RF pins RX/TX require controlled impedance matching structures
- Ground stitching is essential
- Shielding + absorber strongly recommended for harmonic suppression

---
