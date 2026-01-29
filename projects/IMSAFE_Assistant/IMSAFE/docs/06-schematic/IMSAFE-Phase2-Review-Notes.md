# IMSAFE – Phase 2 Design Rationale Review Notes (Upload to Claude Code)

This document captures required corrections and additions based on the latest review of:

- `design-rationale.md`
- `IMSAFE_MainBoard_Schematic.pdf`

Claude Code should apply these updates before proceeding further in Phase 2.

---

## 1. Required Corrections (Fix Immediately)

### 1.1 I²C Pull-Up Calculation Conflict — ✅ RESOLVED

In Section 03 (BQ25792 Charger), the rationale derives:

- Rp_max ≈ 1.8 kΩ for the stated rise-time/bus capacitance target

…but then selects **4.7 kΩ**, which is inconsistent and likely too weak for 400 kHz operation.

**Decision (2026-01-29):** Keep 4.7 kΩ pull-ups.

**Justification:** Accept slower rise time; actual bus capacitance is lower than worst-case calculation. 100 kHz I²C standard mode is acceptable for sensor polling rates. If 400 kHz fast mode is required later, pull-ups can be changed during PCB spin.

---

### 1.2 USB Speed Claim Must Match Hardware — ✅ RESOLVED

The rationale states USB may operate at:

- Full Speed (12 Mbps) OR High Speed (480 Mbps)

However, the current design appears to implement **USB OTG FS only** unless an ULPI PHY is included.

**Decision (2026-01-29):** USB is **Full Speed (FS) only** (12 Mbps).

**Justification:** No ULPI PHY is included. FS is sufficient for data logging and configuration. Documentation updated to remove HS claims.

---

### 1.3 Balance Resistor Thermal Margin Must Be Resolved — ✅ RESOLVED

The balancing resistor calculation shows:

- ~284 mW dissipation into a 0.25 W 0805 part (marginal)

**Decision (2026-01-29):** Use **1206 package, 0.5 W rated resistor**.

**Action Required:**
- Update schematic: Change balancing resistor footprint from 0805 to 1206
- Update BOM with 0.5 W part (e.g., RC1206JR-07100RL or equivalent JLCPCB basic part)

**Deliverable:**
- Updated schematic + rationale entry

---

## 2. Mandatory Additions to Design Rationale Template

### 2.1 Sheet Cross-Reference Block (Per Section)

Each schematic section must include:

- KiCad sheet filename (e.g. `Section_03_Charger.kicad_sch`)
- Revision identifier (timestamp or git hash)
- Key netnames in/out (VBUS, VSYS, VBAT, SPI buses, IRQ lines)

This enables fast printed review against the PDF structure.

---

### 2.2 Layout-Critical Constraints (Per Section)

Add a “Layout-Critical Must-Do List” for every block, e.g.:

- Switch node keepout rules
- Kelvin sense routing
- Crystal guard ring + placement
- ESD diode placement distance
- Thermal pad via matrix requirements
- Controlled impedance where applicable

These are constraints that can break the design if ignored.

---

### 2.3 Open Items Table (Per Section)

Every section must end with an Open Items table:

| Open Item ID | Description | Owner | Status | Blocking? |
|------------|-------------|-------|--------|----------|
| OI-001 | Charger thermal validation | Dean | Open | Yes |

This prevents unresolved risks from being buried in prose.

---

### 2.4 Explicit Assumptions Section

Add a separate **Assumptions** subsection distinct from Risks, e.g.:

- Battery pack includes NTC
- USB limited to 5V input only
- Vibration sensor mounting location ≤105°C
- SD card socket uses locking mechanism

Assumptions form the audit trail for later review.

---

### 2.5 Manufacturability / JLC Constraints Field

For every IC and critical component include:

- JLC/LCSC part number
- Basic vs Extended assembly class
- Package risk (QFN/BGA fine pitch)
- Second-source alternates

---

## 3. Work Hook Gating Update (Phase Completion Rules)

Phase 2 schematic blocks must NOT be marked complete unless:

1. Design rationale entry exists
2. Diff has been applied to working tree
3. KiCad ERC has been run via `kicad-cli sch erc`
4. ERC errors = 0
5. Any warnings are explicitly waived with justification
6. PDF export generated for human print review
7. Dean has signed off Proposed vs Approved decisions

---

## 4. Next Claude Code Actions

Claude Code should now:

1. Apply the corrections in Section 1
2. Update the design rationale template with additions in Section 2
3. Ensure the Work Hook gating rules in Section 3 are enforced
4. Resubmit updated diffs for human review before any further schematic expansion

---

**End of Review Notes**
