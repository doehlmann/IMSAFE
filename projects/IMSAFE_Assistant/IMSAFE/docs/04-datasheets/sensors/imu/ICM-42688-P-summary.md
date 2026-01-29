# ICM-42688-P - 6-Axis IMU (Gyroscope + Accelerometer)

## Manufacturer
TDK InvenSense

## JLCPCB Part Number
C2904398

## Datasheet
[ICM-42688-P Datasheet](https://invensense.tdk.com/download-pdf/icm-42688-p-datasheet/)

---

## Key Specifications

### Power Supply
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| VDD (Core) | 1.71 | 1.8 | 3.6 | V |
| VDDIO (I/O) | 1.71 | 1.8 | 3.6 | V |
| Supply Current (6-axis) | - | 0.55 | 0.96 | mA |
| Sleep Mode Current | - | 7 | 20 | uA |

### Gyroscope
| Parameter | Value | Unit |
|-----------|-------|------|
| Full-Scale Range | +/-15.6, +/-31.2, +/-62.5, +/-125, +/-250, +/-500, +/-1000, +/-2000 | dps |
| Sensitivity (2000dps) | 16.4 | LSB/(dps) |
| Noise Density | 2.8 | mdps/sqrt(Hz) |
| Output Data Rate | 1.5625 - 32000 | Hz |
| Resolution | 16 | bits |

### Accelerometer
| Parameter | Value | Unit |
|-----------|-------|------|
| Full-Scale Range | +/-2, +/-4, +/-8, +/-16 | g |
| Sensitivity (16g) | 2048 | LSB/g |
| Noise Density | 70 | ug/sqrt(Hz) |
| Output Data Rate | 1.5625 - 32000 | Hz |
| Resolution | 16 | bits |

### Interface
| Parameter | Value |
|-----------|-------|
| I2C Address (AP_AD0=LOW) | 0x68 |
| I2C Address (AP_AD0=HIGH) | 0x69 |
| I2C Speed | Up to 1 MHz |
| SPI Speed | Up to 24 MHz |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -40 | +85 | C |
| Storage Temperature | -40 | +125 | C |

### Package
- Type: LGA-14
- Dimensions: 2.5 x 3.0 x 0.91 mm
- Pitch: 0.5 mm

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| 1 | AP_SDO/AP_AD0 | SPI Data Out / I2C Address Select |
| 2 | RESV | Reserved (10nF cap to GND required) |
| 3 | AP_CS | SPI Chip Select / I2C Mode Select |
| 4 | AP_SCL/AP_SCLK | I2C Clock / SPI Clock |
| 5 | AP_SDA/AP_SDIO | I2C Data / SPI Data In |
| 6 | VDDIO | I/O Supply Voltage |
| 7 | GND | Ground |
| 8 | RESV | Reserved (NC) |
| 9 | INT1/INT | Interrupt 1 Output |
| 10 | RESV | Reserved (NC) |
| 11 | INT2/FSYNC/CLKIN | Interrupt 2 / Frame Sync / External Clock |
| 12 | VDD | Core Supply Voltage |
| 13 | GND | Ground |
| 14 | RESV | Reserved (NC) |

---

## Application Circuit Notes

### I2C Mode Configuration
- AP_CS must be tied to VDDIO for I2C mode
- AP_AD0 selects I2C address (LOW=0x68, HIGH=0x69)

### Required Capacitors
- C_VDD: 100nF ceramic close to pin 12
- C_VDDIO: 100nF ceramic close to pin 6
- C_RESV: 10nF ceramic on pin 2 to GND (required)

### Recommended Layout
- Place all bypass capacitors as close to IC as possible
- Keep digital and analog grounds connected at a single point
- Shield clock lines from analog signals
- Mount away from sources of mechanical vibration

---

## IMSAFE Project Usage
- Section: 08 (IMU Sensor)
- Interface: I2C1 @ 400kHz
- I2C Address: 0x68 (AP_AD0 tied to GND)
- Interrupt: IMU_INT connected to MCU

## Related Components
- C801: 100nF VDD bypass (C14663)
- C802: 100nF VDDIO bypass (C14663)
- C803: 10nF RESV capacitor (C1588)
- C804: 100nF additional decoupling (C14663)
