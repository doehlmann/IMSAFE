# Power Subsystem Trade Study: 2S BMS IC Selection

**Date:** 2026-01-28
**Phase:** 2.2 Power & Battery
**Status:** Draft for Review

---

## 1. Requirements

| Requirement | Value | Notes |
|-------------|-------|-------|
| Cell configuration | 2S Li-ion | 7.4V nominal, 8.4V max |
| Pack capacity | 16000 mAh | Large pack, balancing important |
| Discharge current | 1A typical, 2A peak | Conservative system load |
| Charge current | Up to 5A (from BQ25792) | Limited by USB source |
| Protection features | OVP, UVP, OCP, SCP | Standard Li-ion protection |
| Cell balancing | Required | Top-balancing during charge |

---

## 2. Architecture: BQ25792 + BMS Integration

```
USB-C 5V ──► BQ25792 ──► BAT pin ──► BMS P+/P- ──► 2S Cells
                │                        │
                │                        ├── BM (balance tap to cell mid-point)
                │                        │
                └── VSYS ──► TPS62130 (5V) ──► WS2812 LEDs
                         ├── TPS62133 (3.3V) ──► STM32H747 + peripherals
```

**Power-Path Operation:**
- USB connected: VSYS powered from USB, battery charges through BMS
- USB disconnected: VSYS powered from battery through BMS
- Battery depleted: VSYS powered from USB (BMS may be in cutoff, but charger power-path active)

---

## 3. Candidates Evaluated

### 3.1 TI BQ29200 + External MOSFETs

| Parameter | Value |
|-----------|-------|
| Function | 2S OVP/UVP + cell balancing |
| Overvoltage threshold | 4.35V ±25mV |
| Undervoltage threshold | 2.8V (typ) |
| Balance current | ~12mA (internal) |
| Balance method | Automatic voltage-based |
| Package | TSSOP-14 |
| Datasheet | [TI BQ29200](https://www.ti.com/lit/ds/symlink/bq29200.pdf) |

**Pros:**
- TI ecosystem matches BQ25792
- Automatic balancing, no MCU control needed
- Single IC for protection + balancing
- Well-documented

**Cons:**
- Low balance current (12mA) - slow for large packs
- Requires external charge/discharge MOSFETs
- TSSOP-14 larger footprint

---

### 3.2 HY2120-CB + HY2213-BB3A (HYCON Technology)

| Parameter | HY2120-CB | HY2213-BB3A |
|-----------|-----------|-------------|
| Function | 2S protection | Cell balancer |
| OVP threshold | 4.28V ±25mV | N/A |
| UVP threshold | 2.9V ±80mV | N/A |
| OCP detection | 200mV sense | N/A |
| Balance current | N/A | ~68mA |
| Balance threshold | N/A | 4.2V ±25mV |
| Package | SOT-23-6 | SOT-23-6 |

**Circuit (from reverse-engineered HX-2S-JH20):**
- 2× MOSFETs (e.g., CJ2310 N-channel) for charge/discharge switching
- 2× HY2213-BB3A (one per cell) for balancing
- Sense resistor for overcurrent detection

**Pros:**
- Very common, proven in countless products
- Good JLCPCB/LCSC availability
- Higher balance current than BQ29200
- Small SOT-23 packages

**Cons:**
- Two IC types (protection + balancer)
- More external components
- Less documentation than TI parts

---

### 3.3 S-82B2A/B (ABLIC)

| Parameter | Value |
|-----------|-------|
| Function | 2S protection only |
| OVP threshold | 4.275V / 4.350V options |
| UVP threshold | 2.3V - 3.0V options |
| Package | SNT-8A (small) |

**Verdict:** Protection only, no balancing. Would need separate balancer. Less common at JLCPCB.

---

## 4. Recommendation

### Primary: HY2120-CB + HY2213-BB3A

**Rationale:**
- Widely available at LCSC/JLCPCB
- Proven design (used in thousands of commercial BMS boards)
- 68mA balance current adequate for top-balancing
- Small footprint (SOT-23-6 packages)
- Well-understood failure modes

**BOM for 2S BMS:**
| Component | Part Number | Qty | Notes |
|-----------|-------------|-----|-------|
| Protection IC | HY2120-CB | 1 | 2S protection |
| Balancer IC | HY2213-BB3A | 2 | One per cell |
| Discharge MOSFET | CJ2310 / AO3400 | 2 | Dual N-ch or single dual-FET |
| Charge MOSFET | CJ2310 / AO3400 | 2 | Dual N-ch or single dual-FET |
| Balance resistor | 62Ω 1/8W | 2 | Sets balance current |
| Sense resistor | 5mΩ | 1 | Overcurrent detection |

### Alternate: TI BQ29200

Use if TI-only BOM preferred. Accept slower balancing (12mA vs 68mA).

---

## 5. Connection Diagram

```
                    ┌─────────────────────────────────────────┐
                    │                BQ25792                  │
                    │                                         │
   USB-C ──────────►│ VBUS                            VSYS ──►│──► 3.3V/5V regulators
                    │                                         │
                    │ BAT+ ◄──────────────────────────────────│──┐
                    │                                         │  │
                    └─────────────────────────────────────────┘  │
                                                                 │
    ┌────────────────────────────────────────────────────────────┘
    │
    │  Pack P+
    │  ┌─────────────────────────────────────────────────────────┐
    │  │                      2S BMS                              │
    ▼  │                                                          │
  ──●──┤ P+    ┌─────────┐    ┌─────────┐                        │
       │       │HY2213   │    │HY2213   │     ┌──────────┐       │
       │       │Cell 1   │    │Cell 2   │     │ HY2120-CB│       │
       │       │Balancer │    │Balancer │     │Protection│       │
       │       └────┬────┘    └────┬────┘     └────┬─────┘       │
       │            │              │               │              │
  ──●──┤ BM (mid) ──┴──────────────┴───────────────┘              │
       │                                                          │
  ──●──┤ P-    ◄──── Charge/Discharge MOSFETs ◄──── Sense R      │
       │                                                          │
       └──────────────────────────────────────────────────────────┘
           │              │              │
           ▼              ▼              ▼
        Cell 1+        Cell 1-        Cell 2-
        (8.4V)        / Cell 2+        (0V)
                      (4.2V mid)
```

---

## 6. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Balance current too low for severe imbalance | Extended charge time | Cells factory-matched; top-balance only needed |
| BMS in UVP cutoff during power-path operation | System brownout | BQ25792 VSYS_MIN prevents this; power-path from USB |
| MOSFET Rds(on) limits current | Voltage drop at high current | Select 10mΩ or lower MOSFETs |
| HY2120 availability | BOM risk | BQ29200 as alternate |

---

## 7. References

- [TI BQ29200 Datasheet](https://www.ti.com/lit/ds/symlink/bq29200.pdf)
- [HY2120 Datasheet](https://www.hycontek.com/wp-content/uploads/DS-HY2120_EN.pdf)
- [HX-2S-JH20 Reverse Engineering (GitHub)](https://github.com/megasaturnv/HX-2S-JH20-JH10-reverse-engineered_aka-2S-10A-Balance-Li-ion-Protection-PCB)
