# Distance2GoL Radar Firmware Migration Analysis: XMC4700 to STM32H7

**Document ID:** IMSAFE-ANALYSIS-001
**Date:** 2026-01-28
**Status:** Draft
**Related Decision:** IMSAFE-DEC-002 (MCU Architecture)

---

## Executive Summary

This document analyzes the effort required to port the Infineon Distance2GoL (D2GL) radar firmware from the XMC4700 microcontroller to the STM32H7 family. The analysis is based on examination of the actual DAVE project source code, including the BSP layer, SC-FMCW implementation, and radar control modules.

**Bottom Line:** This is a **significant porting effort** (estimated 4-8 weeks for an experienced embedded developer). The application logic and algorithms are reasonably portable, but the timing-critical BSP layer requires complete rewrite due to fundamental architectural differences between XMC and STM32 peripherals.

---

## 1. Source Firmware Architecture Overview

The Distance2GoL firmware has a well-structured layered architecture:

```
+----------------------------------------------------------+
|                    Application Layer                      |
|  (application.c - state machine, USB protocol, callbacks) |
+----------------------------------------------------------+
|                    Radar Control Layer                    |
|  (radar_api.h, radar_control.c - high-level radar API)   |
+----------------------------------------------------------+
|                    Algorithm Layer                        |
|  (FFT processing, target detection, range calculation)    |
+----------------------------------------------------------+
|                       BSP Layer                           |
|  (d2gl_bsp.c, d2gl_sc_fmcw.c - hardware abstraction)     |
+----------------------------------------------------------+
|               DAVE Code Generation (XMC4700)              |
|  (DAVE.h - peripheral configuration, similar to CubeMX)   |
+----------------------------------------------------------+
```

### Key Source Files Analyzed

| File | Purpose | Lines | Portability |
|------|---------|-------|-------------|
| `radar_api.h` | Public radar driver API | ~1100 | High (interface only) |
| `radar_control.h/c` | Radar state management | ~120 | High |
| `d2gl_bsp.h/c` | Hardware abstraction | ~320 | Low (requires rewrite) |
| `d2gl_sc_fmcw.c` | FMCW chirp generation | ~755 | Low (timing-critical) |
| `d2gl_sc_fmcw_timer.c` | Timer control | ~112 | Low (XMC-specific) |
| `d2gl_dac_control.c` | DAC LUT for ramp | ~200 | Low (DMA-coupled) |

---

## 2. Migration Effort Summary by Layer

| Layer | Effort Level | Estimated Work | Notes |
|-------|--------------|----------------|-------|
| **Application** | Low | Minimal changes | Portable C, no HW deps |
| **Radar Control** | Low-Medium | Interface adaptation | Clean abstraction |
| **Algorithm** | Low | Direct reuse | Math/DSP portable |
| **ComLib (USB)** | Medium | Protocol portable, USB stack different | CDC implementation |
| **BSP** | **High** | Complete rewrite | Timer, DMA, ADC, DAC |
| **SC-FMCW** | **High** | Complete rewrite | Timing-critical |

---

## 3. Code Portability Assessment

### 3.1 Directly Reusable Code (Minimal Changes)

The following components can be reused with little to no modification:

**Algorithm Layer:**
- FFT processing routines (if using ARM CMSIS-DSP, directly compatible)
- Target detection algorithms
- Range/velocity calculations
- Data structures (`Frame_Info_t`, `Device_Info_t`, `Adc_Configuration_t`)

**Application Layer:**
- State machine logic
- Configuration parameter handling
- USB protocol message parsing (ComLib protocol layer)

**Data Structures:**
```c
// These are portable - no XMC dependencies
typedef struct {
    uint32_t samplerate_Hz;
    uint8_t resolution;
    uint8_t use_post_calibration;
} Adc_Configuration_t;

typedef struct {
    const void* sample_data;
    uint32_t frame_number;
    uint32_t num_chirps;
    // ... etc
} Frame_Info_t;
```

### 3.2 Code Requiring Minor Modifications

**Radar Control Layer (`radar_control.c`, `radar_api.h`):**
- Callback registration mechanism - portable with minor adaptation
- Device handle management - abstraction friendly
- Configuration validation - pure logic

