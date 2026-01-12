# AUX PCB #1: Power Board Design

## Overview

Central power and signal hub for bike light:
- Provides 5V LED rail from 1S LiPo battery (3.0-4.2V) or USB-C 5V input
- Passes through handlebar switch signals to XIAO controller
- Single cable connection to XIAO board (power + signals)

## Specifications

| Parameter | Value |
|-----------|-------|
| Output Voltage | 5.0V nominal |
| Output Current | 250mA typical, **500mA peak** |
| Battery Input | 1S LiPo (3.0V - 4.2V) |
| USB Input | 5.0V via USB-C |
| OR-ing Method | Schottky diodes |
| Boost Converter | MT3608 |
| LED Strip | 24× WS2812 |
| Signal Passthrough | 3× digital inputs (handlebar switches) |

### Connector Summary
| Connector | Type | Function |
|-----------|------|----------|
| J1 | USB-C | 5V power input (charge/dev) |
| J2 | JST-PH 2P | Battery input (1S LiPo) |
| J3 | JST-PH 6P | **To XIAO** (5V, GND, 3× switch signals, LED data) |
| J4 | JST-PH 5P | **From Handlebar** (5V out, GND, 3× switch inputs) |
| J5 | JST-PH 3P | **To LED Strip** (5V, GND, LED data) |

### Current Budget (24× WS2812)
| Mode | LEDs | Color | Current |
|------|------|-------|---------|
| Brake (full red) | 24 | Red 100% | 480mA |
| Night steady | 24 | Red 50% | 240mA |
| Turn (orange) | 12 | Red+Green | 480mA |
| Idle | 0 | Off | 0mA |

---

## Schematic Block Diagram

```
                    ┌──────────────────────────────────────────────────────────────┐
                    │                  AUX PCB #1 - POWER HUB                       │
                    │                                                              │
USB-C VBUS ────────►├──► D1 (SS14) ─────────────────┬─────────────────────────────►│──► J5: LED Strip
(J1)                │                                │                              │    (5V, GND, DATA)
                    │                                │                              │
                    │    ┌──────────────┐           │       ┌───────────────────┐  │
1S LiPo ───────────►├──►│   MT3608     │──► D2 ────┤       │  Signal Passthru  │  │
(J2)                │   │   Boost      │  (SS14)   │       │                   │  │
                    │   │  3.3V→5.1V   │           │       │  SW_LEFT  ────────┼──┼──► J3: To XIAO
                    │   └──────────────┘           │       │  SW_RIGHT ────────┼──┼──► (5V, GND,
                    │                               │       │  SW_MODE  ────────┼──┼──►  3× SW, DATA)
                    │                          C_BULK      │  LED_DATA ◄───────┼──┤
                    │                       470µF + 100nF  └───────────────────┘  │
                    │                               │               ▲              │
                    │                              GND              │              │
                    │                                               │              │
                    │                              J4: From Handlebar Pod         │
                    │                              (5V out, GND, 3× SW inputs)    │
                    └──────────────────────────────────────────────────────────────┘
```

### Connector Pinouts

**J3 - To XIAO (JST-PH 6P)**
| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | 5V | Power to XIAO |
| 2 | GND | Ground |
| 3 | SW_LEFT | Left turn switch (directly from J4) |
| 4 | SW_RIGHT | Right turn switch (directly from J4) |
| 5 | SW_MODE | Mode button (directly from J4) |
| 6 | LED_DATA | WS2812 data from XIAO |

**J4 - From Handlebar Pod (JST-PH 5P)**
| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | 5V | Power to handlebar LEDs (from 5V rail) |
| 2 | GND | Ground |
| 3 | SW_LEFT | Left turn button input |
| 4 | SW_RIGHT | Right turn button input |
| 5 | SW_MODE | Mode button input |

**J5 - To LED Strip (JST-PH 3P)**
| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | 5V_LED | LED power rail |
| 2 | GND | Ground |
| 3 | LED_DATA | WS2812 data (from J3 pin 6) |

---

## Circuit Description

### Power Path Logic

1. **USB Connected**: USB 5V passes through D1 (Schottky), providing ~4.7V to rail.
   Boost converter output is lower than USB path, so D2 is reverse-biased.

2. **Battery Only**: Boost converter steps up 3.0-4.2V to 5.1V, passes through D2
   to provide 5V rail. D1 is reverse-biased (no USB).

3. **Both Connected**: USB dominates due to similar voltage; boost converter
   reduces output or goes idle. Battery is preserved.

### Boost Converter Selection

**MT3608** selected for:
- Wide availability (JLCPCB basic part)
- Input: 2V - 24V
- Output: Up to 28V (set by resistor divider)
- Current: 2A switch → **~600-800mA output** at 5V (sufficient for 500mA peak)
- SOT-23-6 package
- Note: Will run warm at sustained 500mA; adequate for intermittent brake/turn

**Alternative**: TPS61200 (more efficient, higher cost)

### Signal Passthrough

