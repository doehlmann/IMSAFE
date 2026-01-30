# BQ25792 Power-Path / Charger Block — IMSAFE (2S Pack + USB‑C)

> **Goal:** NVDC power‑path so the IMSAFE system can run from **USB‑C (VBUS)** when present, or from a **2S Li‑ion pack** when USB is absent; and still power the system from USB even if the battery is missing.  
> **Device:** TI **BQ25792** buck‑boost charger with NVDC power‑path.

---

## 0) Assumptions / Items you must confirm

- **USB‑C input voltage**: **5–20 V** supported by the design example. fileciteturn17file4L10-L16 **[Sourced]**
- **USB‑C behaviour**: This block assumes **default 5 V VBUS** is available (no USB‑C PD negotiation shown). **[Unverified]**
- **2S pack**: Battery regulation voltage in the example is **8.4 V** (2S Li‑ion). fileciteturn17file4L12-L16 **[Sourced]**
- **System load rail**: SYS is ~8 V with 2S configuration per datasheet text. fileciteturn17file3L10-L13 **[Sourced]**

If you want **9/12/15/20 V PD** for higher charge power, you’ll need to add a USB‑C PD sink controller and set BQ25792 input limits accordingly. **[Unverified]**

---

## 1) Core topology (what you should draw in KiCad)

Use the TI **“Single input source and no ship FET”** application diagram as the baseline. fileciteturn17file6L4-L70 **[Sourced]**

### Nets (recommended)
- `VBUS_USB`  : USB‑C VBUS after protection/filtering
- `PMID`      : Power‑path midpoint (charger internal sense node)
- `SW1`, `SW2` : Switching nodes to inductor
- `VSYS_2S`   : System supply output from BQ25792 SYS pin (feeds downstream bucks/LDOs)
- `VBAT_2S`   : Battery terminal (pack positive)
- `PGND` / `GND` : Power ground vs signal ground (follow TI layout guidance)
- `REGN_5V1`  : Internal LDO output pin REGN (decouple only; not a system rail)

---

## 2) “Full block” schematic connections (textual netlist style)

### 2.1 USB‑C front-end (sink / UFP)
- J_USB-C receptacle:
  - `VBUS` → `VBUS_USB` (through protection/filtering below)
  - `CC1` → **5.1 kΩ** to GND (Rd)
  - `CC2` → **5.1 kΩ** to GND (Rd)
  - `D+`, `D-` → ESD device → (optional) to BQ25792 D+/D- pins if you want BC1.2 detection; otherwise leave BQ25792 D+/D- unconnected. **[Unverified]**
- ESD: place a low-cap USB ESD array on D+/D-. Example already used in your draft schematic. fileciteturn17file14L1-L12 **[Sourced]**
- VBUS filtering:
  - `C_VBUS_HF` **0.1 µF** close to VBUS/GND
  - `C_VBUS_BULK` **10 µF** (or more) close to VBUS/GND
  - The TI typical-characteristics setups use **2 × 10 µF** on VBUS. fileciteturn17file0L38-L41 **[Sourced]**

### 2.2 BQ25792 power stage wiring
- U_CHG: **BQ25792**
  - `VBUS` pin → `VBUS_USB`
  - `PMID` pin → `PMID`
  - `SW1` pin → `SW1`
  - `SW2` pin → `SW2`
  - `SYS` pin → `VSYS_2S`
  - `BAT` pins → `VBAT_2S` (battery power connection) fileciteturn17file8L24-L27 **[Sourced]**
  - `BATP` pin → `VBAT_2S` **through 100 Ω series resistor** fileciteturn17file7L31-L34 **[Sourced]**
  - `PGND` + exposed pad → `PGND` (stitch to GND plane with thermal vias per TI guidance) **[Unverified]**
- Inductor (buck‑boost inductor between SW1 and SW2):
  - L = **1.0 µH @ 1.5 MHz** OR **2.2 µH @ 750 kHz**; TI notes 1.5 MHz “only works with 1 µH” and 750 kHz “only works with 2.2 µH”. fileciteturn17file4L18-L22 **[Sourced]**
- Bootstrap caps:
  - `C_BTST1` **47 nF** between `BTST1` and `SW1` (≥10 V rated) fileciteturn17file6L45-L47 **[Sourced]**
  - `C_BTST2` **47 nF** between `BTST2` and `SW2` (≥10 V rated) fileciteturn17file8L13-L16 **[Sourced]**
- REGN decoupling:
  - `C_REGN` **4.7 µF** (typical application) fileciteturn17file1L52-L56 **[Sourced]**
  - Add `0.1 µF` close to REGN (layout best practice) **[Unverified]**
- BAT decoupling (at BAT pins):
  - Recommended: **2 × 10 µF** ceramic at BAT. fileciteturn17file8L24-L28 **[Sourced]**
