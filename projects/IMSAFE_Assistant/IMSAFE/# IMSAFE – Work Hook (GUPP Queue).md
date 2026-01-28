# IMSAFE – Work Hook (GUPP Queue)

## Goal
Turn the IMSAFE Functional Spec into:
1) a component shortlist + decisions,
2) schematic blocks per feature,
3) an integrated PCB plan (single-board vs multi-board),
4) initial KiCad projects ready for capture/layout.

Source of truth: docs/00-functional-spec/IMSAFE-Functional-Spec.md (Rev A, 2026-01-27).  (see Key Features list and architecture/testing sections) :contentReference[oaicite:3]{index=3} :contentReference[oaicite:4]{index=4}

---

## Definition of Done (DoD)
- [ ] Repo structure exists and all current docs/datasheets are ingested + categorized.
- [ ] Every feature in the spec has:
  - a named schematic block (inputs/outputs, interfaces, rails),
  - a selected primary component + at least 1 alternate,
  - a short decision record (why chosen, risks, availability, footprint/package).
- [ ] Open contradictions/unknowns in the spec are resolved or explicitly tracked (e.g., battery sizing mismatch, “best unit unknown” IMU for vibration, MCU split vs single). :contentReference[oaicite:5]{index=5} :contentReference[oaicite:6]{index=6} :contentReference[oaicite:7]{index=7}
- [ ] A single integration choice is made:
  - Single PCB OR multi-PCB (and why),
  - with defined interconnects.
- [ ] KiCad projects created (even if only skeleton):
  - power + MCU/core + IO expansion,
  - UI (switches/LEDs/encoders),
  - sensors,
  - display,
  - storage,
  - radar interface,
  - BLE.
- [ ] Initial verification plan exists that maps to integration tests in spec (USB enum, logging, low battery behavior, etc.). :contentReference[oaicite:8]{index=8}

---

## Operating Rules (for multi-agent work)
### Coordinator rules
- Work only by advancing unchecked items below.
- Any design decision must be recorded in docs/03-component-selection/decision-log.md with:
  date, decision, options considered, selection criteria, risks, and links to datasheets.

### Worker freedom policy (important)
Workers MAY propose better components than those named in the spec (e.g., alternatives to BMP390, MiCS-4514, MCP23017, ST7796, etc.). :contentReference[oaicite:9]{index=9}  
But every proposal must include:
- at least 2 candidates (primary + alternate),
- a short trade study against our requirements (voltage, current, temp range, interface, package/assembly, availability),
- and a datasheet link/file path to store under docs/04-datasheets/.

### No shared state between workers
Workers do NOT update decisions directly. They produce:
- a “recommendation memo” + links,
- or a unified diff if we use patch-worker.
Coordinator applies and logs.

---

## Phase 0 — Repo + Document Ingest (do this first)
- [ ] Create/confirm repo structure for:
  docs/, hardware/, firmware/, mechanical/, tools/, references/
- [ ] Create docs folders:
  - docs/00-functional-spec/
  - docs/01-requirements/
  - docs/02-architecture/
  - docs/03-component-selection/
  - docs/04-datasheets/{mcu,power,sensors,display,storage,rf,connectors,mechanical}/
- [ ] Place functional spec at:
  docs/00-functional-spec/IMSAFE-Functional-Spec.md
- [ ] Add WORK_HOOK.md (this file) and commit.
- [ ] Add .gitignore (KiCad, build artifacts) and .gitattributes (Git LFS rules for PDFs/STEP).
- [ ] Decide Git LFS policy for PDFs/STEP and enable if chosen.

---

## Phase 1 — Requirements & Unknowns Resolution
### 1.1 Extract requirements into structured form
- [ ] Create docs/01-requirements/requirements.md:
  - include power targets (active current, sleep current, supply rail) and env targets. :contentReference[oaicite:10]{index=10}
- [ ] Create docs/01-requirements/verification-plan.md:
  - map to integration tests listed in the spec (USB enum, logging, low battery, charging, factory reset). :contentReference[oaicite:11]{index=11}

### 1.2 Resolve spec contradictions / unknowns (log outcomes)
- [ ] Battery sizing mismatch:
  - Feature list says “7.4V 16000 mAh backup” :contentReference[oaicite:12]{index=12}
  - But power section uses “500mAh Li-Po” for estimation. :contentReference[oaicite:13]{index=13}
  Decide target battery class and update requirements.
- [ ] MCU architecture decision:
  - Spec discusses XMC4700 for radar and possibly a second MCU for other functions. :contentReference[oaicite:14]{index=14}
  Decide single vs dual MCU (or MCU + coprocessor) and capture trade-offs.
- [ ] Vibration IMUs: “best unit unknown” :contentReference[oaicite:15]{index=15}
  Define measurement bandwidth, mounting method, interface, and candidate sensors.
