# Connectivity Trade Study: SWD Debug & Bluetooth

**Date:** 2026-01-28
**Phase:** 2.1 SWD, 2.7 Bluetooth
**Status:** Draft for Review

---

## Part A: SWD Debug Connector

### A.1 Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Interface | SWD (Serial Wire Debug) | ARM Cortex-M standard |
| Signals | SWDIO, SWCLK, GND, VCC | Minimum required |
| Optional | SWO, NRST | Trace output, reset |
| Debugger | ST-Link, J-Link, etc. | Standard ARM debuggers |

### A.2 Connector Options

| Type | Pins | Pitch | Size | Debugger Compatibility |
|------|------|-------|------|------------------------|
| ARM 10-pin | 10 | 1.27mm | 12.7×5.1mm | All standard (with adapter) |
| ARM 20-pin | 20 | 2.54mm | 25.4×5.1mm | All standard (legacy JTAG) |
| ST 14-pin | 14 | 1.27mm | 17.8×5.1mm | ST-Link native |
| Tag-Connect TC2030 | 6 | N/A | Pogo pads | All (with cable) |
| Custom 6-pin header | 6 | 2.54mm | 15.2×2.5mm | With jumper wires |
| Custom 4-pin header | 4 | 2.54mm | 10.2×2.5mm | Minimal, with jumpers |

### A.3 Selection: Tag-Connect TC2030-IDC-NL + 6-pin Header

**Primary: Tag-Connect TC2030-IDC-NL (No-Legs)**

| Parameter | Value |
|-----------|-------|
| Footprint | 6 pogo pads, 9.9×4.3mm |
| Signals | VCC, SWDIO, NRST, SWCLK, GND, SWO |
| Cost | Footprint only (~$0) |
| Cable cost | ~$35-40 (one-time) |

**Rationale:**
- Minimal PCB footprint (no connector)
- Production-friendly (no connector to populate)
- Industry standard for small boards
- Supports SWO trace output

