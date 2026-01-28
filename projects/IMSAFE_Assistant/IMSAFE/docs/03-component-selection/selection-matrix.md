# IMSAFE Component Selection Matrix

## 1. Main MCU Selection

| Criteria | Weight | STM32F446 | STM32G474 | XMC4500 | Notes |
|----------|--------|-----------|-----------|---------|-------|
| Processing Power | 20% | 180MHz | 170MHz | 120MHz | |
| Flash Size | 10% | 512KB | 512KB | 1MB | |
| RAM Size | 10% | 128KB | 128KB | 160KB | |
| I2C Interfaces | 15% | 3 | 4 | 2 | Need 2 minimum |
| SPI Speed | 10% | 45MHz | 42MHz | 50MHz | Display |
| USB Support | 10% | Yes | Yes | Yes | |
| JLCPCB Stock | 15% | Good | Good | Limited | |
| Price | 10% | $5 | $5 | $8 | |
| **SCORE** | 100% | **85** | **88** | **72** | |

**Decision:** STM32G474 preferred - good balance of features and availability

---

## 2. Altitude Sensor Selection

| Criteria | Weight | BMP390 | BMP280 | MS5611 | Notes |
|----------|--------|--------|--------|--------|-------|
| Accuracy | 30% | ±0.5hPa | ±1hPa | ±0.15hPa | |
| Resolution | 20% | 0.01hPa | 0.01hPa | 0.01hPa | |
| Interface | 15% | I2C/SPI | I2C/SPI | I2C/SPI | |
| Power | 10% | 3.2uA | 2.7uA | 1uA | |
| JLCPCB Stock | 15% | Good | Good | Good | |
| Price | 10% | $2 | $1.50 | $8 | |
| **SCORE** | 100% | **88** | **78** | **82** | |

**Decision:** BMP390 - good accuracy at reasonable cost

---

## 3. IMU Selection

| Criteria | Weight | MPU9250 | ICM-20948 | BMI270 | Notes |
|----------|--------|---------|-----------|--------|-------|
| Axes | 15% | 9 (6+3) | 9 (6+3) | 6 | Gyro+Accel+Mag |
| Gyro Range | 15% | ±2000°/s | ±2000°/s | ±2000°/s | |
| Accel Range | 15% | ±16g | ±16g | ±16g | |
| Interface | 10% | I2C/SPI | I2C/SPI | I2C/SPI | |
| JLCPCB Stock | 25% | Limited | Good | Good | |
| Price | 20% | $3 | $5 | $4 | |
| **SCORE** | 100% | **70** | **85** | **75** | |

**Decision:** ICM-20948 - better availability, 9-axis integrated

---

## 4. Gas Sensor Selection

| Criteria | Weight | MICS-4514 | MQ-7 | CCS811 | Notes |
|----------|--------|-----------|------|--------|-------|
| CO Detection | 30% | Yes | Yes | No | |
| NO2 Detection | 20% | Yes | No | No | |
| CO2 Detection | 10% | No | No | Yes | |
| Interface | 15% | Analog | Analog | I2C | |
| Power | 15% | 50mW | 350mW | 46mW | |
| Price | 10% | $5 | $2 | $8 | |
| **SCORE** | 100% | **88** | **65** | **55** | |

**Decision:** MICS-4514 - dual gas sensing capability

---

## 5. Display Selection

| Criteria | Weight | ST7796 3.5" | ILI9488 3.5" | ILI9341 2.8" | Notes |
|----------|--------|-------------|--------------|--------------|-------|
| Size | 25% | 3.5" | 3.5" | 2.8" | |
| Resolution | 20% | 320x480 | 320x480 | 240x320 | |
| Interface | 15% | SPI | SPI | SPI | |
| Brightness | 15% | 400cd/m2 | 350cd/m2 | 300cd/m2 | |
| JLCPCB Stock | 15% | Good | Good | Good | |
| Price | 10% | $8 | $7 | $5 | |
| **SCORE** | 100% | **90** | **85** | **70** | |

**Decision:** ST7796 3.5" - specified in requirements

---

## 6. I/O Expander Selection

| Criteria | Weight | MCP23017 | PCF8574 | PCA9555 | Notes |
|----------|--------|----------|---------|---------|-------|
| GPIO Count | 30% | 16 | 8 | 16 | Need 16 minimum |
| Interrupt | 20% | Yes | Yes | Yes | |
| Speed | 15% | 1.7MHz | 100kHz | 400kHz | |
| JLCPCB Stock | 20% | Good | Good | Good | |
| Price | 15% | $1.50 | $0.80 | $1.20 | |
| **SCORE** | 100% | **92** | **60** | **85** | |

**Decision:** MCP23017 - 16 GPIO, fast, good availability

---

## 7. Summary - Selected Components

| Category | Part Number | Supplier | Notes |
|----------|-------------|----------|-------|
| Main MCU | STM32G474RET6 | JLCPCB | Pending final review |
| Radar MCU | XMC4700 | Infineon | Distance2goL platform |
| Altitude | BMP390 | JLCPCB | |
| IMU | ICM-20948 | JLCPCB | |
| Compass | Integrated in ICM-20948 | | |
| Gas Sensor | MICS-4514 | JLCPCB | |
| Display | ST7796 3.5" | JLCPCB | |
| I/O Expander | MCP23017 | JLCPCB | |
| LEDs | WS2812B | JLCPCB | |
| Encoders | PEC12R-4130F-S0012 | | |
