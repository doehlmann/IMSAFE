# Storage Subsystem Trade Study: MicroSD for Flight Data Logging

**Date:** 2026-01-28
**Phase:** 2.6 Storage
**Status:** Draft for Review

---

## 1. Application Requirements

### 1.1 Use Case: Flight Data Logging

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Data types | Sensor data, GPS, events, settings | Continuous + event-triggered |
| Sample rate | 10-50 Hz sensors, 1 Hz GPS | ~1-5 KB/sec typical |
| Flight duration | 4-8 hours typical | GA mission profile |
| Storage per flight | 50-150 MB | Depends on sample rate |
| Retention | Multiple flights | User downloads periodically |
| Access | User-removable | PC-readable required |

### 1.2 Environmental Requirements

| Parameter | Requirement | Notes |
|-----------|-------------|-------|
| Operating temp | -20°C to +70°C | Cockpit environment |
| Storage temp | -40°C to +85°C | Aircraft parked in sun/cold |
| Vibration | Aircraft cockpit level | Engine-transmitted vibration |
| Shock | Hard landing, engine start | Transient events |
| Humidity | Non-condensing | Cockpit environment |
| Power stability | Sudden power loss possible | Engine shutdown, electrical failure |

### 1.3 Data Integrity Requirements

| Requirement | Priority | Notes |
|-------------|----------|-------|
| No data loss during flight | Critical | Safety-related logging |
| Power-loss resilience | High | Must not corrupt filesystem |
| PC readability | High | User convenience |
| Wear leveling | Medium | Continuous logging degrades flash |

---

## 2. Storage Media Comparison

### 2.1 Options Considered

| Media | Capacity | Removable | PC-Readable | Vibration | Notes |
|-------|----------|-----------|-------------|-----------|-------|
| MicroSD | 8-512 GB | Yes | Yes | Socket-dependent | Primary choice |
| eMMC | 8-128 GB | No | Via USB | Excellent (soldered) | Backup option |
| SPI Flash (W25Q) | 16-128 MB | No | Via USB | Excellent (soldered) | Too small |
| USB Flash | 8-256 GB | Yes | Yes | Poor | Connector issues |

### 2.2 Decision: MicroSD

**Rationale:**
- User-removable for easy download to PC
- Large capacity (32-128 GB typical)
- Standard FAT32/exFAT filesystem = universal PC compatibility
- Cost-effective
- Replaceable if worn

---

## 3. MicroSD Card Selection

### 3.1 Consumer vs Industrial Grade

| Parameter | Consumer | High Endurance | Industrial |
|-----------|----------|----------------|------------|
| Operating temp | 0°C to +70°C | -25°C to +85°C | -40°C to +85°C or +105°C |
| Storage temp | -25°C to +85°C | -40°C to +85°C | -40°C to +85°C |
| Endurance (P/E cycles) | 500-3,000 | 3,000-10,000 | 10,000-30,000+ |
| TBW (128GB) | 50-100 TB | 180-400 TB | 500+ TB |
| Power-loss protection | No | Some models | Yes |
| Sudden power-off | Data corruption risk | Partial protection | Full protection |
| Price (32GB) | $5-10 | $15-25 | $40-80 |

### 3.2 Flash Technology Comparison

| Technology | Bits/Cell | Endurance (P/E) | Speed | Cost | Notes |
|------------|-----------|-----------------|-------|------|-------|
| SLC | 1 | 100,000 | Fastest | 4× | Gold standard, rare |
| pSLC (pseudo-SLC) | 1 (from MLC) | 20,000-30,000 | Fast | 2× | Best value for industrial |
| MLC | 2 | 3,000-10,000 | Fast | 1.5× | Good balance |
| TLC | 3 | 500-3,000 | Medium | 1× | Consumer standard |
| QLC | 4 | 100-1,000 | Slow | 0.8× | Not for continuous write |

### 3.3 Recommended Cards

| Manufacturer | Model | Type | Temp Range | Endurance | Price (32GB) |
|--------------|-------|------|------------|-----------|--------------|
| **ATP** | Industrial AF32GUD3 | pSLC | -40 to +85°C | 30,000 P/E | ~$50 |
| **Swissbit** | S-46u | pSLC | -40 to +85°C | 30,000 P/E | ~$60 |
| **Cactus Tech** | 808 Series | pSLC | -40 to +85°C | 30,000 P/E | ~$55 |
| **SanDisk** | Industrial | MLC | -40 to +85°C | 10,000 P/E | ~$25 |
| **Kingston** | High Endurance | MLC | -25 to +85°C | 20,000 hrs | ~$15 |
| **SanDisk** | Max Endurance | MLC | -25 to +85°C | 15,000 hrs | ~$18 |

