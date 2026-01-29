# BMP390 - Digital Barometric Pressure Sensor

## Manufacturer
Bosch Sensortec

## JLCPCB Part Number
(Check JLCPCB for availability - also consider BMP388)

## Datasheet
Local: `../9#bst-bmp390-ds002.pdf`
[BMP390 Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/)

---

## Key Specifications

### Power Supply
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| VDD | 1.71 | 1.8 | 3.6 | V |
| VDDIO | 1.2 | 1.8 | 3.6 | V |
| Supply Current (Normal) | - | 3.4 | - | uA |
| Supply Current (Ultra-low power) | - | 0.7 | - | uA |
| Sleep Mode Current | - | 0.5 | - | uA |

### Pressure Sensor
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Pressure Range | 300 | - | 1250 | hPa |
| Absolute Accuracy | - | +/-0.5 | - | hPa |
| Relative Accuracy | - | +/-0.03 | - | hPa |
| Resolution | - | 0.016 | - | Pa |
| Noise (Lowest BW) | - | 0.02 | - | Pa (RMS) |
| Long-term Stability | - | +/-0.33 | - | hPa/year |

### Altitude Performance
| Parameter | Value | Unit |
|-----------|-------|------|
| Relative Accuracy | +/-0.25 | m |
| Resolution | 0.13 | m |

### Temperature Sensor
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Range | -40 | - | +85 | C |
| Accuracy | - | +/-0.5 | - | C |
| Resolution | - | 0.01 | - | C |

### Interface
| Parameter | Value |
|-----------|-------|
| I2C Address (SDO=GND) | 0x76 |
| I2C Address (SDO=VDD) | 0x77 |
| I2C Speed | Up to 3.4 MHz |
| SPI Speed | Up to 10 MHz |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -40 | +85 | C |
| Operating Humidity | 0 | 100 | %RH |

### Package
- Type: LGA-10
- Dimensions: 2.0 x 2.0 x 0.75 mm

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| 1 | VDD | Supply voltage |
| 2 | VSS | Ground |
| 3 | VDDIO | I/O voltage (can be same as VDD) |
| 4 | SDI/SDA | SPI Data In / I2C Data |
| 5 | SDO/ADR | SPI Data Out / I2C Address |
| 6 | SCK/SCL | SPI/I2C Clock |
| 7 | CSB | SPI Chip Select (tie to VDD for I2C) |
| 8 | INT | Interrupt output |
| 9 | VSS | Ground |
| 10 | VSS | Ground |

---

## Key Features

### High Accuracy
- +/-0.03 hPa relative accuracy
- Equivalent to +/-0.25 m altitude resolution
- Factory calibrated

### Low Power Consumption
- 0.7 uA in ultra-low power mode
- Ideal for battery-powered applications

### Built-in Filtering
- IIR filter for noise reduction
- Configurable oversampling

### FIFO Buffer
- 512-byte FIFO
- Reduces host processor interrupts

---

## Operating Modes

| Mode | Description | Use Case |
|------|-------------|----------|
| Sleep | Lowest power, no measurements | Power saving |
| Forced | Single measurement, then sleep | Periodic sampling |
| Normal | Continuous measurements | Real-time monitoring |

### Oversampling Settings
| Setting | Pressure Noise | Current |
|---------|----------------|---------|
| Ultra Low Power | 2.64 Pa | 0.7 uA |
| Low Power | 1.32 Pa | 1.2 uA |
| Standard | 0.66 Pa | 2.4 uA |
| High Resolution | 0.33 Pa | 4.8 uA |
| Ultra High Resolution | 0.17 Pa | 9.6 uA |

---

## Application Circuit Notes

### I2C Mode Configuration
- CSB tied to VDDIO for I2C mode
- SDO selects I2C address:
  - SDO = GND: Address 0x76
  - SDO = VDDIO: Address 0x77

### Required Capacitors
- C_VDD: 100nF ceramic close to pin 1
- C_VDDIO: 100nF ceramic close to pin 3

### Altitude Calculation
```
Altitude = 44330 * (1 - (P / P0)^0.1903)

Where:
P = measured pressure (hPa)
P0 = sea level pressure (typically 1013.25 hPa)
```

### Compensation
- Built-in temperature compensation
- Calibration coefficients stored in device
- No user calibration required

---

## IMSAFE Project Usage
- Section: 07 (Barometric Sensor)
- Interface: I2C1 @ 400kHz
- I2C Address: 0x76 (SDO=GND)
- Use: Altitude measurement for aviation

### Related Components
- U9: BMP390 sensor
- C701: 100nF VDD bypass
- C702: 100nF VDDIO bypass

---

## Aviation Notes

### Pressure Altitude
- Standard atmosphere: 1013.25 hPa at sea level
- Pressure decreases ~1 hPa per 8.5 m
- For accurate altitude: use local altimeter setting

### Typical Cabin Altitudes
| Aircraft Type | Cabin Altitude |
|--------------|----------------|
| Unpressurized | Ambient |
| Pressurized (cruise) | 6000-8000 ft |
| High altitude jet | 8000 ft max |
