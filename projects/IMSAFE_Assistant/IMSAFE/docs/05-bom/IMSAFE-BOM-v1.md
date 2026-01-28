# IMSAFE Bill of Materials (BOM) v1.0

**Date:** 2026-01-28
**Phase:** Phase 2 Complete
**Status:** Draft - Verify JLCPCB availability before ordering

---

## Summary

| Category | Items | Est. Cost |
|----------|-------|-----------|
| MCU & Support | 5 | $12 |
| Power Management | 12 | $18 |
| Sensors (Internal) | 5 | $19 |
| Sensors (External Vibration) | 2 | $75 |
| Display | 1 | $15 |
| UI Components | 28 | $5 |
| Storage | 2 | $2 |
| Connectivity | 2 | $3 |
| Connectors | 6 | $6 |
| Passives (est.) | ~80 | $5 |
| **Total** | ~143 | **~$160** |

*Note: Prices are estimates. External vibration sensors (~$75) and display (~$15) dominate cost.*

---

## 1. MCU & Core

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U1 | MCU Dual-Core M7+M4 | STM32H747AGI6 | LQFP-176 | 1 | $8.00 | $8.00 | C2062629 |
| Y1 | Crystal 8MHz | X322508MSB4SI | 3225 | 1 | $0.20 | $0.20 | C9002 |
| Y2 | Crystal 32.768kHz | X322532768MSB4GI | 3215 | 1 | $0.15 | $0.15 | C32346 |
| C_HSE | Load cap 15pF | CL10C150JB8NNNC | 0603 | 2 | $0.01 | $0.02 | C1644 |
| C_LSE | Load cap 6.8pF | CL10C6R8CB8NNNC | 0603 | 2 | $0.01 | $0.02 | C105620 |
| | | | | **Subtotal** | | **$8.39** | |

---

## 2. Power Management

### 2.1 USB-C Charging (BQ25792)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U2 | Buck-Boost Charger | BQ25792RQMR | QFN-29 4x4 | 1 | $4.00 | $4.00 | C5446540 |
| L1 | Inductor 2.2µH 5A | MWSA0503S-2R2MT | 5x5mm | 1 | $0.30 | $0.30 | C408356 |
| C_IN | Input cap 10µF 25V | CL21A106KAYNNNE | 0805 | 2 | $0.05 | $0.10 | C15850 |
| C_OUT | Output cap 22µF 10V | CL21A226MAQNNNE | 0805 | 2 | $0.05 | $0.10 | C5672 |
| R_ILIM | Current sense 10mΩ | 0603WAF100LT5E | 0603 | 1 | $0.02 | $0.02 | C22859 |

### 2.2 3.3V Regulator (TPS62133)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U3 | Buck 3.3V 3A | TPS62133RGTR | QFN-16 3x3 | 1 | $1.50 | $1.50 | C129780 |
| L2 | Inductor 1.5µH 4A | XAL4020-152MEB | 4x4mm | 1 | $0.40 | $0.40 | C191761 |
| C_3V3_IN | Input cap 10µF 16V | CL21A106KAYNNNE | 0805 | 1 | $0.05 | $0.05 | C15850 |
| C_3V3_OUT | Output cap 22µF 10V | CL21A226MAQNNNE | 0805 | 2 | $0.05 | $0.10 | C5672 |

### 2.3 5V Regulator (TPS62130)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U4 | Buck Adj 3A | TPS62130RGTR | QFN-16 3x3 | 1 | $1.50 | $1.50 | C129779 |
| L3 | Inductor 1.5µH 4A | XAL4020-152MEB | 4x4mm | 1 | $0.40 | $0.40 | C191761 |
| R_FB1 | Feedback 100kΩ | 0603WAF1003T5E | 0603 | 1 | $0.01 | $0.01 | C25803 |
| R_FB2 | Feedback 24.3kΩ | 0603WAF2432T5E | 0603 | 1 | $0.01 | $0.01 | C23351 |
| C_5V_IN | Input cap 10µF 16V | CL21A106KAYNNNE | 0805 | 1 | $0.05 | $0.05 | C15850 |
| C_5V_OUT | Output cap 22µF 10V | CL21A226MAQNNNE | 0805 | 2 | $0.05 | $0.10 | C5672 |

