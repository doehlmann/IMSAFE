# IMSAFE – Component Selection Decision Log

This document records all significant component and architectural decisions
made during the IMSAFE design process.

Purpose:
- Preserve design intent
- Enable traceability from requirements → components
- Support future revisions, audits, and redesigns

All entries must reference:
- Functional spec section(s)
- Requirements (electrical, mechanical, environmental)
- Datasheets stored under docs/04-datasheets/

---

## Decision Record Template

### Decision ID
IMSAFE-DEC-XXX

### Date
YYYY-MM-DD

### Feature / Subsystem
(e.g. Power Management, MCU Core, Barometric Sensor, UI Switches)

### Related Spec Sections
(List section numbers or headings from IMSAFE-Functional-Spec.md)

### Context / Problem Statement
(Why a decision was required. Constraints, unknowns, risks.)

### Options Considered
| Option | Description | Key Pros | Key Cons |
|------|-------------|----------|----------|
| A | | | |
| B | | | |
| C | | | |

### Selection Criteria
(Check all that apply and explain relevance)
- [ ] Electrical compatibility (voltage, current)
- [ ] Performance (accuracy, speed, noise, bandwidth)
- [ ] Environmental rating (temp, vibration)
- [ ] Package / assembly constraints
- [ ] Power consumption
- [ ] Availability / lifecycle
- [ ] Cost (secondary unless critical)
- [ ] Firmware complexity
- [ ] Integration risk

### Decision
(Chosen component + package + manufacturer)

### Rationale
(Why this option best satisfies requirements)

### Alternates / Second Source
(Approved fallback options)

### Risks / Open Issues
(What could go wrong, mitigation plan)

### Datasheets / References
- docs/04-datasheets/<category>/<file>.pdf
- External links (if relevant)

### Downstream Impact
- PCB layout impact:
- Firmware impact:
- Power budget impact:
- Manufacturing impact:

### Status
- [ ] Proposed
- [ ] Approved
- [ ] Superseded (link to new decision)

---

### IMSAFE-DEC-001
**Date:** 2026-01-28
**Feature:** Power Management - Battery Sizing
**Context:** Spec contradiction: Key Features lists "7.4V 16000mAh Li-ion" but power estimation uses "500mAh Li-Po".
**Options:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | 16000mAh 2S Li-ion | Long runtime, matches spec | Large, heavy |
| B | 2000-4000mAh 2S Li-Po | Smaller, lighter | Shorter runtime |
| C | 500mAh Li-Po | Compact | Insufficient |

**Decision:** Option A: 7.4V 16000mAh 2S Li-ion pack
**Rationale:** Spec explicitly states 16000mAh; 500mAh was placeholder; GA flights need large capacity
**Alternates:** Option B acceptable for prototypes
**Risks:** Actual power TBD, form factor, charging time
**Status:** [x] Proposed

**OPEN ISSUES (Phase 2 Review Required):**
- [ ] Complete power budget analysis to validate capacity requirement
- [ ] USB-C charging architecture: boost to 7.4V pack OR charge cells individually (balance charging)
- [ ] Charging IC selection depends on architecture decision
- [ ] Safety/protection circuit requirements for 2S Li-ion

---

### IMSAFE-DEC-002 (SUPERSEDED)
**Date:** 2026-01-28
**Feature:** MCU Architecture
**Status:** Superseded by IMSAFE-DEC-002A

---

### IMSAFE-DEC-002A
**Date:** 2026-01-28
**Feature:** MCU Architecture - Final Selection
**Related Spec Sections:** All system functions, radar integration (DEC-005)

**Context:**
Following radar integration analysis (DEC-005), a single high-performance MCU can handle all functions including radar processing. The STM32H747 dual-core architecture provides M7 for radar/DSP and M4 for UI/sensors.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | STM32H747AG | Dual-core M7+M4, LTDC, 2MB flash, 1MB RAM | Complex, high power |
| B | Dual MCU (XMC4700 + STM32G4) | Proven radar, clear separation | Two toolchains, inter-MCU protocol |
| C | STM32H743 | Single M7 core, simpler | No dedicated core for UI |

**Selection Criteria:**
- [x] Performance: M7 @ 480MHz handles radar DSP
- [x] Integration: Single toolchain (STM32CubeIDE)
- [x] Graphics: Built-in LTDC for display
- [x] Firmware complexity: Easier than dual-MCU protocol

**Decision:** STM32H747AG (LQFP-176)

**Rationale:**
- Dual-core allows clean separation: M7 for radar/DSP, M4 for sensors/UI/logging
- Hardware semaphores (HSEM) for inter-core communication (no external protocol)
- Built-in LTDC for display, SDMMC for storage, multiple I2C/SPI
- Single STM32CubeIDE toolchain
- Consolidates DEC-005 primary path (full STM32H7 radar integration)

**Alternates:** STM32H757 (crypto accelerator), STM32H745 (cost-reduced)

**Risks / Open Issues:**
- [x] Radar porting effort (addressed in DEC-005 with fallback strategy)
- [ ] Power consumption under full load (~500mA @ 480MHz)
- [ ] LQFP-176 routing complexity (4-layer PCB minimum)
- [ ] CubeMX dual-core configuration learning curve

**Datasheets / References:**
- docs/04-datasheets/mcu/STM32H747AG-summary.md
- docs/04-datasheets/mcu/stm32h747ag.pdf

**Downstream Impact:**
- PCB: LQFP-176 requires careful power plane design
- Firmware: FreeRTOS AMP configuration for dual-core
- Power: Higher current draw than STM32G4, budget updated

**Status:**
- [ ] Proposed
- [x] Approved (2026-01-28)

---

### IMSAFE-DEC-003
**Date:** 2026-01-28 (Revised)
**Feature:** Vibration Analysis - External IMUs
**Related Spec:** FR-VB-001, FR-VB-002, FR-VB-003

**Context:**
Engine/gearbox vibration monitoring for Rotax 912 series (4200-5800 RPM). This is a key differentiating feature requiring proper engineering, not "this will do" approach. Different mounting locations have different requirements - purpose-matched sensors.

**Target Application:**
- Rotax 912: 4200-5800 RPM = 70-97 Hz fundamental
- 6X harmonic at redline: 580 Hz
- Engine case surface temp: 80-120°C
- Gearbox surface temp: 60-90°C (cooler)

**Dual-Sensor Philosophy:**

| Location | Environment | Primary Need | Sensor |
|----------|-------------|--------------|--------|
| Engine case | Hot (80-120°C), continuous | Precision harmonics, subtle trending | ADXL355 |
| Gearbox | Cooler (60-90°C), impulsive | Shock capture, gear mesh, impacts | ADXL372 |

