# Remote Vibration Sensor Pods (ADXL355 + ADXL372) — Schematic & Connection Strategy (v2: S32K146 + 5V VSYS + Grade 1/0 parts)

Cable length target: **~2.0–2.5 m** per pod (gearbox pod + motor pod).  
System supply available: **5V VSYS** (fed down the cable).

This document is written to feed **Claude Code** for schematic generation.

---

## 0) Grade requirement (MANDATORY for all parts in these pods)
All components/passives used in the pod schematics MUST be specified as one of:
- **Grade 1**: automotive / high-temp suitable for **+125°C ambient** operation **[Unverified]**
- **Grade 0**: automotive / high-temp suitable for **+150°C ambient** operation **[Unverified]**

Implementation rule for the BOM/schematic notes:
- Every line item must be explicitly tagged **(Grade 1)** or **(Grade 0)**.
- Passives must be **AEC‑Q200 qualified** and temperature rated accordingly **[Unverified]**.
- ICs must be automotive-qualified (AEC‑Q100 or equivalent) and temperature grade stated **[Unverified]**.

---

## A) Recommended architecture (robust for motor/gearbox EMI): “Smart Pod” + CAN‑FD
Direct SPI over meters is feasible but borderline in high-noise machinery environments.  
Recommended: acquire IMU data locally and transmit over **CAN‑FD** on a twisted pair.

### Pod functional blocks
1) IMU (ADXL355 or ADXL372) on short local SPI
2) **MCU: NXP S32K146 Grade 1** (FS32K146HAT0MLHR) — local sampling, buffering, timestamping
3) **CAN‑FD transceiver** (5V supplied) + protection/EMC network
4) Power conditioning from **5V VSYS** to required local rails
5) Rugged connector (M8/M12) + shield termination strategy

---

## B) Power strategy (5V VSYS over 2–2.5 m cable)
### B1) Cable power
- Send **+5V** and **GND** over cable.
- Use separate **power pair** and **CAN twisted pair** (or a combined industrial 4‑core with overall shield).

### B2) Pod regulators (preferred)
From 5V VSYS, generate:
- **VDD_5V_LOCAL** (after protection/filtering)
- **3V3_LOCAL** for sensors / MCU IO if needed (LDO or buck‑LDO chain)
- Optional **1V8_LOCAL** if you choose 1.8V IO for sensors **[Unverified]**

**Regulator selection requirement:** must be explicitly specified as (Grade 1) or (Grade 0).

### B3) Input protection / filtering (at pod connector)
Add these blocks (all with Grade tagging):
- Reverse polarity / ideal diode OR series Schottky (Grade 1/0)
- TVS diode on 5V (Grade 1/0)
- Ferrite bead or small inductor feeding local 5V plane (Grade 1/0)
- Bulk cap bank at entry: e.g., 22–47 µF + 1 µF + 100 nF (Grade 1/0)

---

## C) CAN‑FD link design (2–2.5 m)
### C1) Physical layer
- Cable: **shielded twisted pair** for CANH/CANL (overall shield preferred).
- Termination: **120 Ω** at each end of the bus (main board and far end) **[Unverified]**.
- If each pod is a spur off a trunk: keep stubs short; if pods are end nodes, terminate at pod. **[Unverified]**

### C2) CAN EMC network (pod side)
Include on the schematic (all Grade-tagged):
- Common‑mode choke on CANH/CANL (Grade 1/0)
- ESD/TVS array for CAN lines (Grade 1/0)
- Optional split termination (2×60 Ω + 4.7 nF to GND/CHASSIS) for EMI if needed **[Unverified]**

### C3) Transceiver (5V)
Pick a **CAN‑FD capable** transceiver that operates from **5V** and is explicitly Grade 1 or Grade 0.
(Do not leave as a generic placeholder in the schematic.)

---

## D) Sensor pod schematic templates (what Claude should draw)

### D1) ADXL355 pod (precision, high-temp capable)
**Sensor interface** (local only, short traces):
- SPI: SCLK, MOSI, MISO, CS (to S32K146)
- DRDY to MCU interrupt (recommended for sampling alignment)
- Optional INT1/INT2 if used

