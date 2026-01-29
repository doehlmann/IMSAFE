# Sensor Datasheets Index

This directory contains datasheets and summaries for all sensor components used in the IMSAFE project.

## Directory Structure

```
sensors/
├── audio/              # Buzzers and audio components
├── barometric/         # Pressure/altitude sensors
├── diodes/             # Protection diodes
├── gas/                # Gas sensors (CO, NO2)
├── imu/                # Inertial measurement units
├── magnetometer/       # Compass/magnetic sensors
├── temp-humidity/      # Temperature and humidity sensors
├── transistors/        # MOSFETs and switching devices
└── (root)              # Legacy datasheets (unsorted)
```

## Component Summaries

### IMU (Inertial Measurement Unit)
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| ICM-42688-P | TDK InvenSense | C2904398 | [Summary](imu/ICM-42688-P-summary.md) |

### Magnetometer
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| MMC5983MA | MEMSIC | C2903306 | [Summary](magnetometer/MMC5983MA-summary.md) |

### Gas Sensor
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| MICS-4514 | SGX Sensortech | C688686 | [Summary](gas/MICS-4514-summary.md) |

### Temperature/Humidity
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| Si7021-A20 | Silicon Labs | C82650 | [Summary](temp-humidity/Si7021-A20-summary.md) |

### Barometric Pressure
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| BMP390 | Bosch | TBD | [Summary](barometric/BMP390-summary.md) |

### Audio
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| MLT-8530 | Jiangsu Huaneng | C94599 | [Summary](audio/MLT-8530-summary.md) |

### Transistors (Switching)
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| AO3400A | Alpha & Omega | C20917 | [Summary](transistors/AO3400A-summary.md) |

### Diodes (Protection)
| Component | Part Number | JLCPCB | Summary |
|-----------|-------------|--------|---------|
| 1N4148W | Generic | C81598 | [Summary](diodes/1N4148W-summary.md) |

---

## Schematic Section Mapping

| Section | Components | Datasheets |
|---------|------------|------------|
| 07 Barometric | BMP390 | barometric/ |
| 08 IMU | ICM-42688-P | imu/ |
| 09 Magnetometer | MMC5983MA | magnetometer/ |
| 10 Gas Sensor | MICS-4514, AO3400A | gas/, transistors/ |
| 11 Temp/Humidity | Si7021-A20 | temp-humidity/ |
| 19 Audio | MLT-8530, AO3400A, 1N4148W | audio/, transistors/, diodes/ |

---

## Legacy Datasheets (Root Folder)

The following PDFs are in the root sensors folder and may need sorting:

- `1#WS2812B-2020...pdf` - RGB LED (move to display/)
- `7#bq25887.pdf` - Battery charger (move to power/)
- `9#bst-bmp390-ds002.pdf` - BMP390 (copied to barometric/)
- `MCP23017_Data_Sheet...pdf` - I/O Expander
- `ST7789V2-3314280.pdf` - Display controller (move to display/)
- `pcf8574 I2C expander.pdf` - I/O Expander
- `tlv700.pdf` - LDO regulator (move to power/)

---

## Datasheet Sources

- **JLCPCB Parts Library**: Most datasheets available via JLCPCB part search
- **Manufacturer Websites**: Links provided in summary files
- **LCSC Electronics**: Alternative source for Chinese-market parts

## Notes

- Summary files contain key specifications for quick reference
- Full datasheets should be downloaded from manufacturer websites
- JLCPCB part numbers included for SMT assembly ordering
