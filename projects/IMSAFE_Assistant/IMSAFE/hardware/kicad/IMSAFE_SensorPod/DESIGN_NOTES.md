# IMSAFE Vibration Sensor Pod - Design Notes

**Project:** IMSAFE_SensorPod
**Version:** 0.1
**Date:** 2026-01-30

---

## Overview

Remote vibration sensor pod for engine and gearbox monitoring on Rotax 912 aircraft engines. Communicates with main IMSAFE board via CAN-FD bus.

## Variants

| Variant | Sensor | Package | Temp Rating | Use Case |
|---------|--------|---------|-------------|----------|
| **Engine Pod** | ADXL355 | LGA-14 (6×6mm) | +125°C | Combustion harmonics, precision trending |
| **Gearbox Pod** | ADXL372 | LGA-16 (3×3.25mm) | +105°C | Gear mesh, shock events, ±200g range |

## Block Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    Sensor Pod PCB                               │
│                                                                 │
│  ┌─────────┐     ┌──────────┐     ┌──────────┐     ┌─────────┐│
│  │   M8    │     │ AP2112K  │     │STM32G0B1 │     │ ADXL355 ││
│  │ 4-pin   │────►│  3.3V    │────►│          │────►│   or    ││
│  │  +5V    │     │   LDO    │     │   SPI    │     │ ADXL372 ││
│  │         │     └──────────┘     │          │     └─────────┘│
│  │ CANH ◄──────────────────────────┤  FDCAN  │                 │
│  │ CANL ◄──────────────────────────┤         │                 │
│  │  GND    │     ┌──────────┐     └──────────┘                 │
│  └─────────┘     │TCAN1042  │                                  │
│                  │   Q1     │                                  │
│                  └──────────┘                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Pin Assignments

### STM32G0B1KET6 (LQFP-32)

| Pin | Function | Signal | Notes |
|-----|----------|--------|-------|
| 1 | VDD | +3.3V | Power |
| 5 | VDDA | +3.3V | Analog power |
| 17 | VSS | GND | Ground |
| 18 | VDD | +3.3V | Power |
| 11 | PA5 | SPI1_SCK | Accelerometer clock |
| 12 | PA6 | SPI1_MISO | Accelerometer data out |
| 13 | PA7 | SPI1_MOSI | Accelerometer data in |
| 10 | PA4 | SPI1_CS | Accelerometer chip select |
| 14 | PB0 | ACCEL_DRDY | Accelerometer data ready |
| 23 | PA11 | FDCAN_RX | CAN receive |
| 24 | PA12 | FDCAN_TX | CAN transmit |
| 25 | PA13 | SWDIO | Debug |
| 26 | PA14 | SWCLK | Debug |

### M8 Connector (A-coded, 4-pin)

| Pin | Signal | Wire Color |
|-----|--------|------------|
| 1 | +5V | Red |
| 2 | CANH | White |
| 3 | GND | Black |
| 4 | CANL | Blue |

## BOM Summary

### Common Components (Both Variants)

| Ref | Part | Value/PN | Package | LCSC |
|-----|------|----------|---------|------|
| U1 | STM32G0B1KET6 | — | LQFP-32 | C2890291 |
| U2 | TCAN1042-Q1 | — | SOIC-8 | C129966 |
| U3 | AP2112K-3.3 | 3.3V LDO | SOT-23-5 | C51118 |
| J1 | M8 Connector | 4-pin A-coded | Panel | — |
| C1-C4 | Capacitor | 100nF | 0603 | C14663 |
| C5-C6 | Capacitor | 10µF | 0805 | C15850 |
| R1 | Resistor | 60Ω | 0603 | — |
| R2 | Resistor | 60Ω | 0603 | — |
| C7 | Capacitor | 4.7nF | 0603 | C1590 |
| D1 | TVS | PESD2CAN | SOT-23 | C85391 |

### Engine Pod Additions (ADXL355)

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| U4 | ADXL355BEZ-RL | — | LGA-14 | +125°C, 20 µg/√Hz |
| C10 | Capacitor | 100nF | 0603 | VSUPPLY |
| C11 | Capacitor | 1µF | 0603 | VSUPPLY bulk |
| C12 | Capacitor | 100nF | 0603 | VDDIO |
| C13 | Capacitor | 1µF | 0603 | VDDIO bulk |
| C14 | Capacitor | 100nF | 0603 | V1P8ANA |
| C15 | Capacitor | 100nF | 0603 | V1P8DIG |
| R3 | Resistor | 100kΩ | 0603 | V1P8ANA discharge |
| R4 | Resistor | 100kΩ | 0603 | V1P8DIG discharge |

### Gearbox Pod Additions (ADXL372)

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| U4 | ADXL372BCCZ-RL7 | — | LGA-16 | +105°C, ±200g |
| C10 | Capacitor | 100nF | 0603 | VS bypass |
| C11 | Capacitor | 100nF | 0603 | VDDIO bypass |

## CAN-FD Configuration

| Parameter | Value |
|-----------|-------|
| Nominal bit rate | 500 kbps |
| Data bit rate | 2 Mbps |
| Node ID (Engine) | 0x10 |
| Node ID (Gearbox) | 0x20 |
| Termination | 120Ω (split: 2×60Ω + 4.7nF) |

## PCB Specifications

| Parameter | Value |
|-----------|-------|
| Size | 20mm × 25mm (target) |
| Layers | 4-layer (preferred) or 2-layer |
| Thickness | 1.6mm |
| Copper | 1oz |
| Finish | ENIG (for harsh environment) |
| Min trace/space | 0.127mm / 0.127mm |

## Mechanical

- Housing: Aluminum, stud-mount base
- Sealing: Potted or O-ring sealed
- Operating temp: -40°C to +125°C (engine), +105°C (gearbox)
- Vibration: Aircraft engine environment
- IP rating: IP67 (with sealed connector)

## Firmware Notes

- Uses STM32CubeIDE / HAL
- Internal HSI oscillator (no external crystal)
- FDCAN peripheral for CAN-FD
- SPI1 at 1-4 MHz for accelerometer
- Sample rate: 3.2 kHz (matching ADXL ODR)
- 10 samples per CAN-FD frame (64-byte payload)
- Frame rate: 320 Hz

## References

- [ADXL355 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl354_adxl355.pdf)
- [ADXL372 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl372.pdf)
- [STM32G0B1 Datasheet](https://www.st.com/resource/en/datasheet/stm32g0b1ke.pdf)
- [TCAN1042-Q1 Datasheet](http://www.ti.com/lit/ds/symlink/tcan1042.pdf)
- [AP2112K Datasheet](https://www.diodes.com/assets/Datasheets/AP2112.pdf)