**Powering**:
- Provide the required rails with close decoupling at pins (Grade-tagged capacitors).
- Include discharge resistors for internal rails if you implement the ADXL355 internal LDO mode **[Unverified]**.

**Passives (examples — choose values to match the datasheet guidance):**
- 0.1 µF + 1 µF on VSUPPLY and VDDIO (Grade 1/0)
- 0.1 µF on internal 1.8V rails (Grade 1/0)
- 100 kΩ discharge resistors on internal rails (Grade 1/0) **[Unverified]**

### D2) ADXL372 pod (cost-optimized, +105°C target sensor)
**Sensor interface**:
- SPI: SCLK, MOSI/SDA, MISO, CS
- INT1/INT2 optional (use at least one for event/data-ready if desired)

**Powering / filtering**:
- 0.1 µF close to VS and VDDIO (Grade 1/0)
- Optional series ferrite/resistor + 1 µF bulk at VS if supply noise is high (Grade 1/0) **[Unverified]**

---

## E) S32K146 pod MCU integration (Claude schematic requirements)
### E1) MCU core
- Instantiate **FS32K146HAT0MLHR (Grade 1)** as the pod controller.
- Run in **RUN mode @ 80 MHz** (do not rely on HSRUN at high temp) **[Unverified]**.

### E2) Mandatory passives around MCU
- Per‑pin decoupling: 100 nF per VDD pin (Grade 1/0)
- Bulk near MCU: 10 µF (Grade 1/0)
- ADC/reference decoupling caps (Grade 1/0) if using ADC
- Optional crystal footprint (Grade 1/0) but internal clock can be used **[Unverified]**

### E3) MCU peripherals used
- SPI (LPSPI): IMU interface (short, local)
- FlexCAN (CAN‑FD): uplink to main board
- GPIO/IRQ: DRDY/INT lines

---

## F) Connector + pinout (recommended for each pod)
### Option F1 (simple, rugged): **M8 A‑coded 4‑pin** per pod
Use when each pod is a CAN node with power over the same cable.

| Pin | Signal | Notes |
|---:|---|---|
| 1 | +5V_VSYS | Power in |
| 3 | GND | Power return |
| 2 | CANH | Twisted with CANL |
| 4 | CANL | Twisted with CANH |

Shield/drain:
- If using shielded cable, terminate shield to **chassis** at main enclosure end (single‑point) **[Unverified]**.
- Provide optional chassis pad/strap at pod for mechanical bonding if required (do not create ground loops). **[Unverified]**

### Option F2 (development / expansion): **M12 A‑coded 5‑pin**
Adds a dedicated shield/drain or SYNC line.
- Pin 5 can be SHIELD/CHASSIS or SYNC.

---

## G) Data strategy (so the electronics choice makes sense)
At 2–2.5 m, CAN‑FD is robust; however, vibration FFT streaming can be data‑heavy.
Two practical approaches:
1) **Raw streaming bursts** over CAN‑FD during maintenance windows **[Unverified]**
2) **On‑pod feature extraction** (RMS, bands, envelope, crest factor, harmonics) and send features continuously; raw only on request **[Unverified]**

The S32K146 has enough headroom for basic feature extraction while staying in the thermal envelope **[Unverified]**.

---

## H) Deliverables to generate in CAD (explicit Claude instructions)
When generating each pod schematic, include:
1) Connector (M8‑4 or M12‑5) with pinout above
2) 5V input protection + filtering + regulator(s)
3) S32K146 (Grade 1) core + decoupling network (every cap/resistor Grade‑tagged)
4) IMU block (ADXL355 or ADXL372) with required decoupling + SPI + DRDY/INT
5) CAN‑FD transceiver (Grade‑tagged) + EMC network (CMC + TVS + termination)
6) Programming/debug header (Grade‑tagged connector) and any boot strapping required **[Unverified]**

---

## I) Open items (safe placeholders for now)
- Exact CAN‑FD transceiver part number (must be selected Grade 1 or 0)
- Whether pods are end‑nodes (termination on pod) or mid‑bus nodes (no termination)
- Whether you need galvanic isolation (rare for 2 m, but possible in harsh grounds) **[Unverified]**