- SYS decoupling (at SYS pin):
  - Recommended: **5 × 10 µF + 1 × 0.1 µF** ceramic at SYS; place 0.1 µF closest. fileciteturn17file8L33-L38 **[Sourced]**
  - TI also notes **1 × 0.1 µF + 5 × 10 µF** suggested for up to 5 A charging current in 2S configuration. fileciteturn17file2L1-L6 **[Sourced]**
- PMID decoupling (at PMID pin):
  - The design procedure suggests **1 × 0.1 µF + 3 × 10 µF** for up to 3.3 A input current limit. fileciteturn17file3L24-L31 **[Sourced]**

### 2.3 Control / sensing pins (minimum set)
- `CE` (charge enable): must be pulled HIGH or LOW, not floating. fileciteturn17file7L13-L15 **[Sourced]**
- `SCL`, `SDA` (I²C): connect each to logic rail via **10 kΩ** pull‑ups. fileciteturn17file7L15-L18 **[Sourced]**
- `INT`: open‑drain interrupt output; connect to logic rail via **10 kΩ** pull‑up. fileciteturn17file8L22-L24 **[Sourced]**
- `TS`: connect NTC thermistor network; TI recommends a **10 kΩ 103AT‑2** NTC and a divider from REGN to TS to GND. fileciteturn17file7L19-L23 **[Sourced]**
- `ILIM_HIZ`: program input current limit via divider; TI gives formula **VILIM_HIZ = 1 V + 800 mΩ × IINDPM**. fileciteturn17file7L23-L31 **[Sourced]**
- `PROG`: strap resistor sets default switching frequency and default charging profile at power‑up. fileciteturn17file7L38-L42 **[Sourced]**

### 2.4 Ship FET (optional)
- If you *don’t* implement ship FET, TI recommends a **1 nF** cap from `SDRV` to GND or SDRV to BAT. fileciteturn17file8L28-L33 **[Sourced]**

---

## 3) Parts/value table (TI baseline values)

| Ref | Value | Connects | Why / notes |
|---|---:|---|---|
| L1 | 2.2 µH (if 750 kHz) **or** 1.0 µH (if 1.5 MHz) | SW1–SW2 | Frequency/inductor pairing constraint. fileciteturn17file4L18-L22 **[Sourced]** |
| C_SYS_HF | 0.1 µF | SYS–GND | Must be closest to IC. fileciteturn17file8L33-L38 **[Sourced]** |
| C_SYS_BULK | 5 × 10 µF | SYS–GND | SYS stability / ripple. fileciteturn17file8L33-L38 **[Sourced]** |
| C_PMID_HF | 0.1 µF | PMID–GND | Critical placement. fileciteturn17file5L65-L67 **[Sourced]** |
| C_PMID_BULK | 3 × 10 µF | PMID–GND | Sized for ~3.3 A IINDPM example. fileciteturn17file3L24-L31 **[Sourced]** |
| C_BAT_BULK | 2 × 10 µF | BAT–GND | Battery ripple filtering. fileciteturn17file8L24-L28 **[Sourced]** |
| C_BTST1 | 47 nF | BTST1–SW1 | High‑side gate drive bootstrap. fileciteturn17file6L45-L47 **[Sourced]** |
| C_BTST2 | 47 nF | BTST2–SW2 | High‑side gate drive bootstrap. fileciteturn17file8L13-L16 **[Sourced]** |
| C_REGN | 4.7 µF | REGN–GND | LDO bypass. fileciteturn17file1L52-L56 **[Sourced]** |
| R_BATP | 100 Ω | BAT+ → BATP | TI recommended series resistor. fileciteturn17file7L31-L34 **[Sourced]** |
| R_SCL, R_SDA | 10 kΩ | SCL/SDA → logic rail | I²C pull‑ups. fileciteturn17file7L15-L18 **[Sourced]** |
| R_INT | 10 kΩ | INT → logic rail | INT pull‑up. fileciteturn17file8L22-L24 **[Sourced]** |
| NTC | 10 kΩ @25°C (103AT‑2) | TS network | Recommended thermistor family. fileciteturn17file7L19-L23 **[Sourced]** |
| C_SDRV (if no ship FET) | 1 nF, 50 V, 0402 | SDRV→GND (or SDRV→BAT) | TI recommended when ship FET not used. fileciteturn17file8L28-L33 **[Sourced]** |

---

## 4) What this block guarantees (functionally)

- To provide an output on SYS, TI states the device requires either an input supply (3.6–24 V) on VBUS **or** a 1S–4S battery connected to BAT. fileciteturn17file2L38-L42 **[Sourced]**
- The application diagram shows SYS feeding a “SYSTEM LOAD” while VBUS and BAT are connected. fileciteturn17file6L20-L27 **[Sourced]**

---

## 5) Layout constraints (treat as “hard rules”)

- Place the **0.1 µF** caps closest; keep loops tiny. fileciteturn17file5L1-L9 **[Sourced]**
- Place inductor close; keep SW copper small. fileciteturn17file5L9-L12 **[Sourced]**
- Keep BATP away from SW nodes. fileciteturn17file5L19-L20 **[Sourced]**
