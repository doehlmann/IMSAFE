# Vibration Sensor Trade Study: Engine & Gearbox Monitoring

**Date:** 2026-01-28 (Updated: 2026-01-30)
**Phase:** 2.4 Sensors (Vibration Analysis)
**Status:** APPROVED — Architecture Confirmed

> **Decision Summary:**
> - **Engine Mount:** ADXL355 (precision, +125°C)
> - **Gearbox Mount:** ADXL372 (high-g, shock-capable)
> - **Interface:** Smart Pod with CAN-FD (FDCAN1 on PD0/PD1 confirmed available)
> - See: `Vibration_SensorPods_Schematic_and_Connection_Strategy.md` for implementation details

---

## 1. Application Requirements

### 1.1 Target Engine: Rotax 912 Series

| Parameter | Value | Notes |
|-----------|-------|-------|
| Max RPM | 5800 RPM (912 ULS) | Redline |
| Cruise RPM | 4200-5000 RPM | Typical operation |
| Fundamental freq | 70-97 Hz | At cruise/climb |
| Gearbox ratio | 2.43:1 | Prop shaft slower |
| Prop RPM | ~1728-2058 RPM | Cruise range |
| CHT max | 150°C | Old config |
| Coolant max | 120°C | New config |
| Case surface temp | 80-120°C | Near cylinder heads |

### 1.2 Frequency Analysis Requirements

| Harmonic | At 4200 RPM | At 5800 RPM | Source |
|----------|-------------|-------------|--------|
| 0.5X | 35 Hz | 48 Hz | Camshaft (half crank speed) |
| 1X | 70 Hz | 97 Hz | Crankshaft imbalance |
| 2X | 140 Hz | 193 Hz | Firing pulses (4-cyl) |
| 3X | 210 Hz | 290 Hz | Higher order |
| 4X | 280 Hz | 387 Hz | Prop blade passing (2-blade) |
| 6X | 420 Hz | 580 Hz | Upper diagnostic range |

**Minimum sensor bandwidth:** 1 kHz (Nyquist margin for 6X at redline)
**Recommended bandwidth:** 2-5 kHz (capture higher harmonics, bearing faults)

### 1.3 Environmental Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Operating temp | -40°C to +125°C min | Engine compartment exposure |
| Vibration | Aircraft engine environment | High amplitude, continuous |
| Shock | 100g+ | Engine start, hard landing |
| Contamination | Oil, fuel, moisture | Engine bay environment |
| EMI | Ignition system noise | Shielded cabling required |

---

## 2. Technology Comparison: MEMS vs Piezoelectric

### 2.1 Overview

| Parameter | Consumer MEMS | Industrial MEMS | ICP Piezoelectric | Charge Piezoelectric |
|-----------|---------------|-----------------|-------------------|---------------------|
| Noise density | 80-220 µg/√Hz | 20-25 µg/√Hz | 5-10 µg/√Hz | 2-5 µg/√Hz |
| Bandwidth | 1-6 kHz | 1.5-24 kHz | 10-30 kHz | 30+ kHz |
| Temp range | -40 to +85°C | -40 to +125°C | -40 to +125°C (std) | -40 to +650°C |
| Power | µA to mA | mA | 2-20 mA (constant current) | Charge amp required |
| Interface | Digital (I2C/SPI) | Digital or Analog | Analog (voltage) | Analog (charge) |
| Cost | $0.50-5 | $10-70 | $50-200 | $100-500 |
| Size | 3×3mm to 5×5mm | 5×5mm to 14×14mm | 10-25mm diameter | 10-30mm diameter |

### 2.2 Suitability Assessment

| Technology | Temp Rating | Bandwidth | Noise | Engine Mount | Verdict |
|------------|-------------|-----------|-------|--------------|---------|
| LIS3DH | +85°C max | 5 kHz | 220 µg/√Hz | **NO** | Disqualified - temp |
| ADXL345 | +85°C max | 3.2 kHz | 290 µg/√Hz | **NO** | Disqualified - temp |
| ADXL355 | +125°C | 1.5 kHz | 20 µg/√Hz | YES | Suitable |
| ADXL1002 | +125°C | 11 kHz | 25 µg/√Hz | YES | Excellent |
| PCB 603C01 | +125°C | 10 kHz | 6 µg/√Hz | YES | Professional grade |

---

