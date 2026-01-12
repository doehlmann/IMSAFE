# KiCad Project Template - JLCPCB Design Rules

## Project Setup

### File Structure
```
project_name/
├── project_name.kicad_pro     # Project file
├── project_name.kicad_sch     # Schematic
├── project_name.kicad_pcb     # PCB layout
├── fp-lib-table               # Project footprint libraries
├── sym-lib-table              # Project symbol libraries
├── gerbers/                   # Gerber output directory
├── bom/                       # BOM exports
└── docs/                      # Documentation
    ├── schematic.pdf
    └── assembly.pdf
```

---

## JLCPCB Design Rules (Standard Process)

### Board Specifications

| Parameter | Minimum | Standard | Notes |
|-----------|---------|----------|-------|
| Board thickness | 0.4mm | 1.6mm | 1.6mm most common |
| Copper weight | 1oz | 1oz | 2oz available |
| Layers | 1 | 2 | Up to 6+ available |
| Min board size | 10x10mm | - | Per panel |
| Max board size | 400x500mm | - | Larger available |

### Trace/Space Rules (2-layer, 1oz copper)

| Parameter | Minimum | Recommended |
|-----------|---------|-------------|
| Trace width | 0.127mm (5mil) | 0.2mm (8mil) |
| Trace spacing | 0.127mm (5mil) | 0.2mm (8mil) |
| Min annular ring | 0.13mm | 0.25mm |
| Via hole | 0.3mm | 0.4mm |
| Via diameter | 0.6mm | 0.8mm |

### Drill Specifications

| Parameter | Minimum | Standard |
|-----------|---------|----------|
| Min drill diameter | 0.2mm | 0.3mm |
| Max drill diameter | 6.3mm | - |
| PTH hole tolerance | ±0.08mm | - |
| NPTH hole tolerance | ±0.05mm | - |

### Silkscreen

| Parameter | Minimum | Recommended |
|-----------|---------|-------------|
| Line width | 0.15mm | 0.2mm |
| Text height | 0.8mm | 1.0mm |
| Text width | 0.15mm | 0.2mm |
| Clearance to pads | 0.15mm | 0.2mm |

### Solder Mask

| Parameter | Value |
|-----------|-------|
| Solder mask color | Green (default), Black, White, Blue, Red |
| Solder mask clearance | 0.05mm typical |
| Min solder mask bridge | 0.4mm (between pads) |

---

## KiCad Design Rules Setup

### Board Setup (File → Board Setup)

```
Design Rules → Constraints:
  Minimum clearance: 0.15mm
  Minimum track width: 0.15mm
  Minimum via diameter: 0.6mm
  Minimum via drill: 0.3mm
  Minimum through hole: 0.3mm
  Minimum annular width: 0.15mm

Design Rules → Net Classes:
  Default:
    Clearance: 0.2mm
    Track Width: 0.25mm
    Via Size: 0.8mm
    Via Drill: 0.4mm

  Power:
    Clearance: 0.3mm
    Track Width: 0.5mm
    Via Size: 0.8mm
    Via Drill: 0.4mm
```

### Layer Stack (2-layer)

```
F.Cu          - Front copper
F.Paste       - Front solder paste (for stencil)
F.SilkS       - Front silkscreen
F.Mask        - Front solder mask
Edge.Cuts     - Board outline
B.Mask        - Back solder mask
B.SilkS       - Back silkscreen
B.Paste       - Back solder paste
B.Cu          - Back copper
```

---

## Gerber Export Settings

### Plot Settings (File → Plot)

**Layers to plot:**
- F.Cu
- B.Cu
- F.SilkS
- B.SilkS
- F.Mask
- B.Mask
- Edge.Cuts

**Plot Options:**
- Plot reference designators: Yes
- Plot footprint values: No (or Yes, your preference)
- Plot format: Gerber
- Use Protel filename extensions: Yes
- Subtract soldermask from silkscreen: Yes

**Gerber Options:**
- Coordinate format: 4.6 (unit mm)
- Use extended X2 format: No (for maximum compatibility)
- Include netlist attributes: No
- Disable aperture macros: No

### Drill File Settings

**Drill Units:** Millimeters
**Zeros Format:** Decimal format
**Map File Format:** Gerber
**Drill Origin:** Absolute
**Drill File Type:** Excellon
**PTH and NPTH in single file:** Yes (or separate, both work)

### Output Files

```
gerbers/
├── project_name-F_Cu.gtl       # Front copper
├── project_name-B_Cu.gbl       # Back copper
├── project_name-F_SilkS.gto    # Front silkscreen
├── project_name-B_SilkS.gbo    # Back silkscreen
├── project_name-F_Mask.gts     # Front solder mask
├── project_name-B_Mask.gbs     # Back solder mask
├── project_name-Edge_Cuts.gm1  # Board outline
├── project_name.drl            # Drill file
└── project_name-drl_map.gbr    # Drill map
```

---

## SMT Assembly Files (JLCPCB)

### BOM Format

JLCPCB requires specific BOM format:

| Column | Description | Example |
|--------|-------------|---------|
| Comment | Component value | 10uF |
| Designator | Reference | C1, C2 |
| Footprint | Package | 0805 |
| LCSC Part # | LCSC part number | C15850 |

**KiCad Export:** Use BOM plugin or manual export

### CPL (Component Placement List)

| Column | Description |
|--------|-------------|
| Designator | Reference designator |
| Mid X | Center X position (mm) |
| Mid Y | Center Y position (mm) |
| Layer | Top or Bottom |
| Rotation | Rotation in degrees |

**KiCad Export:** File → Fabrication Outputs → Component Placement

**Important:** Verify rotation matches JLCPCB orientation. May need adjustment.

---

## Design Checklist

### Before Starting
- [ ] Create project from template
- [ ] Set up design rules per JLCPCB specs
- [ ] Add LCSC part numbers to components

### Schematic
- [ ] All components have footprints
- [ ] All components have LCSC numbers (for SMT)
- [ ] Power symbols connected correctly
- [ ] ERC passes with no errors

### PCB Layout
- [ ] Board outline on Edge.Cuts layer
- [ ] All components placed
- [ ] DRC passes with no errors
- [ ] Silkscreen readable and clear of pads
- [ ] Mounting holes correct size
- [ ] Fiducials added (for SMT assembly)

### Export
- [ ] Gerbers generated
- [ ] Drill files generated
- [ ] BOM exported (if SMT assembly)
- [ ] CPL exported (if SMT assembly)
- [ ] All files zipped for upload

### JLCPCB Order
- [ ] Upload gerber ZIP
- [ ] Select board options
- [ ] Review DFM analysis
- [ ] Upload BOM and CPL (if SMT)
- [ ] Review part matching
- [ ] Place order

---

## Tips

### Cost Reduction
- Use standard 1.6mm thickness
- Use green solder mask
- Keep board under 100x100mm for best pricing
- Use JLCPCB "Basic Parts" for lowest assembly cost
- Combine multiple designs into panel

### Common Issues
- **Silkscreen on pads**: Set clearance rules
- **Drill too small**: Use 0.3mm minimum
- **Track too thin**: Watch DRC for 0.127mm minimum
- **Missing outline**: Ensure Edge.Cuts is complete and closed
