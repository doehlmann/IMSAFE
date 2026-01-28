# IMSAFE – Work Hook (GUPP Queue)

## Goal
Turn the IMSAFE Functional Spec into:
1) a component shortlist + decisions,
2) schematic blocks per feature,
3) an integrated PCB plan (single-board vs multi-board),
4) initial KiCad projects ready for capture/layout.

Source of truth: docs/00-functional-spec/IMSAFE-Functional-Spec.md (Rev A, 2026-01-27).

---

## Definition of Done (DoD)
- [x] Repo structure exists and all current docs/datasheets are ingested + categorized.
- [x] Every feature in the spec has:
  - a named schematic block (inputs/outputs, interfaces, rails),
  - a selected primary component + at least 1 alternate,
  - a short decision record (why chosen, risks, availability, footprint/package).
- [x] Open contradictions/unknowns in the spec are resolved or explicitly tracked.
- [ ] A single integration choice is made:
  - Single PCB OR multi-PCB (and why),
  - with defined interconnects.
- [ ] KiCad projects created (even if only skeleton).
- [x] Initial verification plan exists that maps to integration tests in spec.

---

## Operating Rules (for multi-agent work)
### Coordinator rules
- Work only by advancing unchecked items below.
- Any design decision must be recorded in docs/03-component-selection/decision-log.md with:
  date, decision, options considered, selection criteria, risks, and links to datasheets.

### Worker freedom policy (important)
Workers MAY propose better components than those named in the spec.
But every proposal must include:
- at least 2 candidates (primary + alternate),
- a short trade study against our requirements,
- and a datasheet link/file path to store under docs/04-datasheets/.

### Design Rules
- **Minimum component size: 0603** (unless no alternative exists)
- **MCU package: LQFP** (hand-solderable)
- **4-layer PCB** recommended for STM32H7

---

## Phase 0 — Repo + Document Ingest ✓ COMPLETE
- [x] Create/confirm repo structure
- [x] Create docs folders
- [x] Place functional spec
- [x] Add WORK_HOOK.md and commit
- [x] Add .gitignore and .gitattributes
- [x] Decide Git LFS policy and enable

---

## Phase 1 — Requirements & Unknowns Resolution ✓ COMPLETE
- [x] Extract requirements into structured form (requirements.md)
- [x] Create verification-plan.md
- [x] Battery sizing mismatch → **16000mAh 2S Li-ion (DEC-001)**
- [x] MCU architecture decision → **STM32H747AG single MCU (DEC-002A)**
- [x] Vibration IMUs → **ADXL355 (engine) + ADXL372 (gearbox) (DEC-003)**
- [x] Storage decision → **microSD only with FATFS (DEC-004)**

---

## Phase 2 — Component Selection ✓ COMPLETE (30 Decisions)

### Decision Summary
| DEC | Component | Selection |
|-----|-----------|-----------|
| 001 | Battery | 7.4V 16000mAh 2S Li-ion |
| 002A | MCU | STM32H747AG (LQFP-176) |
| 003 | Vibration | ADXL355 + ADXL372 |
| 004 | Storage | microSD (GCT MEM2067) |
| 005 | Radar | De-risked strategy |
| 006 | Charger | BQ25792 |
| 007 | 3.3V Reg | TPS62133 |
| 008 | USB ESD | USBLC6-2SC6 |
| 009 | 5V Reg | TPS62130 |
| 010 | BMS | HY2120 + HY2213 |
| 011 | Barometric | BMP390 |
| 012 | IMU | ICM-42688-P |
| 013 | Magnetometer | MMC5983MA |
| 014 | Gas Sensor | MICS-4514 |
| 015 | Temp/Humidity | Si7021 |
| 016 | Display | ST7789 3.5" TFT |
| 017 | Switches | MTS-102 ×8 |
| 018 | LEDs | WS2812B-2020 ×16 |
| 019 | Encoders | EC11 ×2 |
| 020 | I/O Expander | MCP23017 |
| 021 | SWD | Tag-Connect + Header |
| 022 | Bluetooth | ESP32-C3-MINI-1 |
| 023 | HSE Crystal | 8 MHz |
| 024 | LSE Crystal | 32.768 kHz |
| 025 | USB-C | 16-pin receptacle |
| 026 | Battery Conn | XT30 + JST-XH |
| 027 | Sensor Conn | M8 4-pin IP67 |
| 028 | Buzzer | MLT-8530 |
| 029 | Power Switch | SS12D00 + soft |
| 030 | Protection | PTC + TVS |

