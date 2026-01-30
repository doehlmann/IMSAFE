# Vibration Sensor Pod — Enclosure + Connector Strategy (IP68, No Potting, Serviceable)

This document captures the agreed mechanical packaging approach for the **gearbox/motor vibration sensor pods**:

- **No potting at this stage**
- **IP68 sealing**
- **Aluminium enclosure**
- **Robust connectoring**
- **Serviceable cable strategy**
- Designed for high-temperature automotive-grade electronics (Grade 1 / Grade 0 parts)

---

## 1. Key Requirements

- Mounted on **gearbox** and **motor** housings for vibration frequency analysis
- Ambient temperature target: **up to +125 °C (Grade 1)**
- Cable length: **~2 m**
- Communications: **CAN‑FD over twisted pair**
- Supply: **+5V VSYS**
- Field maintainability is important

---

## 2. Potting Decision

### Current approach
✅ **No potting for development and early deployment**

Reason:
- Allows rework, debugging, sensor replacement
- Avoids trapped heat and thermal stress during cycling
- Potting can be added later selectively if required

Baseline protection instead:
- **Conformal coating** (high‑temp silicone or urethane)

---

## 3. Enclosure Material and Fatigue Considerations

### Aluminium body is appropriate
- Die‑cast or machined aluminium provides:
  - Excellent mechanical rigidity
  - EMI shielding
  - Heat spreading
  - Long-term durability

### Work hardening concern
Work hardening is not a practical issue for a rigid enclosure.

Real fatigue risks come from:
- Stress concentrators at mounting ears
- Thin sections around connector bosses
- Sharp internal corners

Design rules:
- Use generous radii
- Avoid thin mounting tabs
- Use threaded inserts if repeated servicing is expected
- Apply anodizing or corrosion-resistant finish

---

## 4. Box Shape vs “Fancy” Heat Sink Geometry

### Recommendation: simple sealed box with thermal backplane
A complex finned heatsink is unnecessary at pod power levels.

Best compromise:
- **Flat-backed aluminium enclosure**
- PCB thermally coupled to the back wall with a **thermal pad**
- Back wall acts as:
  - Heat spreader
  - Mounting interface
  - Thermal conduction path into gearbox/motor casing

External fins only become useful if:
- Pod is mounted on an insulating bracket
- Pod is in free air without conduction into a large mass

---

## 5. Connector Strategy (Robust + IP68)

### Selected connector family
✅ **M8 A‑coded 5‑pin**, silicone-sheathed cable

Reasons:
- Compact, rugged industrial connector
- Excellent sealing and vibration tolerance
- Widely available molded cable assemblies
- Provides one spare pin for shield/drain or SYNC

### Recommended pin assignment
| Pin | Signal |
|---:|--------|
| 1 | +5V VSYS |
| 2 | CANH |
| 3 | GND |
| 4 | CANL |
| 5 | Shield/Drain (or SYNC spare) |

Cable:
- Shielded twisted pair for CANH/CANL
- Separate power pair within same cable bundle

---

## 6. Bulkhead Connector vs Flylead

### Decision: Bulkhead connector on pod (recommended)

#### Bulkhead M8 connector on enclosure wall
**Pros**
- Cable is replaceable without opening pod
- Cleaner IP68 sealing (connector O‑ring rated)
- Better strain handling under vibration
- Pod assembly and cable assembly remain separate
- Field servicing is straightforward

**Cons**
- Slightly higher BOM cost
- Requires proper mechanical support in housing

✅ **Preferred for aviation/industrial maintenance systems**

---

### Flylead exiting pod (not recommended)

**Cons**
- Sealing depends on gland + overmold quality
- Cable damage requires replacing entire pod
- Fatigue risk at exit point unless heavily strain relieved

Use only for low-cost disposable sensors, not serviceable pods.

---

## 7. Additional Practical Mechanical Notes

To achieve real IP68 reliability:

- Use proper **O‑ring groove**, not flat gasket only
- Add **pressure equalization vent membrane** if strong thermal cycling expected
- Provide internal strain relief even with bulkhead connector
- Do not allow connector loads to flex the PCB
- Bond cable shield at main enclosure end (single-point) to avoid ground loops

---

## 8. Final Recommended Pod Packaging Stack

**Development → Early Production Strategy**

- Die‑cast aluminium IP68 enclosure
- Flat thermal mounting backplane
- Conformal coating (no potting)
- Bulkhead M8 5‑pin connector
- Removable 2 m molded shielded cable assembly
- Thermal pad coupling PCB to enclosure back wall
- Optional vent membrane for breathing

---

## 9. Next Mechanical Outputs (if required)

If desired, next deliverables can include:
- Specific enclosure part suggestions (Hammond, Takachi, Bopla)
- Mounting footprint + bolt pattern recommendation
- Connector BOM (Grade 1 rated parts)
- Full pod mechanical drawing for CAD integration

---

**This document is formatted for direct use in Claude Code as the enclosure/interface design basis.**