- [ ] Storage decision: spec mentions microSD (feature list) :contentReference[oaicite:16]{index=16}
  but detailed section includes external flash W25Q128. :contentReference[oaicite:17]{index=17}
  Decide: microSD only, flash only, or both.

---

## Phase 2 — Component Selection (per feature)
For each block, produce:
- primary + alternate,
- “why” summary,
- and save datasheets.

### 2.1 Core compute + debug/programming
- [ ] MCU shortlist for whole system (including display + logging + IMU fusion + radar integration).
- [ ] SWD/programming connector approach (tag-connect, pogo, etc.) consistent with layout. (SWD noted in architecture) :contentReference[oaicite:18]{index=18}

### 2.2 Power & battery
- [ ] USB-C: decide if USB-PD is required or basic USB-C 5V is enough (spec shows “USB PD IC” in block diagram). :contentReference[oaicite:19]{index=19}
- [ ] Charger + power-path IC (battery backup + operation while charging).
- [ ] 3.3V rail design (buck vs LDO, quiescent current target).
- [ ] Protection: TVS/ESD plan for USB, battery safety plan. :contentReference[oaicite:20]{index=20}

### 2.3 UI: switches, LEDs, encoders
- [ ] Toggle switches: selection + footprint constraints + mounting.
- [ ] IO expander: MCP23017 (or equivalent) selection and alternates. :contentReference[oaicite:21]{index=21}
- [ ] WS2812 LEDs: packaging choice + current budget implications. :contentReference[oaicite:22]{index=22}
- [ ] Encoders: PEC12R-4130F-S0012 noted; confirm alternates/availability. :contentReference[oaicite:23]{index=23}

### 2.4 Sensors
- [ ] Barometric pressure: BMP390 candidate + alternates. :contentReference[oaicite:24]{index=24}
- [ ] IMU + compass for synthetic display (Pixracer-style mentioned). :contentReference[oaicite:25]{index=25}
- [ ] CO/NO2 sensor: MiCS-4514 candidate + alternates + analog front-end needs. :contentReference[oaicite:26]{index=26}
- [ ] Cabin temperature sensor (explicit feature). :contentReference[oaicite:27]{index=27}
- [ ] Vibration IMUs (engine + gearbox) shortlist. :contentReference[oaicite:28]{index=28}

### 2.5 Display + graphics pipeline
- [ ] 3.5" TFT: ST7796 interface decision (SPI vs parallel) + connector choice. :contentReference[oaicite:29]{index=29}

### 2.6 Storage
- [ ] microSD subsystem (socket + level shifting if needed) and/or external flash (W25Q128) decision. :contentReference[oaicite:30]{index=30} :contentReference[oaicite:31]{index=31}

### 2.7 Connectivity
- [ ] Bluetooth: module vs SoC vs integrated MCU; antenna constraints. :contentReference[oaicite:32]{index=32}

### 2.8 Landing radar integration
- [ ] Interface plan to Infineon BGT24LTR11 shield / Distance2GoL (UART/SPI/etc), power domains, mounting. :contentReference[oaicite:33]{index=33}

---

## Phase 3 — Schematic Blocks (one per “feature cluster”)
- [ ] Schematic block: Power entry (USB-C) + charger + battery + rails
- [ ] Schematic block: MCU core + clocking + SWD + reset + boot
- [ ] Schematic block: I2C bus (pullups, addressing plan) + IO expander
- [ ] Schematic block: WS2812 LED chain + power injection strategy
- [ ] Schematic block: Toggles + encoders (debounce/ESD)
- [ ] Schematic block: Display interface + connector + backlight power
- [ ] Schematic block: Storage (microSD and/or flash) + ESD
- [ ] Schematic block: Sensor suite (baro, IMU/compass, cabin temp, gas sensor AFE)
- [ ] Schematic block: BLE subsystem
- [ ] Schematic block: Radar interface + any level shifting/isolations

Each block gets:
- named nets,
- power rails,
- test points,
- and a “bring-up checklist”.

---

## Phase 4 — Integration: single PCB vs multi-PCB
- [ ] Decide partitioning:
  - candidate multi-board split: (1) main board (MCU/power/display), (2) switch+LED board, (3) radar interface (if needed).
- [ ] Define connectors between boards (pinout, power, I2C/SPI, mechanical).
- [ ] Confirm PCB constraints (spec includes 4-layer, 40×30mm, etc—verify if still applicable to a 3.5" screen assembly). :contentReference[oaicite:34]{index=34}

---

## Phase 5 — Verification hooks (keep us honest)
- [ ] Define “smoke tests” for first bring-up matching spec tests:
  - USB enumeration, sensor reads, logging, low battery warning/shutdown, charging indicator. :contentReference[oaicite:35]{index=35}
- [ ] Add a manufacturing test checklist aligned to the production test table. :contentReference[oaicite:36]{index=36}

---

## Next task to run (coordinator picks one)
Start with: Phase 0 items until repo/doc ingest is complete.
