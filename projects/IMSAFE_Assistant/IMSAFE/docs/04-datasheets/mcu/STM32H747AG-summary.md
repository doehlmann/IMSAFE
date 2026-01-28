# STM32H747AG – Datasheet Summary

## Device Overview
**Part:** STM32H747AG  
**Vendor:** STMicroelectronics  
**Family:** STM32H7 (High‑Performance, Dual‑Core)  
**CPU Cores:**  
- ARM Cortex‑M7 up to 480 MHz  
- ARM Cortex‑M4 up to 240 MHz  

Targeted at high‑performance embedded systems combining real‑time control, graphics, and rich peripheral integration.

---

## Architecture
- Dual‑core asymmetric multiprocessing (AMP)
- Hardware semaphores (HSEM) for inter‑core coordination
- Independent boot and reset domains
- Shared and tightly‑coupled memories

**Design implication:** Allows clean separation of time‑critical processing vs UI, comms, logging.

---

## Memory
- Flash: **2 MB dual‑bank**
- SRAM: ~**1 MB total**
  - AXI SRAM
  - DTCM / ITCM (zero‑wait‑state)
- External memory:
  - Quad‑SPI / Octo‑SPI
  - FMC (SDRAM, NOR, NAND)

Suitable for frame buffers, logging, and radar/graphics workloads.

---

## Key Peripherals
- **Graphics:** LTDC, DMA2D (Chrom‑ART)
- **Storage:** SDMMC (microSD), QSPI/OSPI flash
- **Connectivity:** USB FS/HS, Ethernet MAC, CAN‑FD
- **Serial:** SPI, I²C, UART (multiple instances)
- **Timers:** High‑resolution + motor‑control capable
- **Analog:** 3× ADC (up to 16‑bit), DAC

---

## Power
- Core supply: ~1.2 V
- I/O supply: 1.8 V / 3.3 V
- Dynamic voltage scaling
- Multiple low‑power modes

**Risk:** High peak current under full M7 + graphics load.

---

## Package
- LQFP‑176
- High pin‑count → routing and power‑plane complexity

---

## Software Ecosystem
- STM32CubeH7 HAL / LL
- FreeRTOS support
- CubeMX / CubeIDE
- Strong community and long‑term availability

---

## Strengths
- Extremely high integration
- Excellent graphics and compute
- Mature tooling ecosystem

## Risks
- Power consumption
- Configuration complexity
- May be overkill if dual‑MCU architecture retained

---

## IMSAFE Relevance
Potential candidate to **replace dual‑MCU design**, pending Phase 2 trade study on timing, power, and complexity.

