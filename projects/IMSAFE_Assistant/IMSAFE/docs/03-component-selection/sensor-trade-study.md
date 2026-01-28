# Sensor Trade Study: Environmental & Navigation Sensors

**Date:** 2026-01-28
**Phase:** 2.4 Sensors
**Status:** Draft for Review

---

## 1. Scope

This trade study covers all non-vibration sensors for the IMSAFE aviation instrument:

| Sensor Type | Requirement | Priority |
|-------------|-------------|----------|
| Barometric pressure | FR-SN-001 | Must |
| IMU (attitude) | FR-SN-002 | Must |
| Magnetometer | FR-SN-003 | Must |
| CO gas detector | FR-SN-004 | Must |
| NO2 gas detector | FR-SN-005 | Should |
| Cabin temperature | FR-SN-006 | Should |

**Excluded:** Vibration sensors (see vibration-sensor-trade-study.md - DEC-003)

---

## 2. Environmental Requirements

| Parameter | Min | Max | Notes |
|-----------|-----|-----|-------|
| Operating temp | -20°C | +70°C | Cockpit environment |
| Storage temp | -40°C | +85°C | Hangar/vehicle |
| Humidity | 10% | 90% RH | Non-condensing |
| Vibration | - | Aircraft cockpit | Mounted on instrument panel |
| Altitude | 0 | 25,000 ft | GA aircraft ceiling |

---

## 3. Barometric Pressure Sensor

### 3.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Pressure range | 300-1100 hPa | Sea level to 30,000 ft |
| Absolute accuracy | ±1 hPa | ~±8m altitude equivalent |
| Resolution | 0.01 hPa | ~10 cm altitude |
| Interface | I2C or SPI | I2C preferred for simplicity |
| Temperature range | -20°C to +70°C | Cockpit environment |

### 3.2 Candidates

| Parameter | BMP390 (Bosch) | BMP388 (Bosch) | MS5611 (TE) | LPS22HB (ST) |
|-----------|----------------|----------------|-------------|--------------|
| Pressure range | 300-1250 hPa | 300-1250 hPa | 10-1200 mbar | 260-1260 hPa |
| Abs. accuracy | ±0.5 hPa | ±0.5 hPa | ±1.5 mbar | ±1 hPa |
| Rel. accuracy | ±0.03 hPa | ±0.06 hPa | ±0.5 mbar | ±0.1 hPa |
| Noise | 0.02 Pa | 0.04 Pa | 0.012 mbar | 0.65 Pa |
| Temp range | -40 to +85°C | -40 to +85°C | -40 to +85°C | -40 to +85°C |
| Interface | I2C/SPI | I2C/SPI | I2C/SPI | I2C/SPI |
| Package | LGA 2×2mm | LGA 2×2mm | QFN 5×3mm | HLGA 2×2mm |
| Current | 3.2µA @ 1Hz | 3.4µA @ 1Hz | ~1µA | 4µA @ 1Hz |
| JLCPCB | Yes | Yes | Yes | Yes |
| Price | ~$3 | ~$2.50 | ~$8 | ~$2 |

### 3.3 Selection: BMP390

**Rationale:**
- Best-in-class relative accuracy (±0.03 hPa = ~25 cm altitude)
- Low noise (excellent for rate-of-climb calculation)
- Same Bosch ecosystem as common drone/aviation designs
- Proven in UAV and aviation applications
- Good JLCPCB availability
- FIFO buffer reduces MCU interrupt load

**Alternate:** BMP388 (lower cost, slightly less accurate)

---

## 4. IMU - Attitude Sensor (Accelerometer + Gyroscope)

### 4.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Axes | 6-axis (3-accel + 3-gyro) | For synthetic horizon |
| Accel range | ±2g to ±8g | Normal flight accelerations |
| Gyro range | ±250 to ±500 °/s | Normal flight rates |
| Sample rate | ≥100 Hz | Smooth attitude display |
| Interface | I2C or SPI | |
| Temperature range | -20°C to +70°C | |

