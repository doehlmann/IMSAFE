# IMSAFE Work Hook - Session Context

## Current Focus

- [ ] Complete functional specification
- [ ] Finalize component selection for main MCU
- [ ] Create initial schematic block diagrams

## Open Questions

1. **MCU Selection**: XMC4700 for radar - need to determine second MCU for main functions
2. **Display Interface**: SPI vs parallel for ST7796
3. **Vibration Analysis**: IMU selection for engine/gearbox monitoring
4. **Power Budget**: Calculate actual consumption with all sensors active

## Recent Decisions

| Date | Decision | Rationale |
|------|----------|-----------|
| 2026-01-27 | XMC4700 for radar | Compatible with Distance2goL platform |
| 2026-01-27 | BMP390 for altitude | I2C, good accuracy, JLCPCB available |
| 2026-01-27 | MICS-4514 for CO/NO2 | Dual gas sensing capability |

## Next Actions

1. Complete block diagram in `docs/02-architecture/`
2. Create component selection matrix for MCU options
3. Begin MainBoard schematic

## Notes

- Landing radar based on Distance2goL platform (Infineon BGT24LTR11)
- Consider CPU load for vibration FFT analysis - may run periodically (every 10s)
- Two MCU architecture may require I2C or SPI inter-processor communication
