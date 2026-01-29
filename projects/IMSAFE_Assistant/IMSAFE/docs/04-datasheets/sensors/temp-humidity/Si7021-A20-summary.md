# Si7021-A20 - Temperature and Humidity Sensor

## Manufacturer
Silicon Labs

## JLCPCB Part Number
C82650

## Datasheet
[Si7021-A20 Datasheet](https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf)

---

## Key Specifications

### Power Supply
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| VDD | 1.9 | 3.0 | 3.6 | V |
| Supply Current (Active) | - | 150 | - | uA |
| Standby Current | - | 60 | - | nA |

### Temperature Sensor
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Range | -40 | - | +125 | C |
| Accuracy (typical) | - | +/-0.3 | - | C |
| Accuracy (max, -10 to +85C) | - | - | +/-0.4 | C |
| Resolution | - | 0.01 | - | C |
| Response Time | - | - | 10 | sec |

### Humidity Sensor
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Range | 0 | - | 100 | %RH |
| Accuracy (typical) | - | +/-2 | - | %RH |
| Accuracy (max, 20-80%) | - | - | +/-3 | %RH |
| Resolution | - | 0.025 | - | %RH |
| Response Time (63%) | - | 8 | - | sec |

### Interface
| Parameter | Value |
|-----------|-------|
| I2C Address | 0x40 (fixed) |
| I2C Speed | Standard (100kHz), Fast (400kHz) |
| Data Format | 14-bit (default) |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -40 | +125 | C |
| Operating Humidity | 0 | 100 | %RH |

### Package
- Type: DFN-6 with exposed pad
- Dimensions: 3.0 x 3.0 x 1.1 mm
- Pitch: 1.0 mm

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| 1 | VDD | Supply Voltage |
| 2 | GND | Ground |
| 3 | NC | No Connect |
| 4 | SDA | I2C Data |
| 5 | SCL | I2C Clock |
| 6 | GND (EP) | Ground / Exposed Pad |

---

## Key Features

### Factory Calibration
- Individually calibrated at factory
- Calibration data stored in on-chip memory
- No user calibration required

### Measurement Modes
- **Hold Master Mode**: Clock stretching during conversion
- **No Hold Master Mode**: Polling-based measurement
- **Temperature from Previous RH**: Use last RH measurement's temp

### On-chip Heater
- Built-in resistive heater
- Can be used for:
  - Functional diagnostics
  - De-condensation
  - Drying the sensor

---

## Application Circuit Notes

### Minimal Application Circuit
```
VDD ──┬── 100nF ──┬── GND
      │           │
      └── Si7021 ─┘
          │   │
        SDA SCL
          │   │
         I2C Bus (with external pull-ups)
```

### Required Capacitors
- C_VDD: 100nF ceramic close to VDD pin

### I2C Pull-ups
- External pull-ups required
- Typical: 4.7k for 100kHz, 2.2k for 400kHz
- Shared with other I2C devices on bus

### PCB Layout Recommendations
- Avoid placing near heat sources (MCU, regulators)
- For best accuracy, mount away from PCB on short cable
- Provide adequate airflow around sensor
- Do not conformal coat the sensor opening

---

## Measurement Commands

| Command | Code | Description |
|---------|------|-------------|
| Measure RH (Hold) | 0xE5 | Humidity measurement, hold master |
| Measure RH (No Hold) | 0xF5 | Humidity measurement, no hold |
| Measure Temp (Hold) | 0xE3 | Temperature measurement, hold master |
| Measure Temp (No Hold) | 0xF3 | Temperature measurement, no hold |
| Read Temp from RH | 0xE0 | Temperature from previous RH measurement |
| Reset | 0xFE | Software reset |

---

## IMSAFE Project Usage
- Section: 11 (Temperature & Humidity)
- Interface: I2C1 @ 400kHz
- I2C Address: 0x40 (fixed)
- Pull-ups: Located in Section 03 (Charger)

## Related Components
- C1101: 100nF VDD bypass (C14663)
