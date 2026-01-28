# UI Trade Study: Switches, Encoders, LEDs, I/O Expander

**Date:** 2026-01-28
**Phase:** 2.3 UI
**Status:** Draft for Review

---

## 1. UI Requirements Summary

| Component | Quantity | Function | Interface |
|-----------|----------|----------|-----------|
| Toggle switches | 8 | Checklist input (takeoff/landing) | MCP23017 I2C |
| Status LEDs | 16 | Switch position feedback | WS2812 (GPIO) |
| Rotary encoders | 2 | Menu navigation, value adjust | GPIO (quadrature) |
| I/O expander | 1 | Switch multiplexing | I2C |

---

## 2. Toggle Switches

### 2.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Type | SPDT (ON-ON) | Two stable positions |
| Quantity | 8 | One per checklist item |
| Mounting | Panel mount | Cockpit instrument style |
| Current rating | >10 mA | Signal level only |
| Feel | Positive detent | Clear tactile feedback |
| Environment | -20°C to +70°C | Cockpit temperatures |

### 2.2 Candidates

| Part | Type | Size | Current | Temp | JLCPCB | Price |
|------|------|------|---------|------|--------|-------|
| MTS-102 | SPDT ON-ON | 6mm | 3A/125V | -25 to +85°C | Yes | ~$0.15 |
| SS12D00 | SPDT slide | 7mm | 0.3A | -20 to +70°C | Basic | ~$0.05 |
| E-Switch 100SP1T1B1M1 | SPDT toggle | 5mm | 0.4A | -30 to +85°C | No | ~$1.50 |
| C&K 7101 | SPDT toggle | 6.35mm | 5A/120V | -30 to +85°C | Extended | ~$3 |

### 2.3 Selection: MTS-102 (or equivalent mini toggle)

**Rationale:**
- Standard mini toggle, widely available
- Good tactile feel for cockpit use
- Panel-mount with nut
- Very low cost
- JLCPCB stock

**Alternate:** SS12D00 slide switch (lower profile, less tactile)

---

## 3. Status LEDs (WS2812)

### 3.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Type | Addressable RGB | Individual color control |
| Quantity | 16 | 2 per switch (above/below) |
| Colors | Green, Red, Amber minimum | Status indication |
| Brightness | Adjustable | Day/night cockpit |
| Voltage | 5V | Dedicated rail (DEC-009) |

### 3.2 Candidates

| Part | Package | Voltage | Current | Temp | Price |
|------|---------|---------|---------|------|-------|
| WS2812B | 5050 (5×5mm) | 5V | 60mA max | -25 to +80°C | ~$0.05 |
| WS2812B-2020 | 2020 (2×2mm) | 5V | 12mA max | -25 to +80°C | ~$0.08 |
| SK6812 | 5050 | 5V | 60mA max | -20 to +70°C | ~$0.04 |
| APA102 | 5050 | 5V | 60mA max | -40 to +85°C | ~$0.15 |

### 3.3 Selection: WS2812B-2020

**Rationale:**
- Compact 2×2mm package fits near switches
- Lower current (12mA max) reduces total power
- Same protocol as WS2812B (800 kHz)
- Good JLCPCB availability
- 16 LEDs × 12mA = 192mA max (vs 960mA for 5050)

**Alternate:** WS2812B 5050 if larger/brighter needed

### 3.4 Interface

```
STM32H747 GPIO (PA0 or TIM PWM)
    │
    └──► DIN ──► WS2812 #1 ──► DOUT ──► DIN ──► WS2812 #2 ──► ... ──► #16
              │                      │
              └── 5V + GND ──────────┘
```

**Timing:** 800 kHz NRZ protocol
- Use DMA + timer for jitter-free generation
- Or bit-bang with interrupts disabled

---

## 4. Rotary Encoders

### 4.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Type | Incremental with push | Menu + confirm |
| Quantity | 2 | Left (mode), Right (value) |
| Detents | Yes | Tactile clicks preferred |
| Pulses/rev | 12-24 | Standard feel |
| Mounting | Panel or PCB | TBD based on enclosure |
| Debounce | Firmware | 10-20ms typical |