### 4.2 Candidates

| Parameter | ICM-42688-P (TDK) | BMI270 (Bosch) | MPU-6500 (TDK) | LSM6DSO (ST) |
|-----------|-------------------|----------------|----------------|--------------|
| Axes | 6 | 6 | 6 | 6 |
| Accel range | ±2/4/8/16g | ±2/4/8/16g | ±2/4/8/16g | ±2/4/8/16g |
| Gyro range | ±15.6-2000°/s | ±125-2000°/s | ±250-2000°/s | ±125-2000°/s |
| Accel noise | 70 µg/√Hz | 120 µg/√Hz | 300 µg/√Hz | 70 µg/√Hz |
| Gyro noise | 0.0028°/s/√Hz | 0.007°/s/√Hz | 0.01°/s/√Hz | 0.004°/s/√Hz |
| ODR max | 32 kHz | 6.4 kHz | 1 kHz | 6.6 kHz |
| Temp range | -40 to +85°C | -40 to +85°C | -40 to +85°C | -40 to +85°C |
| Interface | I2C/SPI | I2C/SPI | I2C/SPI | I2C/SPI |
| Package | LGA 2.5×3mm | LGA 2.5×3mm | QFN 3×3mm | LGA 2.5×3mm |
| JLCPCB | Yes | Yes | Limited | Yes |
| Price | ~$3-4 | ~$3 | ~$5 (if available) | ~$4 |
| Status | Current | Current | **NRND** | Current |

**Note:** MPU-6500/MPU-9250 are NRND (Not Recommended for New Design). TDK recommends ICM-42xxx series as replacement.

### 4.3 Selection: ICM-42688-P

**Rationale:**
- Current production, long-term availability
- Best-in-class gyro noise (0.0028°/s/√Hz) for smooth attitude
- High ODR (32 kHz) provides headroom for filtering
- Well-documented, used in modern flight controllers (Pixhawk 6, etc.)
- Direct replacement path from legacy MPU-6500

**Alternate:** BMI270 (Bosch ecosystem consistency with BMP390)

---

## 5. Magnetometer - Heading Reference

### 5.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Axes | 3-axis | Full heading calculation |
| Range | ±8 Gauss minimum | Earth field ~0.25-0.65 G |
| Resolution | 0.1° heading | Display requirement |
| Noise | Low enough for ±2° heading | |
| Temperature range | -20°C to +70°C | |

### 5.2 Candidates

| Parameter | QMC5883L (QST) | MMC5983MA (MEMSIC) | BMM150 (Bosch) | LIS3MDL (ST) | HMC5883L |
|-----------|----------------|-------------------|----------------|--------------|----------|
| Range | ±8 G | ±8 G | ±13 G | ±4/8/12/16 G | ±8 G |
| Resolution | 0.2 mG | 0.0625 mG | 0.3 µT | 1.5 mG | 2 mG |
| Noise | 2 mG RMS | 0.4 mG RMS | 0.3 µT | 3.2 mG | 2 mG |
| Temp range | -40 to +85°C | -40 to +105°C | -40 to +85°C | -40 to +85°C | -30 to +85°C |
| Interface | I2C | I2C/SPI | I2C/SPI | I2C/SPI | I2C |
| Package | LGA 3×3mm | LGA 3×3mm | WLCSP | LGA 2×2mm | LCC-16 |
| ODR max | 200 Hz | 1000 Hz | 30 Hz | 80 Hz | 75 Hz |
| JLCPCB | Yes | Yes | Yes | Yes | **Discontinued** |
| Price | ~$0.50 | ~$2 | ~$1.50 | ~$2 | N/A |
| Status | Current | Current | Current | Current | **EOL** |

**Note:** HMC5883L/HMC5983 are **discontinued**. QMC5883L is pin-compatible replacement but with different register map.

### 5.3 Selection: MMC5983MA

