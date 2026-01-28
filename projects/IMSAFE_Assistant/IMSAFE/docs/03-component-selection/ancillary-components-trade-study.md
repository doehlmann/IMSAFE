# Ancillary Components Trade Study

**Date:** 2026-01-28
**Phase:** 2.x (Miscellaneous)
**Status:** Quick Decisions

---

## 1. HSE Crystal (Main Clock)

### Requirements
- STM32H747 external high-speed oscillator
- Drives PLL for 480 MHz core clock
- Must be stable, low jitter

### Selection: 8 MHz Crystal

| Parameter | Value |
|-----------|-------|
| Frequency | 8.000 MHz |
| Tolerance | ±20 ppm |
| Load capacitance | 10-20 pF (typical) |
| Package | 3.2×2.5mm |
| Example part | ABM8-8.000MHZ-B2-T (Abracon) |
| JLCPCB alt | X322508MSB4SI (basic) |

**Rationale:**
- 8 MHz is STM32 standard, well-supported by CubeMX
- Small 3225 package
- Wide availability

**Load capacitors:** 2× 10-20pF (calculate per crystal datasheet)

---

## 2. LSE Crystal (RTC)

### Requirements
- Real-time clock for timestamps
- Low power for battery backup operation
- 32.768 kHz standard

### Selection: 32.768 kHz Crystal

| Parameter | Value |
|-----------|-------|
| Frequency | 32.768 kHz |
| Tolerance | ±20 ppm |
| Load capacitance | 6-12.5 pF |
| Package | 3.2×1.5mm or 2.0×1.2mm |
| Example part | ABS07-32.768KHZ-T (Abracon) |
| JLCPCB alt | X322532768MSB4GI (basic) |

**Rationale:**
- Required for accurate flight log timestamps
- Enables RTC wakeup from low-power modes
- Standard part, widely available

**Load capacitors:** 2× 6.8pF typical

---

## 3. USB-C Connector

### Requirements
- USB 2.0 sufficient (charging + data)
- Panel mount or PCB mount
- Robust for repeated insertion

### Selection: USB-C 16-pin (USB 2.0)

| Parameter | Value |
|-----------|-------|
| Type | USB Type-C receptacle |
| Pins | 16 (USB 2.0 subset) |
| Mounting | Mid-mount SMD |
| Current rating | 3A minimum |
| Example part | TYPE-C-31-M-12 |
| JLCPCB alt | U262-161N-4BVC11 (basic) |

**Rationale:**
- 16-pin sufficient for USB 2.0 + USB-PD negotiation
- Full 24-pin not needed (no USB 3.x)
- Common, low cost

**CC resistors:** 5.1kΩ to GND on each CC pin (for UFP/sink role)

---

## 4. Battery Connector

### Requirements
- 2S Li-ion pack connection
- Handle 2A charge, 2A discharge
- Secure, aviation-appropriate
- Balance tap access

### Selection: XT30 + JST-XH Balance

| Connector | Purpose | Rating |
|-----------|---------|--------|
| XT30PW (PCB mount) | Main power (P+/P-) | 30A continuous |
| JST-XH 3-pin | Balance tap (B+/BM/B-) | 3A |

**Rationale:**
- XT30 is robust, common in aviation/RC
- Polarized, prevents reverse connection
- JST-XH standard for balance charging
- Both available at JLCPCB

**Alternative:** XT60 if higher current margin desired

---

## 5. Vibration Sensor Connectors

### Requirements
- Connect external ADXL355/ADXL372 sensor pods
- 4-wire SPI + power (6 conductors minimum)
- IP67 for engine bay environment
- Cable length up to 1m

### Selection: M8 4-pin Circular Connector

| Parameter | Value |
|-----------|-------|
| Type | M8 circular, 4-pin |
| Rating | IP67 |
| Pins | VCC, GND, SCLK, MOSI/MISO (shared or 5-pin) |
| Example | Amphenol M8 series, TE M8 |
| JLCPCB | Limited - may need external source |

**Pin assignment (4-pin):**
| Pin | Signal |
|-----|--------|
| 1 | VCC (3.3V) |
| 2 | GND |
| 3 | SPI_CLK + MOSI (shared clock, unidirectional) |
| 4 | SPI_MISO (data from sensor) |

**Note:** If bidirectional SPI needed, use M8 5-pin or M12 connector.