**Required Changes:**
- Replace `XMC_DEBUG()` macros with STM32 equivalents
- Update include paths
- Adapt callback function signatures if needed

### 3.3 Code Requiring Complete Rewrite

**BSP Layer - Critical Hardware Interfaces:**

| Component | XMC4700 Implementation | STM32H7 Equivalent |
|-----------|------------------------|-------------------|
| Timer System | CCU4 slices (CCU4_PRT_TRIGGER, CCU4_ADC_TRIGGER, CCU4_DAC_TRIGGER) | TIM1/TIM8 advanced timers or HRTIM |
| ADC | VADC with DMA (4 channels: I1, Q1, I2, Q2) | ADC1/ADC2/ADC3 with DMA |
| DAC | DAC with DMA LUT playback | DAC1/DAC2 with DMA |
| DMA | GPDMA with linked list | DMA1/DMA2 or MDMA |
| GPIO | DIGITAL_IO DAVE APP | HAL_GPIO |
| USB | USB_DEVICE DAVE APP | USB_DEVICE middleware |

**SC-FMCW Module - Timing Critical:**

The `d2gl_sc_fmcw.c` file implements software-controlled FMCW with:
- Precise timer synchronization for chirp timing
- DAC-driven frequency ramp generation
- ADC sampling synchronized to chirp
- Frequency calibration using divided output

```c
// XMC-specific timer configuration - requires complete rewrite
scfmcw_instance.p_prt_timer = scfmcw_timer_init(&prt_timer, &CCU4_PRT_TRIGGER,
                                                  XMC_CCU4_SLICE_PRESCALER_32);
scfmcw_instance.p_adc_trigger = scfmcw_timer_init(&adc_trigger, &CCU4_ADC_TRIGGER,
                                                    XMC_CCU4_SLICE_PRESCALER_1);
scfmcw_instance.p_dac_trigger = scfmcw_timer_init(&dac_trigger, &CCU4_DAC_TRIGGER,
                                                    XMC_CCU4_SLICE_PRESCALER_1);
```

---

## 4. Hardware Compatibility Analysis

### 4.1 ADC Comparison

| Parameter | XMC4700 VADC | STM32H7 ADC | Assessment |
|-----------|--------------|-------------|------------|
| Resolution | 12-bit | 16-bit (or 12-bit mode) | STM32 superior |
| Max Sample Rate | 1.8 MSPS (per group) | 3.6 MSPS (ADC1/2), 5 MSPS (ADC3) | STM32 superior |
| Channels Needed | 4 (I1, Q1, I2, Q2) | Easily achievable | Compatible |
| DMA Support | GPDMA | DMA/MDMA | Both support |
| Trigger Sources | CCU4/CCU8 | Timer TRGO | Compatible |
| Simultaneous Sample | Via multiple converters | Dual ADC mode available | Compatible |

**Key Consideration:** The D2GL uses 4-channel ADC acquisition at rates up to ~500 kSPS per channel. STM32H7 can handle this with margin using dual ADC mode or sequential sampling with faster ADC.

**Configuration from source:**
```c
#define BSP_ADC_RESOLUTION           (uint8_t) 12U
#define BSP_MAX_NUM_SAMPLES_PER_CHIRP 256U
// Typical chirp time: 1.5ms, so ~170 kSPS per channel
```

### 4.2 Timer Comparison

| Parameter | XMC4700 CCU4 | STM32H7 TIM/HRTIM | Assessment |
|-----------|--------------|-------------------|------------|
| Resolution | 16-bit | 16-bit (TIM), 16-bit@184ps (HRTIM) | HRTIM superior |
| Prescaler Range | 1-32768 | 1-65536 | Similar |
| Timer Interconnect | CCU slice linking | Master/slave, TRGO | Both capable |
| DMA Trigger | Compare/Period events | Update/Compare events | Compatible |
| PWM Channels | 4 per slice | Multiple per timer | Compatible |

