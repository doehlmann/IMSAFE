# Power Subsystem Trade Study: 3.3V Buck Regulator

**Date:** 2026-01-28
**Phase:** 2.2 Power & Battery
**Status:** Draft for Review

---

## 1. Requirements

| Requirement | Value | Notes |
|-------------|-------|-------|
| Input voltage | 6.0V - 8.4V | 2S Li-ion (depleted to full) |
| Output voltage | 3.3V | Main logic rail |
| Output current | ≥500mA, prefer 1A+ | STM32H747 + peripherals |
| Quiescent current | <50µA preferred | For sleep mode battery life |
| Efficiency | >85% at typical load | Battery-powered system |
| Package | Hand-solderable | QFN acceptable with thermal pad |

---

## 2. Candidates

### 2.1 TPS62130 (Texas Instruments) - PRIMARY

| Parameter | Value |
|-----------|-------|
| Input voltage | 3V - 17V |
| Output current | 3A |
| Output voltage | 0.9V - 6V (adjustable) |
| Quiescent current | 25µA (PFM mode) |
| Shutdown current | 1.5µA |
| Efficiency | Up to 95% |
| Switching freq | 2.5MHz |
| Package | QFN-16 (3x3mm) |
| Datasheet | [TI TPS62130](https://www.ti.com/lit/ds/symlink/tps62130.pdf) |

**Pros:**
- Wide input range covers 2S battery easily
- 3A output provides ample headroom
- DCS-Control for fast transient response
- Proven, well-documented
- Fixed 3.3V version available (TPS62133)

**Cons:**
- Slightly higher Iq than ultra-low power alternatives
- QFN package requires reflow or hot air

---

### 2.2 TPS62150 (Texas Instruments) - ALTERNATE

| Parameter | Value |
|-----------|-------|
| Input voltage | 3V - 17V |
| Output current | 1A |
| Quiescent current | 25µA |
| Package | QFN-16 (3x3mm) |
| Datasheet | [TI TPS62150](https://www.ti.com/lit/ds/symlink/tps62152.pdf) |

**Pros:**
- Lower current rating may be sufficient
- Pin-compatible with TPS62130/62140
- Slightly lower cost

**Cons:**
- Less headroom for peak loads

---

### 2.3 AP62301 (Diodes Inc) - ALTERNATE

| Parameter | Value |
|-----------|-------|
| Input voltage | 4.2V - 18V |
| Output current | 3A |
| Quiescent current | 22µA (PFM mode) |
| Package | SOT563 (1.6x1.6mm) |
| Datasheet | [Diodes AP62301](https://www.diodes.com/part/view/AP62301) |

**Pros:**
- Very small package
- Good availability (Diodes parts often at JLCPCB)
- Low Iq

**Cons:**
- Smaller package harder to rework
- Less TI-ecosystem familiarity

---

### 2.4 NOT RECOMMENDED

| Part | Reason |
|------|--------|
| TPS62840 | Max input 6.5V - cannot handle 8.4V 2S full charge |
| RT6150 | Max input 5.5V - insufficient |
| BD70522GUL | Max input 5.5V - insufficient |
| LDO (any) | At 8.4V→3.3V, wastes 60% power as heat |

---

## 3. Recommendation

### Primary: TPS62133 (fixed 3.3V version of TPS62130)

**Rationale:**
- 3A output handles STM32H747 + all peripherals with margin
- 25µA Iq acceptable for sleep mode (battery is 16Ah)
- Well-documented with reference designs
- 17V max input provides safety margin

### Alternate: AP62301WG-7 (fixed 3.3V)

**Rationale:**
- Good JLCPCB availability typically
- Similar performance in smaller package
- Fallback if TI parts sourcing issues

---

## 4. Design Notes

### Output Capacitance
- TPS6213x requires 22µF-47µF ceramic output capacitors
- X5R or X7R dielectric recommended

### Input Capacitance
- 10µF ceramic at input
- Place close to VIN pin

### Inductor Selection
- 1µH - 2.2µH, DCR <100mΩ
- Current rating ≥ 1.3x max load
- Shielded preferred for EMI

### Enable Control
- Connect to battery monitoring for controlled shutdown
- Can be driven by BQ25792 power-good output

---

## 5. Additional Rails

### 5V Rail - REQUIRED for WS2812 LEDs

**Load Analysis:**
- 16× WS2812 LEDs @ 60mA max = 960mA peak
- Typical operation (20mA/LED average) = 320mA
- Recommend 1A regulator minimum

**Recommendation: TPS62130 (adjustable) configured for 5V**

| Parameter | Value |
|-----------|-------|
| Input voltage | 3V - 17V |
| Output voltage | 5.0V (set via resistor divider) |
| Output current | 3A (ample headroom) |
| Quiescent current | 25µA |
| Package | QFN-16 (3x3mm) |

**Resistor Divider for 5V output:**
- R1 (VOUT to FB) = 100kΩ
- R2 (FB to GND) = 24.3kΩ
- Formula: VOUT = 0.8V × (1 + R1/R2) = 0.8 × (1 + 100/24.3) ≈ 5.09V

**Alternate:** AP62300 fixed 5V (if available at JLCPCB)

**Layout:** Same family as TPS62133 (3.3V), share inductor/capacitor footprints for easier sourcing.

### 1.2V Core (STM32H747)
- STM32H747 has internal SMPS/LDO for 1.2V core
- No external 1.2V regulator needed
- Configure via PWR registers in firmware

---

## 6. References

- [TI TPS62130 Product Page](https://www.ti.com/product/TPS62130)
- [TI TPS62133 Product Page](https://www.ti.com/product/TPS62133)
- [Diodes AP62301 Product Page](https://www.diodes.com/part/view/AP62301)