### 3.4 Decision: Industrial pSLC or High Endurance MLC

**Primary:** ATP Industrial AF32GUD3 or Swissbit S-46u (pSLC, -40°C)
- Full temperature range for aircraft
- 30,000 P/E cycles handles years of logging
- Power-loss protection built-in

**Alternate:** SanDisk Industrial or Kingston High Endurance (MLC)
- Lower cost for prototyping
- Adequate endurance for typical use
- Marginal at temperature extremes

---

## 4. MicroSD Socket Selection

### 4.1 Socket Types Comparison

| Type | Mechanism | Vibration | Shock | Eject Risk | Profile | Notes |
|------|-----------|-----------|-------|------------|---------|-------|
| Push-Push | Spring return | Poor | Poor | High | 1.4mm | Card can vibrate loose |
| Push-Pull | Slide eject | Medium | Medium | Medium | 1.8mm | Better than push-push |
| Hinged | Flip cover lock | **Excellent** | **Excellent** | **None** | 1.8mm | Card locked in place |

### 4.2 Vibration/Shock Performance

From industry specifications and testing:
- Push-push connectors may lose contact at vibration frequencies that excite card resonance
- Hinged connectors lock card mechanically, eliminating card movement
- Critical for aircraft environment with continuous engine vibration

### 4.3 Socket Candidates

| Manufacturer | Model | Type | Temp Range | Features | Price |
|--------------|-------|------|------------|----------|-------|
| **GCT** | MEM2067-02-180-00-A | Hinged | -40 to +85°C | No card detect, SMT | ~$1.50 |
| **GCT** | MEM2075 | Push-Push | -40 to +85°C | Card detect, SMT | ~$1.20 |
| **Molex** | 104031-0811 | Push-Pull | -40 to +85°C | Card detect, lock | ~$2.00 |
| **Amphenol** | 114-00841-68 | Hinged | -40 to +85°C | Card detect | ~$2.50 |
| **JAE** | ST12S006V4AR2000 | Push-Lock | -40 to +85°C | Mechanical lock | ~$3.00 |

### 4.4 Decision: GCT MEM2067 (Hinged)

**Rationale:**
- Hinged design locks card mechanically - cannot vibrate loose
- -40°C to +85°C temperature range
- Used in automotive infotainment where vibration is similar to aircraft
- Low profile (1.8mm)
- Good availability, reasonable cost

**Trade-off:** No card detect switch. Implement software card detection via SDMMC initialization attempt, or add external detect switch if needed.

---

## 5. Filesystem Selection

### 5.1 Options Comparison

| Filesystem | PC-Readable | Power-Loss Safe | Wear Leveling | RAM Usage | Notes |
|------------|-------------|-----------------|---------------|-----------|-------|
| FAT32/exFAT (FatFS) | Yes, native | No | No (card handles) | ~4KB | Universal compatibility |
| FileX | Via driver | Yes (failsafe) | Via driver | ~6KB | ST ThreadX ecosystem |
| LittleFS | Tools needed | Yes (COW) | Yes | ~1KB | Embedded-focused |
| ext4 | Linux only | Yes (journal) | Via filesystem | ~64KB | Overkill |

### 5.2 Power-Loss Resilience Analysis

**FAT32/exFAT (FatFS) Vulnerabilities:**
- FAT table update and file allocation are not atomic
- Power loss during write can corrupt FAT table
- File size metadata may not match actual data
- Mitigation: Frequent f_sync(), 2-FAT copy option

**LittleFS Strengths:**
- Copy-on-write: Never overwrites data in place
- Power loss only loses uncommitted write (not filesystem)
- Built-in wear leveling
- Limitation: Not PC-readable without tools

**FileX Strengths:**
- Fault-tolerant journaling option
- Part of STM32 ThreadX ecosystem
- FAT-compatible but with protection
- Limitation: More complex integration

### 5.3 Decision: FAT32 with Mitigation Strategy