### 4.2 Candidates

| Part | Pulses/Rev | Switch | Detent | Mounting | Price |
|------|------------|--------|--------|----------|-------|
| PEC11R-4215F-S0024 | 24 | Yes | Yes | PCB/Panel | ~$1.50 |
| PEC11R-4115F-S0012 | 12 | Yes | Yes | PCB | ~$1.20 |
| EC11 (generic) | 20 | Yes | Yes | PCB | ~$0.30 |
| Alps EC11E | 15 | Yes | Yes | PCB | ~$2 |

### 4.3 Selection: EC11 (generic) or PEC11R-4115F-S0012

**Rationale:**
- Standard footprint, widely available
- 12-20 pulses/revolution is comfortable
- Integrated push switch
- Low cost
- Good JLCPCB availability (EC11 generic)

**For better quality:** PEC11R series (Bourns) if budget allows

### 4.4 Interface

```
Encoder Left:
├── A ──► GPIO PA1 (EXTI or timer input capture)
├── B ──► GPIO PA2
├── SW ──► GPIO PA3 (active low, internal pull-up)
└── COM ──► GND

Encoder Right:
├── A ──► GPIO PB0
├── B ──► GPIO PB1
├── SW ──► GPIO PB2
└── COM ──► GND
```

**Decoding options:**
1. **Timer encoder mode** (best) - Hardware quadrature decoder
2. **EXTI interrupt** - Software decode on edge
3. **Polling** - Simplest but may miss fast rotation

**Recommendation:** Use STM32 timer in encoder mode (TIM2, TIM3, etc.)

---

## 5. I/O Expander (MCP23017)

### 5.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| I/O pins | ≥16 | 8 switches × 2 positions |
| Interface | I2C | Minimize MCU pins |
| Interrupt | Optional | For efficient polling |
| Voltage | 3.3V | Logic level |

### 5.2 Candidates

| Part | I/O Pins | Interface | Interrupt | Package | Price |
|------|----------|-----------|-----------|---------|-------|
| MCP23017 | 16 | I2C | Yes (2) | SOIC-28, QFN | ~$1 |
| MCP23S17 | 16 | SPI | Yes (2) | SOIC-28 | ~$1 |
| PCF8575 | 16 | I2C | Yes (1) | SOIC-24 | ~$0.80 |
| TCA9555 | 16 | I2C | Yes (1) | TSSOP-24 | ~$1.50 |

### 5.3 Selection: MCP23017

**Rationale:**
- 16 I/O pins sufficient for 8 switches (even with both throw positions)
- I2C interface (2 pins only)
- Interrupt output for change detection
- Configurable pull-ups (simplifies switch wiring)
- Well-documented, widely used
- Excellent JLCPCB availability

### 5.4 Switch Wiring with MCP23017

```
MCP23017 (I2C addr 0x20)
│
├── GPA0 ──► Switch 1 ──► GND
├── GPA1 ──► Switch 2 ──► GND
├── GPA2 ──► Switch 3 ──► GND
├── GPA3 ──► Switch 4 ──► GND
├── GPA4 ──► Switch 5 ──► GND
├── GPA5 ──► Switch 6 ──► GND
├── GPA6 ──► Switch 7 ──► GND
├── GPA7 ──► Switch 8 ──► GND
│
├── GPB0-7 ──► (spare or second throw position)
│
├── INTA ──► STM32 GPIO (interrupt on change)
│
├── SDA ──► I2C1_SDA
└── SCL ──► I2C1_SCL
```

**Configuration:**
- Enable internal pull-ups on GPA0-7
- Configure as inputs
- Enable interrupt-on-change
- Read INTCAP register to get switch states on interrupt

---

## 6. Complete UI Schematic Block

