# ADXL354 / ADXL355 (Analog Devices) — Summary for Vibration Pods (v2: includes Grade 1/0 passive requirements)

## 0) Mandatory BOM rule for this project
Every external component in the pod schematic (including passives) MUST be specified as either:
- **(Grade 1)** suitable for +125°C ambient, or
- **(Grade 0)** suitable for +150°C ambient.

For passives, interpret this as **AEC‑Q200 qualified** with appropriate temperature rating **[Unverified]**.
For ICs, interpret this as **automotive/high-temp qualified** with an explicit ambient temperature grade **[Unverified]**.

---

## 1) Device overview
- **ADXL354**: analog-output 3‑axis accelerometer (XOUT/YOUT/ZOUT + TEMP output) used where analog capture is preferred.
- **ADXL355**: **digital-output** 3‑axis accelerometer with integrated 20‑bit ADC; interfaces via **SPI or I²C**; selectable ranges **±2 g / ±4 g / ±8 g**.
- Package: **14‑terminal LCC**.

This summary emphasizes **ADXL355** because your pod architecture uses a local MCU and digital IMU capture.

---

## 2) Supply domains (ADXL355) — what matters for schematic capture
ADXL355 uses multiple supply domains:
- **VSUPPLY**: 2.25–3.6 V input (feeds internal LDOs).
- **V1P8ANA**: 1.8 V analog rail (LDO output or external 1.8 V if LDO bypassed).
- **V1P8DIG**: 1.8 V digital core rail (LDO output or external 1.8 V if LDO bypassed).
- **VDDIO**: digital I/O supply (set to match MCU logic level; typically 3.3 V in this project **[Unverified]**).

### Power sequencing / cycling note (reliability)
- If using internal LDOs: **power VDDIO first**, then VSUPPLY after ~10 µs.
- For power cycling: discharge **VSUPPLY and VDDIO** to 0 V for ≥200 ms.
- Add **~100 kΩ** discharge resistors from **V1P8ANA → GND** and **V1P8DIG → GND** to ensure clean shutdown/restart (esp. at low temps).

---

## 3) ADXL355 pinout (Table 9; SPI focus)
| Pin | Name | SPI use | Notes |
|---:|---|---|---|
| 1 | CS/SCL | CS | Active-low chip select |
| 2 | SCLK/VSSIO | SCLK | In I²C mode: tie to VSSIO to select I²C |
| 3 | MOSI/SDA | MOSI | |
| 4 | MISO/ASEL | MISO | In I²C mode: ASEL |
| 5 | VDDIO | VDDIO | Digital I/O supply |
| 6 | VSSIO | GND | Digital ground |
| 7 | RESERVED | — | Tie to GND or leave open per datasheet |
| 8 | V1P8DIG | 1.8 V | Decouple (see below) |
| 9 | VSS | GND | Analog ground |
| 10 | V1P8ANA | 1.8 V | Decouple (see below) |
| 11 | VSUPPLY | 2.25–3.6 V | LDO input (or bypass config) |
| 12 | INT1 | INT1 | Optional interrupt |
| 13 | INT2 | INT2 | Optional interrupt |
| 14 | DRDY | DRDY | Data ready output (recommended) |

---

## 4) Required external passives (tagged Grade 1/0)
### 4.1 Decoupling (place within 1–2 mm of pins; shortest ground return)
- VSUPPLY: **0.1 µF + 1 µF** to GND **(Grade 1 or Grade 0)**
- VDDIO: **0.1 µF + 1 µF** to GND **(Grade 1 or Grade 0)**
- V1P8ANA: **0.1 µF** to GND **(Grade 1 or Grade 0)**
- V1P8DIG: **0.1 µF** to GND **(Grade 1 or Grade 0)**

### 4.2 Discharge resistors (recommended)
- V1P8ANA → GND: **100 kΩ** **(Grade 1 or Grade 0)**
- V1P8DIG → GND: **100 kΩ** **(Grade 1 or Grade 0)**

### 4.3 SPI signal conditioning (short local traces; optional damping)
- Series resistors at MCU pins: **22–47 Ω** on SCLK/MOSI/CS **(Grade 1 or Grade 0)** **[Unverified]**
- DRDY routing: direct to MCU interrupt; optional series 22–47 Ω if ringing observed **(Grade 1 or Grade 0)** **[Unverified]**

---

## 5) Pod integration notes (S32K146 smart pod)
- Use **SPI mode** (not I²C) for deterministic sampling and better noise margin.
- Use **DRDY** to timestamp samples precisely for FFT analysis.
- Keep sensor + MCU + CAN transceiver on the pod; long cable carries only 5V/GND/CANH/CANL.

---

## 6) Schematic checklist (for Claude)
When drawing the ADXL355 block, include:
1) ADXL355 symbol with pin names per Section 3
2) Decoupling network per Section 4.1 (each capacitor tagged Grade 1/0)
3) Discharge resistors per Section 4.2 (tagged Grade 1/0)
4) SPI nets named: ADXL355_CS, ADXL355_SCLK, ADXL355_MOSI, ADXL355_MISO
5) DRDY net: ADXL355_DRDY to MCU IRQ pin
6) Ground split: VSS/VSSIO to pod ground plane (single solid ground is acceptable at pod scale **[Unverified]**)