### 2.4 BMS (HY2120 + HY2213)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U5 | 2S Protection IC | HY2120-CB | SOT-23-6 | 1 | $0.15 | $0.15 | C113632 |
| U6, U7 | Cell Balancer | HY2213-BB3A | SOT-23-6 | 2 | $0.10 | $0.20 | C113633 |
| Q1-Q4 | N-ch MOSFET 30V | AO3400A | SOT-23 | 4 | $0.03 | $0.12 | C20917 |
| R_BAL | Balance 62Ω 1/8W | 0603WAF620JT5E | 0603 | 2 | $0.01 | $0.02 | C23220 |
| R_SENSE | Sense 5mΩ | 0805W8F005LT5E | 0805 | 1 | $0.05 | $0.05 | C318841 |

### 2.5 ESD & Protection

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U8 | USB ESD | USBLC6-2SC6 | SOT-23-6 | 1 | $0.10 | $0.10 | C7519 |
| U9 | SD ESD | USBLC6-2SC6 | SOT-23-6 | 2 | $0.10 | $0.20 | C7519 |
| F1 | PTC Fuse 2A | 1812L200DR | 1812 | 1 | $0.15 | $0.15 | C145565 |
| D1 | TVS 12V 600W | SMBJ12A | SMB | 1 | $0.10 | $0.10 | C64944 |
| | | | | **Subtotal** | | **$9.63** | |

---

## 3. Sensors (Internal/PCB-Mount)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U10 | Barometric | BMP390 | LGA 2x2 | 1 | $3.00 | $3.00 | C2682150 |
| U11 | IMU 6-axis | ICM-42688-P | LGA 2.5x3 | 1 | $4.00 | $4.00 | C2839277 |
| U12 | Magnetometer | MMC5983MA | LGA 3x3 | 1 | $2.00 | $2.00 | C2842878 |
| U13 | Gas CO/NO2 | MICS-4514 | SMD | 1 | $8.00 | $8.00 | C3659364 |
| U14 | Temp/Humidity | Si7021-A20-GM | DFN 3x3 | 1 | $2.00 | $2.00 | C82463 |
| | | | | **Subtotal** | | **$19.00** | |

---

## 4. Sensors (External Vibration Pods)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U_VIB1 | Engine Accel | ADXL355BCCZ | LGA 6x6 | 1 | $65.00 | $65.00 | Extended |
| U_VIB2 | Gearbox Accel | ADXL372BCCZ | LGA 3x3.25 | 1 | $10.00 | $10.00 | C2828056 |
| | | | | **Subtotal** | | **$75.00** | |

*Note: ADXL355 may need direct order from ADI/Mouser/Digikey*

---

## 5. Display

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| DISP1 | 3.5" TFT 320x480 | ST7789V2 module | FPC | 1 | $15.00 | $15.00 | Module |
| | | | | **Subtotal** | | **$15.00** | |

*Note: Display is typically a module purchase, not JLCPCB assembly*

---

## 6. UI Components

### 6.1 Switches & Encoders

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| SW1-8 | Toggle SPDT | MTS-102 | Through-hole | 8 | $0.15 | $1.20 | C431540 |
| ENC1-2 | Rotary Encoder | EC11 | Through-hole | 2 | $0.30 | $0.60 | C470754 |
| SW_PWR | Slide switch | SS12D00G3 | Through-hole | 1 | $0.05 | $0.05 | C431541 |

### 6.2 LEDs

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| LED1-16 | RGB Addressable | WS2812B-2020 | 2020 | 16 | $0.08 | $1.28 | C965555 |

### 6.3 I/O Expander

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U15 | I/O Expander 16-bit | MCP23017-E/SO | SOIC-28 | 1 | $1.00 | $1.00 | C47023 |

### 6.4 Audio

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| BZ1 | Buzzer 85dB | MLT-8530 | SMD | 1 | $0.30 | $0.30 | C94599 |
| Q5 | Buzzer driver | AO3400A | SOT-23 | 1 | $0.03 | $0.03 | C20917 |
| | | | | **Subtotal** | | **$4.46** | |

---

## 7. Storage

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| J_SD | microSD Socket Hinged | MEM2067-02-180-00-A | SMD | 1 | $1.50 | $1.50 | C393941 |
| | | | | **Subtotal** | | **$1.50** | |

*Note: microSD card purchased separately (~$25-50 for industrial grade)*

