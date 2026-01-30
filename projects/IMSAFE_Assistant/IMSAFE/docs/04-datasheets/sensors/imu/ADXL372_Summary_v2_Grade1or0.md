# ADXL372 (Analog Devices) — Summary for Vibration Pods (v2: includes Grade 1/0 passive requirements)

## 0) Mandatory BOM rule for this project
Every external component in the pod schematic (including passives) MUST be specified as either:
- **(Grade 1)** suitable for +125°C ambient, or
- **(Grade 0)** suitable for +150°C ambient.

For passives, interpret this as **AEC‑Q200 qualified** with appropriate temperature rating **[Unverified]**.
For ICs, interpret this as **automotive/high-temp qualified** with an explicit ambient temperature grade **[Unverified]**.

---

## 1) Device overview
- 3‑axis digital accelerometer targeted at **high‑g / shock / wide bandwidth** use cases.
- Interfaces via **SPI or I²C** (pin-multiplexed).
- Uses separate **VS** (core) and **VDDIO** (digital I/O) rails.

---

## 2) ADXL372 pinout (Table 7; SPI focus)
| Pin | Name | Practical use |
|---:|---|---|
| 1 | VDDIO | Digital I/O supply (match MCU IO level) |
| 2 | NIC | No connect (leave floating) |
| 3 | RESERVED | Leave or tie to GND per datasheet |
| 4 | SCLK | SPI clock |
| 5 | RESERVED | Leave or tie to GND per datasheet |
| 6 | MOSI/SDA | SPI MOSI |
| 7 | MISO | SPI MISO |
| 8 | CS/SCL | SPI CS |
| 9 | INT2 | Optional interrupt / sync feature |
| 10 | RESERVED | Leave or tie to GND per datasheet |
| 11 | INT1 | Optional interrupt / clock feature |
| 12 | GND | Ground (must connect) |
| 13 | GND | Ground (must connect) |
| 14 | VS | Core supply |
| 15 | NIC | No connect |
| 16 | GND | Ground (must connect) |

---

## 3) Required external passives (tagged Grade 1/0)
### 3.1 Decoupling (mandatory)
Place close to pins with shortest return to ground:
- VS: **0.1 µF** to GND **(Grade 1 or Grade 0)**
- VDDIO: **0.1 µF** to GND **(Grade 1 or Grade 0)**

### 3.2 Extra supply filtering (recommended if VS is noisy)
- Series element on VS feed: **ferrite bead** or **≤100 Ω resistor** **(Grade 1 or Grade 0)** **[Unverified]**
- Add bulk on VS: **1 µF** in parallel with 0.1 µF **(Grade 1 or Grade 0)**

### 3.3 SPI signal conditioning (optional)
- Series resistors at MCU pins: **22–47 Ω** on SCLK/MOSI/CS **(Grade 1 or Grade 0)** **[Unverified]**

---

## 4) Pod integration notes (S32K146 smart pod)
- Use **SPI mode** for deterministic sampling and noise margin.
- Route at least one interrupt (INT1 or INT2) if you want event-based capture; otherwise poll over SPI.
- Keep ADXL372 physically close to the MCU on the pod PCB; long cable carries CAN‑FD only.

---

## 5) Schematic checklist (for Claude)
When drawing the ADXL372 block, include:
1) ADXL372 symbol with pin names per Section 2
2) VS and VDDIO decoupling per Section 3.1 (caps tagged Grade 1/0)
3) Optional VS filter (bead/resistor + 1 µF) per Section 3.2 (tagged Grade 1/0)
4) SPI nets named: ADXL372_CS, ADXL372_SCLK, ADXL372_MOSI, ADXL372_MISO
5) Optional INT nets: ADXL372_INT1, ADXL372_INT2 to MCU GPIOs
