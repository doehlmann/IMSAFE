# IMSAFE Component Decision Log

## Decision Record Format

Each decision includes:
- **Date:** When the decision was made
- **Context:** Why a decision was needed
- **Options:** What was considered
- **Decision:** What was chosen
- **Rationale:** Why it was chosen
- **Status:** Active / Superseded / Under Review

---

## DEC-001: Radar Platform

**Date:** 2026-01-27
**Context:** Landing radar implementation
**Options:**
1. Custom radar design with BGT24LTR11
2. Distance2goL reference design (XMC4700 + BGT24LTR11)
3. Alternative ultrasonic/lidar sensors

**Decision:** Distance2goL reference design

**Rationale:**
- Proven reference design available
- XMC4700 handles complex radar signal processing
- Reduces development time significantly
- Full schematic and firmware available

**Status:** Active

---

## DEC-002: Main MCU Family

**Date:** 2026-01-27
**Context:** Selecting main processor for non-radar functions
**Options:**
1. STM32F4 series
2. STM32G4 series
3. XMC4xxx series (same as radar)
4. ESP32

**Decision:** Under evaluation (STM32G4 leading)

**Rationale:**
- STM32G4 has good peripheral set
- Strong JLCPCB availability
- Familiar development environment
- Consider XMC for single-ecosystem approach

**Status:** Under Review

---

## DEC-003: Altitude Sensor

**Date:** 2026-01-27
**Context:** Barometric altitude measurement
**Options:**
1. BMP390 (Bosch)
2. BMP280 (Bosch)
3. MS5611 (TE Connectivity)

**Decision:** BMP390

**Rationale:**
- Good accuracy (±0.5hPa)
- Available on JLCPCB
- Low power consumption
- Good temperature compensation

**Status:** Active

---

## DEC-004: IMU Selection

**Date:** 2026-01-27
**Context:** Attitude reference for synthetic horizon
**Options:**
1. MPU9250 (TDK)
2. ICM-20948 (TDK)
3. BMI270 + BMM150 (Bosch)

**Decision:** ICM-20948

**Rationale:**
- 9-axis integrated (accel + gyro + mag)
- Better availability than MPU9250
- Active production (MPU9250 is legacy)

**Status:** Active

---

## DEC-005: Gas Sensor

**Date:** 2026-01-27
**Context:** Carbon monoxide detection for pilot safety
**Options:**
1. MICS-4514 (CO + NO2)
2. MQ-7 (CO only)
3. CCS811 (VOC + eCO2)

**Decision:** MICS-4514

**Rationale:**
- Detects both CO and NO2
- Lower power than MQ-7
- Smaller package
- Direct analog output

**Status:** Active

---

## DEC-006: Switch Interface

**Date:** 2026-01-27
**Context:** 8 toggle switches need GPIO
**Options:**
1. Direct MCU GPIO (16 pins for SPDT)
2. MCP23017 I2C expander
3. Shift register (74HC165)

**Decision:** MCP23017

**Rationale:**
- Frees up MCU pins
- Built-in interrupt support
- Easy I2C interface
- Can also drive additional outputs if needed

**Status:** Active

---

## DEC-007: LED Technology

**Date:** 2026-01-27
**Context:** 16 indicator LEDs with programmable color
**Options:**
1. Individual LEDs with driver IC
2. WS2812B addressable RGB
3. APA102 addressable RGB

**Decision:** WS2812B

**Rationale:**
- Single data line for 16 LEDs
- Full RGB control
- Brightness control
- Cost effective
- Widely available

**Status:** Active

---

## DEC-008: Display Interface

**Date:** 2026-01-27
**Context:** ST7796 supports multiple interfaces
**Options:**
1. SPI (4-wire)
2. 8-bit parallel
3. 16-bit parallel

**Decision:** SPI (4-wire)

**Rationale:**
- Fewer pins required
- Simpler PCB routing
- 40MHz SPI sufficient for display updates
- DMA can handle transfers

**Status:** Active

---

## Future Decisions Pending

| ID | Topic | Target Date |
|----|-------|-------------|
| DEC-009 | Final MCU selection | TBD |
| DEC-010 | Battery management IC | TBD |
| DEC-011 | Bluetooth module selection | TBD |
| DEC-012 | Vibration IMU selection | TBD |
| DEC-013 | Enclosure material/design | TBD |
