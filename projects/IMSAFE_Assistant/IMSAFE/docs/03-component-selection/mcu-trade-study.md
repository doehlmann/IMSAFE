# IMSAFE Flying Assistant - MCU Trade Study

| Document Info | |
|---------------|---|
| Project Code | IMSAFE |
| Revision | B |
| Date | 2026-01-28 |
| Author | Trade Study Team |
| Status | Draft |

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [System Requirements Analysis](#2-system-requirements-analysis)
3. [DSP Requirements Deep Dive](#3-dsp-requirements-deep-dive)
4. [Bluetooth Requirements Analysis](#4-bluetooth-requirements-analysis)
5. [MCU Candidate Analysis](#5-mcu-candidate-analysis)
6. [Architecture Options](#6-architecture-options)
7. [Weighted Scoring Matrix](#7-weighted-scoring-matrix)
8. [Recommendation](#8-recommendation)
9. [Risks and Mitigations](#9-risks-and-mitigations)
10. [Open Questions](#10-open-questions)
11. [Next Steps](#11-next-steps)
12. [References](#12-references)

---

## 1. Executive Summary

### 1.1 Purpose

This trade study evaluates microcontroller options for the IMSAFE Flying Assistant, a GA pilot safety device combining:
- Physical checklist interface (8 toggle switches, 16 WS2812 LEDs)
- 3.5" TFT display with synthetic horizon
- Multiple sensors (altitude, IMU, compass, gas detection)
- 24GHz FMCW landing radar (Infineon BGT24LTR11)
- Real-time vibration analysis for engine/gearbox monitoring
- **Bluetooth Low Energy connectivity for smartphone companion app** (NEW)
- Data logging to microSD

### 1.2 Revision B Changes

This revision adds Bluetooth as a key selection factor following upgraded requirements:
- FR-CN-002: Bluetooth connectivity elevated to "Should" priority
- FR-CN-003 through FR-CN-007: BLE 5.0, flight log download, companion app, 10m range, secure bonding

### 1.3 Key Finding

**Recommended Architecture: STM32H747 Dual-Core + RN4870 External BLE Module**

After evaluating integrated BLE options (STM32WB55, ESP32-S3, nRF52840), the STM32H747 dual-core MCU with an external Microchip RN4870 BLE 5.0 module provides the optimal balance of:
- DSP performance for radar and vibration FFT
- Mature BLE 5.0 connectivity with pre-certified module
- Development ecosystem quality
- Risk mitigation

**Rationale for External BLE vs Integrated:**
- STM32WB55: Insufficient DSP performance for radar + vibration + display (64 MHz M4, no CORDIC/FMAC, 256KB RAM)
- ESP32-S3: FFT performance ~100x slower than STM32H7; real-time concerns with WiFi interrupts
- nRF52840: Similar DSP limitations to STM32WB55 (64 MHz M4, 256KB RAM)
- External module: Adds ~$5-8 BOM cost but preserves full STM32H7 DSP capability

### 1.4 Decision Drivers

| Factor | Weight | Winner | Notes |
|--------|--------|--------|-------|
| DSP Performance | 25% | STM32H747 | Radar FFT, vibration FFT require M7 power |
| Peripheral Availability | 15% | STM32H747 | SDMMC, USB HS, ample I/O |
| Development Ecosystem | 15% | STM32 Family | CubeIDE, HAL, massive community |
| **Bluetooth Integration** | **15%** | RN4870 module | Pre-certified, BLE 5.0, simple UART |
| Power Consumption | 10% | Integrated BLE MCUs | But DSP needs outweigh power savings |
| Cost (including BLE) | 10% | ESP32-S3 | But inadequate DSP performance |
| Risk/Proven Design | 5% | External module | Certified, well-documented |
| Long-term Availability | 5% | STM32 Family | Strong supply chain |

---

## 2. System Requirements Analysis

### 2.1 Peripheral Requirements Summary

| Interface | Requirements | Notes |
|-----------|-------------|-------|
| **SPI #1** | ST7796 3.5" TFT Display | High bandwidth, 320x480 @ 30Hz |
| **SPI #2 / SDIO** | MicroSD Card | SDIO preferred for performance |
| **I2C #1** | BMP390, MPU9250, HMC5983, MCP23017 | Internal sensor bus |
| **I2C #2** | 2x External vibration IMUs | Engine/gearbox monitoring |
| **UART #1** | Bluetooth module | BLE 5.0 connectivity |
| **UART #2** | Radar MCU (if dual MCU) | Inter-MCU communication |
| **GPIO** | WS2812 data (1 pin), 2x rotary encoders (6 pins) | Timing-critical for LEDs |
| **ADC** | MICS-4514 (2 channels: CO + NO2), Temperature | Moderate sample rate |
| **USB** | USB-C for programming, data, charging | Full-speed or High-speed |
| **Timers** | PWM, encoder interface, WS2812 timing | Multiple independent timers |
| **RTC** | Real-time clock with backup | Flight timer, timestamps |

### 2.2 Performance Requirements

| Function | Requirement | Notes |
|----------|-------------|-------|
| Display refresh | 30 Hz minimum | 320x480 @ 16bpp = 307KB/frame |
| Radar FFT | 50 Hz frame rate | Configurable chirp parameters |
| Vibration FFT | Periodic analysis | 5 kHz ODR accelerometers |
| Sensor fusion | 100 Hz update rate | IMU + compass + baro |
| SD logging | Continuous during flight | Power-loss safe writes |
| **BLE throughput** | **>10 KB/s for log download** | **Flight log file transfer** |

### 2.3 Environmental Requirements

| Parameter | Min | Max | Notes |
|-----------|-----|-----|-------|
| Operating Temperature | -20C | 70C | Cockpit environment |
| Supply Voltage | 3.0V | 3.6V | 3.3V logic rails |

---

## 3. DSP Requirements Deep Dive

### 3.1 Radar FFT Requirements

The Distance2GoL reference design provides critical context for radar signal processing:

**Current Configuration (from PROJECT_NOTES.md):**
- Chirp duration: 1.5 ms typical
- Samples per chirp: 64-256 (configurable)
- Chirps per frame: 10 (configurable)
- Frame rate: 10 Hz default, 50 Hz recommended for landing

**FFT Processing Requirements:**

| Parameter | Typical Value | Notes |
|-----------|---------------|-------|
| Range FFT Size | 64-256 points | Per chirp |
| Doppler FFT Size | 16-64 points | Across chirps |
| Data Type | Complex float32 or Q15/Q31 | I/Q channels |
| Processing Rate | 50 Hz frame rate | For landing approach |
| Memory per frame | ~4-16 KB | Raw ADC samples |

**Range Resolution:**
- 200 MHz bandwidth gives ~0.75m theoretical resolution
- FFT resolution = 1/T_chirp Hz
- Practical range: 0.3m - 30m detection

**Estimated Cycles (CMSIS-DSP library):**

| FFT Size | Cortex-M4 @ 64 MHz | Cortex-M4 @ 170 MHz | Cortex-M7 @ 480 MHz | Notes |
|----------|-------------------|---------------------|---------------------|-------|
| 64-point complex | ~47 us | ~18 us | ~3 us | Radix-4 |
| 256-point complex | ~234 us | ~88 us | ~16 us | Radix-4 |
| 1024-point complex | ~1.17 ms | ~441 us | ~73 us | Radix-4 |

**Critical Finding:** At 64 MHz (STM32WB55, nRF52840), a 256-point FFT takes ~234 us. At 50 Hz frame rate with 10 chirps, this consumes:
- 234 us x 10 chirps x 50 Hz = 117 ms/second = **11.7% CPU just for range FFT**
- Add Doppler FFT, magnitude calculation, peak detection: **~20-30% CPU**
- Leaves insufficient headroom for display, sensors, and BLE stack

### 3.2 Vibration FFT Requirements

**Accelerometer Specifications (LIS3DH selected in DEC-003):**
- Output Data Rate: Up to 5 kHz
- Resolution: 12-bit
- Channels: 3-axis x 2 sensors = 6 total

**FFT Analysis Requirements:**

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| Sample Rate | 2-5 kHz | Capture engine harmonics |
| FFT Window | 512-2048 samples | Balance resolution vs latency |
| Update Rate | 1-10 Hz | Health monitoring, not control |
| Frequency Range | 10 Hz - 2.5 kHz | Engine RPM fundamentals + harmonics |

**Typical Light Aircraft Engine:**
- RPM range: 2000-2700 RPM
- Fundamental frequency: 33-45 Hz
- Harmonics of interest: Up to 10th (330-450 Hz)
- Gearbox frequencies: Higher, depends on ratio

**Estimated Processing Load:**

| Operation | At 64 MHz (WB55) | At 170 MHz (G4) | At 480 MHz (H7) |
|-----------|------------------|-----------------|-----------------|
| 1024-pt FFT | 1.17 ms | 441 us | 73 us |
| Magnitude | 78 us | 29 us | 10 us |
| Peak detection | 31 us | 12 us | 4 us |
| **Total per analysis** | **1.28 ms** | **482 us** | **87 us** |

At 10 Hz update rate: 12.8% CPU on WB55 vs 0.9% on H7.

### 3.3 Display Rendering

**ST7796 SPI Display:**
- Resolution: 320 x 480 pixels
- Color depth: 16-bit (RGB565)
- Frame size: 307,200 bytes
- Target refresh: 30 Hz

**SPI Bandwidth Requirements:**
- Full frame: 307,200 bytes x 30 Hz = 9.2 MB/s = 73.7 Mbps
- Typical SPI clock: 40-60 MHz
- With 8-bit transfers: 40 MHz = 5 MB/s max
- **Implication:** Cannot do full-frame updates at 30 Hz via SPI

**Practical Approach:**
- Partial screen updates (dirty rectangles)
- DMA-driven SPI transfers
- Double buffering in RAM
- Synthetic horizon: ~50% screen area = 4.6 MB/s feasible

**Memory Requirements:**
- Single framebuffer: 307 KB
- Double buffer: 614 KB
- STM32WB55: 256 KB RAM - **insufficient for framebuffer**
- STM32H7: 1 MB RAM - adequate
- ESP32-S3: 512 KB + 8 MB PSRAM - adequate (but slow PSRAM access)

### 3.4 Combined DSP Load Analysis

**Question: Can STM32WB55 (64 MHz M4) handle radar + vibration + display?**

| Task | STM32WB55 @ 64 MHz | STM32H747 M7 @ 480 MHz |
|------|-------------------|------------------------|
| Radar FFT (50 Hz) | 20-30% CPU | 0.8% CPU |
| Vibration FFT (10 Hz) | 12.8% CPU | 0.9% CPU |
| Sensor fusion (200 Hz) | 3% CPU | 0.3% CPU |
| Display rendering | 20-30% CPU | 5% CPU |
| BLE stack | 10-20% CPU | Offloaded to M4 |
| **Total** | **65-93% CPU** | **7% M7 + M4 headroom** |

**Conclusion:** STM32WB55 is marginal to inadequate for this application. The integrated BLE does not offset the DSP performance deficit.

---

## 4. Bluetooth Requirements Analysis

### 4.1 Requirements Summary (from requirements.md)

| ID | Requirement | Priority | Implementation Impact |
|----|-------------|----------|----------------------|
| FR-CN-002 | Bluetooth connectivity | Should | MCU selection |
| FR-CN-003 | BLE 5.0 minimum | Should | Module/MCU capability |
| FR-CN-004 | Flight log download to smartphone | Should | Throughput requirement |
| FR-CN-005 | Configuration via companion app | Should | GATT services needed |
| FR-CN-006 | 10m minimum range | Should | RF performance |
| FR-CN-007 | Secure bonding | Should | Pairing capability |

### 4.2 Bluetooth Implementation Options

| Option | Description | BOM Cost | Pros | Cons |
|--------|-------------|----------|------|------|
| **A** | External BLE module (HM-10) | ~$3-5 | Cheap, simple UART | BLE 4.0, no GATT server |
| **B** | External BLE module (RN4870) | ~$5-8 | BLE 5.0, GATT, certified | Higher cost than HM-10 |
| **C** | STM32WB55 (integrated) | ~$5-7 | Single chip, ST ecosystem | DSP performance inadequate |
| **D** | ESP32-S3 (integrated WiFi+BLE) | ~$3-4 | Lowest cost, WiFi bonus | Xtensa, FFT 100x slower |
| **E** | nRF52840 (integrated BLE) | ~$5-6 | Best BLE stack | DSP inadequate, different ecosystem |
| **F** | STM32H747 + RN4870 | ~$25-30 | Full DSP + BLE 5.0 | Highest cost, two chips |

### 4.3 Integrated BLE MCU Evaluation

#### 4.3.1 STM32WB55

| Specification | Value | Assessment |
|---------------|-------|------------|
| Core | Cortex-M4 @ 64 MHz + M0+ for radio | M4 for application, M0+ handles BLE stack |
| Flash | 256 KB - 1 MB | Adequate |
| RAM | 256 KB | **Insufficient for display framebuffer** |
| BLE | 5.0 | Meets requirement |
| FPU | Single precision | OK |
| DSP | No CORDIC/FMAC | **Missing accelerators** |
| SDIO | No | **SPI-only SD card** |
| USB | Full-Speed | OK |
| Price (JLCPCB) | ~$4.50-7 | Good value |

**DSP Verdict:** The 64 MHz clock and lack of hardware accelerators make STM32WB55 inadequate for combined radar FFT + vibration FFT + display workload.

**Module Option: STM32WB5MMG**
- Pre-certified module with antenna
- Price: ~$10-12
- Same DSP limitations as bare chip

#### 4.3.2 ESP32-S3

| Specification | Value | Assessment |
|---------------|-------|------------|
| Core | Dual Xtensa LX7 @ 240 MHz | Non-ARM architecture |
| Flash | External (up to 16 MB) | Requires external flash |
| RAM | 512 KB + 8 MB PSRAM option | Adequate with PSRAM |
| BLE | 5.0 | Meets requirement |
| WiFi | 802.11 b/g/n | Bonus feature |
| FPU | Single precision | OK |
| DSP | ESP-DSP library | **Much slower than CMSIS-DSP** |
| SDIO | No | **SPI-only SD card** |
| USB | OTG | OK |
| Price | ~$2-4 | **Lowest cost** |

**FFT Performance Comparison:**

| FFT Size | ESP32-S3 (ESP-DSP) | STM32H7 (CMSIS-DSP) | Ratio |
|----------|-------------------|---------------------|-------|
| 256-point | ~1.6 ms | ~16 us | 100x slower |
| 1024-point | ~8 ms | ~73 us | 110x slower |

**ESP32-S3 Verdict:** FFT performance is approximately 100x slower than STM32H7. While adequate for vibration analysis alone (8 ms at 10 Hz = 8% CPU), it cannot handle radar FFT at 50 Hz frame rate (1.6 ms x 10 chirps x 50 = 800 ms/second = **80% CPU just for range FFT**).

Additionally:
- Real-time predictability concerns due to WiFi interrupts
- Different toolchain (ESP-IDF vs STM32CubeIDE)
- Less industrial heritage

#### 4.3.3 nRF52840

| Specification | Value | Assessment |
|---------------|-------|------------|
| Core | Cortex-M4F @ 64 MHz | Same as STM32WB55 |
| Flash | 1 MB | Adequate |
| RAM | 256 KB | **Insufficient for framebuffer** |
| BLE | 5.3 (Long Range, Direction Finding) | **Best BLE capability** |
| FPU | Single precision | OK |
| DSP | SIMD only | Standard M4 DSP |
| SDIO | No | **SPI-only** |
| USB | Full-Speed | OK |
| Price | ~$5-6 | Good value |

**nRF52840 Verdict:** Best-in-class BLE stack (Nordic is the BLE leader), but same DSP limitations as STM32WB55. The 64 MHz M4 core cannot handle the combined DSP workload.

### 4.4 External BLE Module Comparison

| Module | BLE Version | Interface | GATT Server | Range | Certification | Price (LCSC) |
|--------|-------------|-----------|-------------|-------|---------------|--------------|
| HM-10 | 4.0 | UART | No | ~10m | CC2541 | ~$3-5 |
| HM-19 | 5.0 | UART | Limited | ~15m | CC2640 | ~$5-7 |
| RN4870 | 5.0 | UART + GPIO | Yes | ~10m | FCC/CE/IC | ~$8 (LCSC: $17) |
| RN4871 | 5.0 | UART | Yes | ~10m | FCC/CE/IC | ~$7-8 |
| MDBT50Q | 5.0 | UART/SPI | Yes (nRF52840) | ~20m | FCC/CE | ~$8-10 |

**Recommended External Module: Microchip RN4870**

Rationale:
- BLE 5.0 with 2.5x throughput improvement over BLE 4.1
- Full GATT server capability for companion app
- Pre-certified (FCC, CE, IC) - no additional certification cost
- Simple UART interface with AT commands
- 3 GPIO pins for status/control
- Well-documented integration with microcontrollers
- Microchip long-term support

### 4.5 BOM Cost Comparison

| Architecture | MCU Cost | BLE Cost | Total | Notes |
|--------------|----------|----------|-------|-------|
| STM32WB55 (integrated) | $5-7 | $0 | $5-7 | **Inadequate DSP** |
| ESP32-S3 (integrated) | $3-4 | $0 | $3-4 | **Inadequate FFT** |
| nRF52840 (integrated) | $5-6 | $0 | $5-6 | **Inadequate DSP** |
| STM32G4 + RN4870 | $10 | $8 | $18 | **Inadequate RAM** |
| STM32H743 + RN4870 | $15 | $8 | $23 | Viable single-core |
| **STM32H747 + RN4870** | $25 | $8 | **$33** | **Recommended** |
| XMC4700 + STM32G4 + RN4870 | $10+$10 | $8 | $28 | Dual MCU complexity |

**Cost Analysis:**
- Integrated BLE MCUs are $25-28 cheaper than STM32H747 + RN4870
- However, integrated BLE MCUs cannot meet DSP requirements
- The $8 cost of RN4870 is justified by:
  - Pre-certification saves $10,000+ in FCC/CE testing
  - Proven BLE 5.0 stack vs developing custom firmware
  - Reduced development time and risk

---

## 5. MCU Candidate Analysis

### 5.1 Integrated BLE MCUs

#### 5.1.1 STM32WB55

**The ST Integrated BLE Option**

| Specification | Value |
|---------------|-------|
| Core | ARM Cortex-M4F @ 64 MHz + M0+ (radio) |
| Flash | 256 KB - 1 MB |
| RAM | 256 KB |
| FPU | Single precision |
| DSP | SIMD only (no CORDIC/FMAC) |
| BLE | 5.0 |
| ADC | 1x 12-bit, 4.26 MSPS |
| SPI | 2 channels |
| I2C | 2 channels |
| UART | 1 channel (+ LPUART) |
| USB | Full-Speed Device |
| SDIO | No |
| Package | UFQFPN-48 to VFBGA-129 |
| Price (JLCPCB) | ~$4.50-7 |

**Pros:**
- Integrated BLE 5.0 (no external module)
- ST ecosystem (CubeIDE, CubeMX)
- Lower total BOM cost if DSP adequate
- Dual-core architecture (M0+ handles BLE stack)

**Cons:**
- 64 MHz M4 insufficient for radar + vibration FFT
- 256 KB RAM cannot hold display framebuffer
- No CORDIC/FMAC hardware accelerators
- No SDIO peripheral
- Only 1 UART (need 2 for radar MCU + debug)

**Verdict:** **Not recommended** - DSP performance and RAM are inadequate.

#### 5.1.2 ESP32-S3

**Cost-Effective with Wireless (but slow DSP)**

| Specification | Value |
|---------------|-------|
| Cores | Dual Xtensa LX7 @ 240 MHz |
| Flash | External (up to 16 MB) |
| RAM | 512 KB + 8 MB PSRAM option |
| FPU | Single precision |
| WiFi | 802.11 b/g/n |
| Bluetooth | BLE 5.0 |
| USB | OTG |
| Price | ~$2-4 |

**FFT Performance (ESP-DSP):**

| FFT Size | Time |
|----------|------|
| 256-point | ~1.6 ms |
| 1024-point | ~8 ms |

**Pros:**
- Extremely low cost
- Built-in WiFi and Bluetooth
- Dual core for task separation
- PSRAM option for framebuffer

**Cons:**
- FFT ~100x slower than Cortex-M7
- Xtensa core, not ARM
- Real-time predictability concerns
- No SDIO
- Not designed for industrial applications

**Verdict:** **Not recommended** for primary MCU. Could serve as BLE/WiFi coprocessor if needed, but RN4870 is simpler.

#### 5.1.3 nRF52840

**Best BLE Stack, Limited DSP**

| Specification | Value |
|---------------|-------|
| Core | ARM Cortex-M4F @ 64 MHz |
| Flash | 1 MB |
| RAM | 256 KB |
| FPU | Single precision |
| BLE | 5.3 (Long Range, Direction Finding) |
| USB | Full-Speed |
| Price | ~$5-6 |

**Pros:**
- Best-in-class BLE stack (Nordic is BLE leader)
- BLE 5.3 with Long Range and Direction Finding
- Thread/Zigbee support
- Good power consumption

**Cons:**
- Same 64 MHz M4 limitation as STM32WB55
- 256 KB RAM insufficient for framebuffer
- Different ecosystem (nRF Connect SDK)
- No SDIO

**Verdict:** **Not recommended** - same DSP limitations as STM32WB55. Excellent for BLE-focused designs, but not for DSP-heavy applications.

### 5.2 High-Performance MCUs (Require External BLE)

#### 5.2.1 Infineon XMC4700

**The Reference Design MCU**

| Specification | Value |
|---------------|-------|
| Core | ARM Cortex-M4F @ 144 MHz |
| Flash | 2 MB |
| RAM | 352 KB |
| FPU | Single precision |
| DSP | SIMD instructions |
| ADC | 4x 12-bit, up to 1.6 MSPS total |
| Package | LQFP-100 to BGA-196 |
| Price (LCSC) | ~$9-10 |

**Pros:**
- Proven in Distance2GoL radar reference design
- Radar firmware and algorithms available
- Industrial temperature range

**Cons:**
- Smaller development community
- DAVE IDE less mature
- No SDIO
- 352 KB RAM marginal for framebuffer

**Verdict:** Good for radar subsystem in dual-MCU architecture.

#### 5.2.2 STM32G474

**Cost-Effective DSP with Hardware Accelerators**

| Specification | Value |
|---------------|-------|
| Core | ARM Cortex-M4F @ 170 MHz |
| Flash | Up to 512 KB |
| RAM | 128 KB |
| DSP | SIMD + CORDIC + FMAC |
| Package | LQFP-48 to LQFP-128 |
| Price (JLCPCB) | ~$6-17 |

**Pros:**
- CORDIC accelerator (2.5x faster sin/cos)
- FMAC for filtering
- Excellent ecosystem
- CAN-FD

**Cons:**
- Only 128 KB RAM - cannot hold framebuffer
- No SDIO
- Would need separate radar MCU

**Verdict:** Excellent for dual-MCU as main application processor, but RAM limits display capability.

#### 5.2.3 STM32H743 (Single Core)

**High-Performance Single Core**

| Specification | Value |
|---------------|-------|
| Core | Cortex-M7 @ 480 MHz |
| Flash | 2 MB |
| RAM | 1 MB |
| FPU | Double precision |
| L1 Cache | 16+16 KB |
| SDIO | SDMMC x2 |
| USB | HS + FS |
| Price | ~$12-15 |

**Pros:**
- Massive performance (1027 DMIPS)
- 1 MB RAM for framebuffer + FFT
- Dual SDMMC
- USB High-Speed

**Cons:**
- Single core limits parallel processing
- Higher power consumption

**Verdict:** Strong single-MCU candidate with external BLE module.

#### 5.2.4 STM32H747 (Dual Core)

**The Flagship Option**

| Specification | Value |
|---------------|-------|
| Cores | M7 @ 480 MHz + M4 @ 240 MHz |
| Flash | 2 MB |
| RAM | 1 MB (shared) |
| FPU | M7: Double, M4: Single |
| L1 Cache | M7: 16+16 KB |
| SDIO | SDMMC x2 |
| USB | HS + FS |
| Package | LQFP-100 to BGA-240 |
| Price (JLCPCB) | ~$18-25 |

**Pros:**
- Dual-core allows clean workload separation
- M7: Radar FFT, Vibration FFT, Display DMA
- M4: Sensors, UI logic, logging, BLE UART
- 1 MB RAM for all buffers
- Single chip, single toolchain
- Excellent inter-core communication

**Cons:**
- Most expensive option
- Higher power consumption
- Requires external BLE module

**Verdict:** **Recommended** - best balance of DSP performance and system integration.

---

## 6. Architecture Options

### 6.1 Option A: STM32H747 + RN4870 (Recommended)

```
┌─────────────────────────────────────────────────────────────┐
│                    STM32H747 Dual-Core                       │
│  ┌─────────────────────┐  ┌─────────────────────┐          │
│  │   Cortex-M7 480MHz  │  │   Cortex-M4 240MHz  │          │
│  │                     │  │                     │          │
│  │  - Radar FFT        │  │  - Sensor polling   │          │
│  │  - Vibration FFT    │  │  - UI state machine │          │
│  │  - Display DMA      │  │  - SD card logging  │          │
│  │  - Sensor fusion    │  │  - BLE UART driver  │          │
│  └─────────────────────┘  └─────────────────────┘          │
│                    Shared 1MB RAM                           │
│                    HSEM for sync                            │
└───────────────┬───────────────┬───────────────┬────────────┘
                │               │               │
        ┌───────▼───────┐ ┌─────▼─────┐ ┌───────▼───────┐
        │  BGT24LTR11   │ │  Display  │ │   RN4870      │
        │  (SPI/ADC)    │ │  (SPI)    │ │   (UART)      │
        └───────────────┘ └───────────┘ └───────────────┘
```

**Advantages:**
- Single MCU with full DSP capability
- M4 core dedicated to I/O, BLE, logging
- Pre-certified BLE module
- Single toolchain (STM32CubeIDE)
- Clean architecture with defined responsibilities

**BLE Integration Details:**
- RN4870 connected via UART to M4 core
- AT command interface for configuration
- GATT services defined in RN4870 firmware
- Smartphone app connects to RN4870, MCU provides data
- 115200-921600 baud for log downloads

**Estimated BOM Impact:**
- STM32H747: ~$25
- RN4870: ~$8
- Supporting components: ~$5
- Total BLE subsystem: ~$38

### 6.2 Option B: STM32WB55 + XMC4700 (Not Recommended)

```
┌────────────────────────┐     ┌────────────────────────┐
│   XMC4700 (Radar)      │     │  STM32WB55 (Main+BLE)  │
│                        │     │                        │
│  - BGT24LTR11 control  │     │  - Display (limited)   │
│  - Range FFT           │────►│  - Sensors             │
│  - Doppler FFT         │UART │  - UI                  │
│  - Target detection    │     │  - Integrated BLE      │
└────────────────────────┘     └────────────────────────┘
```

**Problems:**
- STM32WB55 RAM (256 KB) insufficient for display framebuffer
- Two different toolchains (DAVE + CubeIDE)
- Complex inter-MCU protocol
- BLE stack competes with application for CPU

**Verdict:** Not viable due to RAM limitation.

### 6.3 Option C: ESP32-S3 as Coprocessor

```
┌────────────────────────┐     ┌────────────────────────┐
│  STM32H743 (Main DSP)  │     │  ESP32-S3 (Wireless)   │
│                        │     │                        │
│  - Radar FFT           │     │  - BLE connectivity    │
│  - Vibration FFT       │────►│  - WiFi (future)       │
│  - Display             │SPI  │  - OTA updates         │
│  - Sensors             │     │  - Log buffering       │
└────────────────────────┘     └────────────────────────┘
```

**Advantages:**
- ESP32-S3 provides both BLE and WiFi
- Lower cost than RN4870
- Could handle OTA firmware updates

**Disadvantages:**
- More complex integration (SPI protocol)
- Two different toolchains
- ESP32 reliability concerns
- Overkill for simple BLE requirements

**Verdict:** Viable but unnecessarily complex. RN4870 is simpler.

---

## 7. Weighted Scoring Matrix

### 7.1 Updated Evaluation Criteria (Revision B)

| Criterion | Weight | Description |
|-----------|--------|-------------|
| DSP Performance | 25% | FFT capability for radar + vibration |
| Peripheral Availability | 15% | SPI, I2C, SDIO, ADC, USB, timers |
| Development Ecosystem | 15% | IDE, HAL, community, examples |
| **Bluetooth Integration** | **15%** | **Integrated vs external, certification, BLE version** |
| Power Consumption | 10% | Active and sleep modes |
| Cost (including BLE) | 10% | MCU + BLE module + supporting components |
| Risk/Proven Design | 5% | Existing implementations, reliability |
| Long-term Availability | 5% | Manufacturer commitment, lifecycle |

### 7.2 Scoring Scale

| Score | Description |
|-------|-------------|
| 5 | Excellent - exceeds requirements significantly |
| 4 | Good - meets all requirements with margin |
| 3 | Adequate - meets minimum requirements |
| 2 | Marginal - may have limitations |
| 1 | Poor - significant gaps |

### 7.3 Integrated BLE MCU Comparison

| Criterion | Weight | STM32WB55 | ESP32-S3 | nRF52840 |
|-----------|--------|-----------|----------|----------|
| DSP Performance | 25% | 2 | 1 | 2 |
| Peripherals | 15% | 2 | 2 | 2 |
| Ecosystem | 15% | 4 | 3 | 3 |
| **Bluetooth** | **15%** | **4** | **4** | **5** |
| Power | 10% | 4 | 3 | 5 |
| Cost | 10% | 4 | 5 | 4 |
| Risk | 5% | 3 | 2 | 4 |
| Availability | 5% | 4 | 4 | 4 |
| **Weighted Score** | | **2.95** | **2.55** | **3.10** |

**Analysis:** All integrated BLE MCUs score below 3.5 due to inadequate DSP performance. nRF52840 leads due to excellent BLE stack and power efficiency, but still fails the DSP requirement.

### 7.4 Architecture Comparison (Including BLE)

| Criterion | Weight | H747+RN4870 | H743+RN4870 | WB55+XMC4700 | G4+XMC+RN4870 |
|-----------|--------|-------------|-------------|--------------|---------------|
| DSP Performance | 25% | 5 | 5 | 2 | 4 |
| Peripherals | 15% | 5 | 5 | 2 | 3 |
| Ecosystem | 15% | 5 | 5 | 3 | 3 |
| **Bluetooth** | **15%** | **4** | **4** | **4** | **4** |
| Power | 10% | 2 | 2 | 4 | 3 |
| Cost | 10% | 2 | 3 | 3 | 3 |
| Risk | 5% | 3 | 3 | 2 | 3 |
| Availability | 5% | 4 | 4 | 3 | 4 |
| **Weighted Score** | | **4.00** | **4.05** | **2.70** | **3.40** |

### 7.5 Scoring Rationale

**STM32H747 + RN4870 (Score 4.00):**
- DSP: 5 - Dual core provides excellent FFT performance with clean separation
- Peripherals: 5 - SDMMC, USB HS, ample SPI/I2C/UART, 1MB RAM
- Ecosystem: 5 - Best-in-class STM32 tools, massive community
- Bluetooth: 4 - External module adds integration effort but is pre-certified
- Power: 2 - Higher power than M4-only solutions
- Cost: 2 - ~$33 total is highest, but justified by capability
- Risk: 3 - Custom radar interface needed, but BLE is proven
- Availability: 4 - Good STM32 and Microchip supply chains

**STM32H743 + RN4870 (Score 4.05):**
- Slightly higher score due to lower cost, but single-core limits parallelism
- For workload separation, H747 dual-core is preferred

**STM32WB55 + XMC4700 (Score 2.70):**
- Low score due to WB55 RAM limitation (cannot hold framebuffer)
- Bluetooth integration is good, but overall system fails

---

## 8. Recommendation

### 8.1 Primary Recommendation: STM32H747 + RN4870

**Final Architecture:**

```
┌─────────────────────────────────────────────────────────────┐
│                    STM32H747 Dual-Core                       │
│  ┌─────────────────────┐  ┌─────────────────────┐          │
│  │   Cortex-M7 480MHz  │  │   Cortex-M4 240MHz  │          │
│  │   DSP Workload      │  │   I/O Management    │          │
│  │                     │  │                     │          │
│  │  - Radar FFT        │  │  - BLE protocol     │          │
│  │  - Vibration FFT    │  │  - SD card logging  │          │
│  │  - Display DMA      │  │  - Sensor polling   │          │
│  │  - Sensor fusion    │  │  - UI state machine │          │
│  └─────────────────────┘  └─────────────────────┘          │
│                    Shared 1MB RAM                           │
└───────────────────────────┬─────────────────────────────────┘
                            │ UART (115200-921600 baud)
                    ┌───────▼───────┐
                    │   RN4870      │
                    │   BLE 5.0     │
                    │   Module      │
                    └───────────────┘
                            │ BLE
                    ┌───────▼───────┐
                    │  Smartphone   │
                    │  Companion    │
                    │  App          │
                    └───────────────┘
```

**Rationale:**

1. **DSP Performance:** M7 core at 480 MHz handles all real-time DSP with <10% CPU load, leaving massive headroom.

2. **BLE Integration:** RN4870 provides:
   - BLE 5.0 with 2.5x throughput improvement
   - Pre-certified (FCC, CE, IC) - no certification cost
   - Simple UART interface
   - Built-in GATT server for companion app
   - Secure bonding support
   - 10m+ range

3. **Development Efficiency:** Single STM32CubeIDE project for all MCU firmware. RN4870 configured via AT commands.

4. **Risk Mitigation:**
   - Proven STM32H7 platform
   - Microchip RN4870 widely deployed
   - If BLE issues arise, module can be swapped without MCU changes

### 8.2 BLE Module Specification

**Recommended Part: RN4870-I/RM140**

| Parameter | Specification |
|-----------|---------------|
| Bluetooth Version | 5.0 |
| Profiles | GATT Server/Client, Custom Services |
| Interface | UART (up to 921600 baud) |
| GPIO | 3 pins for status/control |
| Range | >10m with integrated antenna |
| Certification | FCC, CE, IC |
| Operating Temperature | -40C to +85C |
| Supply Voltage | 1.9V to 3.6V |
| Package | 11.5 x 8.3 mm SMD |
| Price | ~$8 (LCSC: $17 at low qty) |

**Alternative: RN4871-I/RM130** (smaller, lower cost, same capability)

### 8.3 BLE Communication Protocol

**GATT Services for IMSAFE:**

| Service | UUID | Characteristics |
|---------|------|-----------------|
| Device Info | 0x180A | Manufacturer, Model, Firmware Version |
| Flight Log | Custom | Log List, Log Download, Log Delete |
| Configuration | Custom | Display Settings, Alarm Thresholds, Calibration |
| Real-time Data | Custom | Altitude, Attitude, Radar Height (notify) |

**Log Download Protocol:**
- MTU: 247 bytes (BLE 5.0)
- Throughput: ~20-30 KB/s with notifications
- 1 MB flight log: ~40 seconds transfer time

### 8.4 Secondary Recommendation: STM32H743 + RN4870

**Use this if:**
- Budget is constrained (saves ~$10)
- Dual-core complexity is a concern
- Single-threaded architecture is preferred

**Trade-off:** Less clean separation of DSP and I/O tasks, but still adequate performance.

### 8.5 Recommended Part Numbers

**Primary (STM32H747 + RN4870):**
- **STM32H747IIT6** - LQFP-176, 2MB Flash, 1MB RAM
- **RN4870-I/RM140** - BLE 5.0 module, integrated antenna

**Secondary (Single Core):**
- **STM32H743IIT6** - LQFP-176, 2MB Flash, 1MB RAM
- **RN4870-I/RM140** - Same BLE module

---

## 9. Risks and Mitigations

### 9.1 Technical Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| RN4870 throughput insufficient for log download | Medium | Low | Test early; fall back to USB for large files |
| BLE range < 10m in cockpit environment | Medium | Low | Antenna placement; external antenna option on RN4870 |
| RN4870 AT command latency impacts UI | Low | Low | Buffer commands; use hardware flow control |
| Dual-core synchronization issues | Medium | Medium | Use proven HSEM patterns; ST provides examples |
| Radar interface on STM32H7 fails | High | Medium | Keep XMC4700 as fallback option |

### 9.2 Schedule Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| BLE companion app development | Medium | Medium | Use standard BLE libraries; start app early |
| RN4870 GATT service configuration | Low | Low | Well-documented; use Microchip MCC tools |
| Dual-core firmware complexity | Medium | Medium | Start with M4-only; add M7 radar later |

### 9.3 Supply Chain Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| RN4870 stock-out | Medium | Low | Qualify RN4871 as alternate; 6-month buffer |
| STM32H747 allocation issues | High | Low | Order early; H745 as backup |

---

## 10. Open Questions

### 10.1 Bluetooth Questions

1. **Companion App Platform:** iOS, Android, or both? Affects BLE library selection.

2. **Real-time Data Streaming:** Is continuous altitude/attitude streaming required, or only on-demand?

3. **Multi-device Pairing:** Should IMSAFE support pairing with multiple phones, or single device only?

4. **OTA Firmware Updates:** Should BLE support firmware updates, or USB-only?

### 10.2 Hardware Questions

1. **RN4870 Antenna:** Integrated antenna adequate, or need external antenna option?

2. **Power Budget:** RN4870 adds ~10-15 mA during advertising, ~5 mA during connection. Acceptable?

3. **Wake from BLE:** Should BLE be able to wake MCU from deep sleep?

### 10.3 Firmware Questions

1. **BLE Stack Location:** Run RN4870 protocol handler on M4 core?

2. **Log Buffer:** How much RAM to dedicate to BLE transmit buffer?

3. **Bonding Storage:** Use RN4870 internal storage or external EEPROM for pairing data?

---

## 11. Next Steps

### 11.1 Immediate (Week 1-2)

1. **Procurement:**
   - Order STM32H747 Nucleo-144 development board (NUCLEO-H747ZI)
   - Order RN4870 PICtail Plus evaluation board
   - Order ST7796 display module

2. **BLE Proof of Concept:**
   - Connect RN4870 to Nucleo board via UART
   - Configure GATT services for simple data transfer
   - Test smartphone connection with nRF Connect app
   - Measure throughput with bulk data transfer

### 11.2 Short Term (Week 3-4)

3. **Integration Testing:**
   - Implement BLE protocol handler on M4 core
   - Test concurrent BLE + SD card + sensor operation
   - Verify no interference with M7 DSP tasks

4. **Companion App Prototype:**
   - Select framework (React Native, Flutter, or native)
   - Implement BLE connection and data display
   - Test log download function

### 11.3 Medium Term (Week 5-8)

5. **Full System Integration:**
   - Combine radar, display, sensors, and BLE
   - Validate all requirements (FR-CN-002 through FR-CN-007)
   - Power consumption measurement

6. **Decision Gate:**
   - Confirm RN4870 meets all BLE requirements
   - OR evaluate alternative modules if issues found

---

## 12. References

### 12.1 STM32H747 Resources

- [STM32H747/757 Product Page](https://www.st.com/en/microcontrollers-microprocessors/stm32h747-757.html)
- [STM32H747 Datasheet](https://www.st.com/resource/en/datasheet/stm32h747ag.pdf)
- [AN4841: Digital Signal Processing for STM32 using CMSIS](https://www.st.com/resource/en/application_note/an4841-digital-signal-processing-for-stm32-microcontrollers-using-cmsis-stmicroelectronics.pdf)
- [JLCPCB STM32H745IIT6 (C730191)](https://jlcpcb.com/partdetail/STMicroelectronics-STM32H745IIT6/C730191)

### 12.2 RN4870 BLE Module Resources

- [RN4870 Product Page](https://www.microchip.com/en-us/product/rn4870)
- [RN4870 User Guide](https://onlinedocs.microchip.com/pr/GUID-7D49B43E-34D2-4A33-BCB9-511C710342D5-en-US-2/GUID-388BEDEA-ADC6-4F19-BE27-DE930114CBA3.html)
- [RN4870/71 Bluetooth Low Energy Module Command Reference](https://ww1.microchip.com/downloads/en/DeviceDoc/RN4870-71-Bluetooth-Low-Energy-Module-User-Guide-DS50002466C.pdf)
- [LCSC RN4870-V/RM118](https://www.lcsc.com/product-detail/RF-Transceiver-ICs_Microchip-Tech-RN4870-V-RM118_C638584.html)

### 12.3 STM32WB55 Resources

- [STM32WB55 Product Page](https://www.st.com/en/microcontrollers-microprocessors/stm32wb55rg.html)
- [JLCPCB STM32WB55 Parts](https://jlcpcb.com/partdetail/STMicroelectronics-STM32WB55CEU6TR/C2965229)
- [LCSC STM32WB55CGU6](https://www.lcsc.com/product-detail/C404023.html)

### 12.4 ESP32-S3 Resources

- [ESP-DSP Library Documentation](https://docs.espressif.com/projects/esp-dsp/en/latest/esp32/index.html)
- [ESP-DSP Benchmarks](https://docs.espressif.com/projects/esp-dsp/en/latest/esp32/esp-dsp-benchmarks.html)
- [ESP32-S3 Product Page](https://www.espressif.com/en/products/socs/esp32-s3)

### 12.5 nRF52840 Resources

- [nRF52840 Product Page](https://www.nordicsemi.com/Products/nRF52840)
- [nRF52840 Datasheet](https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf)

### 12.6 BLE Integration Resources

- [STM32 with HM10 BLE Integration](https://blog.embeddedexpert.io/?p=1822)
- [Arduino with RN4870/71](https://www.martyncurrey.com/arduino-with-rn48701/)
- [Bluetooth LE Chipset Guide](https://argenox.com/library/bluetooth-low-energy/bluetooth-le-chipset-guide-2019)

### 12.7 FMCW Radar Resources

- [Infineon FMCW Radar Digital Signal Processing](https://www.infineon.com/dgdl/Infineon-FMCW_RADAR_Digital_Signal_Processing_Handout-Training-v01_00-EN.pdf)
- [Distance2GoL Software User Manual](https://static4.arrow.com/-/media/arrow/files/pdf/infineon-distance2gol-software-user-manual.pdf)

### 12.8 FFT Benchmarks

- [ARM DSP Capabilities White Paper](https://community.arm.com/cfs-file/__key/communityserver-blogs-components-weblogfiles/00-00-00-21-42/7563.ARM-white-paper-_2D00_-DSP-capabilities-of-Cortex_2D00_M4-and-Cortex_2D00_M7.pdf)
- [CMSIS-DSP FFT Performance](https://community.st.com/t5/stm32-mcus-products/fft-performance/td-p/352937)
- [DSP Accelerators (CORDIC/FMAC)](https://notblackmagic.com/bitsnpieces/dsp-accelerators/)

---

## Revision History

| Rev | Date | Author | Description |
|-----|------|--------|-------------|
| A | 2026-01-28 | Trade Study Team | Initial trade study |
| B | 2026-01-28 | Trade Study Team | Added Bluetooth as key selection factor; evaluated integrated BLE MCUs (STM32WB55, ESP32-S3, nRF52840); recommended RN4870 external module with STM32H747 |
