# MICS-4514 - Dual Gas Sensor (CO + NO2)

## Manufacturer
SGX Sensortech (Amphenol)

## JLCPCB Part Number
C688686

## Datasheet
Local: `../2411261432_Amphenol-ICC-MICS-4514_C3659364.pdf`
[MICS-4514 Datasheet](https://www.sgxsensortech.com/content/uploads/2014/08/1084_Datasheet-MiCS-4514.pdf)

---

## Key Specifications

### Power Supply
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Heater Voltage | 1.7 | 2.0 | 2.5 | V |
| Heater Power | - | 76 | - | mW |
| Heater Resistance | 66 | 76 | 86 | Ohm |

### CO Sensor (Reducing Gas)
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Detection Range | 1 | - | 1000 | ppm |
| Sensitivity (Rs ratio) | 1.5 | - | 30 | Rs/Ro |
| Sensor Resistance | 100k | - | 1.5M | Ohm |
| Response Time (T90) | - | - | 60 | sec |

### NO2 Sensor (Oxidizing Gas)
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Detection Range | 0.05 | - | 10 | ppm |
| Sensitivity (Rs ratio) | 1.5 | - | 15 | Rs/Ro |
| Sensor Resistance | 0.8k | - | 20k | Ohm |
| Response Time (T90) | - | - | 60 | sec |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -30 | +85 | C |
| Operating Humidity | 5 | 95 | %RH |
| Storage Temperature | -40 | +85 | C |

### Warm-up
| Parameter | Value | Unit |
|-----------|-------|------|
| Initial Warm-up | 30 | seconds |
| Continuous Operation | Recommended | - |

---

## Sensor Principle

### Reducing Sensor (RED) - CO Detection
- MEMS micro-hotplate with SnO2 sensing layer
- Resistance DECREASES in presence of CO
- Also responds to: H2, NH3, C2H5OH, CH4

### Oxidizing Sensor (OX) - NO2 Detection
- MEMS micro-hotplate with WO3 sensing layer
- Resistance INCREASES in presence of NO2
- Also responds to: Ozone

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| 1 | HEATER+ | Heater positive |
| 2 | HEATER- | Heater negative (common heater) |
| 3 | RED_A | Reducing sensor electrode A |
| 4 | RED_B | Reducing sensor electrode B |
| 5 | OX_A | Oxidizing sensor electrode A |
| 6 | OX_B | Oxidizing sensor electrode B |

---

## Application Circuit Notes

### Heater Control
- Use low-side MOSFET switch for heater
- PWM control for temperature regulation
- Monitor heater current for fault detection
- Common heater for both sensors

### Signal Conditioning (Per Sensor)
```
VCC ─┬─ R_load (10k) ─┬─ Sensor ─┬─ GND
     │                │          │
     │                └── R_series (1k) ─┬─ To ADC
     │                                   │
     └─────── C_filter (100nF) ──────────┘
```

### Load Resistor Selection
- RED sensor: 10k typical (high resistance sensor)
- OX sensor: 10k typical
- Adjust based on sensor resistance range

### ADC Requirements
- 12-bit ADC minimum
- Input range: 0-3.3V
- Sample rate: 10 Hz typical

---

## Safety Considerations

### CO Alarm Thresholds (Reference)
| Concentration | Alarm Time |
|---------------|------------|
| 70 ppm | 1-4 hours |
| 150 ppm | 10-50 minutes |
| 400 ppm | 4-15 minutes |

### Important Notes
- NOT for life-safety certified applications without proper certification
- Requires periodic calibration
- Cross-sensitivity to other gases must be considered
- Mount with airflow to cabin air
- Keep away from exhaust gases

---

## IMSAFE Project Usage
- Section: 10 (Gas Sensor)
- Interface: Analog (ADC)
- Signals:
  - GAS_HEATER_EN: GPIO with PWM capability
  - GAS_CO_ADC: ADC input for CO (reducing) sensor
  - GAS_NO2_ADC: ADC input for NO2 (oxidizing) sensor

## Related Components
- Q5: AO3400A heater switch (C20917)
- R1001: 10k gate pull-down (C25804)
- R1002: 100R gate series (C22775)
- R1003: 10k CO load resistor (C25804)
- R1004: 1k CO series protection (C22548)
- C1001: 100nF CO ADC filter (C14663)
- R1005: 10k NO2 load resistor (C25804)
- R1006: 1k NO2 series protection (C22548)
- C1002: 100nF NO2 ADC filter (C14663)