**Options Evaluated:**
| Sensor | Temp Rating | Range | Bandwidth | Noise | Cost | Best For |
|--------|-------------|-------|-----------|-------|------|----------|
| LIS3DH | +85°C | ±16g | 5 kHz | 220 µg/√Hz | $0.50 | **REJECTED** - temp |
| ADXL345 | +85°C | ±16g | 3.2 kHz | 290 µg/√Hz | $2 | **REJECTED** - temp |
| ADXL355 | +125°C | ±8g | 1.5 kHz | 20 µg/√Hz | $65 | **Engine** - metrology |
| ADXL372 | +105°C | ±200g | 3.2 kHz | 6.7 mg/√Hz | $10 | **Gearbox** - shock |
| ADXL1002 | +125°C | ±50g | 11 kHz | 25 µg/√Hz | $20 | V2 upgrade |

**Decision:** ADXL355 (engine) + ADXL372 (gearbox)

**Engine Mount - ADXL355:**
- [x] Temperature: +125°C ≥ 120°C engine case
- [x] Noise: 20 µg/√Hz (best-in-class MEMS) for subtle fault detection
- [x] DC stable: Excellent for trending amplitude changes over time
- [x] Metrology-grade: Detects combustion harmonics, developing imbalance

**Gearbox Mount - ADXL372:**
- [x] Temperature: +105°C ≥ 90°C gearbox surface
- [x] Range: ±200g captures gear tooth impacts without saturation
- [x] Bandwidth: 3.2 kHz for gear mesh frequencies
- [x] Shock: Won't clip on hard landings, engine start transients
- [x] Cost: ~$10 vs $65 (total system cost reduced)

**Why Not Same Sensor for Both:**
- ADXL355 on gearbox: ±8g max would saturate on gear mesh impacts
- ADXL372 on engine: +105°C marginal for hot case; high noise floor misses subtle changes

**Mounting Strategy:**
- Common sensor pod design (aluminum housing, stud mount base)
- Same PCB footprint with different IC (both SPI interface)
- Stud mount (10-32 or 1/4-28, 30 in-lbs torque)
- Shielded SPI cable, max 1m
- IP67 M8 connectors for engine bay

**FFT Parameters (Firmware):**

| Parameter | Engine (ADXL355) | Gearbox (ADXL372) |
|-----------|------------------|-------------------|
| Sample rate | 3.2 kHz | 6.4 kHz |
| FFT size | 512 pts | 1024 pts |
| Resolution | 6.25 Hz | 6.25 Hz |
| Window | Hanning | Hanning |
| Focus | 0.5X-6X harmonics | Gear mesh, bearing |

**Cost Impact:**
- 1× ADXL355: $65
- 1× ADXL372: $10
- Sensor pods + cabling: ~$90
- Total vibration subsystem: **~$165** (down from $220)

**Upgrade Path:**
- V2: 3× ADXL1002 per location for 11 kHz bandwidth
- Professional: ICP piezoelectric for certified/HUMS applications