```
                           ┌──────────────────────────────────────┐
                           │              STM32H747               │
                           │                                      │
  ┌─────────────┐          │   I2C1_SDA ◄───────────────────────┐│
  │  MCP23017   │◄────────►│   I2C1_SCL ◄───────────────────────┤│
  │  (0x20)     │          │   PC0 (INT) ◄──────────────────────┤│
  └─────┬───────┘          │                                    ││
        │                  │   PA0 (TIM2_CH1) ────► WS2812 DIN  ││
   8× Toggle               │                                    ││
   Switches                │   TIM3_CH1 (PA6) ◄── Encoder L-A   ││
        │                  │   TIM3_CH2 (PA7) ◄── Encoder L-B   ││
        ▼                  │   PA8 ◄────────────── Encoder L-SW ││
  ┌───┬───┬───┐            │                                    ││
  │ 1 │...│ 8 │            │   TIM4_CH1 (PB6) ◄── Encoder R-A   ││
  └───┴───┴───┘            │   TIM4_CH2 (PB7) ◄── Encoder R-B   ││
                           │   PB8 ◄────────────── Encoder R-SW ││
  16× WS2812B-2020         │                                    ││
  ┌──┬──┬──┬──┐            └────────────────────────────────────┘│
  │G │R │G │R │...                                               │
  └──┴──┴──┴──┘                                                  │
     5V rail (TPS62130)                                          │
```

---

## 7. Pin Summary

| Function | STM32H747 Pin | Notes |
|----------|---------------|-------|
| I2C1_SDA | PB9 | MCP23017 + sensors |
| I2C1_SCL | PB6 | Shared I2C bus |
| MCP23017 INT | PC0 | Interrupt on switch change |
| WS2812 Data | PA0 (TIM2_CH1) | DMA + timer for protocol |
| Encoder L-A | PA6 (TIM3_CH1) | Hardware encoder mode |
| Encoder L-B | PA7 (TIM3_CH2) | |
| Encoder L-SW | PA8 | GPIO input, pull-up |
| Encoder R-A | PB6 (TIM4_CH1) | Hardware encoder mode |
| Encoder R-B | PB7 (TIM4_CH2) | |
| Encoder R-SW | PB8 | GPIO input, pull-up |

**Note:** Pin assignments are preliminary - verify against display FMC pins and other peripherals during PCB design.

---

## 8. Power Budget

| Component | Qty | Typical | Peak | Rail |
|-----------|-----|---------|------|------|
| Toggle switches | 8 | <1 mA | <1 mA | 3.3V |
| MCP23017 | 1 | 1 mA | 1 mA | 3.3V |
| WS2812B-2020 | 16 | 100 mA | 192 mA | 5V |
| Rotary encoders | 2 | <1 mA | <1 mA | 3.3V |
| **Total** | | ~103 mA | ~195 mA | |

**Note:** WS2812B-2020 significantly lower power than 5050 variant.

---

## 9. BOM Summary

| Component | Part Number | Qty | Unit Price | Total |
|-----------|-------------|-----|------------|-------|
| Toggle switch | MTS-102 | 8 | $0.15 | $1.20 |
| LED | WS2812B-2020 | 16 | $0.08 | $1.28 |
| Rotary encoder | EC11 | 2 | $0.30 | $0.60 |
| I/O expander | MCP23017-E/SO | 1 | $1.00 | $1.00 |
| **Total** | | | | **~$4.10** |

---

## 10. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| WS2812 timing sensitive | Medium | Use DMA + timer, not bit-bang |
| Encoder bounce | Low | Hardware filter cap + firmware debounce |
| MCP23017 I2C conflict | Low | Verify address 0x20 unique on bus |
| Switch contact bounce | Low | Read multiple times, majority vote |

---

## 11. References

- [MCP23017 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf)
- [WS2812B-2020 Datasheet](docs/04-datasheets/sensors/1#WS2812B-2020_V10_EN_181106150240761#1.pdf)
- [Bourns PEC11R Datasheet](https://www.bourns.com/docs/Product-Datasheets/PEC11R.pdf)