**Backup: 6-pin 2.54mm Header (0.1" pitch)**

For prototype convenience, also include standard header pads:

| Pin | Signal | STM32H747 |
|-----|--------|-----------|
| 1 | VCC | 3.3V |
| 2 | SWDIO | PA13 |
| 3 | GND | GND |
| 4 | SWCLK | PA14 |
| 5 | NRST | NRST |
| 6 | SWO | PB3 |

### A.4 Design Notes

- 10kΩ pull-up on NRST (required)
- 100nF cap on NRST for noise filtering
- Keep SWD traces short, away from high-speed signals
- Test points for probe access during debug

---

## Part B: Bluetooth (BLE 5.0)

### B.1 Requirements

| Parameter | Requirement | Source |
|-----------|-------------|--------|
| Standard | BLE 5.0 minimum | FR-CN-003 |
| Range | ≥10m | FR-CN-006 |
| Data rate | >50 KB/s | For practical log download |
| Security | Secure bonding | FR-CN-007 |
| Interface | UART | System architecture |
| Use cases | Log download, app config | FR-CN-004, FR-CN-005 |
| Priority | Should | Not safety-critical |

### B.2 Candidates

| Module | BLE Ver | Range | Interface | Antenna | FCC/CE | JLCPCB | Price |
|--------|---------|-------|-----------|---------|--------|--------|-------|
| HM-19 | 5.0 | 30m | UART | Onboard | Yes | No | ~$5 |
| RN4870 | 5.0 | 10m | UART | Onboard | Yes | No | ~$4 |
| ESP32-C3-MINI-1 | 5.0 | 30m+ | UART/SPI | Onboard | Yes | Yes | ~$2 |
| JDY-23 | 5.0 | 20m | UART | Onboard | Yes | Basic | ~$2 |
| nRF52840 module | 5.0 | 100m | UART/SPI | Onboard | Yes | No | ~$8 |

### B.3 Detailed Evaluation

#### ESP32-C3-MINI-1 (Espressif)

| Parameter | Value |
|-----------|-------|
| BLE | 5.0 |
| WiFi | 802.11 b/g/n (2.4GHz) |
| CPU | RISC-V 160MHz |
| Flash | 4MB |
| Interface | UART, SPI, I2C, GPIO |
| Antenna | PCB antenna (onboard) |
| Package | 13.2×16.6×2.4mm |
| Certification | FCC, CE, IC, MIC, KCC |
| JLCPCB | Yes (basic/extended) |
| Price | ~$2 |

**Pros:**
- Very low cost
- BLE 5.0 + WiFi bonus (future expansion)
- Pre-certified module
- JLCPCB stock
- Can run AT firmware or custom firmware
- 4MB flash for OTA updates

**Cons:**
- Requires flashing AT firmware (or use pre-flashed)
- More complex than simple UART module
- Higher power than dedicated BLE

#### JDY-23 (Generic BLE 5.0)

| Parameter | Value |
|-----------|-------|
| BLE | 5.0 |
| Interface | UART (115200 default) |
| Range | ~20m |
| Package | 10.5×13.5mm |
| Certification | FCC (check supplier) |
| Price | ~$1.50-2 |

**Pros:**
- Very simple UART interface
- AT command set
- Low cost
- Small footprint

**Cons:**
- Generic Chinese module, documentation quality varies
- Certification may need verification
- Limited configurability

#### RN4870 (Microchip)

| Parameter | Value |
|-----------|-------|
| BLE | 5.0 |
| Interface | UART (ASCII command interface) |
| Range | 10m (class 2) |
| Package | 11.5×9×1.8mm |
| Certification | FCC, IC, CE, MIC, KC |
| Price | ~$4-5 |

**Pros:**
- Reputable manufacturer (Microchip)
- Excellent documentation
- Simple ASCII command interface
- Full certification

**Cons:**
- Higher cost
- Not typically at JLCPCB

### B.4 Selection: ESP32-C3-MINI-1

**Rationale:**
- BLE 5.0 with WiFi bonus (future app/cloud features)
- Pre-certified (FCC/CE/IC)
- JLCPCB availability
- Lowest cost (~$2)
- Ample processing power for BLE stack
- Can run transparent UART bridge firmware
- 4MB flash for potential future features

**Firmware approach:**
1. **Simple:** Flash with ESP-AT firmware (UART transparent bridge)
2. **Advanced:** Custom firmware for optimized BLE profile

### B.5 Interface Design

```
STM32H747                    ESP32-C3-MINI-1
─────────                    ───────────────
USART2_TX (PA2) ──────────► RXD (GPIO20)
USART2_RX (PA3) ◄────────── TXD (GPIO21)
GPIO (PC1)      ──────────► EN (enable, active high)
GPIO (PC2)      ──────────► IO9 (boot mode, pull-up for normal)
3.3V            ──────────► 3V3
GND             ──────────► GND
```

**UART Configuration:**
- Baud: 115200 (default ESP-AT)
- 8N1 (8 data, no parity, 1 stop)
- Hardware flow control: optional (RTS/CTS)

### B.6 Power Budget

| State | Current | Notes |
|-------|---------|-------|
| Deep sleep | 5 µA | BLE advertising off |
| Light sleep | 130 µA | BLE advertising |
| Active (BLE conn) | 20-50 mA | Connected, moderate traffic |
| TX peak | 350 mA | Short bursts during TX |

**Typical average:** ~25 mA during BLE operation

### B.7 Antenna Considerations

ESP32-C3-MINI-1 has onboard PCB antenna:
- Keep antenna area clear of ground plane (per module datasheet)
- No metal enclosure directly over antenna
- Position near enclosure edge if possible
- For extended range: external antenna variant available (ESP32-C3-MINI-1U)

---

## Summary

### SWD Debug (DEC-021)

| Item | Selection |
|------|-----------|
| Primary | Tag-Connect TC2030-IDC-NL footprint |
| Backup | 6-pin 2.54mm header pads |
| Signals | VCC, SWDIO, SWCLK, GND, NRST, SWO |
| Cost | ~$0 (footprint only) |

### Bluetooth (DEC-022)

| Item | Selection |
|------|-----------|
| Module | ESP32-C3-MINI-1 |
| Standard | BLE 5.0 (+WiFi 802.11n) |
| Interface | UART (115200, 8N1) |
| Firmware | ESP-AT (transparent bridge) |
| Certification | FCC, CE, IC |
| Cost | ~$2 |

---

## References

- [Tag-Connect TC2030-IDC](https://www.tag-connect.com/product/tc2030-idc-nl)
- [ESP32-C3-MINI-1 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_en.pdf)
- [ESP-AT User Guide](https://docs.espressif.com/projects/esp-at/en/latest/)
- [RN4870 Datasheet](https://www.microchip.com/en-us/product/RN4870)
