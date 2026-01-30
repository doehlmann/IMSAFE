# ADXL372 (Analog Devices) — Datasheet Summary (vibration / shock accelerometer)
*(Emphasis on **pinout** + **required passives** + what matters for remote sensor pods.)*

## Quick identity
- 3‑axis digital accelerometer intended for **high‑g / shock + wide bandwidth** use cases.
- Interfaces: **SPI or I²C** (same pins are multiplexed).
- Supply rails: **VS** (core) and **VDDIO** (digital I/O).

## Supply requirements & power cycling (key points)
- VS operating voltage range: **1.6 V to 3.3 V** (device designed for 1.6–3.6 V rails overall).
- Recommendation: **VS and VDDIO be separate supplies** to minimize digital noise on VS when possible.
- Power cycling requirement: when power is removed or VS falls below range, **discharge supplies to ≤100 mV (VRESET)** and hold for **≥200 ms** before reapplying power.

## ADXL372 pinout (Table 7)
| Pin | Name | Description / practical use |
|---:|---|---|
| 1 | VDDIO | Digital I/O supply |
| 2 | NIC | No connect (not internally connected) |
| 3 | RESERVED | May be left unconnected or tied to GND |
| 4 | SCLK | SPI clock |
| 5 | RESERVED | May be left unconnected or tied to GND |
| 6 | MOSI/SDA | SPI MOSI or I²C SDA |
| 7 | MISO | SPI MISO |
| 8 | CS/SCL | SPI CS or I²C SCL |
| 9 | INT2 | Interrupt 2 output; can also be used as sync input (datasheet feature) |
| 10 | RESERVED | May be left unconnected or tied to GND |
| 11 | INT1 | Interrupt 1 output; can also be used as external clock input (datasheet feature) |
| 12 | GND | Ground (must connect) |
| 13 | GND | Ground (must connect) |
| 14 | VS | Core supply |
| 15 | NIC | No connect (not internally connected) |
| 16 | GND | Ground (must connect) |

## Required passives / “must‑have” external components
### Recommended bypass capacitors
- **VS**: **0.1 µF ceramic** close to VS.
  - If more filtering needed: add **≤100 Ω resistor or ferrite bead** in series with VS, and increase VS bypassing with **1 µF (tantalum) + 0.1 µF (ceramic)** in parallel.
- **VDDIO**: **0.1 µF ceramic** close to VDDIO.

### Grounding note
- Ensure **low impedance** ground connection from the sensor to supply ground (ground noise couples like supply noise).

## Remote vibration sensor notes
- Prefer **SPI** for higher data throughput and deterministic timing (vs I²C).
- INT1/INT2 are optional but useful for: data-ready/event flags, external sync/clocking features.