## 3. Candidate Evaluation

### 3.1 ADXL355 (Analog Devices) - Industrial MEMS Triaxial

| Parameter | Value |
|-----------|-------|
| Axes | 3-axis |
| Range | ±2g / ±4g / ±8g selectable |
| Noise density | 20 µg/√Hz (best-in-class MEMS) |
| Bandwidth | 1.5 kHz (-3dB) |
| Temperature | -40°C to +125°C |
| Interface | SPI / I2C |
| Package | LGA-14 (6×6×2mm) |
| Power | 200 µA |
| LCSC availability | Yes (~$65/unit, limited stock) |
| Datasheet | [ADI ADXL355](https://www.analog.com/en/products/adxl355.html) |

**Strengths:**
- Triaxial in single package (simplifies design)
- Industrial temperature range (+125°C)
- Ultra-low noise for MEMS (20 µg/√Hz)
- Excellent DC stability (tilt sensing capability)
- Digital interface (SPI/I2C) - noise immune over cable
- Hermetic LGA package for long-term stability

**Weaknesses:**
- Bandwidth limited to 1.5 kHz (marginal for high-speed bearing faults)
- High cost (~$65/unit)
- Single source (ADI only)

**Suitability for Rotax 912:**
- ✓ Temperature: +125°C > 120°C case surface
- ✓ Bandwidth: 1.5 kHz > 580 Hz (6X at 5800 RPM)
- ✓ Noise: 20 µg/√Hz adequate for gross fault detection
- ⚠ Limited headroom for high-frequency bearing diagnostics

---

### 3.2 ADXL372 (Analog Devices) - High-G Shock/Impact MEMS

| Parameter | Value |
|-----------|-------|
| Axes | 3-axis |
| Range | ±200g |
| Noise density | 6.7 mg/√Hz (shock-optimized, not low-g metrology) |
| Bandwidth | 200 Hz to 3200 Hz (selectable) |
| Temperature | -40°C to +105°C |
| Interface | SPI / I2C |
| Package | LGA-14 (3×3.25×1.06mm) |
| Power | 22 µA at 3200 Hz ODR |
| Price (est) | ~$8-12/unit |
| Datasheet | [ADI ADXL372](https://www.analog.com/en/products/adxl372.html) |

**Strengths:**
- ±200g range captures impulsive/shock events without saturation
- 4-pole antialiasing filter attenuates out-of-band signals
- Wide bandwidth (3200 Hz) for gear mesh frequencies
- Ultra-low power (22 µA) for continuous monitoring
- Built-in high-pass filter eliminates DC drift
- Triaxial in single compact package (3×3.25mm)
- Digital SPI/I2C interface

**Weaknesses:**
- Lower temperature rating (+105°C vs +125°C)
- Higher noise floor (not designed for subtle fault detection)
- Not suitable for precision metrology

**Suitability for Rotax 912 Gearbox:**
- ✓ Temperature: +105°C > 60-90°C gearbox surface
- ✓ Bandwidth: 3.2 kHz captures gear mesh harmonics
- ✓ Range: ±200g handles gear tooth impacts, bearing cage events
- ✓ Shock: Won't saturate on hard landings or engine start transients
- ⚠ Not suitable for engine case (temp limit)

---

### 3.3 ADXL1002 (Analog Devices) - High Bandwidth MEMS

| Parameter | Value |
|-----------|-------|
| Axes | Single axis (X) |
| Range | ±50g |
| Noise density | 25 µg/√Hz |
| Bandwidth | 11 kHz (-3dB), 21 kHz resonance |
| Temperature | -40°C to +125°C |
| Interface | Analog voltage output |
| Package | LFCSP-16 (5×5×1.8mm) |
| Power | ~1 mA |
| Price (est) | ~$15-20/unit |
| Datasheet | [ADI ADXL1002](https://www.analog.com/en/products/adxl1002.html) |

**Strengths:**
- Wide bandwidth (11 kHz) for bearing fault detection
- Industrial temperature (+125°C)
- High g-range (±50g) handles engine vibration
- Low noise for MEMS (25 µg/√Hz)
- Designed specifically for condition monitoring

**Weaknesses:**
- Single axis (need 3 units for triaxial = 3× cost, 3× board space)
- Analog output requires ADC (adds complexity for remote sensor)
- Less common, harder to source

**Suitability for Rotax 912:**
- ✓ Temperature: +125°C adequate
- ✓ Bandwidth: 11 kHz excellent for all diagnostics
- ✓ Range: ±50g handles high vibration
- ⚠ Need 3 units for triaxial measurement

---

### 3.4 ICP/IEPE Piezoelectric (e.g., PCB 603C01, Dytran 3055B)

| Parameter | Value |
|-----------|-------|
| Axes | Single axis (typical) |
| Range | ±50g to ±500g |
| Noise | 5-10 µg/√Hz |
| Bandwidth | 0.5 Hz to 10-30 kHz |
| Temperature | -40°C to +125°C (std), +163°C (HT versions) |
| Interface | Analog (2-wire ICP, constant current) |
| Mounting | 10-32 stud, adhesive |
| Power | 2-20 mA constant current source |
| Price | $80-250/unit |

**Strengths:**
- Lowest noise (5-10 µg/√Hz) - gold standard for vibration
- Wide bandwidth (10-30 kHz)
- Ruggedized industrial housings
- Proven in aerospace applications
- 2-wire interface simplifies cabling

**Weaknesses:**
- Expensive ($100-250/unit)
- Requires constant current power (ICP power supply)
- Larger size (10-25mm diameter)
- Single axis (need 3 for triaxial)
- Not suitable for JLCPCB assembly

**Suitability for Rotax 912:**
- ✓ Temperature: Standard +125°C, HT versions to +163°C
- ✓ Bandwidth: Excellent (10-30 kHz)
- ✓ Noise: Best performance
- ✓ Proven in aircraft engine monitoring
- ⚠ High cost, not PCB-integrated

---

### 3.5 LIS3DH (STMicroelectronics) - REJECTED

| Parameter | Value | Issue |
|-----------|-------|-------|
| Temperature | -40°C to +85°C | **FAILS** - Engine surface 80-120°C |
| Noise | 220 µg/√Hz | Marginal |
| Bandwidth | 5 kHz | Adequate |

**Verdict:** **DISQUALIFIED** - Maximum temperature +85°C is below engine case surface temperature of 80-120°C. Cannot be mounted on or near engine without thermal management that would compromise vibration coupling.

---

## 4. Installation Considerations

### 4.1 Mounting Methods (Ranked)

| Method | Frequency Response | Reliability | Notes |
|--------|-------------------|-------------|-------|
| Stud mount | Best (to 10+ kHz) | Excellent | Requires drilling/tapping |
| Adhesive pad | Good (to 5 kHz) | Good | Cement pad with stud |
| Epoxy bond | Good (to 5 kHz) | Good | Permanent, hard to replace |
| Magnetic | Poor (<2 kHz) | Poor | Not suitable for aircraft |

**Recommendation:** Stud mount with 10-32 or 1/4-28 thread, 30 in-lbs torque

### 4.2 Sensor Pod Design

Common sensor pod design for both locations (same PCB footprint, different IC):

```
┌─────────────────────────────────────────┐
│           Sensor Pod Assembly           │
│                                         │
│  Engine Pod          Gearbox Pod        │
│  ┌────────────┐     ┌────────────┐     │
│  │  ADXL355   │     │  ADXL372   │     │
│  │  (LGA-14)  │     │  (LGA-14)  │     │
│  └────────────┘     └────────────┘     │
│                                         │
│  Common PCB: 15×15mm, 4-layer           │
│  - Both use SPI interface               │
│  - Both are LGA-14 footprint (swap IC)  │
│  - Decoupling, ESD, connector           │
│                                         │
│  Housing: Aluminum, stud mount base     │
│  Cable: Shielded 4-wire SPI + GND       │
│  Connector: M8 4-pin IP67               │
│  Sealing: Potted or O-ring sealed       │
│                                         │
└─────────────────────────────────────────┘
```

**Note:** ADXL355 (6×6mm LGA-14) and ADXL372 (3×3.25mm LGA-14) have different physical footprints despite same pin count. Design PCB with both footprint options or use two PCB variants.

### 4.3 Thermal Management

If mounting near hot surfaces (>100°C):
- Use thermal isolation standoffs
- Aluminum housing acts as heat sink
- Consider mounting on gearbox (cooler than cylinder heads)
- Use silicone thermal gasket if needed

### 4.4 Cabling

| Interface | Cable Type | Max Length | Notes |
|-----------|------------|------------|-------|
| SPI | Shielded 4-wire + GND | 1-2m | Higher frequency capable |
| I2C | Shielded 2-wire + GND | 1m | Address selection needed |
| Analog | Coax or shielded pair | 5-10m | ADC at main board |
| ICP | 2-wire coax | 30m+ | Constant current power |

---

## 5. Selection Matrix

### 5.1 Engine Mount (Hot, Precision Metrology)

| Criterion | Weight | LIS3DH | ADXL355 | ADXL372 | ADXL1002×3 |
|-----------|--------|--------|---------|---------|------------|
| Temperature (≥125°C) | **PASS/FAIL** | FAIL | PASS | FAIL | PASS |
| Noise (<50 µg/√Hz) | 25% | 2 | 5 | 1 | 5 |
| Bandwidth (>1kHz) | 20% | 5 | 4 | 5 | 5 |
| Triaxial capability | 15% | 5 | 5 | 5 | 3 |
| Digital interface | 10% | 5 | 5 | 5 | 2 |
| **Weighted Score** | | **FAIL** | **4.65** | **FAIL** | **4.10** |

**Engine mount winner: ADXL355** - Only triaxial option with +125°C and low noise

### 5.2 Gearbox Mount (Cooler, Shock/Impact Detection)

| Criterion | Weight | ADXL355 | ADXL372 | ADXL1002×3 | ICP Piezo |
|-----------|--------|---------|---------|------------|-----------|
| Temperature (≥105°C) | **PASS/FAIL** | PASS | PASS | PASS | PASS |
| g-Range (≥100g) | 25% | 2 | 5 | 3 | 5 |
| Bandwidth (>2kHz) | 20% | 3 | 5 | 5 | 5 |
| Shock handling | 20% | 2 | 5 | 3 | 5 |
| Triaxial capability | 15% | 5 | 5 | 3 | 2 |
| Cost | 10% | 2 | 5 | 4 | 1 |
| **Weighted Score** | | **2.75** | **4.85** | **3.70** | **4.00** |

**Gearbox mount winner: ADXL372** - High-g range, wide bandwidth, triaxial, low cost

---

## 6. Recommendation: Dual-Sensor Strategy

### Philosophy: Purpose-Matched Sensors

Different mounting locations have different requirements:

| Location | Environment | Primary Need | Best Sensor |
|----------|-------------|--------------|-------------|
| Engine case | Hot (80-120°C), continuous vibration | Precision harmonics, subtle fault trending | ADXL355 |
| Gearbox | Cooler (60-90°C), impulsive events | Shock capture, gear mesh, bearing impacts | ADXL372 |

### Primary Selection

#### Engine Mount: ADXL355

**Rationale:**
- **Temperature:** +125°C rating handles 80-120°C engine case surface
- **Noise:** 20 µg/√Hz enables detection of subtle spectral changes
- **Metrology:** DC-stable, excellent for trending amplitude changes over time
- **Interface:** Digital SPI immune to EMI over shielded cable
- **Application:** Combustion harmonics (1X, 2X, 4X), crankshaft balance

**Use Case:** Detect gradual onset faults - worn bearings, developing imbalance, detonation signature changes

#### Gearbox Mount: ADXL372

**Rationale:**
- **Temperature:** +105°C adequate for gearbox surface (60-90°C typical)
- **g-Range:** ±200g captures gear tooth impacts, bearing cage events without saturation
- **Bandwidth:** 3.2 kHz captures gear mesh frequencies and harmonics
- **Shock:** Won't clip on engine start transients, hard landings
- **Cost:** ~$10 vs $65 for ADXL355
- **Interface:** Digital SPI, same firmware infrastructure as engine sensor

**Use Case:** Detect impulsive faults - gear tooth wear, bearing spalling, prop strike damage

### Why Not Use Same Sensor for Both?

| Approach | Problem |
|----------|---------|
| ADXL355 on gearbox | ±8g max range - will saturate on gear mesh impacts, losing critical fault data |
| ADXL372 on engine | +105°C limit - marginal for hot engine case; high noise floor misses subtle combustion changes |

### Upgrade Path: ADXL1002 (V2)

For future version requiring advanced bearing fault detection:
- 3× ADXL1002 per location for triaxial (11 kHz bandwidth)
- Analog signal conditioning board in sensor pod
- Higher cost but superior diagnostic capability

### Professional Option: ICP Piezoelectric

For certified/professional variant:
- PCB Piezotronics or Dytran industrial accelerometers
- Meets aerospace HUMS standards
- 10× cost but proven flight heritage

---

## 7. Firmware Requirements

### 7.1 Sampling

| Parameter | Value | Notes |
|-----------|-------|-------|
| Sample rate | 3.2 kHz | Nyquist for 1.5 kHz bandwidth |
| ADC resolution | 20-bit (ADXL355 native) | Matches sensor resolution |
| Acquisition mode | Continuous or triggered | RPM-synchronized preferred |

### 7.2 FFT Processing

| Parameter | Value |
|-----------|-------|
| FFT size | 512 or 1024 points |
| Window | Hanning (amplitude accuracy) |
| Overlap | 50-75% |
| Averaging | 4-8 spectra |
| Frequency resolution | 3.2 kHz / 512 = 6.25 Hz per bin |

### 7.3 Order Tracking

Synchronize analysis to engine RPM:
- Use tachometer input (ignition pulse or separate sensor)
- Extract 0.5X, 1X, 2X, 3X, 4X, 6X amplitudes
- Trend over time for fault detection

### 7.4 Baseline Learning

- Record baseline signature at known-good condition
- Store spectral envelope for comparison
- Alert on deviation from baseline (configurable threshold)

---

## 8. Similar Use Cases (Reference)

| Application | Sensor Type | Notes |
|-------------|-------------|-------|
| Wind turbine CbM | ADXL355, ADXL1002 | ADI reference designs |
| Helicopter HUMS | ICP piezoelectric | Meggitt, Vibro-Meter |
| Industrial gearbox | ADXL1002, ICP | Predictive maintenance |
| Automotive engine | MEMS + piezo hybrid | OEM development |
| Aircraft piston engine | ICP piezoelectric | Chadwick-Helmuth systems |

---

## 9. Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| ADXL355 temp limit exceeded | Low | High | Mount on gearbox (cooler), thermal isolation |
| Bandwidth insufficient for bearing faults | Medium | Medium | Document limitation, upgrade path to ADXL1002 |
| Sensor pod vibration isolation | Medium | High | Rigid stud mount, avoid rubber gaskets |
| Cable noise on SPI | Low | Medium | Shielded cable, keep <1m, reduce SPI clock |
| Single source (ADI) | Low | Medium | ADXL357 as alternate (same family) |

---

## 10. Cost Estimate

| Item | Qty | Unit Cost | Total | Notes |
|------|-----|-----------|-------|-------|
| ADXL355 | 1 | $65 | $65 | Engine mount |
| ADXL372 | 1 | $10 | $10 | Gearbox mount |
| Sensor pod PCB | 2 | $5 | $10 | Same footprint, different BOM |
| Aluminum housing | 2 | $15 | $30 | Stud mount base |
| Shielded cable (2m) | 2 | $10 | $20 | 4-wire SPI |
| Connectors (IP67) | 4 | $5 | $20 | M8 or equivalent |
| Mounting hardware | 2 | $5 | $10 | Studs, washers, compound |
| **Total** | | | **~$165** | Down from $220 with dual-355 |

---

## 11. References

- [ADI ADXL355 Product Page](https://www.analog.com/en/products/adxl355.html)
- [ADI ADXL372 Product Page](https://www.analog.com/en/products/adxl372.html)
- [ADI ADXL1002 Product Page](https://www.analog.com/en/products/adxl1002.html)
- [Choosing the Right Accelerometer (ADI)](https://www.analog.com/en/resources/analog-dialogue/articles/choosing-the-most-suitable-mems-accelerometer-for-your-application-part-2.html)
- [Wind Turbine Vibration Monitoring (ADI)](https://www.analog.com/en/resources/analog-dialogue/articles/choosing-the-best-vibration-sensor-for-wind-turbine-condition-monitoring.html)
- [Accelerometer Mounting Best Practices (Endaq)](https://blog.endaq.com/accelerometer-mounting-best-practices-for-vibration-measurement)
- [SAE ARP1839 - Aircraft Engine Vibration Monitoring](https://www.sae.org/standards/content/arp1839/)
- [PCB Piezotronics High-Temp Accelerometers](https://www.pcb.com/industrial-sensors/accelerometers/high-temperature)
