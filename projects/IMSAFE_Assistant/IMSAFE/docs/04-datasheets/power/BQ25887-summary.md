# BQ25887 – Datasheet Summary

## Device Overview
**Part:** BQ25887  
**Vendor:** Texas Instruments  
**Function:** 2‑Cell (2S) Li‑Ion / Li‑Po Charger with Cell Balancing  
**Control:** I²C  

Designed for USB‑powered 2S battery systems with integrated monitoring and balancing.

---

## Charging Features
- Supports 2S Li‑Ion / Li‑Po
- Charge current up to **2.2 A**
- Charge voltage accuracy ±0.5 %
- Pre‑charge, fast‑charge, termination
- Safety timer and watchdog

---

## Input / Power Path
- Input voltage: 3.9 V – 6.2 V
- Absolute max: 20 V
- USB‑centric design
- Programmable input current limit (100 mA steps)

⚠ **No power‑path / OTG support**  
System runs from battery during charging.

---

## Cell Balancing
- Integrated balancing FETs
- Up to ~400 mA balance current
- Automatic and programmable thresholds

Major advantage for large 2S packs.

---

## Telemetry & Monitoring
- Integrated 16‑bit ADC
- Measures:
  - Input voltage/current
  - Battery voltage/current
  - Individual cell voltages
  - Die temperature
  - NTC temperature

---

## Interfaces
- I²C up to 1 MHz
- Status outputs: PG, STAT, INT
- Control pins: Charge enable, current‑limit select

---

## Thermal & Switching
- Switching frequency ~1.5 MHz
- Thermal regulation ~120 °C
- Shutdown ~150 °C
- Requires careful layout (SW node, inductor, vias)

---

## Package
- 4 × 4 mm VQFN‑24
- External inductor and ceramic capacitors required

---

## Strengths
- Integrated cell balancing
- Rich telemetry
- Reduced external BOM

## Risks
- No load‑sharing
- Charge time for large packs (e.g. 16 Ah)
- Thermal design critical

---

## IMSAFE Relevance
Strong candidate for 2S battery strategy, pending Phase 2 evaluation of charge time and system power‑path requirements.

