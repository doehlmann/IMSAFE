# Electronics Development Context

## Development Workflow

```
Concept Design → Schematic → PCB Layout → Fabrication → Assembly → Firmware → Test
```

## Tools & Environment

### EDA Tools
- **KiCad**: Local schematic and PCB design
- **EasyEDA**: Web-based design with JLCPCB integration

### MCU Development
- **Microcontroller**: STM32 family (F0, F1, F4, G4, H7, etc.)
- **IDE**: STM32CubeIDE (Eclipse-based)
- **Configuration**: STM32CubeMX for peripheral initialization
- **Programming**: STM32Programmer (direct flashing, bypassing Arduino)

### Manufacturing
- **PCB Fabrication**: JLCPCB
- **Assembly**: JLCPCB SMT service
- **Parts**: JLCPCB parts library for SMT assembly

---

## STM32 Development Guidelines

### Project Structure (STM32CubeIDE)
```
project_name/
├── Core/
│   ├── Inc/           # Header files
│   │   ├── main.h
│   │   └── stm32xxxx_it.h
│   └── Src/           # Source files
│       ├── main.c
│       ├── stm32xxxx_it.c  # Interrupt handlers
│       └── syscalls.c
├── Drivers/
│   ├── CMSIS/         # ARM CMSIS headers
│   └── STM32xxxx_HAL_Driver/  # HAL library
├── project_name.ioc   # CubeMX configuration
└── STM32xxxx_FLASH.ld # Linker script
```

### HAL vs Register-Level
- **HAL**: Use for standard peripherals, faster development
- **LL (Low-Layer)**: Use for timing-critical code
- **Register**: Use only when HAL/LL insufficient

### Common Peripheral Initialization Pattern
```c
// In main.c after MX_xxx_Init() calls
void App_Init(void) {
    // Application-specific initialization
}

// In main loop
while (1) {
    App_Run();
    // or state machine
}
```

### Interrupt Handling
- Keep ISRs short - set flags, use DMA
- Process in main loop or RTOS task
- Use `__disable_irq()` / `__enable_irq()` for critical sections

---

## KiCad Guidelines

### Project Structure
```
project_name/
├── project_name.kicad_pro    # Project file
├── project_name.kicad_sch    # Schematic
├── project_name.kicad_pcb    # PCB layout
├── fp-lib-table              # Footprint library table
├── sym-lib-table             # Symbol library table
├── gerbers/                  # Export directory
└── docs/                     # Documentation
```

### JLCPCB Design Rules (2-layer)
| Parameter | Value |
|-----------|-------|
| Min trace width | 0.127mm (5mil) |
| Min spacing | 0.127mm (5mil) |
| Min drill | 0.3mm |
| Min annular ring | 0.13mm |
| Board thickness | 1.6mm (standard) |
| Copper weight | 1oz (35um) |
| Min silkscreen | 0.15mm width |

### Export for JLCPCB
1. Plot Gerbers: F.Cu, B.Cu, F.SilkS, B.SilkS, F.Mask, B.Mask, Edge.Cuts
2. Generate drill files (Excellon format)
3. Zip all files
4. Optional: Generate pick-and-place (CPL) and BOM for SMT assembly

---

## EasyEDA / JLCPCB Integration

### Advantages
- Direct parts library from JLCPCB/LCSC
- One-click ordering
- Integrated DFM check

### SMT Assembly Files
- **BOM**: Part number, designator, quantity
- **CPL**: Component Placement List (X, Y, rotation, side)

### Part Selection Tips
- Use "Basic Parts" for lowest assembly cost
- Check stock levels before design
- Verify footprint matches datasheet

---

## Common Circuit Blocks

### Power Supply (3.3V LDO)
- Input: 5V USB or external
- LDO: AMS1117-3.3 or similar
- Caps: 10uF input, 10uF + 100nF output

### USB (STM32)
- Use internal USB peripheral (F4, G4, etc.)
- ESD protection: USBLC6-2SC6
- 1.5k pull-up on D+ (or internal)

### Crystal Oscillator
- HSE: 8MHz typical (check STM32 variant)
- Load capacitors: Per crystal datasheet
- LSE: 32.768kHz for RTC

### Debug Interface (SWD)
- SWDIO, SWCLK, GND, VCC
- Optional: SWO (trace), NRST
- 10k pull-up on NRST recommended

### I2C Pull-ups
- 4.7k for 100kHz
- 2.2k for 400kHz
- Consider bus capacitance

---

## Design Checklist

### Schematic Review
- [ ] Power rails correct voltage
- [ ] Decoupling caps on all ICs
- [ ] Crystal load caps calculated
- [ ] Reset circuit correct
- [ ] ESD protection on external interfaces
- [ ] Test points for debug
- [ ] All pins accounted for (NC pins handled)

### PCB Review
- [ ] Design rules pass
- [ ] Power planes/traces adequate
- [ ] Decoupling caps near IC pins
- [ ] Crystal traces short, guarded
- [ ] Mounting holes correct size
- [ ] Silkscreen readable, correct
- [ ] Fiducials for SMT assembly

### Firmware Review
- [ ] Clock configuration correct
- [ ] Peripheral initialization complete
- [ ] Interrupt priorities set
- [ ] Watchdog configured (if used)
- [ ] Error handling in place

---

## Templates Location

- `_templates/stm32-cubeide/` - STM32 project structure reference
- `_templates/kicad-project/` - KiCad project with JLCPCB rules
- `_templates/product-concept/` - Full product documentation template

## Knowledge Base Location

- `_knowledge-base/stm32-reference/` - Peripheral patterns, HAL usage
- `_knowledge-base/component-library/` - Common parts, JLCPCB parts
- `_knowledge-base/design-rules/` - PCB rules, manufacturing specs