---

## 8. Connectivity

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| U16 | BLE/WiFi Module | ESP32-C3-MINI-1 | Module | 1 | $2.00 | $2.00 | C2838502 |
| | | | | **Subtotal** | | **$2.00** | |

---

## 9. Connectors

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| J_USB | USB-C 16-pin | U262-161N-4BVC11 | SMD | 1 | $0.30 | $0.30 | C319148 |
| J_BAT | Battery XT30 | XT30PW-M | PCB mount | 1 | $0.30 | $0.30 | C2907192 |
| J_BAL | Balance JST-XH 3P | S3B-XH-A | Through-hole | 1 | $0.10 | $0.10 | C144394 |
| J_VIB1 | Sensor M8 4-pin | (External) | Panel | 2 | $1.50 | $3.00 | External |
| J_SWD | Tag-Connect pads | TC2030-IDC-NL | Pads only | 1 | $0.00 | $0.00 | - |
| J_HDR | Debug header 6-pin | (Optional) | 2.54mm | 1 | $0.10 | $0.10 | C376104 |
| R_CC | USB CC resistors 5.1k | 0603WAF5101T5E | 0603 | 2 | $0.01 | $0.02 | C23186 |
| | | | | **Subtotal** | | **$3.82** | |

---

## 10. Decoupling & Passives (Estimated)

| Ref | Description | Part Number | Package | Qty | Unit $ | Ext $ | JLCPCB |
|-----|-------------|-------------|---------|-----|--------|-------|--------|
| C_100n | Decoupling 100nF | CL10B104KB8NNNC | 0603 | 30 | $0.01 | $0.30 | C14663 |
| C_10u | Bulk 10µF | CL21A106KAYNNNE | 0805 | 10 | $0.05 | $0.50 | C15850 |
| C_100u | Bulk 100µF 16V | EEE-1CA101UP | 6.3mm | 2 | $0.15 | $0.30 | C134580 |
| C_VCAP | MCU VCAP 2.2µF | CL10A225KQ8NNNC | 0603 | 2 | $0.02 | $0.04 | C23630 |
| R_10k | Pull-up/down 10kΩ | 0603WAF1002T5E | 0603 | 20 | $0.01 | $0.20 | C25804 |
| R_4k7 | I2C pull-up 4.7kΩ | 0603WAF4701T5E | 0603 | 4 | $0.01 | $0.04 | C23162 |
| R_22 | Series resistors 22Ω | 0603WAF220JT5E | 0603 | 10 | $0.01 | $0.10 | C23345 |
| | | | | **Subtotal** | | **~$2.00** | |

---

## Cost Summary

| Category | Cost |
|----------|------|
| MCU & Support | $8.39 |
| Power Management | $9.63 |
| Sensors (Internal) | $19.00 |
| Sensors (Vibration) | $75.00 |
| Display | $15.00 |
| UI Components | $4.46 |
| Storage | $1.50 |
| Connectivity | $2.00 |
| Connectors | $3.82 |
| Passives | ~$2.00 |
| **PCB BOM Total** | **~$141** |

### Additional Costs (Not in BOM)

| Item | Est. Cost |
|------|-----------|
| PCB fabrication (4-layer, 100x100mm) | $15-30 |
| JLCPCB assembly | $30-50 |
| Industrial microSD card | $25-50 |
| Display module | Included above |
| Vibration sensor pods (housing, cable) | $50-100 |
| Battery pack (2S 16Ah) | $50-100 |
| Enclosure | $20-50 |
| **Total System Cost** | **~$330-520** |

---

## Notes

1. **JLCPCB Part Numbers:** Verify availability and pricing before ordering. Extended parts have assembly fee surcharge.

2. **ADXL355:** May not be available at JLCPCB. Order separately from Mouser/Digikey/ADI direct.

3. **Display Module:** Purchase as complete module with FPC connector. Various suppliers (AliExpress, Waveshare, etc.)

4. **Vibration Sensor Pods:** Custom assembly - sensor + PCB + housing + cable + M8 connector.

5. **Quantity Pricing:** Prices based on qty 1-10. Higher quantities significantly reduce per-unit cost.

6. **Alternates:** See decision-log.md for approved alternate parts if primary unavailable.

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-28 | Initial BOM from Phase 2 component selection |
