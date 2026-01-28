# BQ25792 – Datasheet Summary

## Device Overview
**Part:** BQ25792 / BQ25792RQMR  
**Vendor:** Texas Instruments  
**Function:** High-integration, buck-boost battery charger IC with power-path and USB-PD/OTG support :contentReference[oaicite:0]{index=0}

The BQ25792 is a fully integrated **switch-mode buck-boost charger** capable of charging **1–4 series cells** (Li-ion/Polymer/LiFePO₄) with up to **5 A charge current**, featuring integrated FETs, power path control, multiple input sources, and USB Power Delivery/USB-OTG support. :contentReference[oaicite:1]{index=1}

---

## Key Features

### Charging and Power Path
- **Buck-boost charger** supports 1–4 cells (wide battery voltage range). :contentReference[oaicite:2]{index=2}  
- Integrated **four MOSFETs + BATFET**, plus current sensing for input and battery paths. :contentReference[oaicite:3]{index=3}  
- High efficiency at typical conditions (e.g., ~96.5 % at 16 V battery, 3 A charge). :contentReference[oaicite:4]{index=4}  
- **NVDC (Narrow Voltage DC) power path**: system voltage regulated slightly above battery without dropping below a minimum—good for reliable system operation. :contentReference[oaicite:5]{index=5}  
- **Dual-input power mux** option allows selection between two input sources via I²C. :contentReference[oaicite:6]{index=6}  

### Input Support & USB
- **Wide input voltage range:** typically **3.6 V to 24 V**, with absolute max ~30 V—suitable for USB-PD adapters or legacy adapters. :contentReference[oaicite:7]{index=7}  
- Supports USB charging standards (BC1.2, SDP, CDP, DCP, HVDCP). :contentReference[oaicite:8]{index=8}  
- **USB PD 3.0 OTG output:** battery can power the USB port at **2.8 V–22 V**, with adjustable current regulation. :contentReference[oaicite:9]{index=9}  

### Control & Monitoring
- **I²C interface** (configurable charging and monitoring). :contentReference[oaicite:10]{index=10}  
- Integrated ADC monitors battery/input/system voltages and temperatures. :contentReference[oaicite:11]{index=11}  

### Charging Control & Safety
- Supports autonomous charging (trickle, pre-charge, CC, CV phases). :contentReference[oaicite:12]{index=12}  
- Safety features:  
  - Battery temperature monitoring (JEITA profile)  
  - Over-voltage protection (OVP) / over-current protection (OCP)  
  - Thermal regulation and shutdown  
  - Charging safety timer :contentReference[oaicite:13]{index=13}  

---

## Performance Summary

| Parameter | Typical Value |
|-----------|---------------|
| Maximum charge current | **5 A** (10 mA resolution) :contentReference[oaicite:14]{index=14} |
| Input voltage | **3.6 – 24 V** operating, **30 V** abs max :contentReference[oaicite:15]{index=15} |
| USB-PD OTG output | **2.8 – 22 V** with regulation :contentReference[oaicite:16]{index=16} |
| Efficiency | ~96.5 % @ 3 A, 16 V battery :contentReference[oaicite:17]{index=17} |
| Quiescent battery current | ~21 µA (charger only) :contentReference[oaicite:18]{index=18} |
| Shutdown quiescent | ~600 nA :contentReference[oaicite:19]{index=19} |
| Operating temp | –40 °C to 85 °C :contentReference[oaicite:20]{index=20} |
| Package | **29-pin QFN, 4×4 mm** :contentReference[oaicite:21]{index=21} |

---

## Interfaces and Configuration

### I²C Control
- Full register configuration for:
  - Charging current
  - Input current limits
  - Battery voltage thresholds
  - OTG output settings :contentReference[oaicite:22]{index=22}

### Status & Pins
- **STAT output:** indicates charging state or fault conditions. :contentReference[oaicite:23]{index=23}  
- **INT pin:** host interrupt on faults. :contentReference[oaicite:24]{index=24}  
- **D+/D– pins:** support USB data line negotiation for PD/BC1.2 detection. :contentReference[oaicite:25]{index=25}

---

## Package & Layout Considerations
- **29-pin 4×4 mm QFN** with thermal pad. :contentReference[oaicite:26]{index=26}  
- Requires careful layout for:
  - Switching nodes (SW1/SW2)
  - Inductor and input caps
  - Thermal vias under pad :contentReference[oaicite:27]{index=27}

---

## Strengths & Risks (IMSAFE context)

### Strengths
- **Buck-boost support (1–4 cells):** suited for 2S and 3S battery systems. :contentReference[oaicite:28]{index=28}  
- **USB PD 3.0 OTG:** allows bidirectional power and load sharing from USB-C. :contentReference[oaicite:29]{index=29}  
- **High integration:** reduces external components and simplifies converter design. :contentReference[oaicite:30]{index=30}  
- **Flexible control:** supports both I²C control and autonomous charging. :contentReference[oaicite:31]{index=31}

### Design Risks
- **Thermal management:** high charge currents at elevated battery voltages require careful layout. :contentReference[oaicite:32]{index=32}  
- **Layout sensitivity:** switching and power path nodes are critical for EMI and stability. :contentReference[oaicite:33]{index=33}  
- **Software complexity:** I²C configuration increases firmware development effort.

---

## Suggested IMSAFE Use Cases
- **Primary charger for 2S/3S/4S battery packs**
- **USB-PD based charging with OTG support**
- **Systems requiring wide input voltage range**
- **Charging + power path management on a single IC**

---

## Notes
- Full datasheet (~111 pages) exists and should be referenced for detailed electrical constraints, recommended layouts, and register maps. :contentReference[oaicite:34]{index=34}

