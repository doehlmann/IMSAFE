# Power Subsystem Trade Study: 2S Charger IC with Power-Path

**Date:** 2026-01-28
**Phase:** 2.2 Power & Battery
**Status:** Draft for Review

---

## 1. Requirements Summary

| Requirement | Value | Source |
|-------------|-------|--------|
| Battery configuration | 2S Li-ion, 7.4V nom, 8.4V max | DEC-001 |
| Battery capacity | 16000 mAh | DEC-001 |
| Charging input | USB-C 5V (no USB-PD high voltage) | User clarification |
| Power-path | **Required** - system must operate when battery missing/depleted | User clarification |
| Cell balancing | External BMS (pack has individual cell leads) | User clarification |
| System load | ~355mA typical, ~820mA peak | Architecture doc |
| Control interface | I2C preferred | System integration |

### Critical Constraint

The charger must **boost** 5V USB input to charge 2S pack (8.4V max). Standard linear or buck-only chargers cannot work.

---

## 2. Candidates Evaluated

### 2.1 BQ25792 (Texas Instruments)

**Overview:** 1-4 cell buck-boost charger with NVDC power-path

| Parameter | Value |
|-----------|-------|
| Topology | Buck-boost |
| Input voltage | 3.6V - 24V |
| Charge current | Up to 5A programmable |
| Cell count | 1S to 4S |
| Power-path | NVDC - system runs from USB when battery depleted/missing |
| Cell balancing | None (external required) |
| Interface | I2C (full register control) |
| Quiescent current | <1µA |
| USB-PD/OTG | Yes, 2.8V-22V OTG output |
| Package | QFN-29 (4mm x 4mm) |
| Datasheet | [TI BQ25792](https://www.ti.com/lit/ds/symlink/bq25792.pdf) |

**Pros:**
- Proven, widely used
- Excellent documentation and TI support
- Full I2C telemetry (voltages, currents, temperature)
- USB-PD 3.0 compliant
- Very low quiescent current

**Cons:**
- No integrated cell balancing (OK - external BMS handles this)
- Relatively complex I2C configuration
- Extended part at JLCPCB (likely needs sourcing)

---

### 2.2 BQ25798 (Texas Instruments)

**Overview:** Same as BQ25792 with added solar MPPT and backup mode

| Parameter | Value |
|-----------|-------|
| Topology | Buck-boost |
| Input voltage | 3.6V - 24V |
| Charge current | Up to 5A |
| Cell count | 1S to 4S |
| Power-path | NVDC + Backup mode |
| Cell balancing | None |
| MPPT | Yes (solar panel support) |
| Interface | I2C |
| Package | QFN-29 (4mm x 4mm) |
| Datasheet | [TI BQ25798](https://www.ti.com/lit/ds/symlink/bq25798.pdf) |

**Pros:**
- All BQ25792 features
- Backup mode for seamless USB→battery transition
- Solar MPPT if future solar charging desired

**Cons:**
- Higher cost than BQ25792
- MPPT feature unused for USB-only charging
- Same JLCPCB availability concerns

---

### 2.3 MP2760 (Monolithic Power Systems)

**Overview:** 1-4 cell buck-boost charger with NVDC power-path

| Parameter | Value |
|-----------|-------|
| Topology | Buck-boost |
| Input voltage | 4V - 22V |
| Charge current | Up to 6A |
| Cell count | 1S to 4S |
| Power-path | NVDC |
| Cell balancing | None |
| Interface | I2C |
| Default 2S settings | 8.4V charge voltage, 3A charge current, 6.2V VSYS_MIN |
| Package | TQFN-30 (4mm x 5mm) |
| Datasheet | [MPS MP2760](https://www.monolithicpower.com/en/mp2760.html) |

**Pros:**
- Higher charge current capability (6A vs 5A)
- Good 2S factory defaults
- MPS generally good JLCPCB availability
- Slightly simpler than BQ2579x

**Cons:**
- Less comprehensive documentation than TI
- Smaller community/fewer reference designs
- Slightly larger package

---

### 2.4 MAX77962 (Analog Devices/Maxim)

**Overview:** 2S-specific buck-boost charger

| Parameter | Value |
|-----------|-------|
| Topology | Buck-boost |
| Input voltage | 3.5V - 23V |
| Charge current | Up to 3.2A |
| Cell count | 2S only |
| Power-path | Yes |
| Interface | I2C |
| Package | WLP (wafer-level package) |
| Datasheet | [ADI MAX77962](https://www.analog.com/en/products/max77962.html) |

**Pros:**
- Optimized for 2S
- Integrated FETs

**Cons:**
- Lower charge current (3.2A) - 16Ah pack would take ~5+ hours
- WLP package harder to hand-solder/rework
- Less common, fewer reference designs

---

### 2.5 BQ25887 (Texas Instruments) - NOT RECOMMENDED

**Why evaluated:** Already in project datasheets folder

| Parameter | Value |
|-----------|-------|
| Topology | Boost only |
| Power-path | **NO** - system runs from battery during charging |

**Verdict:** Does not meet power-path requirement. System cannot operate when battery is depleted or missing.

---

## 3. Selection Matrix

| Criterion | Weight | BQ25792 | BQ25798 | MP2760 | MAX77962 |
|-----------|--------|---------|---------|--------|----------|
| Power-path (required) | Pass/Fail | ✓ | ✓ | ✓ | ✓ |
| 5V→8.4V boost | Pass/Fail | ✓ | ✓ | ✓ | ✓ |
| Charge current ≥2A | 15% | 5A (5) | 5A (5) | 6A (5) | 3.2A (3) |
| I2C telemetry | 15% | Full (5) | Full (5) | Full (5) | Full (5) |
| Documentation quality | 15% | Excellent (5) | Excellent (5) | Good (4) | Good (4) |
| Package solderability | 10% | QFN-29 (4) | QFN-29 (4) | TQFN-30 (4) | WLP (2) |
| JLCPCB availability | 15% | Extended (3) | Extended (3) | Likely (4) | Unlikely (2) |
| Community/references | 15% | Excellent (5) | Good (4) | Good (4) | Limited (3) |
| Cost estimate | 15% | ~$4 (4) | ~$5 (3) | ~$4 (4) | ~$5 (3) |
| **Weighted Score** | | **4.40** | **4.25** | **4.30** | **3.25** |

---

## 4. Recommendation

### Primary: BQ25792

**Rationale:**
- Best documentation and reference design ecosystem
- Proven in many USB-C battery applications
- Full I2C telemetry for battery monitoring display
- 5A charge capability adequate for this application
- Strong TI support and long-term availability

### Alternate: MP2760

**Rationale:**
- Higher charge current if faster charging desired
- MPS parts often better JLCPCB stock
- Good fallback if BQ25792 sourcing issues

---

## 5. Architecture Notes

### Cell Balancing Strategy

Since user's battery pack includes individual cell leads and external BMS:
- Charger IC does NOT need integrated balancing
- BMS handles cell balancing during charge
- BMS handles protection (OVP, UVP, OCP)

### Power-Path Operation

```
USB Connected + Battery Present:
  → System powered from USB via NVDC
  → Battery charges simultaneously
  → Seamless switchover when USB removed

USB Connected + Battery Missing/Depleted:
  → System powered directly from USB
  → VSYS_MIN prevents brownout (6.2V for 2S)

USB Disconnected:
  → System powered from battery
  → Charger in low-power mode (<1µA)
```

### Charge Time Estimate

With BQ25792 at 2A charge current (conservative):
- 16000mAh / 2000mA = 8 hours
- At 3A: ~5.3 hours
- At 5A (max): ~3.2 hours

Actual charge current limited by:
- USB source capability (5V @ 3A = 15W typical)
- Thermal considerations
- I2C configuration

---

## 6. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| BQ25792 not in JLCPCB basic stock | Assembly cost/lead time | Design footprint compatible with MP2760 alternate |
| Long charge time (16Ah pack) | User inconvenience | Document expected charge time; consider higher current USB-C adapter |
| Thermal management | Reduced charge current | Adequate copper pour, thermal vias under IC |
| I2C configuration complexity | Firmware effort | Use TI reference code, CubeMX I2C driver |

---

## 7. Next Steps

1. [ ] Verify BQ25792/MP2760 JLCPCB availability (manual search)
2. [ ] Download and store datasheet: `docs/04-datasheets/power/BQ25792.pdf`
3. [ ] Select 3.3V buck regulator (Task #2)
4. [ ] Define USB ESD protection (Task #3)
5. [ ] Record decision in decision-log.md (Task #4)

---

## 8. References

- [TI BQ25792 Product Page](https://www.ti.com/product/BQ25792)
- [TI BQ25792 Datasheet (PDF)](https://www.ti.com/lit/ds/symlink/bq25792.pdf)
- [TI BQ25798 Product Page](https://www.ti.com/product/BQ25798)
- [MPS MP2760 Product Page](https://www.monolithicpower.com/en/mp2760.html)
- [ADI MAX77962 Product Page](https://www.analog.com/en/products/max77962.html)
- [TI E2E: BQ25798 vs BQ25792](https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/1055172/bq25798-can-bq25798-replace-bq25792)