**Full details:** docs/03-component-selection/decision-log.md
**BOM:** docs/05-bom/IMSAFE-BOM-v1.md (~$141 PCB BOM)

---

## Phase 3 — Schematic Development (CURRENT PHASE)

### Reference Document
**docs/06-schematic/schematic-sections.md** — Full breakdown with all passives

### Schematic Sections (20 total, ~153 passives)

| # | Section | Main IC | Passives | Status |
|---|---------|---------|----------|--------|
| 1 | MCU Core | STM32H747AG | 28 | [ ] Pending |
| 2 | USB-C Input | - | 4 | [ ] Pending |
| 3 | Battery Charger | BQ25792 | 16 | [ ] Pending |
| 4 | BMS | HY2120/HY2213 | 9 | [ ] Pending |
| 5 | 3.3V Regulator | TPS62133 | 7 | [ ] Pending |
| 6 | 5V Regulator | TPS62130 | 9 | [ ] Pending |
| 7 | Barometric | BMP390 | 2 | [ ] Pending |
| 8 | IMU | ICM-42688-P | 4 | [ ] Pending |
| 9 | Magnetometer | MMC5983MA | 3 | [ ] Pending |
| 10 | Gas Sensor | MICS-4514 | 9 | [ ] Pending |
| 11 | Temp/Humidity | Si7021 | 1 | [ ] Pending |
| 12 | Display | ST7789 FPC | 7 | [ ] Pending |
| 13 | microSD | MEM2067 | 9 | [ ] Pending |
| 14 | I/O Expander | MCP23017 | 5 | [ ] Pending |
| 15 | Status LEDs | WS2812B ×16 | 17 | [ ] Pending |
| 16 | Encoders | EC11 ×2 | 6 | [ ] Pending |
| 17 | Bluetooth | ESP32-C3 | 5 | [ ] Pending |
| 18 | Vibration I/F | M8 conn | 8 | [ ] Pending |
| 19 | Audio | MLT-8530 | 4 | [ ] Pending |
| 20 | Debug | Tag-Connect | 0 | [ ] Pending |

### Workflow for Each Section
1. **Read datasheet** application circuit
2. **List all passives** with values, packages (≥0603)
3. **Assign reference designators** per section numbering
4. **Define net names** (use convention in schematic-sections.md)
5. **Create schematic sheet** in KiCad/EasyEDA
6. **Run ERC** on section
7. **Mark complete** in this table

### Priority Order (Power first, then dependencies)
```
1. Section 2: USB-C Input (power entry)
2. Section 3: Battery Charger (BQ25792)
3. Section 4: BMS
4. Section 5: 3.3V Regulator
5. Section 6: 5V Regulator
6. Section 1: MCU Core (needs power rails)
7. Sections 7-11: Sensors (need MCU I2C/SPI)
8. Sections 12-16: UI/Display
9. Sections 17-20: Connectivity/Debug
```

---

## Phase 4 — Integration: single PCB vs multi-PCB
- [ ] Decide partitioning:
  - Candidate: Single main PCB + separate vibration sensor pods
  - Switch/LED panel: integrated or separate?
- [ ] Define connectors between boards (pinout, power, I2C/SPI, mechanical)
- [ ] Confirm PCB constraints (4-layer, dimensions for 3.5" display)

---

## Phase 5 — Verification hooks
- [x] Verification plan created (docs/01-requirements/verification-plan.md)
- [ ] Define "smoke tests" for first bring-up:
  - USB enumeration
  - Sensor reads (I2C scan)
  - LED test pattern
  - SD card mount
  - BLE advertising
- [ ] Manufacturing test checklist

---

## Current Task
**Phase 3: Schematic Development**
- Start with power sections (2-6)
- Work through each section systematically
- Document all passive values in schematic-sections.md as you go

---

## Quick Links
| Document | Path |
|----------|------|
| Decision Log | docs/03-component-selection/decision-log.md |
| BOM | docs/05-bom/IMSAFE-BOM-v1.md |
| Schematic Sections | docs/06-schematic/schematic-sections.md |
| Requirements | docs/01-requirements/requirements.md |
| Verification Plan | docs/01-requirements/verification-plan.md |