**Critical Timing Requirements:**
```c
// From d2gl_bsp_limits.h
#define BSP_MIN_PRT_USEC    (2500U)   // 2.5ms pulse repetition time
#define BSP_MAX_PRT_USEC    (10000U)  // 10ms
// Chirp components:
// - Up ramp: ~1.5ms
// - Stay time: configurable
// - Down ramp: ~1.0ms
```

**STM32H7 HRTIM Advantage:** For highest timing precision, STM32H7's HRTIM provides 184ps resolution (at 400MHz), far exceeding requirements. Standard TIMs are also adequate.

### 4.3 DAC Comparison

| Parameter | XMC4700 DAC | STM32H7 DAC | Assessment |
|-----------|-------------|-------------|------------|
| Resolution | 12-bit | 12-bit | Equal |
| Sample Rate | Up to 1 MSPS | Up to 1 MSPS | Equal |
| DMA Support | Yes | Yes | Compatible |
| Waveform Gen | LUT with DMA | LUT with DMA, triangle, noise | STM32 more features |
| Output Buffers | Yes | Yes | Equal |

**Ramp Generation Implementation:**
```c
// From d2gl_sc_fmcw.c - DAC drives VCO for frequency ramp
uint32_t VCO_LUT[MAX_DAC_TABLE_COUNTS];  // Look-up table for ramp
// Ramp has ~200-400 steps depending on bandwidth
// DAC updates every ~5-7us during chirp
```

STM32H7 DAC with DMA can handle identical LUT playback for VCO control.

### 4.4 DMA Comparison

| Parameter | XMC4700 GPDMA | STM32H7 DMA/MDMA | Assessment |
|-----------|---------------|------------------|------------|
| Channels | 8 | DMA1/2: 8 each, MDMA: 16 | STM32 more |
| Linked List | Yes | Yes (MDMA) | Compatible |
| Circular Mode | Yes | Yes | Compatible |
| Double Buffer | Yes | Yes | Compatible |
| Priority Levels | 4 | 4 | Equal |

**DMA Configuration in Source:**
```c
// 4 DMA channels for ADC data (I1, Q1, I2, Q2)
void bsp_dma_set_destination_addr(uint32_t* DMA_dst_I1, uint32_t* DMA_dst_Q1,
                                   uint32_t* DMA_dst_I2, uint32_t* DMA_dst_Q2);
```

---

## 5. Risk Assessment

### 5.1 Technical Risks

| Risk | Severity | Likelihood | Mitigation |
|------|----------|------------|------------|
| **Timing precision insufficient** | High | Low | STM32H7 HRTIM provides sub-ns resolution; use hardware triggers, not software |
| **ADC/DAC synchronization issues** | High | Medium | Extensive use of hardware triggers; design timer chain carefully |
| **DMA configuration complexity** | Medium | Medium | Start with simple DMA, add linked list later; use CubeMX |
| **Frequency calibration drift** | Medium | Medium | Port calibration algorithm carefully; verify with RF measurement |
| **USB latency impacts data throughput** | Low | Low | STM32H7 has high-speed USB; buffer sizing adequate |
| **Real-time constraints missed** | High | Medium | Profile thoroughly; consider FreeRTOS for deterministic scheduling |

### 5.2 Schedule Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Underestimating BSP complexity | +2-4 weeks | Build BSP incrementally; test each peripheral in isolation |
| Timer synchronization debugging | +1-2 weeks | Use oscilloscope; add test points; capture waveforms early |
| RF calibration tuning | +1-2 weeks | Test with actual BGT24LTR11 hardware early in development |
| USB enumeration issues | +1 week | Use known-working USB CDC template as starting point |

### 5.3 Hardware Compatibility Risks

| Risk | Assessment |
|------|------------|
| BGT24LTR11 interface voltage levels | Low - both operate at 3.3V |
| Divider output frequency counting | Medium - need to verify counter peripheral compatibility |
| EEPROM I2C interface | Low - standard I2C, HAL supported |
| PGA SPI interface | Low - standard SPI, HAL supported |

---

## 6. Recommended Migration Approach

### Phase 1: Foundation (Week 1-2)

**Goals:**
- Set up STM32H7 development environment
- Create hardware abstraction layer (HAL wrapper)
- Basic peripheral bring-up