**Alternative:** Aviation-grade Deutsch DT series (more robust, larger)

---

## 6. Audible Alarm (Buzzer)

### Requirements
- CO alarm audio alert (safety critical)
- Loud enough for cockpit (>75 dB)
- Low power when idle

### Selection: Electromagnetic Buzzer (Active)

| Parameter | Value |
|-----------|-------|
| Type | Active (internal oscillator) |
| Frequency | 2-4 kHz (attention-getting) |
| SPL | >80 dB @ 10cm |
| Voltage | 3.3V or 5V |
| Current | 30-50 mA |
| Example | CMI-1295-0585T (CUI) |
| JLCPCB alt | MLT-8530 (basic, 85dB) |

**Rationale:**
- Active buzzer: single GPIO on/off control
- No PWM frequency generation needed
- 85 dB sufficient for cockpit alert

**Interface:** GPIO → MOSFET → Buzzer (if >20mA)

---

## 7. Power Switch

### Requirements
- Turn system on/off
- Preserve battery when stored
- Could be hard switch or soft (MCU-controlled)

### Selection: Soft Power with Hard Backup

**Architecture:**
```
Battery ──► Hard switch (SPST) ──► BQ25792 ──► System
                                      │
                              MCU GPIO ──► EN pin (soft control)
```

| Component | Part |
|-----------|------|
| Hard switch | SS12D00 slide switch (backup) |
| Soft control | BQ25792 BATFET control via I2C |

**Rationale:**
- Soft power allows clean shutdown (save logs, sync SD)
- Hard switch as backup/shipping disconnect
- BQ25792 can disable BATFET via I2C for true off

**Alternative:** Latching power button circuit (more complex)

---

## 8. Input Protection

### Requirements
- Protect against reverse polarity (unlikely with XT30)
- Overcurrent protection
- Transient protection

### Selection: PTC Fuse + TVS

| Component | Part | Rating |
|-----------|------|--------|
| PTC fuse | 1812L200 | 2A hold, 4A trip |
| TVS diode | SMBJ12A | 12V standoff, 600W peak |

**Placement:** Between battery connector and BQ25792 input

**Rationale:**
- PTC provides resettable overcurrent protection
- TVS clamps transients (alternator load dump if connected to aircraft power)
- Low cost, simple

---

## 9. Passive Components (Key Parts)

### Buck Regulator Inductors (TPS62130/TPS62133)

| Regulator | Inductance | Current | DCR | Example Part |
|-----------|------------|---------|-----|--------------|
| TPS62133 (3.3V) | 1.5 µH | 4A+ | <50mΩ | XAL4020-152ME |
| TPS62130 (5V) | 1.5 µH | 4A+ | <50mΩ | XAL4020-152ME |

**Note:** Same inductor for both - simplifies BOM

### Key Capacitors

| Location | Value | Type | Voltage | Qty |
|----------|-------|------|---------|-----|
| Buck input | 10 µF | X5R/X7R | 16V | 2 per regulator |
| Buck output | 22 µF | X5R/X7R | 10V | 2 per regulator |
| MCU VCAP | 2.2 µF | X5R | 6.3V | 2 |
| Decoupling | 100 nF | X5R | 16V | ~30 |
| Bulk | 100 µF | Electrolytic | 16V | 2 |

---

## Summary

| DEC | Item | Selection | Price |
|-----|------|-----------|-------|
| 023 | HSE Crystal | 8 MHz 3225 | $0.20 |
| 024 | LSE Crystal | 32.768 kHz | $0.15 |
| 025 | USB-C Connector | 16-pin mid-mount | $0.30 |
| 026 | Battery Connector | XT30 + JST-XH | $0.50 |
| 027 | Sensor Connectors | M8 4-pin IP67 | ~$3/pair |
| 028 | Buzzer | MLT-8530 (85dB) | $0.30 |
| 029 | Power Switch | SS12D00 + soft control | $0.10 |
| 030 | Input Protection | PTC + TVS | $0.40 |

**Total ancillary:** ~$5-6

---

## References

- [Abracon Crystal Selection Guide](https://abracon.com/Support/Crystal-Selection-Guide)
- [TI TPS62130 Layout Guide](https://www.ti.com/lit/an/slva479/slva479.pdf)
- [USB-C Spec (receptacle pinout)](https://www.usb.org/document-library/usb-type-cr-cable-and-connector-specification-revision-21)
