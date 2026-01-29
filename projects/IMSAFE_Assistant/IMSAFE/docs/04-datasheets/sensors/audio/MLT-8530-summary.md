# MLT-8530 - Active Magnetic Buzzer

## Manufacturer
Jiangsu Huaneng Electronics

## JLCPCB Part Number
C94599

## Datasheet
Search: JLCPCB C94599 or MLT-8530 datasheet

---

## Key Specifications

### Electrical
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Rated Voltage | - | 3.0 | - | V |
| Operating Voltage | 2.0 | - | 4.0 | V |
| Rated Current | - | 30 | 35 | mA |
| Coil Resistance | - | 42 | - | Ohm |

### Acoustic
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Sound Pressure Level | - | 85 | - | dB @ 10cm |
| Resonant Frequency | - | 2700 | - | Hz |
| Frequency Tolerance | -300 | - | +300 | Hz |

### Environmental
| Parameter | Min | Max | Unit |
|-----------|-----|-----|------|
| Operating Temperature | -20 | +70 | C |
| Storage Temperature | -30 | +80 | C |

### Physical
| Parameter | Value | Unit |
|-----------|-------|------|
| Diameter | 8.5 | mm |
| Height | 3.0 | mm |
| Pin Spacing | 4.0 | mm |
| Weight | ~1 | g |

---

## Active vs Passive Buzzer

### Active Buzzer (MLT-8530)
- Contains internal oscillator circuit
- Apply DC voltage = sound output
- Fixed frequency (2700 Hz)
- Cannot change pitch
- Simpler to drive

### Passive Buzzer
- No internal oscillator
- Requires external AC signal (PWM)
- Variable frequency/pitch
- More complex to drive
- More flexible for tones

---

## Pin Configuration

| Pin | Name | Function |
|-----|------|----------|
| + | Positive | Supply voltage (marked with +) |
| - | Negative | Ground |

**Note**: Observe polarity - buzzer is polarized!

---

## Driver Circuit

### Low-Side N-Channel MOSFET Switch
```
+3.3V ─────┬───── Buzzer (+)
           │
           │ (-)
           │
           └─── MOSFET Drain
                    │
                    Source ── GND
                    │
                    Gate ── R_series ─┬── GPIO
                                      │
                                R_pulldown ── GND
```

### Component Values
- Q_driver: AO3400A (SOT-23)
- R_pulldown: 10k (ensures OFF at reset)
- R_series: 100R (limits gate ringing)
- D_flyback: 1N4148W (inductive protection)

### Flyback Diode
- Buzzer coil is inductive
- Cathode to +3.3V, Anode to Drain
- Protects MOSFET from voltage spikes

---

## Control Methods

### Simple ON/OFF
```c
// Buzzer ON
HAL_GPIO_WritePin(BUZZER_EN_GPIO_Port, BUZZER_EN_Pin, GPIO_PIN_SET);

// Buzzer OFF
HAL_GPIO_WritePin(BUZZER_EN_GPIO_Port, BUZZER_EN_Pin, GPIO_PIN_RESET);
```

### Beep Pattern (using PWM for duty cycle)
```c
// Short beep
buzzer_on();
HAL_Delay(100);
buzzer_off();
HAL_Delay(100);
```

### Volume Control (PWM duty cycle)
- 100% duty = full volume
- 50% duty = reduced volume (perceived)
- Note: Active buzzer has fixed pitch

---

## Application Notes

### PCB Layout
- Mount with sound holes facing outward
- Ensure adequate clearance for acoustic output
- Keep away from user's ear (loud at close range)

### Safety Applications
- Use for CO alarm (safety critical)
- Use for low battery warning
- Ensure backup power for safety alarms
- Consider redundant buzzer for critical alarms

### Power Considerations
- 30mA @ 3.3V = 99mW per buzzer
- Account for inrush current
- Use adequate trace width

---

## IMSAFE Project Usage
- Section: 19 (Audio Alarm)
- Interface: GPIO (PWM capable)
- Signal: BUZZER_EN
- Use cases:
  - CO alarm (safety critical)
  - Low battery warning
  - Checklist incomplete alert
  - Notification tones

## Related Components
- BZ1: MLT-8530 buzzer (C94599)
- Q7: AO3400A driver MOSFET (C20917)
- R1901: 10k gate pull-down (C25804)
- R1902: 100R gate series (C22775)
- D2: 1N4148W flyback diode (C81598)