**Rationale:**
- Best noise performance (0.4 mG RMS) for accurate heading
- 18-bit resolution (finest available)
- Built-in SET/RESET for offset cancellation (important for accuracy)
- 1000 Hz ODR allows oversampling/filtering
- Industrial temperature range (-40 to +105°C)
- Current production with good availability

**Alternate:** LIS3MDL (ST ecosystem, simpler interface)

**Rejected:** QMC5883L - very cheap but higher noise, less documentation

---

## 6. Gas Sensor - CO and NO2 Detection

### 6.1 Requirements

| Parameter | CO | NO2 | Notes |
|-----------|-------|------|-------|
| Priority | Must | Should | CO is safety-critical |
| Range | 1-1000 ppm | 0.05-10 ppm | Cabin air monitoring |
| Alarm threshold | 50 ppm | N/A | FAA/OSHA guideline |
| Response time | <60s | <60s | Reasonable for cockpit |
| Interface | Analog or digital | | |

### 6.2 Candidates

| Parameter | MICS-4514 (SGX) | MQ-7 (Winsen) | CCS811 (ScioSense) | SGP41 (Sensirion) |
|-----------|-----------------|---------------|-------------------|-------------------|
| Gases | CO + NO2 | CO only | eCO2 + TVOC | VOC + NOx (index) |
| CO range | 1-1000 ppm | 20-2000 ppm | N/A (eCO2 only) | N/A |
| NO2 range | 0.05-10 ppm | N/A | N/A | NOx index |
| Interface | Analog | Analog | I2C | I2C |
| Heater power | ~80 mW | ~350 mW | ~30 mW | ~50 mW |
| Warmup time | 30s | 60s | 20 min | 60s |
| Temp range | -30 to +85°C | -20 to +50°C | -5 to +50°C | -10 to +50°C |
| JLCPCB | Yes | Basic | Yes | Yes |
| Price | ~$8 | ~$2 | ~$10 | ~$8 |
| Calibration | Manual | Manual | Auto-baseline | Auto-baseline |

### 6.3 Selection: MICS-4514

**Rationale:**
- Dual sensor (CO + NO2) in single package
- Direct CO measurement (not derived eCO2)
- Wide temperature range (-30 to +85°C)
- Good sensitivity for aviation CO detection
- Lower heater power than MQ-7
- Available at JLCPCB

**Design Considerations:**
- Analog output requires two ADC channels (reducing + oxidizing)
- Heater requires GPIO control for power management
- 30s warmup before valid readings
- Recommend baseline calibration in clean air at startup
- Mount with airflow access (vented enclosure or exposed)

**Alternate:** None - MICS-4514 is the clear choice for dual CO/NO2

---

## 7. Temperature Sensor - Cabin Air

### 7.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Range | -20°C to +70°C | Cockpit temperatures |
| Accuracy | ±1°C | Informational, not critical |
| Resolution | 0.5°C | Display precision |
| Interface | I2C, SPI, or analog | |
| Response time | <10s | Slow parameter |

### 7.2 Candidates

| Parameter | TMP102 (TI) | STTS751 (ST) | NTC Thermistor | Si7021 (Silicon Labs) |
|-----------|-------------|--------------|----------------|----------------------|
| Accuracy | ±0.5°C | ±0.5°C | ±1-5% | ±0.4°C |
| Resolution | 0.0625°C | 0.0625°C | ADC-dependent | 0.01°C |
| Range | -40 to +125°C | -40 to +125°C | Varies | -40 to +125°C |
| Interface | I2C | I2C | Analog | I2C |
| Package | SOT-563 | UDFN-6 | Leaded/SMD | DFN 3×3mm |
| Current | 10 µA | 4 µA | ~0 (passive) | 150 µA (measurement) |
| JLCPCB | Basic | Yes | Basic | Yes |
| Price | ~$1 | ~$1 | ~$0.10 | ~$2 |
| Bonus | - | - | - | +Humidity |

