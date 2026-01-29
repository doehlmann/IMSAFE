# 1N4148W - High-Speed Switching Diode

## Manufacturer
Various (Generic Part)

## JLCPCB Part Number
C81598

## Datasheet
Standard 1N4148 specifications (widely available)

---

## Key Specifications

### Absolute Maximum Ratings
| Parameter | Symbol | Max | Unit |
|-----------|--------|-----|------|
| Peak Reverse Voltage | VRM | 100 | V |
| DC Blocking Voltage | VR | 75 | V |
| RMS Reverse Voltage | VR(RMS) | 50 | V |
| Average Forward Current | IF(AV) | 150 | mA |
| Peak Forward Current (1us) | IFM | 450 | mA |
| Forward Surge Current (1s) | IFSM | 500 | mA |
| Power Dissipation | PD | 500 | mW |
| Junction Temperature | TJ | 200 | C |

### Electrical Characteristics (TA=25C)
| Parameter | Symbol | Min | Typ | Max | Unit | Conditions |
|-----------|--------|-----|-----|-----|------|------------|
| Forward Voltage | VF | - | 0.72 | 1.0 | V | IF=10mA |
| Forward Voltage | VF | - | - | 1.0 | V | IF=50mA |
| Reverse Current | IR | - | - | 25 | nA | VR=20V |
| Reverse Current | IR | - | - | 5 | uA | VR=75V |
| Breakdown Voltage | V(BR) | 100 | - | - | V | IR=100uA |
| Capacitance | Cj | - | 4 | - | pF | VR=0V, f=1MHz |
| Reverse Recovery Time | trr | - | 4 | - | ns | IF=10mA |

### Package
- Type: SOD-123 (Surface Mount)
- Length: 2.69 mm
- Width: 1.60 mm
- Height: 1.20 mm

---

## Pin Identification

```
SOD-123 Package:
┌─────────────────┐
│   ┌───┐         │
│   │BAR│    ●    │
│   └───┘         │
└─────────────────┘
  Cathode  Anode

Bar marking indicates CATHODE (K)
```

---

## Key Features

- High-speed switching: 4ns recovery time
- Low capacitance: 4pF
- Low forward voltage: 0.72V typical
- Wide temperature range
- Small SOD-123 package
- Lead-free / RoHS compliant

---

## Applications

### Flyback/Freewheeling Diode
Protection for inductive loads when switched by MOSFETs:
- Buzzers
- Relays
- Solenoids
- Small motors

### Flyback Diode Configuration
```
         +V
          │
    ┌─────┴─────┐
    │           │
    │   LOAD    │ (Inductive)
    │ (Buzzer)  │
    │           │
    └─────┬─────┘
          │
    ┌─────┴─────┐
    │     │     │
    ▼     │     │
   D1     │     │
   1N4148W│     │
    │     │     │
    └─────┴─────┘
          │
       MOSFET Drain
          │
       Source
          │
         GND
```

**Diode Orientation:**
- Cathode (K) to +V rail
- Anode (A) to MOSFET drain

### Signal Clamping
- Protect inputs from overvoltage
- Level shifting circuits
- Logic gates protection

### General Purpose Rectification
- Low current rectification
- Detection circuits
- Demodulation

---

## Design Considerations

### Current Rating
- Average current: 150mA max
- For higher currents, use 1N4007 (SOD-123F) or Schottky

### Voltage Rating
- 75V DC blocking (sufficient for 3.3V and 5V circuits)
- For higher voltages, use appropriate part

### Speed
- 4ns recovery time
- Suitable for switching frequencies up to 10MHz+

### Forward Voltage Drop
- 0.72V @ 10mA
- For lower drop, consider Schottky diodes (0.3-0.4V)

---

## Comparison with Similar Parts

| Part | Package | VR | IF(AV) | VF | trr |
|------|---------|----|----|----|----|
| 1N4148W | SOD-123 | 75V | 150mA | 0.72V | 4ns |
| 1N4148 | DO-35 | 75V | 300mA | 0.72V | 4ns |
| BAT54 | SOT-23 | 30V | 200mA | 0.24V | 5ns |
| SS14 | SMA | 40V | 1A | 0.55V | - |

---

## IMSAFE Project Usage

### Section 19: Audio Alarm
- D2: Flyback protection for buzzer
- Protects Q7 (AO3400A) from inductive spikes
- Cathode to +3.3V, Anode to MOSFET drain

### Connection
```
+3.3V ──────┬────────── BZ1 (+)
            │
            ▼ D2 (1N4148W)
            │           (-)
            │            │
            └────────────┴── Q7 Drain
```

## JLCPCB Part
- 1N4148W SOD-123: C81598