**Tasks:**
1. Create STM32CubeIDE project for STM32H750/H743
2. Configure clocks (480MHz core, timer clocks, ADC clocks)
3. Implement basic GPIO control for BGT24LTR11:
   - `bsp_bgt_power_up()` / `bsp_bgt_power_down()`
   - `bsp_bgt_start_tx()` / `bsp_bgt_stop_tx()`
   - `bsp_bb_power_up()` / `bsp_bb_power_down()`
4. Implement I2C driver for EEPROM
5. Implement SPI driver for PGA112
6. Basic UART debug output

**Validation Checkpoint:**
- [ ] BGT24LTR11 powers on/off correctly
- [ ] EEPROM read/write functional
- [ ] PGA gain can be set

### Phase 2: Timer Infrastructure (Week 2-3)

**Goals:**
- Implement timer chain for chirp timing
- Test timing accuracy

**Tasks:**
1. Configure TIM1 or HRTIM for PRT (Pulse Repetition Timer)
2. Configure TIM2/TIM3 for ADC trigger generation
3. Configure TIM4/TIM5 for DAC trigger generation
4. Implement timer synchronization (master/slave chain)
5. Create wrapper functions matching XMC interface:
   - `scfmcw_timer_init()`
   - `scfmcw_timer_config()`
   - `scfmcw_timer_start()` / `scfmcw_timer_stop()`

**Validation Checkpoint:**
- [ ] Timer outputs match expected frequencies (oscilloscope)
- [ ] Timer interrupts fire at correct intervals
- [ ] Timer chain synchronization verified

### Phase 3: DAC Ramp Generation (Week 3-4)

**Goals:**
- Implement DAC with DMA for frequency ramp
- Verify ramp waveform

**Tasks:**
1. Configure DAC1 with DMA
2. Port DAC LUT management:
   - `dac_lut_init()`
   - `dac_lut_set_table()`
   - `dac_set_single_value()`
   - `dac_lut_trigger_auto_start()`
3. Implement DMA transfer complete callback
4. Test ramp waveform with oscilloscope

**Validation Checkpoint:**
- [ ] DAC outputs correct ramp shape
- [ ] Ramp timing matches chirp duration
- [ ] DMA operates in circular mode without CPU intervention

### Phase 4: ADC Acquisition (Week 4-5)

**Goals:**
- Implement 4-channel ADC with DMA
- Synchronize to DAC ramp

**Tasks:**
1. Configure ADC1/ADC2 in dual mode (or sequential)
2. Configure 4 DMA channels for I1, Q1, I2, Q2
3. Implement acquisition buffer management:
   - `bsp_dma_set_destination_addr()`
   - `bsp_set_num_samples_per_dma_transfer()`
4. Configure ADC trigger from timer
5. Implement acquisition complete callback

**Validation Checkpoint:**
- [ ] ADC captures correct number of samples
- [ ] Data appears in correct buffers
- [ ] Timing synchronized to DAC ramp

### Phase 5: Frequency Calibration (Week 5-6)

**Goals:**
- Port frequency measurement and calibration
- Verify RF frequency accuracy

**Tasks:**
1. Implement divider output frequency counter:
   - `init_div_count()`
   - `div_get_frequency_Hz_wait()`
   - `div_clk_enable()` / `div_clk_disable()`
2. Port calibration routines:
   - `bootup_calibration_func()`
   - `do_freq_calibration()`
   - `finding_dac_value_freq_Hz()`
3. Test with spectrum analyzer or frequency counter

**Validation Checkpoint:**
- [ ] Frequency measurement accurate to specification
- [ ] Calibration converges correctly
- [ ] RF output within 24.025-24.225 GHz band

### Phase 6: Integration and Algorithm (Week 6-7)

**Goals:**
- Integrate all components
- Port upper layers

**Tasks:**
1. Port radar control layer
2. Port application layer
3. Implement USB CDC for ComLib protocol
4. Run complete acquisition cycle
5. Verify algorithm outputs (range, velocity)

**Validation Checkpoint:**
- [ ] Complete frame acquisition works
- [ ] USB communication with host GUI
- [ ] Algorithm produces valid range measurements

### Phase 7: Optimization and Testing (Week 7-8)