### 7.3 Selection: Si7021 (with humidity)

**Rationale:**
- Temperature + humidity in single package
- Humidity useful for condensation/icing awareness
- Excellent accuracy (±0.4°C temp, ±3% RH)
- I2C interface, simple integration
- Factory calibrated
- Low power

**Alternate:** TMP102 if humidity not needed (smaller, cheaper)

**Note:** Mount sensor on cable/probe away from PCB heat for accurate cabin air reading.

---

## 8. I2C Bus Architecture

### 8.1 Address Map

| Address | Device | Function | Bus |
|---------|--------|----------|-----|
| 0x76/0x77 | BMP390 | Barometric pressure | I2C1 |
| 0x68/0x69 | ICM-42688-P | IMU (attitude) | I2C1 |
| 0x30 | MMC5983MA | Magnetometer | I2C1 |
| 0x40 | Si7021 | Temperature/humidity | I2C1 |
| 0x20-0x27 | MCP23017 | I/O expander | I2C1 |

**Bus speed:** 400 kHz (Fast Mode)

### 8.2 External Sensors (SPI Bus)

| Device | Function | Interface |
|--------|----------|-----------|
| ADXL355 | Engine vibration | SPI (CS1) |
| ADXL372 | Gearbox vibration | SPI (CS2) |

### 8.3 Analog Inputs

| Channel | Device | Range | Notes |
|---------|--------|-------|-------|
| ADC1 | MICS-4514 RED | 0-3.3V | CO (reducing gas) |
| ADC2 | MICS-4514 OX | 0-3.3V | NO2 (oxidizing gas) |
| ADC3 | VBAT divider | 0-3.3V | Battery voltage monitor |

---

## 9. Power Budget

| Device | Typical | Peak | Duty Cycle | Notes |
|--------|---------|------|------------|-------|
| BMP390 | 3.2 µA | 700 µA | Continuous | Ultra-low power |
| ICM-42688-P | 0.5 mA | 1 mA | Continuous | 100 Hz ODR |
| MMC5983MA | 0.1 mA | 1 mA | Continuous | 100 Hz ODR |
| Si7021 | 0.15 mA | 3.1 mA | 1 Hz sample | Periodic |
| MICS-4514 | 26 mA | 50 mA | Heater on | Significant |
| **Total sensors** | **~28 mA** | **~56 mA** | | Dominated by gas sensor |

---

## 10. Summary - Sensor Selections

| Function | Selected Part | Interface | Address/Pin | Price |
|----------|---------------|-----------|-------------|-------|
| Barometric | BMP390 | I2C | 0x77 | $3 |
| IMU (attitude) | ICM-42688-P | I2C | 0x68 | $4 |
| Magnetometer | MMC5983MA | I2C | 0x30 | $2 |
| Temp + Humidity | Si7021 | I2C | 0x40 | $2 |
| CO + NO2 | MICS-4514 | Analog | ADC1, ADC2 | $8 |
| **Total** | | | | **~$19** |

---

## 11. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| ICM-42688-P availability | Medium | BMI270 as alternate, similar performance |
| MICS-4514 calibration drift | Medium | Baseline at startup, periodic recalibration prompt |
| Magnetometer interference | High | Mount away from motors, speakers; implement soft-iron calibration |
| Si7021 condensation | Low | Mount in vented area, not sealed |

---

## 12. References

- [Bosch BMP390 Product Page](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/)
- [TDK ICM-42688-P Product Page](https://invensense.tdk.com/products/motion-tracking/6-axis/icm-42688-p/)
- [MEMSIC MMC5983MA Product Page](https://www.memsic.com/magnetometer-2)
- [Sensirion Si7021 Product Page](https://sensirion.com/products/catalog/Si7021/)
- [SGX MICS-4514 Product Page](https://www.sgxsensortech.com/products-services/industrial-safety/metal-oxide-sensors/)
