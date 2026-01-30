# STM32H7 ↔ BGT24LTR11 RF Shield (Distance2GoL V3.0) — **Reduced Remote-Head Interface (Development Cable)**

**Revision Goal:** Replace the unrealistic X2/X3 mezzanine concept (60 conductors) with a **small remote radar-head cable interface** carrying **only the necessary signals** for STM32H7 integration.

You requested a **development-safe superset**, so we bring back **both IF gain pairs** (HG + LG).

---

## 1) What NOT to do
- Do **NOT** attempt to cable out the original Hirose board-to-board connectors **X2 + X3**.
- Those were selected for **mechanical stacking integrity**, not remote wiring.
- A remote head should export only: **IF analog + power + a few enables**.

---

## 2) Signals to carry over the shielded lead (revised: both gain pairs)

You **do not** want to cable out the original X2/X3 dual Hirose mezzanine connectors (they were chosen for rigid board-to-board stacking).  
Instead, treat the RF shield as a **radar head** and add a small dedicated cable connector (or pigtail pads) that exports **only the required nets**.

### Bring back BOTH IF gain pairs (development-friendly)
From radar head → STM32H7:
- **IFI_HG** (analog) — high-gain I channel
- **IFQ_HG** (analog) — high-gain Q channel
- **IFI_LG** (analog) — low-gain I channel
- **IFQ_LG** (analog) — low-gain Q channel
- **AGND** (analog return reference)

From STM32H7 → radar head:
- **TX_EN** (digital) — controls TX enable
- **VCC_BGT_EN** (digital) — enables radar supply gating
- **VCC_DIV_EN** (digital) — enables divider rail (optional but recommended to export)
- **VCC_PTAT_EN** (digital) — enables PTAT rail (optional but recommended to export)
- **V_TUNE_DAC** (analog) — tuning voltage input (recommended to export for dev)

Power:
- **+5V_OUT** (preferred) *or* **+3V3_RF_OUT**
- **GND** (power return / digital ground)

Optional:
- **DIV_OUT** (digital-ish) — divider output, if you want monitoring/locking experiments
- **I2C_SCL / I2C_SDA** — if you want EEPROM ID/calibration data

**Practical cable recommendation:** 4 analog conductors as **2× shielded twisted pairs** (HG pair + LG pair) or **4× micro-coax** if you want maximum immunity.



## 3) Proposed Remote-Head Connector (J_RADAR_HEAD)

A clean 14-pin development connector:

| Pin | Net | Dir | Notes |
|----:|-----|-----|------|
| 1 | IFI_HG | Head→MCU | Analog |
| 2 | IFQ_HG | Head→MCU | Analog |
| 3 | IFI_LG | Head→MCU | Analog |
| 4 | IFQ_LG | Head→MCU | Analog |
| 5 | AGND | — | Analog return (pair with IF shields) |
| 6 | +5V_HEAD | MCU→Head | Supply to head |
| 7 | GND_PWR | — | Power return |
| 8 | VCC_BGT_EN | MCU→Head | Radar rail enable |
| 9 | TX_EN | MCU→Head | TX enable |
| 10 | V_TUNE_IN | MCU→Head | Optional DAC tuning |
| 11 | DIV_OUT | Head→MCU | Optional divider output |
| 12 | V_PTAT_OUT | Head→MCU | Optional PTAT monitor |
| 13 | I2C_SCL | MCU↔Head | Optional EEPROM |
| 14 | I2C_SDA | MCU↔Head | Optional EEPROM |

If you want the absolute minimum cable: populate pins 1–10 only.

---

## 4) Proposed dedicated cable connector pin mapping (revised)

Below is a **compact 16-pin mapping** that covers:
- BOTH gain pairs (HG + LG)
- the essential enables
- V_TUNE from DAC
- optional DIV_OUT and I2C

### Connector J_RADAR (Main board side, 16 pins)
| Pin | Net name | Direction | Notes |
|---:|---|---|---|
| 1 | IFI_HG_IN | Head → MCU | Analog high-gain I into ADC front-end |
| 2 | IFQ_HG_IN | Head → MCU | Analog high-gain Q into ADC front-end |
| 3 | IFI_LG_IN | Head → MCU | Analog low-gain I into ADC front-end |
| 4 | IFQ_LG_IN | Head → MCU | Analog low-gain Q into ADC front-end |
| 5 | AGND | — | Analog return for IF channels (keep quiet; star to ADC AGND) |
| 6 | DGND | — | Digital/power return (join to AGND at a controlled point near ADC/ref) |
| 7 | +5V_OUT (preferred) | MCU → Head | Power feed to radar head (regulate to 3.3 V low-noise at head) |
| 8 | TX_EN | MCU → Head | Drives TX enable control on head |
| 9 | VCC_BGT_EN | MCU → Head | Enables radar VCC gating |
| 10 | VCC_DIV_EN | MCU → Head | Enables divider rail gating |
| 11 | VCC_PTAT_EN | MCU → Head | Enables PTAT rail gating |
| 12 | V_TUNE_DAC | MCU → Head | Analog tuning voltage (DAC + filtering) |
| 13 | DIV_OUT | Head → MCU | Optional: divider output monitoring |
| 14 | I2C_SCL | MCU ↔ Head | Optional: EEPROM / calibration |
| 15 | I2C_SDA | MCU ↔ Head | Optional: EEPROM / calibration |
| 16 | SHIELD / CHASSIS | — | Cable shield termination (recommend bond at main enclosure end) |

### If you want a smaller connector
You can omit pins 13–15 (DIV_OUT + I2C) and/or pin 16 (shield lug), yielding **12–15 pins**.



## 5) Cable recommendation

### IF signals
- Ideally: **2× dual micro-coax** (4 coax total) for HG/LG I/Q
- Or: 2× individually shielded twisted pairs + careful AGND

### Control + power
- 6-core shielded bundle is sufficient

Total practical harness:
- 4 coax + 6-core control/power = manageable development cable

---

## 6) What to omit (DNP functions)
The BOM indicates analog S&H switches are **not fitted**, so do not cable:
- IF_S&H_EN
- V_PTAT_S&H_EN

---

## 7) Claude schematic-generation instructions (STM32H7 motherboard)

Generate one schematic sheet containing:

1. Connector **J_RADAR_HEAD** pins 1–14
2. Four identical IF input stages:
   - IFI_HG_IN, IFQ_HG_IN, IFI_LG_IN, IFQ_LG_IN
   - R_SER + C_SHUNT to AGND
3. DAC output conditioning for V_TUNE_IN
4. GPIO control outputs:
   - VCC_BGT_EN, TX_EN with 33–100Ω series resistors + safe pull-downs
5. 5V power feed:
   - Ferrite bead + bulk caps near connector
6. Optional I2C pullups if EEPROM used

---

## 8) Open decisions (human review)
- Exact IF bandwidth → choose RC cutoff accordingly
- Cable length → may require stronger filtering or digitization at head
- Whether DIV_OUT/PTAT/I2C are actually needed beyond development

---

**This revision gives you a realistic remote radar-head interface: ~10 essential conductors + optional extras, not 60.**