**Goals:**
- Performance optimization
- Reliability testing

**Tasks:**
1. Profile CPU usage
2. Optimize DMA and interrupt priorities
3. Test edge cases (min/max chirp configs)
4. Long-duration reliability test
5. Temperature stability test
6. Document differences from XMC implementation

**Validation Checkpoint:**
- [ ] Performance meets requirements
- [ ] No data loss under sustained operation
- [ ] System stable across temperature range

---

## 7. Alternative: Keep XMC4700 for Radar Processing

Instead of porting the firmware, consider keeping the XMC4700 as a dedicated radar processor with an interface to the STM32 main MCU.

### Comparison: Full Port vs Interface Abstraction

| Factor | Full Port to STM32H7 | Keep XMC4700 + Interface |
|--------|---------------------|-------------------------|
| **Development Time** | 6-10 weeks | 2-3 weeks (interface only) |
| **Technical Risk** | Medium-High | Low |
| **BOM Cost** | Lower (single MCU) | Higher (+$5-10 for XMC) |
| **PCB Complexity** | Lower | Higher (two MCU footprints) |
| **Maintenance** | Single codebase | Dual toolchains (DAVE + CubeIDE) |
| **Performance** | Full control | Proven, validated |
| **Future Flexibility** | High | Limited to D2GL capabilities |

### Interface Abstraction Approach

If keeping XMC4700:

```
+----------------+          +------------------+
|   STM32H7      |  UART/   |    XMC4700       |
|   Main MCU     |<-------->|   Radar MCU      |
|                |  SPI     |   (D2GL FW)      |
| - UI/Display   |          | - Chirp gen      |
| - Sensor fusion|          | - ADC acquisition|
| - Logging      |          | - FFT processing |
| - User control |          | - Target detect  |
+----------------+          +------------------+
```

**Interface Protocol Options:**
1. **UART** - Simple, 1-2 Mbps sufficient for processed data
2. **SPI** - Higher bandwidth for raw ADC data
3. **USB** - Can expose XMC USB to STM32 USB host

**Data to Transfer:**
- Configuration: Chirp parameters, detection thresholds
- Results: Range, velocity, signal strength, target count
- Optional: Raw ADC samples for advanced processing

### When to Choose Each Approach

**Choose Full Port if:**
- Single MCU strongly preferred for cost/complexity
- Deep algorithm customization needed
- Long product lifecycle justifies upfront investment
- Team has STM32 radar experience

**Choose Interface Abstraction if:**
- Time-to-market critical
- Proven radar performance required
- Algorithm modifications not expected
- Risk aversion preferred
- Prototype or low-volume production

---

## 8. Estimated Effort Summary

### Full Port Effort

| Phase | Duration | Complexity |
|-------|----------|------------|
| Foundation | 1-2 weeks | Low-Medium |
| Timer Infrastructure | 1-2 weeks | High |
| DAC Ramp Generation | 1 week | Medium |
| ADC Acquisition | 1-2 weeks | High |
| Frequency Calibration | 1-2 weeks | High |
| Integration | 1-2 weeks | Medium |
| Optimization/Testing | 1-2 weeks | Medium |
| **Total** | **6-10 weeks** | **High** |

### Interface Abstraction Effort

| Phase | Duration | Complexity |
|-------|----------|------------|
| Protocol Design | 2-3 days | Low |
| XMC Interface Module | 1 week | Medium |
| STM32 Interface Module | 1 week | Medium |
| Integration Testing | 3-5 days | Low |
| **Total** | **2-3 weeks** | **Low-Medium** |

---

## 9. Recommendation

For the IMSAFE project, **recommend keeping XMC4700 with interface abstraction** (matching IMSAFE-DEC-002) for the following reasons:

1. **Proven radar performance** - D2GL firmware is validated
2. **Lower risk** - No timing-critical code to debug
3. **Faster development** - Focus on IMSAFE-specific features
4. **Clear separation** - Radar and application have distinct responsibilities

If cost optimization becomes critical in production, a full port can be undertaken later with the interface abstraction providing a clear specification for the STM32 implementation.

---

## 10. References

