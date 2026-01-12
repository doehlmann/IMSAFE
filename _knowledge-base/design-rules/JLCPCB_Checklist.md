# JLCPCB Design & Manufacturing Checklist

## Pre-Design Checklist

### Part Selection
- [ ] Check JLCPCB/LCSC part availability
- [ ] Prefer "Basic Parts" for lowest assembly cost
- [ ] Verify stock levels (avoid low-stock parts)
- [ ] Confirm footprint matches JLCPCB library
- [ ] Note LCSC part numbers for BOM

### Design Rules Setup
- [ ] Set minimum trace width: 0.127mm (5mil)
- [ ] Set minimum trace spacing: 0.127mm (5mil)
- [ ] Set minimum via drill: 0.3mm
- [ ] Set minimum annular ring: 0.13mm
- [ ] Set board outline: closed path on Edge.Cuts

---

## Schematic Review

### Power
- [ ] Input voltage protection (reverse polarity, ESD)
- [ ] Decoupling capacitors on all IC power pins
- [ ] Bulk capacitor near power input
- [ ] Power indicator LED (optional)
- [ ] Power-on sequence correct (if multiple rails)

### MCU (STM32)
- [ ] All VDD pins connected with decoupling caps
- [ ] VDDA decoupled with ferrite + capacitors
- [ ] VBAT connected (or tied to VDD)
- [ ] BOOT0 configured (resistor to GND for normal boot)
- [ ] NRST with 100nF cap and optional pullup
- [ ] Crystal with correct load capacitors

### Interfaces
- [ ] USB: ESD protection (USBLC6-2)
- [ ] UART: Level shifter if needed
- [ ] I2C: Pull-up resistors on SDA/SCL
- [ ] SPI: Series resistors if long traces
- [ ] All unused MCU pins handled (NC or GPIO)

### Protection
- [ ] TVS diodes on external interfaces
- [ ] Current limiting resistors on LEDs
- [ ] Input protection on analog pins
- [ ] Proper grounding for analog circuits

### Documentation
- [ ] All components have values
- [ ] All components have footprints
- [ ] Reference designators visible
- [ ] Net names on important signals
- [ ] Power symbols correctly connected

### ERC
- [ ] Run Electrical Rules Check
- [ ] All errors resolved
- [ ] Warnings reviewed and understood

---

## PCB Layout Review

### Board Outline
- [ ] Closed path on Edge.Cuts layer
- [ ] Correct board dimensions
- [ ] Rounded corners (optional, improves handling)
- [ ] Mounting holes correct size and position

### Component Placement
- [ ] All components placed
- [ ] Decoupling caps near IC pins
- [ ] Crystal close to MCU
- [ ] Connectors accessible at board edge
- [ ] Test points accessible
- [ ] No components under tall components

### Routing
- [ ] All nets routed
- [ ] Power traces adequate width
- [ ] High-speed signals: short and direct
- [ ] Analog signals isolated from digital
- [ ] Ground plane intact under critical signals
- [ ] Via stitching for ground continuity

### Silkscreen
- [ ] Reference designators readable
- [ ] Orientation indicators (dot, notch)
- [ ] Board name/version
- [ ] Date or revision
- [ ] No silkscreen on pads
- [ ] Minimum text size: 0.8mm height, 0.15mm width

### Solder Mask
- [ ] Pads properly exposed
- [ ] Solder mask bridges between fine-pitch pins
- [ ] Via-in-pad masked if needed

### Special Considerations
- [ ] Fiducials for SMT assembly (3 recommended)
- [ ] Panelization marks if needed
- [ ] Tooling holes if needed
- [ ] Thermal relief on power planes

### DRC
- [ ] Run Design Rules Check
- [ ] All errors resolved
- [ ] Warnings reviewed

---

## Manufacturing Files

### Gerber Export
- [ ] F.Cu (Front copper)
- [ ] B.Cu (Back copper)
- [ ] F.SilkS (Front silkscreen)
- [ ] B.SilkS (Back silkscreen)
- [ ] F.Mask (Front solder mask)
- [ ] B.Mask (Back solder mask)
- [ ] Edge.Cuts (Board outline)

### Drill Files
- [ ] Excellon format
- [ ] Millimeter units
- [ ] PTH and NPTH included

### Assembly Files (if SMT)
- [ ] BOM with LCSC part numbers
- [ ] CPL (Component Placement List)
- [ ] Verify rotation in CPL matches JLCPCB

---

## JLCPCB Order Review

### Board Options
- [ ] Correct layer count
- [ ] Correct dimensions
- [ ] Appropriate thickness (1.6mm standard)
- [ ] Solder mask color
- [ ] Silkscreen color
- [ ] Surface finish (HASL, ENIG)
- [ ] Copper weight

### DFM Analysis
- [ ] Review JLCPCB DFM report
- [ ] Address any warnings
- [ ] Confirm no errors

### SMT Assembly (if applicable)
- [ ] Parts matched correctly
- [ ] Rotation verified
- [ ] Quantity correct
- [ ] Assembly side correct (top/bottom)

---

## JLCPCB Specifications Quick Reference

### Standard 2-Layer PCB

| Spec | Standard Value |
|------|----------------|
| Thickness | 1.6mm |
| Copper | 1oz (35μm) |
| Min Trace | 0.127mm / 5mil |
| Min Space | 0.127mm / 5mil |
| Min Drill | 0.3mm |
| Min Annular Ring | 0.13mm |
| Min Silkscreen | 0.15mm line, 0.8mm text |

### Lead Times (Typical)

| Service | Time |
|---------|------|
| Standard PCB | 2-5 days |
| SMT Assembly | +2-4 days |
| Shipping (DHL) | 3-7 days |

### Cost Tips

1. **Board size**: Keep under 100x100mm
2. **Quantity**: Price breaks at 5, 10, 30, 50
3. **Parts**: Use Basic Parts for SMT
4. **Color**: Green is cheapest
5. **Finish**: HASL is cheapest

---

## Common Issues & Solutions

### Issue: Silkscreen on pads
**Solution**: Set silkscreen clearance in design rules

### Issue: Traces too thin for DFM
**Solution**: Use minimum 0.15mm (6mil) for safety margin

### Issue: Via drill too small
**Solution**: Use 0.3mm minimum, 0.4mm preferred

### Issue: Missing board outline
**Solution**: Ensure Edge.Cuts is closed polygon

### Issue: Component rotation wrong
**Solution**: Review CPL, adjust rotation by 90/180/270°

### Issue: Part not in JLCPCB library
**Solution**: Use "Extended Parts" or hand-solder