The handlebar switch signals are passed directly through to the XIAO connector:
- **SW_LEFT, SW_RIGHT, SW_MODE**: Direct connection from J4 to J3 (no conditioning)
- **LED_DATA**: Direct connection from J3 to J5 (XIAO drives WS2812 strip)
- Buttons should be wired active-low with internal pull-ups enabled on XIAO GPIO

---

## Schematic (Component Level)

```
                                        D1 (SS14)
    USB-C VBUS (J1) ────────┬──────────►|─────────┬───────────► 5V_LED_OUT
                            │                      │
                           C1                      │
                          10µF                     │
                            │                      │
                           GND                     │
                                                   │
                                                   │
                    ┌──────────────────────────────┤
                    │                              │
                    │      D2 (SS14)               │
    1S BATTERY ─────┼───────────────────►|─────────┤
    (J2)            │                              │
                    │                              │
                   C2                             C5
                  10µF                          470µF
                    │                              │
                    │    MT3608                    │
                    │   ┌───────┐                  │
                    ├───┤IN   SW├───L1───┬─────────┘
                    │   │       │  22µH  │
                    │   │    EN ├───┘    │
                    │   │       │       D3
                    │   │   GND │      SS14
                    │   │       │        │
                    │   │    FB ├────┬───┴────► 5V_BOOST
                    │   └───────┘    │
                    │               R1 ┤
                    │              100k│
                    │                  ├──────────► FB
                    │               R2 ┤
                    │               24k│
                    │                  │
                   GND               GND

    Output Voltage = 0.6V × (1 + R1/R2) = 0.6 × (1 + 100k/24k) = 5.17V
```

---

## Bill of Materials

| Ref | Qty | Value | Package | Description | LCSC |
|-----|-----|-------|---------|-------------|------|
| U1 | 1 | MT3608 | SOT-23-6 | Boost converter | C84817 |
| D1 | 1 | SS14 | SMA | Schottky 1A 40V | C2480 |
| D2 | 1 | SS14 | SMA | Schottky 1A 40V | C2480 |
| D3 | 1 | SS14 | SMA | Boost rectifier | C2480 |
| L1 | 1 | 22µH | 5x5mm | Inductor 1.5A | C408412 |
| C1 | 1 | 10µF | 0805 | Input cap USB | C15850 |
| C2 | 1 | 10µF | 0805 | Input cap BAT | C15850 |
| C3 | 1 | 22µF | 0805 | Boost output cap | C45783 |
| C4 | 1 | 100nF | 0603 | HF bypass LED rail | C14663 |
| C5 | 1 | 470µF | 8×10mm | Bulk cap LED rail | C134766 |
| R1 | 1 | 100k | 0603 | FB divider top | C25803 |
| R2 | 1 | 24k | 0603 | FB divider bottom | C25942 |
| J1 | 1 | USB-C | - | USB-C receptacle (power only) | TBD |
| J2 | 1 | JST-PH 2P | - | Battery connector | C131337 |
| J3 | 1 | JST-PH 6P | - | To XIAO (power + signals) | C131341 |
| J4 | 1 | JST-PH 5P | - | From Handlebar pod | C131340 |
| J5 | 1 | JST-PH 3P | - | To LED strip | C131338 |

---

## PCB Notes

- **Size Target**: ~35mm × 30mm (5 connectors + power circuit)
- **Layers**: 2-layer
- **Ground Plane**: Bottom layer
- **Inductor Placement**: Keep SW trace short
- **Thermal**: SS14 diodes may need thermal relief; boost converter runs warm at 500mA
- **Bulk Capacitor**: 8×10mm electrolytic - position near output connector
- **Connectors**: Group J3/J4/J5 on one edge for cable routing; USB-C on opposite edge
- **Signal Routing**: Keep switch signals (J4→J3) and LED_DATA (J3→J5) short and direct

---

## Design Considerations

### Voltage Drop Analysis
| Path | Components | Drop | Output |
|------|------------|------|--------|
| USB → LED | D1 | 0.3V | 4.7V |
| Battery → Boost → LED | D2 | 0.3V | 4.9V (5.17V - 0.3V) |

WS2812 operates down to 3.5V logic, so 4.7V minimum is acceptable.

### Efficiency Estimate
- MT3608 efficiency: ~80-85% at 500mA load
- At 500mA × 5V = 2.5W output (peak brake/turn)
- Battery draw: ~840mA at 3.7V (accounting for 80% efficiency)
- At 240mA × 5V = 1.2W output (night steady)
- Battery draw: ~400mA at 3.7V

### Protection (Optional for Rev B)
- Battery undervoltage lockout (not in MVP)
- Reverse polarity protection (diode provides some protection)
- ESD on USB-C (TVS diode)

---

## Next Steps

1. ~~Create KiCad project~~ (Done - template created)
2. Complete KiCad schematic with signal routing (J3↔J4↔J5)
3. Select USB-C connector (power-only, 2-pin or 6-pin)
4. Layout PCB (~35×30mm target)
5. Generate gerbers for JLCPCB
6. Design handlebar pod PCB (3× buttons, 2× LEDs, JST-PH 5P connector)