### Source Files Examined
- `DAVE project/D2GL_SC_FMCW/Radar_Control/inc/radar_api.h`
- `DAVE project/D2GL_SC_FMCW/Radar_Control/inc/radar_control.h`
- `DAVE project/D2GL_SC_FMCW/BSP/Distance2GoL/inc/d2gl_bsp.h`
- `DAVE project/D2GL_SC_FMCW/BSP/Distance2GoL/inc/d2gl_sc_fmcw.h`
- `DAVE project/D2GL_SC_FMCW/BSP/Distance2GoL/src/d2gl_sc_fmcw.c`
- `DAVE project/D2GL_SC_FMCW/BSP/Distance2GoL/src/d2gl_sc_fmcw_timer.c`
- `DAVE project/D2GL_SC_FMCW/BSP/Distance2GoL/inc/d2gl_bsp_limits.h`
- `DAVE project/D2GL_SC_FMCW/BSP/radar_mcu4/inc/radar_mcu4_bsp.h`

### Relevant Datasheets
- `docs/04-datasheets/sensors/infineon-bgt24ltr11n16-datasheet-en.pdf`
- `docs/04-datasheets/sensors/https___www.infineon.com_assets_row_public_documents_24_44_infineon-ug100163-radar-baseboard-mcu4-usermanual-en.pdf`
- STM32H7 Reference Manual (RM0433)
- STM32H7 Datasheet

---

## Appendix A: Key XMC4700 DAVE Dependencies

The following DAVE APPs are used in the D2GL firmware and require STM32 equivalents:

| DAVE APP | Function | STM32 Equivalent |
|----------|----------|------------------|
| `TIMER` | General purpose timing | TIM HAL |
| `CCU4_SLICE_CONFIG` | Timer slices | TIM peripheral |
| `ADC_MEASUREMENT_ADV` | Advanced ADC with DMA | ADC HAL + DMA |
| `DAC_LUT` | DAC with waveform LUT | DAC HAL + DMA |
| `DIGITAL_IO` | GPIO control | GPIO HAL |
| `USB_DEVICE` | USB CDC | USB_DEVICE middleware |
| `I2C_MASTER` | EEPROM interface | I2C HAL |
| `SPI_MASTER` | PGA interface | SPI HAL |
| `GLOBAL_DMA` | DMA controller | DMA HAL |

## Appendix B: Critical Timing Parameters

From source code analysis:

```c
// Chirp timing (d2gl_sc_fmcw.c)
#define SC_FMCW_RAMP_UP_TIME_USEC      1500  // 1.5ms up ramp
#define SC_FMCW_RAMP_STAY_TIME_USEC    100   // 100us stay at top
#define SC_FMCW_RAMP_DOWN_TIME_USEC    1000  // 1.0ms down ramp

// Frame timing (d2gl_bsp_limits.h)
#define BSP_MIN_PRT_USEC               2500  // 2.5ms minimum PRT
#define BSP_MAX_PRT_USEC               10000 // 10ms maximum PRT
#define BSP_MIN_FRAME_PERIOD_MSEC      50    // 50ms minimum frame
#define BSP_MAX_FRAME_PERIOD_MSEC      200   // 200ms maximum frame

// ADC parameters
#define BSP_ADC_RESOLUTION             12    // 12-bit ADC
#define BSP_MAX_NUM_SAMPLES_PER_CHIRP  256   // Max samples per chirp
#define BSP_NUM_OF_ADC_CHANNELS        4     // I1, Q1, I2, Q2
```

## Appendix C: Interrupt Handlers to Port

```c
// Timer interrupts (d2gl_sc_fmcw.c)
void SW_IRQ_CCU4_ADC_TRIGGER_EVT_CMP_handler(void);  // ADC trigger compare
void SW_IRQ_CCU4_DAC_TRIGGER_EVT_CMP_handler(void);  // DAC trigger compare
void SW_IRQ_CCU4_PRT_TRIG_EVT_MATCH_handler(void);   // PRT period match
void SW_IRQ_TIMER_TX_EN_handler(void);               // TX enable timing

// These handlers manage:
// - Chirp sequencing
// - ADC start/stop
// - Calibration triggers
// - Power state transitions
```
