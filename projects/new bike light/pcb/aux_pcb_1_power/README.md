# AUX PCB #1 - Power Hub KiCad Project

## Project Files

- `aux_pcb_1_power.kicad_pro` - KiCad project file
- `aux_pcb_1_power.kicad_sch` - Schematic (template, needs component placement)
- `aux_pcb_1_power.kicad_pcb` - PCB layout (35×30mm outline)
- `PowerBoard_Design.md` - Full design documentation
- `BOM_JLCPCB.csv` - Bill of Materials for JLCPCB ordering

## Design Summary

Central power and signal hub providing:
- **5V LED rail** from USB-C (direct) or 1S LiPo (boosted via MT3608)
- **Signal passthrough** for 3× handlebar switches to XIAO controller
- **Single cable** to XIAO board (power + all signals)

Output: 5V @ 500mA peak (brake/turn mode with 24× WS2812)

## Connector Architecture

```
                    ┌─────────────────────┐
   USB-C (J1) ─────►│                     │
                    │                     ├────► J5: LED Strip (5V, GND, DATA)
   Battery (J2) ───►│    POWER HUB        │
                    │                     ├────► J3: To XIAO (6P)
                    │                     │      (5V, GND, 3×SW, LED_DATA)
   Handlebar (J4) ─►│                     │
   (5V out, 3×SW)   └─────────────────────┘
```

| Connector | Type | Pins | Function |
|-----------|------|------|----------|
| J1 | USB-C | - | 5V power input |
| J2 | JST-PH 2P | 2 | Battery input |
| J3 | JST-PH 6P | 6 | To XIAO (5V, GND, SW_L, SW_R, SW_M, LED_DATA) |
| J4 | JST-PH 5P | 5 | From Handlebar (5V, GND, SW_L, SW_R, SW_M) |
| J5 | JST-PH 3P | 3 | To LED Strip (5V, GND, LED_DATA) |

## Signal Routing

| From | To | Signal |
|------|----|--------|
| J4.3 | J3.3 | SW_LEFT (left turn button) |
| J4.4 | J3.4 | SW_RIGHT (right turn button) |
| J4.5 | J3.5 | SW_MODE (mode button) |
| J3.6 | J5.3 | LED_DATA (WS2812 data from XIAO) |
| 5V Rail | J4.1 | 5V power to handlebar LEDs |

## Schematic Capture Instructions

1. Open `aux_pcb_1_power.kicad_sch` in KiCad 8
2. Add symbols from KiCad libraries or JLCPCB library:
   - MT3608: Search for "MT3608" or use generic SOT-23-6 boost
   - SS14: Search for "SS14" or "1N5819" equivalent SMA package
   - Capacitors: Generic C_Small symbols
   - Resistors: Generic R_Small symbols
   - Inductor: Generic L_Small symbol
   - USB-C: Search for "USB_C_Receptacle_USB2.0" (power-only variant)
   - JST-PH: Search for "Conn_01x0N" with JST footprint (N = pin count)
3. Wire power section according to `PowerBoard_Design.md` schematic
4. Add signal passthrough connections (J3↔J4, J3→J5)
5. Annotate and run ERC

## PCB Layout Guidelines

- **Board Size**: 35mm × 30mm (2 layer)
- **Ground Plane**: Bottom layer (B.Cu)
- **Power Traces**: 0.5mm minimum for 5V rail
- **SW Node**: Keep trace from MT3608 SW pin to inductor SHORT
- **Bulk Cap**: Position C5 (470µF) near J5 output connector
- **Connectors**: Group J3/J4/J5 on one edge; USB-C on opposite edge
- **Signal Routing**: Keep switch signals (J4→J3) and LED_DATA (J3→J5) short
- **Thermal**: Allow thermal relief for D1, D2, D3 pads

## JLCPCB Manufacturing

1. Generate Gerbers: File → Plot (select JLCPCB preset)
2. Generate drill file: File → Fabrication Outputs → Drill Files
3. Generate BOM and CPL from schematic for SMT assembly
4. Upload to JLCPCB, select "2 Layer", 1.6mm thickness

## Net Classes (Pre-configured)

| Net Class | Track Width | Clearance | Use |
|-----------|-------------|-----------|-----|
| Default   | 0.2mm       | 0.2mm     | Signal, SW_*, LED_DATA |
| Power     | 0.5mm       | 0.3mm     | VBAT, 5V_LED, 5V_BOOST |
