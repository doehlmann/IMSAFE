# ADXL354 / ADXL355 (Analog Devices) — Datasheet Summary (focus: ADXL355 digital)
*(For vibration sensing / condition monitoring applications; emphasis on **pinout** + **required passives**.)*

## Quick identity
- **ADXL354**: analog outputs (XOUT/YOUT/ZOUT) + temperature output; range is *pin-selectable* (model dependent).
- **ADXL355**: **digital** output (SPI or I²C), integrated 20‑bit ADC; programmable ranges **±2 g / ±4 g / ±8 g**.
- Package: **14‑terminal LCC** (top view in datasheet).

## Supply domains (ADXL355)
The device has **four supply domains**:
- **VSUPPLY**: 2.25–3.6 V input to internal LDOs (generates 1.8 V rails).
- **V1P8ANA**: 1.8 V analog rail (LDO output or external 1.8 V if LDOs bypassed).
- **V1P8DIG**: 1.8 V digital core rail (LDO output or external 1.8 V if LDOs bypassed).
- **VDDIO**: logic level for interface + interrupt outputs (typically 1.8–3.3 V depending on MCU).

### Power sequencing / cycling notes (important for reliability)
- If using internal LDOs: **power VDDIO first**, then VSUPPLY ~10 µs later.
- Recommended power cycling: discharge **VSUPPLY and VDDIO to 0 V for ≥200 ms**.
- To help internal LDO outputs discharge: add **~100 kΩ** from **V1P8ANA → GND** and **V1P8DIG → GND** (especially below −20 °C).

## ADXL355 pinout (Table 9)
| Pin | Name (SPI / I²C multiplex) | Use in SPI mode | Notes |
|---:|---|---|---|
| 1 | CS/SCL | **CS** | Chip select (active low). In I²C: SCL |
| 2 | SCLK/VSSIO | **SCLK** | In I²C: tie to VSSIO to enable I²C mode |
| 3 | MOSI/SDA | **MOSI** | In I²C: SDA |
| 4 | MISO/ASEL | **MISO** | In I²C: alternate address select (ASEL) |
| 5 | VDDIO | VDDIO | Digital I/O supply |
| 6 | VSSIO | GND | Digital ground |
| 7 | RESERVED | — | May be tied to GND or left open |
| 8 | V1P8DIG | 1.8 V rail | **Decouple required** |
| 9 | VSS | GND | Analog ground |
| 10 | V1P8ANA | 1.8 V rail | **Decouple required** |
| 11 | VSUPPLY | 2.25–3.6 V | LDO input (or tie to VSS to bypass LDOs) |
| 12 | INT1 | INT1 | Interrupt output |
| 13 | INT2 | INT2 | Interrupt output |
| 14 | DRDY | DRDY | Data ready output |

## Required passives / “must‑have” external components (from application circuit)
### Recommended bypassing (typical)
Place as close as possible to pins, with short return to ground:
- **VSUPPLY**: **0.1 µF + 1 µF** to GND
- **VDDIO**: **0.1 µF + 1 µF** to GND (if VDDIO is supplied externally)
- **V1P8ANA**: **0.1 µF** to GND
- **V1P8DIG**: **0.1 µF** to GND
- **Discharge resistors**: **~100 kΩ** from **V1P8ANA → GND** and **V1P8DIG → GND** recommended for clean power cycling.

### Interface considerations (recommended for long cables)
If the sensor is remote (meters of cable):
- Keep SPI clock modest (start ~100–500 kHz and validate margin).
- Add **series damping resistors** at the driver (e.g., 22–47 Ω) on SCLK/MOSI/CS to reduce ringing.
- Provide a solid ground return (shield/drain + dedicated GND conductor).
- Consider differential signaling or local MCU (see connection strategy file).

## Notes for vibration sensing / FFT work
- Use **DRDY** to time‑align sampling; route it to an MCU interrupt.
- INT1/INT2 can be used for activity/event flags; optional depending on firmware strategy.