**Datasheets / References:**
- docs/03-component-selection/vibration-sensor-trade-study.md (full analysis)
- [ADI ADXL355](https://www.analog.com/en/products/adxl355.html)
- [ADI ADXL372](https://www.analog.com/en/products/adxl372.html)

**Status:**
- [ ] Proposed
- [x] Approved (Dual-sensor strategy)

---

### IMSAFE-DEC-004
**Date:** 2026-01-28 (Updated)
**Feature:** Data Storage
**Context:** Spec mentions microSD but also W25Q128 flash. Decided microSD for user-accessible flight logs.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | MicroSD only | Large, removable, PC-readable | Slower, vibration concern |
| B | W25Q128 only | Fast, solid state | 16MB limit, not user-accessible |
| C | Both | Best of both | Complexity |

**Decision:** Option A: MicroSD only via SDMMC interface

**Socket Selection (Vibration-Resistant):**

| Type | Model | Notes |
|------|-------|-------|
| **Hinged (Recommended)** | GCT MEM2067 | Locking shell, -40°C to +85°C, best vibration resistance |
| Push-Lock | JAE ST12 series | Mechanical lock, 1.4mm height |
| Push-Push | Molex 502570 | Standard, 10k cycles, adequate with proper mounting |

**Recommendation:** Hinged type (GCT MEM2067 or equivalent) for aircraft vibration environment

**Filesystem Decision:**

| Filesystem | Power-Loss Safe | PC Compatible | RAM Usage |
|------------|-----------------|---------------|-----------|
| FATFS | Moderate (2-FAT copy) | Yes | ~4KB |
| LittleFS | Excellent (copy-on-write) | No (tools needed) | ~1KB |

**Recommendation:** FATFS with mitigation strategy:
- User needs PC-readable logs without special tools
- Enable 2-FAT table copy (default in many implementations)
- Sync after each complete log entry
- Use write buffering (aggregate 4KB blocks before write)
- Implement safe shutdown on power loss detection (via BQ25792 VBUS monitoring)

**ESD Protection:**
- USBLC6-2SC6 on DAT0-DAT3, CMD, CLK lines (or PESD0402-140 array)
- Series resistors (22Ω-33Ω) for impedance matching and ESD current limiting

**STM32H747 Interface:**
- Use SDMMC1 peripheral (4-bit mode)
- DMA for efficient transfers
- Card detect pin for hot-swap handling

**Write Strategy:**
- Buffer log entries in RAM (STM32H747 has 1MB SRAM)
- Flush to card every 1 second or when buffer reaches 4KB
- Sync (f_sync) after each flush
- Log checksum for corruption detection

**Card Recommendations:**
- Industrial-grade microSD (e.g., SanDisk Industrial, ATP, Swissbit)
- MLC or pSLC preferred over TLC for endurance
- Minimum Class 10 / U1 rating

**Fallback Behavior:**
- If card removed mid-flight: continue operation, buffer in RAM
- LED indicator for card status
- Resume logging when card reinserted

**Datasheets / References:**
- docs/03-component-selection/storage-trade-study.md (full analysis)
- [GCT MEM2067](https://gct.co/connector/mem2067)
- [ATP Industrial microSD](https://www.atpinc.com/products/industrial-microsd-cards)

**Status:**
- [ ] Proposed
- [x] Approved (Open issues resolved)

---

### IMSAFE-DEC-005
**Date:** 2026-01-28
**Feature:** Radar Integration Architecture - Development Strategy
**Related Spec Sections:** FR-RD-001, FR-RD-002, FR-RD-003, IMSAFE-DEC-002, IMSAFE-ANALYSIS-001

**Context:**
The radar-migration-analysis.md (IMSAFE-ANALYSIS-001) identified 6-10 weeks of effort to port Distance2GoL firmware from XMC4700 to STM32H7. However, a working USB CDC interface already exists (d2gl_radar.py) that provides processed radar data (range, velocity, strength) from the existing baseboard hardware.

A de-risked development approach allows attempting full integration while preserving a working fallback.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | Full STM32H7 integration (single board) | Single PCB, lower BOM, clean design | 6-10 week port effort, timing-critical debugging |
| B | Keep XMC4700 baseboard, USB interface | Proven working, minimal effort | Two boards, USB cable, larger form factor |
| C | De-risked: Attempt A with B as fallback | Best of both - attempt integration with guaranteed working fallback | Requires designing for both options |

**Selection Criteria:**
- [x] Integration risk - fallback ensures project not blocked by radar porting
- [x] Development velocity - can proceed with main MCU features while radar integration ongoing
- [x] Performance - both paths deliver same radar data (range, velocity)
- [x] Hardware flexibility - main board can interface either way

**Decision:** Option C: De-risked development with fallback path

**Development Strategy:**
1. **Primary Path (STM32H7 Full Integration):**
   - Design main PCB with BGT24LTR11 shield interface footprint
   - Implement BSP layer for STM32H7 (timer, ADC, DAC, DMA)
   - Port SC-FMCW chirp generation
   - Target: Single integrated board

2. **Fallback Path (USB Interface to Baseboard):**
   - Main board includes USB host or UART header for baseboard connection
   - Use existing d2gl_radar.py protocol (USB CDC, 115200 baud)
   - XMC4700 baseboard + BGT24LTR11 shield remain as separate module
   - Protocol already proven: START_BYTE(0x5A) + endpoint + length + payload + END(0xE0DB)

3. **Common Interface Layer:**
   Both paths expose identical high-level API to application:
   ```c
   typedef struct {
       float range_m;
       float velocity_mps;
       float strength;
       uint8_t target_status;
   } RadarResult_t;

   bool radar_init(void);
   bool radar_get_result(RadarResult_t* result);
   bool radar_set_parameters(RadarParameters_t* params);
   ```

**Fallback Trigger Criteria:**
Switch to fallback path if any of:
- Timer synchronization proves intractable (>2 weeks debugging)
- ADC/DAC jitter causes unacceptable range accuracy
- Total integration effort exceeds 8 weeks
- Need to deliver working prototype for flight testing

**Rationale:**
- Existing d2gl_radar.py validates the USB protocol works
- Protocol extracts exactly the data needed: range (0.3-30m), velocity, signal strength
- Baseboard + shield hardware already purchased and proven
- Main application features (display, sensors, logging) can proceed in parallel
- If integration succeeds, fallback hardware becomes spare/test equipment
- If integration fails, project still delivers working radar landing assist

**Alternates:** None - this is the strategy encompassing both approaches

**Risks / Open Issues:**
- [ ] Main PCB must accommodate both interface options (shield footprint OR USB/UART connector)
- [ ] USB host capability required on STM32H7 for fallback (USB HS available)
- [ ] Form factor impact if using fallback (baseboard adds ~60x50mm)
- [ ] Determine go/no-go checkpoint for integration attempt (suggest Week 4)

**Datasheets / References:**
- docs/04-datasheets/Radar/d2gl_radar.py - Working USB protocol implementation
- docs/03-component-selection/radar-migration-analysis.md - Detailed porting analysis
- Infineon baseboard schematics (user has)
- BGT24LTR11 shield design files (user has)

**Downstream Impact:**
- PCB layout: Must reserve space for either shield interface OR USB/UART fallback connector
- Firmware: Radar abstraction layer isolates application from integration method
- Schedule: Can proceed with non-radar features immediately
- Risk: Project cannot be blocked by radar porting difficulties

**Status:**
- [ ] Proposed
- [x] Approved (2026-01-28)

---

### IMSAFE-DEC-006
**Date:** 2026-01-28
**Feature:** Power Management - Charger IC
**Related Spec Sections:** FR-CN-001 (USB-C), Power Budget

**Context:**
System requires charging 2S Li-ion (7.4V/16000mAh) from USB-C 5V input. Must support power-path (system operates when battery missing/depleted). External BMS on battery pack handles cell balancing.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | BQ25792 (TI) | Buck-boost, 5A, NVDC power-path, I2C | No integrated balancing |
| B | MP2760 (MPS) | Buck-boost, 6A, NVDC power-path | Less documentation |
| C | BQ25887 (TI) | Boost, integrated balancing | **No power-path** - REJECTED |
| D | MAX77962 (ADI) | 2S specific, 3.2A | Lower current, WLP package |

**Selection Criteria:**
- [x] Electrical: 5V input, 8.4V charge output (boost required)
- [x] Power-path: System must run when battery missing/depleted
- [x] Interface: I2C for monitoring/configuration
- [x] Availability: Extended part, but widely stocked

**Decision:** BQ25792RQMR (TI, QFN-29 4x4mm)

**Rationale:**
- NVDC power-path ensures system operation during charging or with dead battery
- Buck-boost topology handles 5V USB → 8.4V 2S charge
- 5A charge current adequate (16Ah ÷ 5A ≈ 3.2h charge time)
- Full I2C telemetry (voltage, current, temperature) for display
- Excellent TI documentation and reference designs

**Alternates:** MP2760 (MPS) - similar specs, potentially better JLCPCB availability

**Risks / Open Issues:**
- [x] JLCPCB availability - **Confirmed: $4, good stock**
- [ ] Thermal design critical at high charge currents
- [ ] Charge time with 16Ah pack (3-8 hours depending on USB source)

**Datasheets / References:**
- docs/03-component-selection/power-charger-trade-study.md
- https://www.ti.com/lit/ds/symlink/bq25792.pdf

**Downstream Impact:**
- PCB: Thermal vias under QFN, careful SW node layout
- Firmware: I2C driver, charge monitoring display
- BOM: ~$4-5 per unit

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-007
**Date:** 2026-01-28
**Feature:** Power Management - 3.3V Regulator
**Related Spec Sections:** PR-001 (3.0-3.6V logic rail)

**Context:**
Need efficient 3.3V rail from 2S battery (6.0V-8.4V range). LDO would waste 50%+ power. Must support sleep mode with low quiescent current.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | TPS62133 (TI) | 3-17V in, 3A, 25µA Iq | QFN package |
| B | TPS62150 (TI) | 3-17V in, 1A, 25µA Iq | Lower current headroom |
| C | AP62301 (Diodes) | 4.2-18V in, 3A, 22µA Iq | Smaller package, less docs |
| D | TPS62840 (TI) | 60nA Iq, ultra-low power | **Max 6.5V input** - REJECTED |

**Selection Criteria:**
- [x] Electrical: 8.4V max input (2S full charge)
- [x] Output: 3.3V, ≥500mA (preferably 1A+)
- [x] Efficiency: >85% at typical load
- [x] Quiescent: <50µA for sleep mode

**Decision:** TPS62133RGTR (TI, QFN-16 3x3mm) - Fixed 3.3V output

**Rationale:**
- 3A output handles STM32H747 + all peripherals with margin
- 3-17V input range easily covers 2S battery voltage swing
- 25µA quiescent current acceptable (16Ah battery)
- Fixed 3.3V version simplifies design (no feedback resistors)
- DCS-Control provides fast transient response

**Alternates:** AP62301WG-7 (Diodes) - 3.3V fixed, good JLCPCB availability

**Risks / Open Issues:**
- [ ] Verify JLCPCB stock availability
- [ ] Inductor selection (1-2.2µH, shielded preferred)

**Datasheets / References:**
- docs/03-component-selection/power-regulator-trade-study.md
- https://www.ti.com/lit/ds/symlink/tps62130.pdf

**Downstream Impact:**
- PCB: Small 3x3mm footprint, inductor placement
- Power budget: ~90% efficiency, minimal losses

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-008
**Date:** 2026-01-28
**Feature:** Power Management - USB ESD Protection
**Related Spec Sections:** ER-003 (ESD IEC 61000-4-2 Level 2)

**Context:**
USB-C port exposed to user handling requires ESD protection on data lines. USB 2.0 speed (480Mbps) requires low capacitance TVS.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | USBLC6-2SC6 (ST) | 2-ch + VBUS, ±8kV, 3.5pF | Slightly higher capacitance |
| B | TPD2E2U06 (TI) | 2-ch, ±25kV, 1.5pF | No VBUS protection, 2-ch only |

**Selection Criteria:**
- [x] ESD rating: IEC 61000-4-2 Level 4 (±8kV contact)
- [x] Capacitance: <5pF for USB 2.0
- [x] Availability: JLCPCB basic part preferred

**Decision:** USBLC6-2SC6 (ST, SOT-23-6)

**Rationale:**
- Integrated VBUS protection in single package
- Excellent JLCPCB availability (often basic part)
- ±8kV contact / ±15kV air exceeds requirements
- 3.5pF acceptable for USB 2.0 (480Mbps)
- Very low cost (~$0.10)

**Alternates:** TPD2E2U06 (TI) for lower capacitance applications

**Risks / Open Issues:**
- [ ] If USB-C CC lines routed to MCU, add second USBLC6-2SC6

**Datasheets / References:**
- docs/03-component-selection/power-protection-strategy.md
- https://www.st.com/resource/en/datasheet/usblc6-2.pdf

**Downstream Impact:**
- PCB: Place close to USB connector, short traces
- BOM: ~$0.10, minimal cost impact

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-009
**Date:** 2026-01-28
**Feature:** Power Management - 5V Regulator (WS2812 LEDs)
**Related Spec Sections:** FR-CL-005, FR-CL-006 (LED indicators)

**Context:**
WS2812 LEDs require 5V supply. 16 LEDs at 60mA max = 960mA peak, typical 320mA. Need efficient buck from 2S battery.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | TPS62130 (adjustable) set to 5V | Same family as 3.3V, 3A | Needs resistor divider |
| B | AP62300 fixed 5V | Simple, small | Different family |
| C | Boost from 3.3V | N/A | Inefficient, rejected |

**Decision:** TPS62130RGTR (TI, QFN-16 3x3mm) configured for 5.0V output

**Configuration:**
- R1 (VOUT to FB) = 100kΩ
- R2 (FB to GND) = 24.3kΩ
- VOUT = 0.8V × (1 + 100/24.3) ≈ 5.09V

**Rationale:**
- Same TPS6213x family as 3.3V regulator (shared knowledge, layout)
- 3A output handles 16 LEDs with headroom
- 3-17V input covers 2S battery range

**Alternates:** AP62300WG-7 (Diodes, fixed 5V)

**Datasheets / References:**
- docs/03-component-selection/power-regulator-trade-study.md
- https://www.ti.com/lit/ds/symlink/tps62130.pdf

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-010
**Date:** 2026-01-28
**Feature:** Power Management - 2S BMS (Cell Protection & Balancing)
**Related Spec Sections:** DEC-001 (Battery), DEC-006 (Charger)

**Context:**
BQ25792 charger does not include cell balancing or protection. External BMS required between charger and 2S cells for OVP, UVP, OCP, short circuit protection, and cell balancing.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | HY2120-CB + HY2213-BB3A | Proven design, JLCPCB stock, 68mA balance | Two IC types |
| B | TI BQ29200 | TI ecosystem, single IC | 12mA balance (slow) |
| C | Off-the-shelf BMS module | No design needed | Not integrated on PCB |

**Decision:** HY2120-CB (protection) + 2× HY2213-BB3A (balancers)

**BOM:**
| Component | Part Number | Qty |
|-----------|-------------|-----|
| Protection IC | HY2120-CB | 1 |
| Balancer IC | HY2213-BB3A | 2 |
| N-ch MOSFET | CJ2310 / AO3400 | 4 |
| Balance resistor | 62Ω 1/8W | 2 |
| Sense resistor | 5mΩ | 1 |

**Rationale:**
- Proven in thousands of commercial BMS boards
- Good LCSC/JLCPCB availability
- 68mA balance current adequate for top-balancing
- Small SOT-23-6 packages

**Alternates:** TI BQ29200 (if TI-only BOM preferred)

**Datasheets / References:**
- docs/03-component-selection/power-bms-trade-study.md
- https://www.hycontek.com/wp-content/uploads/DS-HY2120_EN.pdf

**Downstream Impact:**
- PCB: BMS section between charger BAT and cell connector
- Firmware: None (autonomous operation)
- Safety: Critical for Li-ion safety

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-011
**Date:** 2026-01-28
**Feature:** Sensors - Barometric Pressure (Altitude)
**Related Spec:** FR-SN-001

**Context:**
Barometric pressure sensor for altitude measurement and vertical speed calculation. Required for synthetic horizon display and flight data logging.

**Options Considered:**
| Option | Accuracy | Noise | Price | Notes |
|--------|----------|-------|-------|-------|
| BMP390 (Bosch) | ±0.5 hPa abs, ±0.03 hPa rel | 0.02 Pa | $3 | Best relative accuracy |
| BMP388 (Bosch) | ±0.5 hPa abs, ±0.06 hPa rel | 0.04 Pa | $2.50 | Slightly less accurate |
| MS5611 (TE) | ±1.5 mbar | 0.012 mbar | $8 | More expensive |
| LPS22HB (ST) | ±1 hPa | 0.65 Pa | $2 | Higher noise |

**Decision:** BMP390

**Rationale:**
- Best-in-class relative accuracy (±0.03 hPa = ~25 cm altitude)
- Ultra-low noise for smooth rate-of-climb calculation
- FIFO buffer reduces MCU interrupt load
- Proven in UAV/aviation applications (drone flight controllers)
- Good JLCPCB availability

**Interface:** I2C (0x77)

**Alternates:** BMP388 (lower cost, acceptable for V1)

**Datasheets / References:**
- docs/03-component-selection/sensor-trade-study.md
- [Bosch BMP390](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-012
**Date:** 2026-01-28
**Feature:** Sensors - IMU (Attitude Reference)
**Related Spec:** FR-SN-002

**Context:**
6-axis IMU (accelerometer + gyroscope) for synthetic horizon attitude display. Requires low gyro noise for smooth, jitter-free attitude indication.

**Options Considered:**
| Option | Gyro Noise | Accel Noise | ODR | Status | Price |
|--------|------------|-------------|-----|--------|-------|
| ICM-42688-P (TDK) | 0.0028°/s/√Hz | 70 µg/√Hz | 32 kHz | Current | $4 |
| BMI270 (Bosch) | 0.007°/s/√Hz | 120 µg/√Hz | 6.4 kHz | Current | $3 |
| MPU-6500 (TDK) | 0.01°/s/√Hz | 300 µg/√Hz | 1 kHz | **NRND** | $5 |
| LSM6DSO (ST) | 0.004°/s/√Hz | 70 µg/√Hz | 6.6 kHz | Current | $4 |

**Note:** MPU-6500/MPU-9250 specified in original requirements are NRND (Not Recommended for New Design).

**Decision:** ICM-42688-P

**Rationale:**
- Best-in-class gyro noise (0.0028°/s/√Hz) for smooth attitude
- Current production with long-term availability (replaces MPU-6500)
- 32 kHz ODR provides headroom for filtering
- Used in modern flight controllers (Pixhawk 6, etc.)
- Well-documented with reference designs

**Interface:** I2C (0x68) or SPI

**Alternates:** BMI270 (Bosch ecosystem consistency)

**Datasheets / References:**
- docs/03-component-selection/sensor-trade-study.md
- [TDK ICM-42688-P](https://invensense.tdk.com/products/motion-tracking/6-axis/icm-42688-p/)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-013
**Date:** 2026-01-28
**Feature:** Sensors - Magnetometer (Heading Reference)
**Related Spec:** FR-SN-003

**Context:**
3-axis magnetometer for heading indication. Original spec mentioned HMC5883L/HMC5983 which are **discontinued**. Need current-production replacement.

**Options Considered:**
| Option | Noise | Resolution | ODR | Status | Price |
|--------|-------|------------|-----|--------|-------|
| MMC5983MA (MEMSIC) | 0.4 mG RMS | 18-bit | 1000 Hz | Current | $2 |
| QMC5883L (QST) | 2 mG RMS | 16-bit | 200 Hz | Current | $0.50 |
| BMM150 (Bosch) | 0.3 µT | 13-bit | 30 Hz | Current | $1.50 |
| LIS3MDL (ST) | 3.2 mG | 16-bit | 80 Hz | Current | $2 |
| HMC5883L | 2 mG | 12-bit | 75 Hz | **EOL** | N/A |

**Decision:** MMC5983MA

**Rationale:**
- Best noise performance (0.4 mG RMS) for accurate heading
- 18-bit resolution (finest available in class)
- Built-in SET/RESET for offset cancellation (critical for accuracy)
- 1000 Hz ODR allows heavy filtering for smooth heading
- Industrial temperature range (-40 to +105°C)
- Current production, good availability

**Interface:** I2C (0x30)

**Calibration Note:** Soft-iron and hard-iron calibration required. Mount away from motors, speakers, and ferrous components.

**Alternates:** LIS3MDL (simpler, ST ecosystem)

**Datasheets / References:**
- docs/03-component-selection/sensor-trade-study.md
- [MEMSIC MMC5983MA](https://www.memsic.com/magnetometer-2)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-014
**Date:** 2026-01-28
**Feature:** Sensors - Gas Detector (CO + NO2)
**Related Spec:** FR-SN-004 (CO - Must), FR-SN-005 (NO2 - Should)

**Context:**
Carbon monoxide detection is safety-critical in aircraft (exhaust leaks). NO2 is secondary air quality indicator. CO alarm threshold: 50 ppm per FAA/OSHA guidance.

**Options Considered:**
| Option | Gases | CO Range | Interface | Heater | Price |
|--------|-------|----------|-----------|--------|-------|
| MICS-4514 (SGX) | CO + NO2 | 1-1000 ppm | Analog | 80 mW | $8 |
| MQ-7 (Winsen) | CO only | 20-2000 ppm | Analog | 350 mW | $2 |
| CCS811 (ScioSense) | eCO2 + TVOC | N/A | I2C | 30 mW | $10 |
| SGP41 (Sensirion) | VOC + NOx index | N/A | I2C | 50 mW | $8 |

**Decision:** MICS-4514

**Rationale:**
- Dual sensor (CO + NO2) in single package
- Direct CO measurement (not derived eCO2)
- Wide temperature range (-30 to +85°C)
- Lower heater power than MQ-7
- Good sensitivity in aviation CO range (1-1000 ppm)
- Available at JLCPCB

**Interface:**
- ADC1: Reducing gas (CO)
- ADC2: Oxidizing gas (NO2)
- GPIO: Heater enable

**Firmware Requirements:**
- 30s warmup before valid readings
- Baseline calibration in clean air at startup
- 50 ppm CO alarm threshold (configurable)
- Heater power management for battery life

**Mounting:** Requires airflow access - vented enclosure area or exposed to cabin air

**Alternates:** None - MICS-4514 is clear choice for dual CO/NO2

**Datasheets / References:**
- docs/03-component-selection/sensor-trade-study.md
- [SGX MICS-4514](https://www.sgxsensortech.com/products-services/industrial-safety/metal-oxide-sensors/)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-015
**Date:** 2026-01-28
**Feature:** Sensors - Temperature & Humidity
**Related Spec:** FR-SN-006

**Context:**
Cabin temperature display (informational). Humidity added for condensation/icing awareness - useful cockpit information at altitude.

**Options Considered:**
| Option | Temp Accuracy | Humidity | Interface | Price |
|--------|---------------|----------|-----------|-------|
| Si7021 (Silicon Labs) | ±0.4°C | ±3% RH | I2C | $2 |
| TMP102 (TI) | ±0.5°C | No | I2C | $1 |
| STTS751 (ST) | ±0.5°C | No | I2C | $1 |
| NTC Thermistor | ±1-5% | No | Analog | $0.10 |

**Decision:** Si7021

**Rationale:**
- Temperature + humidity in single package
- Excellent accuracy (±0.4°C temp, ±3% RH)
- Humidity useful for icing/condensation awareness
- Factory calibrated, no external components
- I2C interface, simple integration

**Interface:** I2C (0x40)

**Mounting Note:** For accurate cabin air reading, mount sensor on short cable/probe away from PCB heat sources.

**Alternates:** TMP102 (if humidity not needed)

**Datasheets / References:**
- docs/03-component-selection/sensor-trade-study.md
- [Sensirion Si7021](https://sensirion.com/products/catalog/Si7021/)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-016
**Date:** 2026-01-28
**Feature:** Display - 3.5" TFT
**Related Spec:** FR-UI-001, FR-UI-002

**Context:**
Primary display for synthetic horizon, instrument gauges, and system status. Pre-selected by user.

**Decision:** ST7789-based 3.5" TFT (320×480)

**Specifications:**
| Parameter | Value |
|-----------|-------|
| Controller | ST7789V2 |
| Resolution | 320×480 |
| Color depth | 16-bit RGB565 |
| Interface | SPI or 8080 parallel |
| Backlight | LED, PWM dimmable |

**Interface Recommendation:**

| Priority | Interface | Performance | Pins |
|----------|-----------|-------------|------|
| 1st | 8080 parallel via FMC | ~30-60 FPS | ~20 |
| 2nd | SPI with DMA | ~15-20 FPS | 6 |

**Rationale:**
- 3.5" suitable for panel-mount aviation instrument
- 320×480 resolution adequate for synthetic horizon + gauges
- ST7789 well-documented, widely available
- STM32H747 FMC or SPI can drive efficiently

**Graphics Library:** LVGL or TouchGFX recommended

**Framebuffer:** Double-buffer feasible (614 KB) with STM32H747's 1 MB SRAM

**Power:** ~50-130 mA (backlight dominates)

**Datasheets / References:**
- docs/04-datasheets/sensors/ST7789V2-3314280.pdf
- docs/03-component-selection/display-trade-study.md

**Status:**
- [ ] Proposed
- [x] Approved (Pre-selected)

---

### IMSAFE-DEC-017
**Date:** 2026-01-28
**Feature:** UI - Toggle Switches (Checklist)
**Related Spec:** FR-CL-001, FR-CL-002, FR-CL-003

**Context:**
8 physical toggle switches for checklist input. All UP = takeoff complete, all DOWN = landing complete.

**Options Considered:**
| Option | Type | Feel | Price |
|--------|------|------|-------|
| MTS-102 | Mini toggle SPDT | Good detent | $0.15 |
| SS12D00 | Slide switch | Soft | $0.05 |
| C&K 7101 | Premium toggle | Excellent | $3 |

**Decision:** MTS-102 (or equivalent mini toggle SPDT ON-ON)

**Rationale:**
- Standard mini toggle, widely available
- Good tactile feel for cockpit use
- Panel-mount with nut
- Excellent JLCPCB availability
- Very low cost (8× $0.15 = $1.20)

**Interface:** MCP23017 I/O expander (GPA0-GPA7)

**Alternates:** SS12D00 slide switch (lower profile)

**Datasheets / References:**
- docs/03-component-selection/ui-trade-study.md

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-018
**Date:** 2026-01-28
**Feature:** UI - Status LEDs
**Related Spec:** FR-CL-005, FR-CL-006

**Context:**
16 addressable RGB LEDs for switch position feedback. Green = correct, Red = alarm, Amber = attention.

**Options Considered:**
| Option | Package | Current | Price |
|--------|---------|---------|-------|
| WS2812B-2020 | 2×2mm | 12mA max | $0.08 |
| WS2812B | 5×5mm | 60mA max | $0.05 |
| SK6812 | 5×5mm | 60mA max | $0.04 |
| APA102 | 5×5mm | 60mA max | $0.15 |

**Decision:** WS2812B-2020

**Rationale:**
- Compact 2×2mm package fits near switches
- Lower current (12mA max) - 16× = 192mA peak vs 960mA for 5050
- Same 800 kHz protocol as standard WS2812B
- Good JLCPCB availability
- Total power reduced significantly

**Interface:**
- Single GPIO pin (PA0 or TIM2_CH1)
- DMA + timer for jitter-free 800 kHz protocol
- 5V power rail (TPS62130, DEC-009)

**Power:** ~100mA typical, 192mA peak (16 LEDs)

**Alternates:** WS2812B 5050 (if brighter needed)

**Datasheets / References:**
- docs/04-datasheets/sensors/1#WS2812B-2020_V10_EN_181106150240761#1.pdf
- docs/03-component-selection/ui-trade-study.md

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-019
**Date:** 2026-01-28
**Feature:** UI - Rotary Encoders
**Related Spec:** FR-UI-001, FR-UI-002, FR-UI-003, FR-UI-004

**Context:**
2 rotary encoders with push switches for menu navigation (left) and value adjustment (right).

**Options Considered:**
| Option | Pulses/Rev | Quality | Price |
|--------|------------|---------|-------|
| EC11 (generic) | 20 | Good | $0.30 |
| PEC11R-4115F-S0012 (Bourns) | 12 | Excellent | $1.20 |
| Alps EC11E | 15 | Very good | $2 |

**Decision:** EC11 (generic) for V1, PEC11R for production

**Rationale:**
- Standard footprint, widely available
- 12-20 pulses/revolution comfortable for menu navigation
- Integrated push switch (momentary)
- JLCPCB availability (EC11 generic)
- Low cost for prototyping

**Interface:**
- Left encoder: TIM3 encoder mode (PA6/PA7) + GPIO (PA8 switch)
- Right encoder: TIM4 encoder mode (PB6/PB7) + GPIO (PB8 switch)
- Hardware quadrature decoding in STM32 timer

**Debounce:** 10-20ms firmware debounce on switch

**Alternates:** PEC11R (Bourns) for better quality in production

**Datasheets / References:**
- docs/03-component-selection/ui-trade-study.md

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-020
**Date:** 2026-01-28
**Feature:** UI - I/O Expander
**Related Spec:** FR-CL-004

**Context:**
I/O expander to read 8 toggle switches via I2C, minimizing MCU GPIO usage.

**Options Considered:**
| Option | I/O Pins | Interface | Interrupt | Price |
|--------|----------|-----------|-----------|-------|
| MCP23017 | 16 | I2C | 2 outputs | $1 |
| PCF8575 | 16 | I2C | 1 output | $0.80 |
| TCA9555 | 16 | I2C | 1 output | $1.50 |

**Decision:** MCP23017-E/SO (SOIC-28)

**Rationale:**
- 16 I/O pins (8 for switches + 8 spare)
- I2C interface (only 2 MCU pins)
- Configurable internal pull-ups (simplifies switch wiring)
- Interrupt-on-change output for efficient polling
- Well-documented, widely used
- Excellent JLCPCB availability

**Interface:**
- I2C address: 0x20 (A0=A1=A2=GND)
- Interrupt: INTA → STM32 GPIO (PC0)
- Bus: I2C1 @ 400 kHz (shared with sensors)

**Configuration:**
- GPA0-GPA7: Inputs with pull-ups (switches to GND)
- Enable interrupt-on-change
- Read INTCAP on interrupt for debounced state

**Alternates:** PCF8575 (slightly cheaper, less features)

**Datasheets / References:**
- docs/04-datasheets/sensors/MCP23017_Data_Sheet_DS20001952-2998473.pdf
- docs/03-component-selection/ui-trade-study.md

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-021
**Date:** 2026-01-28
**Feature:** Debug - SWD Connector
**Related Spec:** Development/debug requirement

**Context:**
SWD debug interface for firmware development and production test. Needs to support ST-Link, J-Link, and other standard ARM debuggers.

**Options Considered:**
| Option | Pins | Footprint | Notes |
|--------|------|-----------|-------|
| ARM 10-pin (1.27mm) | 10 | 12.7×5.1mm | Standard, needs adapter |
| Tag-Connect TC2030-IDC-NL | 6 | 9.9×4.3mm pads | No connector, production-friendly |
| 6-pin header (2.54mm) | 6 | 15.2×2.5mm | Prototype-friendly |

**Decision:** Tag-Connect TC2030-IDC-NL + 6-pin header pads

**Rationale:**
- Tag-Connect: Minimal footprint, no connector cost, production-friendly
- 6-pin header: Convenient for prototype debugging
- Both support full SWD + SWO trace

**Signals:**
| Pin | Signal | STM32H747 |
|-----|--------|-----------|
| 1 | VCC | 3.3V |
| 2 | SWDIO | PA13 |
| 3 | GND | GND |
| 4 | SWCLK | PA14 |
| 5 | NRST | NRST |
| 6 | SWO | PB3 |

**Design Notes:**
- 10kΩ pull-up on NRST
- 100nF filter cap on NRST
- Keep SWD traces short

**Cost:** ~$0 (footprint only; TC2030 cable is one-time purchase ~$35)

**Datasheets / References:**
- docs/03-component-selection/connectivity-trade-study.md
- [Tag-Connect TC2030](https://www.tag-connect.com/product/tc2030-idc-nl)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-022
**Date:** 2026-01-28
**Feature:** Connectivity - Bluetooth (BLE)
**Related Spec:** FR-CN-002, FR-CN-003, FR-CN-004, FR-CN-005, FR-CN-006, FR-CN-007

**Context:**
BLE connectivity for flight log download to smartphone and companion app configuration. BLE 5.0 minimum, 10m range, secure bonding required.

**Options Considered:**
| Option | BLE Ver | Extra | Interface | Cert | Price |
|--------|---------|-------|-----------|------|-------|
| ESP32-C3-MINI-1 | 5.0 | WiFi | UART | FCC/CE | $2 |
| JDY-23 | 5.0 | - | UART | FCC | $1.50 |
| RN4870 | 5.0 | - | UART | FCC/CE | $4 |
| HM-19 | 5.0 | - | UART | FCC | $5 |

**Decision:** ESP32-C3-MINI-1 (Espressif)

**Rationale:**
- BLE 5.0 compliant
- WiFi 802.11n bonus for future features
- Pre-certified (FCC, CE, IC, MIC, KCC)
- JLCPCB availability
- Lowest cost (~$2)
- 4MB flash for AT firmware or custom
- Excellent documentation (Espressif)

**Specifications:**
| Parameter | Value |
|-----------|-------|
| BLE | 5.0 |
| WiFi | 802.11 b/g/n (2.4GHz) |
| Antenna | PCB (onboard) |
| Interface | UART 115200 8N1 |
| Package | 13.2×16.6×2.4mm |

**Interface:**
```
STM32H747          ESP32-C3
USART2_TX (PA2) ──► RXD
USART2_RX (PA3) ◄── TXD
GPIO (PC1)      ──► EN
GPIO (PC2)      ──► IO9 (boot)
3.3V / GND
```

**Firmware:** ESP-AT (transparent UART bridge) or custom

**Power:**
- Sleep: 5 µA
- BLE active: 20-50 mA
- TX peak: 350 mA (brief)

**Alternates:** RN4870 (if pure BLE preferred, better docs)

**Datasheets / References:**
- docs/03-component-selection/connectivity-trade-study.md
- [ESP32-C3-MINI-1 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_en.pdf)

**Status:**
- [ ] Proposed
- [x] Approved

---

### IMSAFE-DEC-023
**Date:** 2026-01-28
**Feature:** Oscillator - HSE Crystal
**Related Spec:** MCU clocking

**Decision:** 8 MHz crystal, 3225 package (e.g., X322508MSB4SI)

**Rationale:**
- STM32 standard frequency
- CubeMX default, well-supported
- Small 3.2×2.5mm package
- JLCPCB basic part available

**Load capacitors:** 2× 15pF (verify per crystal datasheet)

**Status:** [x] Approved

---

### IMSAFE-DEC-024
**Date:** 2026-01-28
**Feature:** Oscillator - LSE Crystal (RTC)
**Related Spec:** Data logging timestamps

**Decision:** 32.768 kHz crystal (e.g., X322532768MSB4GI)

**Rationale:**
- Required for accurate flight log timestamps
- Enables RTC wakeup from low-power modes
- Standard 32.768 kHz (2^15 Hz for easy binary division)

**Load capacitors:** 2× 6.8pF

**Status:** [x] Approved

---

### IMSAFE-DEC-025
**Date:** 2026-01-28
**Feature:** Connector - USB-C
**Related Spec:** FR-CN-001

**Decision:** USB-C 16-pin mid-mount receptacle (e.g., U262-161N-4BVC11)

**Rationale:**
- USB 2.0 sufficient (charging + data)
- 16-pin simpler than 24-pin, lower cost
- Mid-mount for panel alignment
- JLCPCB basic part

**CC resistors:** 2× 5.1kΩ to GND (UFP/sink role)

**Status:** [x] Approved

---

### IMSAFE-DEC-026
**Date:** 2026-01-28
**Feature:** Connector - Battery
**Related Spec:** Power input

**Decision:** XT30PW (main power) + JST-XH 3-pin (balance tap)

**Rationale:**
- XT30 rated 30A continuous, robust for aviation
- Polarized, prevents reverse connection
- JST-XH standard for 2S balance charging
- Both available at JLCPCB

**Status:** [x] Approved

---

### IMSAFE-DEC-027
**Date:** 2026-01-28
**Feature:** Connector - Vibration Sensors
**Related Spec:** External sensor pods (ADXL355/ADXL372)

**Decision:** M8 4-pin circular connectors, IP67

**Pin assignment:**
| Pin | Signal |
|-----|--------|
| 1 | VCC (3.3V) |
| 2 | GND |
| 3 | SPI_CLK |
| 4 | SPI_MISO |

**Rationale:**
- IP67 rating for engine bay environment
- Standard M8 industrial connector
- 4-pin sufficient for SPI (unidirectional data)
- Compact, cable-friendly

**Note:** For bidirectional SPI, use 5-pin variant

**Status:** [x] Approved

---

### IMSAFE-DEC-028
**Date:** 2026-01-28
**Feature:** Audio - Buzzer
**Related Spec:** CO alarm (FR-SN-004)

**Decision:** Active electromagnetic buzzer, 85+ dB (e.g., MLT-8530)

**Specifications:**
| Parameter | Value |
|-----------|-------|
| Type | Active (internal oscillator) |
| SPL | 85 dB @ 10cm |
| Voltage | 3.3V |
| Current | ~30 mA |

**Rationale:**
- CO alarm requires audible alert (safety critical)
- Active buzzer: simple GPIO on/off control
- 85 dB audible over cockpit noise
- JLCPCB basic part

**Interface:** GPIO → N-ch MOSFET → Buzzer (if >20mA)

**Status:** [x] Approved

---

### IMSAFE-DEC-029
**Date:** 2026-01-28
**Feature:** Power - On/Off Switch
**Related Spec:** System power control

**Decision:** Soft power via BQ25792 + hard backup switch (SS12D00)

**Architecture:**
- Hard switch (SS12D00): Physical disconnect for shipping/storage
- Soft control: BQ25792 BATFET disable via I2C for clean shutdown

**Rationale:**
- Soft power allows clean shutdown (save logs, sync SD card)
- Hard switch as backup/safety disconnect
- Low cost, simple implementation

**Status:** [x] Approved

---

### IMSAFE-DEC-030
**Date:** 2026-01-28
**Feature:** Power - Input Protection
**Related Spec:** Safety, robustness

**Decision:** PTC fuse (1812L200) + TVS diode (SMBJ12A)

**Specifications:**
| Component | Part | Rating |
|-----------|------|--------|
| PTC fuse | 1812L200 | 2A hold, 4A trip |
| TVS diode | SMBJ12A | 12V standoff |

**Placement:** Between battery connector and BQ25792 VBAT

**Rationale:**
- PTC: Resettable overcurrent protection
- TVS: Transient clamping (alternator load dump if connected to aircraft bus)
- Low cost, simple

**Status:** [x] Approved

---

### IMSAFE-DEC-031
**Date:** 2026-01-28
**Feature:** Integration - Multi-Board Architecture
**Related Spec Sections:** System architecture, physical requirements

**Context:**
The system requires integration of MCU, power management, sensors, display, UI controls (switches/LEDs), and radar interface. Need to determine optimal PCB partitioning for manufacturability, serviceability, and mechanical integration.

**Options Considered:**
| Option | Description | Key Pros | Key Cons |
|--------|-------------|----------|----------|
| A | Single PCB (all-in-one) | Lowest cost, simplest assembly | Inflexible panel layout, complex routing |
| B | Multi-board: Main + Switch/LED + Radar | Flexible panel design, modular | More connectors, assembly steps |
| C | Multi-board: Main + UI Panel | Two-board compromise | Still couples switches to main board size |

**Selection Criteria:**
- [x] Mechanical: Panel-mount display and switches with flexible positioning
- [x] Manufacturing: Separate boards allow parallel assembly/test
- [x] Serviceability: UI panel can be replaced independently
- [x] Development: Radar interface isolated for de-risk strategy (DEC-005)

**Decision:** Option B: Multi-board architecture

**Board Breakdown:**

| Board | Functions | KiCad Project |
|-------|-----------|---------------|
| **MainBoard** | MCU, power (charger, BMS, regulators), sensors (BMP390, ICM-42688-P, MMC5983MA, MICS-4514, Si7021), microSD, BLE module, buzzer, debug | IMSAFE_MainBoard |
| **SwitchLED Board** | 8× toggle switches, 16× WS2812B-2020, MCP23017 I/O expander, encoders | IMSAFE_SwitchLED_Board |
| **Radar Interface** | BGT24LTR11 shield connector OR XMC4700 baseboard interface (fallback per DEC-005) | IMSAFE_Radar_Interface |
| **Vibration Pods** (×2) | ADXL355 or ADXL372, M8 connector, external to main enclosure | External assembly |

**Interconnects:**

| Connection | Connector | Signals |
|------------|-----------|---------|
| Main ↔ Switch/LED | FFC/FPC 10-pin | 5V, 3.3V, GND, I2C (SDA, SCL), LED_DATA, INT, 3× spare |
| Main ↔ Display | FFC 40-pin | SPI or 8080 parallel per display module |
| Main ↔ Radar | 10-pin header | UART (TX, RX), SPI (MOSI, MISO, SCK, CS), INT, 3.3V, 5V, GND |
| Main ↔ Vibration | M8 4-pin ×2 | VCC, GND, SPI_CLK, SPI_MISO |

**PCB Specifications:**

| Board | Layers | Dimensions (est.) | Notes |
|-------|--------|-------------------|-------|
| MainBoard | 4 | 100×80mm | STM32H7 routing, thermal management |
| SwitchLED | 2 | 200×40mm | Long panel strip, switch spacing |
| Radar Interface | 2 | 50×50mm | Shield footprint or connector |

**Rationale:**
- Display-centered main board allows optimal MCU-to-display routing
- Switch/LED panel can be sized/positioned for ergonomic cockpit layout
- Radar isolation supports both integration paths (DEC-005)
- External vibration pods already required (engine bay environment)
- FFC/FPC reduces wiring complexity between boards

**Risks / Open Issues:**
- [ ] Define exact FFC pinout and connector (Molex, TE, etc.)
- [ ] Mechanical enclosure design must accommodate board separation
- [ ] EMI considerations between boards (shielding if needed)

**Downstream Impact:**
- Schematic: Hierarchical sheets per board in KiCad
- PCB: Three separate PCB projects already created
- Assembly: Board-level test before final integration
- Enclosure: Multi-board mounting provisions required

**Status:**
- [ ] Proposed
- [x] Approved (2026-01-28)

---

## Decision Index
| Decision ID | Subsystem | Status | Date |
|------------|----------|--------|------|
| IMSAFE-DEC-001 | Power Management - Battery | **Approved** | 2026-01-28 |
| IMSAFE-DEC-002 | MCU Architecture | **Superseded** | 2026-01-28 |
| IMSAFE-DEC-002A | MCU Architecture - STM32H747 | **Approved** | 2026-01-28 |
| IMSAFE-DEC-003 | Vibration IMUs (ADXL355 + ADXL372) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-004 | Data Storage (microSD + FATFS) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-005 | Radar Integration Strategy | **Approved** | 2026-01-28 |
| IMSAFE-DEC-006 | Power - Charger IC (BQ25792) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-007 | Power - 3.3V Regulator (TPS62133) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-008 | Power - USB ESD (USBLC6-2SC6) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-009 | Power - 5V Regulator (TPS62130) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-010 | Power - BMS (HY2120 + HY2213) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-011 | Sensor - Barometric (BMP390) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-012 | Sensor - IMU Attitude (ICM-42688-P) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-013 | Sensor - Magnetometer (MMC5983MA) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-014 | Sensor - Gas CO/NO2 (MICS-4514) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-015 | Sensor - Temp/Humidity (Si7021) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-016 | Display - 3.5" TFT (ST7789) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-017 | UI - Toggle Switches (MTS-102) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-018 | UI - Status LEDs (WS2812B-2020) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-019 | UI - Rotary Encoders (EC11) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-020 | UI - I/O Expander (MCP23017) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-021 | Debug - SWD (Tag-Connect + Header) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-022 | Connectivity - BLE (ESP32-C3-MINI-1) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-023 | Oscillator - HSE 8MHz | **Approved** | 2026-01-28 |
| IMSAFE-DEC-024 | Oscillator - LSE 32.768kHz | **Approved** | 2026-01-28 |
| IMSAFE-DEC-025 | Connector - USB-C 16-pin | **Approved** | 2026-01-28 |
| IMSAFE-DEC-026 | Connector - Battery (XT30 + JST-XH) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-027 | Connector - Vibration Sensors (M8) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-028 | Audio - Buzzer (MLT-8530) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-029 | Power - On/Off Switch | **Approved** | 2026-01-28 |
| IMSAFE-DEC-030 | Power - Input Protection (PTC+TVS) | **Approved** | 2026-01-28 |
| IMSAFE-DEC-031 | Integration - Multi-Board Architecture | **Approved** | 2026-01-28 |
