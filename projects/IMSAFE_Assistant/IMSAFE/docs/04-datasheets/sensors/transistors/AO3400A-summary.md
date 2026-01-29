# AO3400A - N-Channel Enhancement Mode MOSFET

## Manufacturer
Alpha & Omega Semiconductor

## JLCPCB Part Number
C20917

## Datasheet
[AO3400A Datasheet](http://www.aosmd.com/pdfs/datasheet/AO3400A.pdf)

---

## Key Specifications

### Absolute Maximum Ratings
| Parameter | Symbol | Max | Unit |
|-----------|--------|-----|------|
| Drain-Source Voltage | VDS | 30 | V |
| Gate-Source Voltage | VGS | +/-12 | V |
| Continuous Drain Current (Ta=25C) | ID | 5.7 | A |
| Continuous Drain Current (Ta=70C) | ID | 4.4 | A |
| Pulsed Drain Current | IDM | 30 | A |
| Power Dissipation (Ta=25C) | PD | 1.4 | W |
| Junction Temperature | TJ | +150 | C |

### Electrical Characteristics (Tj=25C)

#### Static Parameters
| Parameter | Symbol | Min | Typ | Max | Unit | Conditions |
|-----------|--------|-----|-----|-----|------|------------|
| Drain-Source Breakdown | BVDSS | 30 | - | - | V | VGS=0V, ID=250uA |
| Gate Threshold Voltage | VGS(th) | 0.65 | 1.0 | 1.35 | V | VDS=VGS, ID=250uA |
| Gate-Body Leakage | IGSS | - | - | +/-100 | nA | VGS=+/-12V |
| Zero Gate Voltage Drain Current | IDSS | - | - | 1 | uA | VDS=30V, VGS=0V |
| On-State Resistance | RDS(ON) | - | 26 | 32 | mOhm | VGS=4.5V, ID=5.7A |
| On-State Resistance | RDS(ON) | - | 34 | 44 | mOhm | VGS=2.5V, ID=4.4A |

#### Dynamic Parameters
| Parameter | Symbol | Typ | Max | Unit | Conditions |
|-----------|--------|-----|-----|------|------------|
| Input Capacitance | Ciss | 875 | - | pF | VDS=15V, VGS=0V |
| Output Capacitance | Coss | 110 | - | pF | VDS=15V, VGS=0V |
| Reverse Transfer Cap | Crss | 88 | - | pF | VDS=15V, VGS=0V |
| Total Gate Charge | Qg | 11 | - | nC | VDS=15V, VGS=4.5V |
| Gate-Source Charge | Qgs | 2.5 | - | nC | - |
| Gate-Drain Charge | Qgd | 4.0 | - | nC | - |

### Package
- Type: SOT-23 (TO-236)
- Pin 1: Source (S)
- Pin 2: Gate (G)
- Pin 3: Drain (D)

---

## Pin Configuration

```
        ┌───┐
 Source │ 1 ├─ S
        │   │
   Gate │ 2 ├─ G
        │   │
  Drain │ 3 ├─ D
        └───┘
      SOT-23 Top View
```

---

## Key Features

- Low RDS(ON) at VGS = 2.5V: 34mOhm typ
- Logic level gate drive (2.5V capable)
- High current capability: 5.7A
- Fast switching
- Low input capacitance
- RoHS compliant

---

## Application Notes

### Low-Side Switch Configuration
```
Load ─────┬───── Supply
          │
          Drain
          │
       [MOSFET]
          │
          Source ── GND
          │
          Gate ─── R_series ─┬── GPIO
                             │
                       R_pulldown ── GND
```

### Gate Drive Requirements
| VGS | RDS(ON) | Suitable For |
|-----|---------|--------------|
| 4.5V | 26mOhm | Best performance |
| 3.3V | ~30mOhm | Direct 3.3V GPIO |
| 2.5V | 34mOhm | Low voltage logic |

### Recommended Gate Circuit
- R_pulldown: 10k (ensures OFF when GPIO floating/reset)
- R_series: 100R (limits ringing, reduces EMI)

### Inductive Load Protection
For inductive loads (motors, buzzers, relays):
- Add flyback diode across load
- Diode cathode to supply, anode to drain
- 1N4148W suitable for low current applications

### Thermal Considerations
- Maximum power: 1.4W @ 25C ambient
- Derate 11.2mW/C above 25C
- For high current: use adequate PCB copper for heat sinking

---

## Design Calculations

### Power Dissipation (Resistive Load)
```
P_conduction = ID^2 * RDS(ON)

Example: 100mA load, VGS=3.3V
P = (0.1)^2 * 0.030 = 0.3mW (negligible)
```

### Switching Losses (High Frequency)
```
P_switching = 0.5 * VDS * ID * (tr + tf) * f

Where:
tr = rise time (~10ns)
tf = fall time (~10ns)
f = switching frequency
```

---

## IMSAFE Project Usage

### Section 10: Gas Sensor Heater
- Q5: Heater switch for MICS-4514
- Load: ~40mA @ 2V heater (80mW)
- PWM for temperature control
- R1001: 10k pull-down
- R1002: 100R series

### Section 19: Buzzer Driver
- Q7: Buzzer switch for MLT-8530
- Load: ~30mA @ 3.3V (~100mW)
- ON/OFF control
- R1901: 10k pull-down
- R1902: 100R series
- D2: 1N4148W flyback protection

## Related JLCPCB Parts
- AO3400A: C20917
- 10k 0603 Resistor: C25804
- 100R 0603 Resistor: C22775
- 1N4148W Diode: C81598