**Rationale:**
- PC compatibility is mandatory (user downloads logs directly)
- Industrial cards have built-in power-loss protection at hardware level
- Application-level mitigations make FAT32 adequately safe

**Mitigation Strategy:**
1. Use industrial card with power-loss protection (SPO - Sudden Power Off)
2. Write in complete records (don't leave partial entries)
3. Call f_sync() after each log entry or every 1 second
4. Detect power loss via BQ25792 VBUS monitoring (interrupt on USB removal)
5. Implement graceful shutdown: flush buffers when power loss detected
6. Use 2-FAT copy option in FatFS configuration

---

## 6. STM32H747 Interface

### 6.1 SDMMC Peripheral

| Parameter | Value | Notes |
|-----------|-------|-------|
| Peripheral | SDMMC1 or SDMMC2 | Both available on H747 |
| Bus width | 4-bit | Standard SD mode |
| Clock | Up to 50 MHz (HS) | 200 MHz with UHS-I (if supported) |
| DMA | MDMA or DMA2 | Required for efficient operation |
| Voltage | 3.3V signaling | Level shifting not needed |

### 6.2 Performance Estimates

| Mode | Clock | Throughput (4-bit) | Notes |
|------|-------|-------------------|-------|
| Default Speed | 25 MHz | ~10 MB/s | Safe, compatible |
| High Speed | 50 MHz | ~20 MB/s | Requires card support |
| UHS-I SDR50 | 100 MHz | ~40 MB/s | Requires 1.8V, complex |

**Recommendation:** Use High Speed mode (50 MHz) for good performance without UHS-I complexity.

### 6.3 Pin Assignment (Example)

| Signal | SDMMC1 Pin | Alternate |
|--------|------------|-----------|
| CLK | PC12 | - |
| CMD | PD2 | - |
| D0 | PC8 | - |
| D1 | PC9 | - |
| D2 | PC10 | - |
| D3 | PC11 | - |
| Card Detect | GPIO (optional) | Software detection |

---

## 7. ESD Protection

### 7.1 Requirement

All SD card data lines are exposed to user handling during card insertion/removal. ESD protection required per IEC 61000-4-2.

### 7.2 Protection Options

| Component | Channels | Capacitance | ESD Rating | Package | Notes |
|-----------|----------|-------------|------------|---------|-------|
| USBLC6-2SC6 | 2+VBUS | 3.5pF | ±8kV | SOT-23-6 | Need 3 for 6 lines |
| PESD0402-140 | 1 | 0.5pF | ±8kV | 0402 | 6 required |
| TPD6S300A | 6+VBUS | 1.5pF | ±8kV | WQFN-16 | Single IC solution |
| IP4234CZ6 | 4 | 1.5pF | ±8kV | SOT-457 | 2 required |

### 7.3 Decision: 2× USBLC6-2SC6 or TPD6S300A

**Option A:** 2× USBLC6-2SC6
- Already used for USB (BOM commonality)
- One for D0/D1, one for D2/D3
- CMD/CLK less critical (can add 3rd if needed)

**Option B:** TPD6S300A (if available)
- Single IC protects all lines + power
- Lower capacitance per channel
- Designed for SD card applications

---

## 8. Write Strategy and Data Integrity

### 8.1 Logging Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Flight Data Logging                       │
│                                                              │
│  ┌────────────┐    ┌────────────┐    ┌────────────────────┐ │
│  │  Sensors   │───►│   Buffer   │───►│   SD Card Write    │ │
│  │  (50 Hz)   │    │  (4-8 KB)  │    │   (1 sec batch)    │ │
│  └────────────┘    └────────────┘    └────────────────────┘ │
│                           │                    │             │
│                           ▼                    ▼             │
│                    ┌────────────┐      ┌─────────────┐      │
│                    │Power-Fail  │      │   f_sync()  │      │
│                    │  Handler   │      │  every 1s   │      │
│                    └────────────┘      └─────────────┘      │
│                           │                                  │
│                           ▼                                  │
│                    ┌────────────┐                            │
│                    │Emergency   │                            │
│                    │Flush+Close │                            │
│                    └────────────┘                            │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Write Pattern

1. **Buffer accumulation:** Collect 1 second of sensor data (~1-5 KB)
2. **Batch write:** Write complete buffer to file
3. **Sync:** Call f_sync() to flush filesystem metadata
4. **Repeat**

Benefits:
- Fewer write operations (extends card life)
- Complete records only (no partial entries on power loss)
- Predictable write timing (avoids SD card latency spikes during GC)

### 8.3 Power-Fail Detection

Use BQ25792 charger IC signals:
- **VBUS valid → GPIO interrupt:** Detect USB power removal
- **Battery low threshold:** Pre-emptive shutdown
- **Hold-up time:** ~100ms from battery to complete flush

Implementation:
```c
void EXTI_VBUS_Handler(void) {
    if (!vbus_present) {
        // Power loss imminent - emergency shutdown
        f_sync(&file);
        f_close(&file);
        enter_low_power_mode();
    }
}
```

### 8.4 File Format

**Recommendation:** CSV or simple binary with header

```
# IMSAFE Flight Log v1.0
# Date: 2026-01-28
# Aircraft: N12345
timestamp_ms,alt_m,heading_deg,pitch_deg,roll_deg,vib_engine_g,vib_gearbox_g,...
0,1523.5,270.3,2.1,-1.5,0.45,0.32,...
20,1523.6,270.4,2.0,-1.4,0.44,0.31,...
```

CSV advantages:
- Human-readable
- Opens in Excel/Google Sheets
- Easy post-processing

Binary advantages:
- Smaller file size
- Faster write
- Fixed record size simplifies recovery

**Recommendation:** Binary for logging, CSV export via PC app or Bluetooth

---

## 9. Similar Use Cases

| Application | Storage | Filesystem | Power Protection | Notes |
|-------------|---------|------------|------------------|-------|
| Automotive dash cam | microSD | FAT32 | Card-level SPO | 24/7 recording |
| Aircraft EFIS (Dynon, Garmin) | Internal + SD | Proprietary | UPS/capacitor | Certified systems |
| GoPro cameras | microSD | FAT32/exFAT | None | Consumer, accepts corruption |
| Industrial PLCs | SD/eMMC | LittleFS/ext4 | UPS | Mission-critical |
| Drone flight controllers | microSD | FAT32 | None (short flights) | Similar to our use case |
| Vehicle ECU logging | eMMC | Proprietary | Power supervisor | Automotive-grade |

---

## 10. Bill of Materials

| Component | Part Number | Qty | Unit Cost | Notes |
|-----------|-------------|-----|-----------|-------|
| MicroSD Socket | GCT MEM2067-02-180-00-A | 1 | $1.50 | Hinged, SMT |
| ESD Protection | USBLC6-2SC6 | 2 | $0.10 | Or 1× TPD6S300A |
| Decoupling | 100nF 0402 | 2 | $0.01 | VCCQ, VCC |
| **Industrial Card** | ATP AF32GUD3 | 1 | $50 | pSLC, -40°C, SPO |
| **Alternate Card** | SanDisk Industrial 32GB | 1 | $25 | MLC, -40°C |
| **Total (PCB)** | | | **~$2** | Socket + ESD |
| **Total (with card)** | | | **~$27-52** | Depending on card grade |

---

## 11. Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Card vibrates loose | Low (hinged) | High | Use hinged socket (MEM2067) |
| Power-loss corruption | Medium | High | Industrial card with SPO + f_sync() |
| Card wear-out | Low | Medium | pSLC card, wear monitoring |
| Temperature extremes | Low | Medium | Industrial card rated -40°C |
| User removes card mid-flight | Medium | Medium | Software warning, resume on reinsert |
| ESD damage | Low | High | TVS protection on all lines |
| File too large for FAT32 | Low | Low | Split files at 2GB or use exFAT |

---

## 12. References

- [ATP Industrial SD Cards](https://www.atpinc.com/products/industrial-sd-cards)
- [Swissbit Industrial SD](https://www.swissbit.com/en/products/memory-card/)
- [GCT MEM2067 Datasheet](https://gct.co/connector/mem2067)
- [STM32 SDMMC Tutorial (DeepBlue)](https://deepbluembedded.com/stm32-sdmmc-tutorial-examples-dma/)
- [FatFS Library (elm-chan)](http://elm-chan.org/fsw/ff/00index_e.html)
- [Dash Cam Memory Card Guide (ATP)](https://www.atpinc.com/blog/choosing-memory-cards-for-dashcam-usage)
- [FileX Introduction (ST Wiki)](https://wiki.stmicroelectronics.cn/stm32mcu/wiki/Introduction_to_FILEX)
