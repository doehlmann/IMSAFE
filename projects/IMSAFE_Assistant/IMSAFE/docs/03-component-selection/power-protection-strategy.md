# Power Subsystem: Protection Strategy

**Date:** 2026-01-28
**Phase:** 2.2 Power & Battery
**Status:** Draft for Review

---

## 1. Protection Requirements

| Interface | Threat | Standard | Notes |
|-----------|--------|----------|-------|
| USB-C | ESD, surge | IEC 61000-4-2 Level 4 | Data lines D+/D-, CC lines, VBUS |
| Battery | Overcharge, overdischarge, overcurrent, short | UL 2054 / IEC 62133 | External BMS on pack handles this |
| 3.3V rail | Overcurrent, short | N/A | Regulator has internal protection |

---

## 2. USB-C ESD Protection

### 2.1 Primary: USBLC6-2SC6 (STMicroelectronics)

| Parameter | Value |
|-----------|-------|
| ESD rating (contact) | ±8kV |
| ESD rating (air) | ±15kV |
| Capacitance | 3.5pF max |
| Leakage current | 150nA max |
| Channels | 2 data + VBUS protection |
| Package | SOT-23-6 |
| Datasheet | [ST USBLC6-2](https://www.st.com/resource/en/datasheet/usblc6-2.pdf) |

**Pros:**
- Integrated VBUS protection in single package
- Very common, excellent JLCPCB/LCSC availability (basic part)
- Proven in countless USB designs
- Low cost (~$0.10)

### 2.2 Alternate: TPD2E2U06 (Texas Instruments)

| Parameter | Value |
|-----------|-------|
| ESD rating (contact) | ±25kV |
| ESD rating (air) | ±30kV |
| Capacitance | 1.5pF |
| Clamping voltage | 9.7V @ 1A |
| Package | SOT-5X3 |
| Datasheet | [TI TPD2E2U06](https://www.ti.com/lit/gpn/TPD2E2U06) |

**Pros:**
- Lower capacitance (better for high-speed)
- Higher ESD ratings
- Lower clamping voltage

**Cons:**
- Only 2 channels (need additional for VBUS, CC lines)
- Slightly less common

### 2.3 Recommendation

**Use USBLC6-2SC6** for D+/D- lines:
- Single device protects both data lines
- Handles USB 2.0 speeds easily
- Excellent availability

**For USB-C CC lines** (if CC lines routed to MCU):
- Add second USBLC6-2SC6
- Or use dedicated CC protection (TCPP01-M12)

**For VBUS:**
- USBLC6-2SC6 includes VBUS protection
- Additional TVS (SMBJ6.0A or similar) for surge immunity

---

## 3. Battery Protection

### 3.1 Architecture Decision

**External BMS handles all cell protection:**
- User's battery pack has individual cell leads
- External BMS module provides:
  - Overcharge protection (per cell)
  - Overdischarge protection (per cell)
  - Overcurrent protection
  - Short circuit protection
  - Cell balancing

**PCB-side protection is NOT required** for cell-level safety.

### 3.2 Optional: Input Reverse Polarity

If battery connector could be reversed accidentally:

| Option | Component | Notes |
|--------|-----------|-------|
| P-FET | Si2301 or similar | Low Rds(on), voltage drop |
| Schottky | SS34 | Simple, but 0.4V drop |
| Ideal diode | LTC4357 | Best performance, adds cost |

**Recommendation:** Use P-FET reverse polarity protection if connector is not keyed.

### 3.3 Battery Voltage Monitoring

The BQ25792 charger provides:
- Battery voltage ADC (16-bit)
- Per-cell voltage measurement
- Die temperature monitoring
- NTC input for battery temperature

No additional battery monitoring ICs needed.

---

## 4. 3.3V Rail Protection

### 4.1 Regulator Internal Protection

TPS62133 includes:
- Thermal shutdown
- Overcurrent limiting
- Short circuit protection
- Soft-start

### 4.2 Output Protection

For sensitive downstream components:
- **Ferrite bead** (e.g., BLM18PG121) on rail for noise filtering
- **100nF + 10µF** bulk capacitance at major loads

No additional TVS needed on 3.3V rail (internal regulation handles transients).

---

## 5. Protection Component Summary

| Location | Component | Part Number | Qty | Notes |
|----------|-----------|-------------|-----|-------|
| USB D+/D- | ESD TVS | USBLC6-2SC6 | 1 | SOT-23-6 |
| USB VBUS | TVS (optional) | SMBJ6.0A | 1 | Surge protection |
| USB CC (if used) | ESD TVS | USBLC6-2SC6 | 1 | Optional |
| Battery input | Reverse protection | Si2301 | 1 | If connector not keyed |
| 3.3V rail | Ferrite | BLM18PG121 | 1 | Optional, for noise |

---

## 6. PCB Layout Guidelines

### USB ESD Protection
- Place USBLC6-2SC6 as close as possible to USB connector
- Keep traces from connector to TVS short (<10mm)
- Ground vias directly under TVS package

### Battery Input
- Wide traces for battery current path (≥1mm for 1A)
- Place reverse polarity FET near connector
- Star ground topology for power

### Decoupling
- 10µF ceramic at battery input
- 100nF + 10µF at each major IC
- Place caps as close as possible to IC VDD pins

---

## 7. References

- [ST USBLC6-2 Product Page](https://www.st.com/en/protections-and-emi-filters/usblc6-2.html)
- [TI TPD2E2U06 Product Page](https://www.ti.com/product/TPD2E2U06)
- [TI BQ29200 Product Page](https://www.ti.com/product/BQ29200) (reference for 2S protection)
