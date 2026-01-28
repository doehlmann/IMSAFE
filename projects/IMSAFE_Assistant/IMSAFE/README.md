# IMSAFE Flying Assistant

GA Pilot Safety Assistant Device

## Project Overview

The IMSAFE Flying Assistant is a safety enhancement device for General Aviation (GA) pilots and Light Sport Aircraft (LSA) pilots. It provides electronic monitoring of equipment and procedures through a tactile checklist interface with visual feedback.

**Status:** Development

## Key Features

- **8-Position Toggle Checklist** - SPDT switches for takeoff (up) and landing (down) procedures
- **Programmable LED Feedback** - WS2812 LEDs above/below each switch
- **3.5" TFT Display** - ST7796 320x480 for flight information
- **Environmental Sensors** - CO/NO2 (MICS-4514), altitude (BMP390), temperature
- **IMU & Compass** - MPU9250/6500 + HMC5983 for synthetic horizon display
- **Landing Radar** - Infineon BGT24LTR11 for flare assistance (0-30m)
- **Vibration Analysis** - Engine/gearbox monitoring via external IMUs
- **Data Logging** - MicroSD card for flight recording
- **Bluetooth Connectivity** - Wireless data access
- **Battery Backup** - 7.4V 16000mAh Li-ion with USB-C charging

## Repository Structure

```
IMSAFE/
├── docs/                    # Documentation
│   ├── 00-functional-spec/  # Product specification
│   ├── 01-requirements/     # Requirements & verification
│   ├── 02-architecture/     # System architecture
│   ├── 03-component-selection/
│   ├── 04-datasheets/       # Component datasheets
│   └── 05-test/             # Test procedures & reports
├── hardware/                # Hardware design
│   ├── kicad/               # KiCad projects
│   ├── libraries/           # Symbols, footprints, 3D models
│   └── manufacturing/       # Gerbers, BOM, assembly files
├── firmware/                # Embedded software
│   ├── app/                 # Application code
│   ├── drivers/             # Peripheral drivers
│   ├── boards/              # Board support packages
│   └── tools/               # Build tools & scripts
├── mechanical/              # Enclosure & mounting
├── tools/                   # Development tools & scripts
└── references/              # Vendor appnotes, eval board docs
```

## Hardware Boards

| Board | Description |
|-------|-------------|
| IMSAFE_MainBoard | Main MCU, display, sensors, power management |
| IMSAFE_SwitchLED_Board | 8x toggle switches + 16x WS2812 LEDs |
| IMSAFE_Radar_Interface | BGT24LTR11 radar module interface |

## Development Environment

- **MCU:** Infineon XMC4700 (radar), additional MCU TBD
- **IDE:** Infineon DAVE / STM32CubeIDE
- **EDA:** KiCad 8.x
- **PCB Fab:** JLCPCB

## Target Specifications

| Parameter | Target |
|-----------|--------|
| Unit Cost (BOM) | < $250 |
| Operating Voltage | 3.3V (internal), 7.4V battery |
| Operating Temperature | -20C to +70C |
| Battery Life | TBD |

## Getting Started

1. Clone the repository
2. Review the functional specification in `docs/00-functional-spec/`
3. Check component selection status in `docs/03-component-selection/`
4. Hardware designs are in `hardware/kicad/`

## License

See LICENSE file.

## Author

Dean Oehlmann
