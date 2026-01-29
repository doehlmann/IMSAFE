# MMC5983MA - 3-Axis Magnetometer

## Manufacturer
MEMSIC Inc.

## JLCPCB Part Number
C2903306

## Datasheet
[MMC5983MA Datasheet](https://www.memsic.com/Public/Uploads/uploadfile/files/20220119/MMC5983MADatasheetRevA.pdf)

---

## Key Specifications

### Power Supply
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| VDD (Core) | 2.16 | 3.0 | 3.6 | V |
| VDDIO (I/O) | 1.62 | 1.8 | 3.6 | V |
| Supply Current @ 100Hz | - | 0.8 | - | mA |
| Standby Current | - | 0.01 | - | uA |

### Magnetic Performance
| Parameter | Value | Unit |
|-----------|-------|------|
| Full-Scale Range | +/-8 | Gauss |
| Resolution | 18 | bits |
| Noise Floor (RMS) | 0.4 | mGauss |
| Total RMS Noise | 2 | mGauss |
| Sensitivity | 16384 | counts/Gauss |
| Linearity | 0.1 | %FS |

### Output Data Rate
| Mode | Rate | Unit |
|------|------|------|
| Continuous | Up to 1000 | Hz |
| Single Shot | On demand | - |

### Interface
| Parameter | Value |
|-----------|-------|
| I2C Address (SA0=SA1=0) | 0x30 |
| I2C Address (SA0=1, SA1=0) | 0x31 |
| I2C Address (SA0=0, SA1=1) | 0x32 |
| I2C Address (SA0=SA1=1) | 0x33 |
| I2C Speed | Up to 400 kHz |
| SPI Speed | Up to 10 MHz |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -40 | +105 | C |
| Storage Temperature | -55 | +150 | C |

### Package
- Type: LGA-16
- Dimensions: 3.0 x 3.0 x 1.0 mm
- Pitch: 0.5 mm

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| 1 | VDD | Core Supply |
| 2 | VDDIO | I/O Supply |
| 3 | SCL/SCLK | I2C/SPI Clock |
| 4 | SDA/SDI | I2C Data / SPI Data In |
| 5 | SA0/SDO | Address Select / SPI Data Out |
| 6 | SA1/CS | Address Select / SPI Chip Select |
| 7 | GND | Ground |
| 8 | INT | Interrupt Output |
| 9 | C1 | Internal Cap (4.7uF to GND) |
| 10-16 | GND/NC | Ground and No Connect |

---

## Key Features

### SET/RESET Function
- Built-in degauss coil for offset cancellation
- Eliminates magnetic hysteresis
- Improves long-term stability
- Can be triggered periodically via software

### Automatic Degauss
- Automatic SET/RESET available
- Removes sensor offset errors
- No external components needed

---

## Application Circuit Notes

### Required Capacitors
- C_VDD: 100nF ceramic close to pin 1
- C_VDDIO: 100nF ceramic close to pin 2
- C1: 4.7uF ceramic on pin 9 to GND (required for internal use)

### I2C Address Selection
| SA1 | SA0 | Address |
|-----|-----|---------|
| GND | GND | 0x30 |
| GND | VDD | 0x31 |
| VDD | GND | 0x32 |
| VDD | VDD | 0x33 |

### Recommended Layout
- Mount AWAY from magnetic field sources (motors, inductors, speakers)
- Keep distance from ferromagnetic materials
- Requires soft-iron and hard-iron calibration in firmware

---

## IMSAFE Project Usage
- Section: 09 (Magnetometer)
- Interface: I2C1 @ 400kHz
- I2C Address: 0x30 (SA0=SA1=GND)
- Interrupt: MAG_INT connected to MCU

## Related Components
- C901: 100nF VDD bypass (C14663)
- C902: 100nF VDDIO bypass (C14663)
- C903: 4.7uF C1 internal cap (C19666)
