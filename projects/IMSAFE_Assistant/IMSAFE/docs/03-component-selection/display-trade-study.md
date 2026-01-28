# Display Trade Study: 3.5" TFT Selection

**Date:** 2026-01-28
**Phase:** 2.5 Display
**Status:** Pre-Selected - Documentation Only

---

## 1. Selection

**Pre-selected by user:** ST7789-based 3.5" TFT display

No trade study required - documenting integration considerations.

---

## 2. ST7789 Display Specifications

| Parameter | Typical Value | Notes |
|-----------|---------------|-------|
| Controller | ST7789V2 | Sitronix |
| Resolution | 320×480 (3.5") | Portrait orientation |
| Color depth | 16-bit RGB565 / 18-bit | 65K or 262K colors |
| Interface | SPI or 8080 parallel | 4-wire SPI common |
| Voltage | 3.3V logic, LED backlight | |
| Viewing angle | IPS ~170° typical | |

**Datasheet reference:** `docs/04-datasheets/sensors/ST7789V2-3314280.pdf`

---

## 3. Interface Options with STM32H747

### 3.1 Option A: SPI (4-wire)

| Parameter | Value |
|-----------|-------|
| Pins required | 4-5 (SCK, MOSI, CS, DC, [RST]) |
| Max SPI clock | ~60-80 MHz (ST7789 limit) |
| STM32H747 SPI | Up to 150 MHz |
| Frame rate (320×480×16bit) | ~15-20 FPS at 60MHz |
| DMA | Required for performance |

**Pros:**
- Fewer pins
- Simple wiring
- Common module pinout

**Cons:**
- Slower than parallel
- Full-screen updates limited to ~15-20 FPS
- CPU/DMA bandwidth for large updates

**Recommendation:** Acceptable for instrument display with partial updates

### 3.2 Option B: 8080 Parallel (8-bit or 16-bit)

| Parameter | 8-bit | 16-bit |
|-----------|-------|--------|
| Data pins | 8 | 16 |
| Control pins | 4 (CS, WR, RD, DC) | 4 |
| STM32H747 interface | FMC (SRAM mode) | FMC |
| Throughput | ~10 MB/s | ~20 MB/s |
| Frame rate | ~30 FPS | ~60 FPS |

**Pros:**
- Much faster than SPI
- FMC handles timing automatically
- Better for full-screen graphics

**Cons:**
- More pins (12-20)
- More complex PCB routing

**Recommendation:** Preferred if PCB space allows

### 3.3 Option C: LTDC (RGB Interface)

**Not applicable** - ST7789 does not support RGB parallel interface. LTDC would require a different display controller (e.g., ILI9806, SSD1963, or raw RGB panel + driver).

---

## 4. Recommended Configuration

### Primary: 8080 Parallel via FMC

For 320×480 display with synthetic horizon graphics, 8080 parallel provides best performance:

```
STM32H747 FMC (Bank 1)
├── D0-D7 or D0-D15  →  DB0-DB7/DB15
├── NE1 (CS)         →  CS
├── NWE (WR)         →  WR
├── NOE (RD)         →  RD (optional, write-only OK)
├── A16 or GPIO      →  DC (Data/Command)
└── GPIO             →  RST, BL_EN
```

**FMC Configuration:**
- SRAM Bank 1, Mode A (8080)
- 8-bit or 16-bit data width
- Write timing: ~60ns cycle (matches ST7789)

### Fallback: SPI with DMA

If parallel pins unavailable:

```
STM32H747 SPI6 (or SPI1-5)
├── SCK   →  SCL
├── MOSI  →  SDA
├── GPIO  →  CS
├── GPIO  →  DC
└── GPIO  →  RST, BL_EN
```

**SPI Configuration:**
- 8-bit mode, Mode 0 or 3
- 50-80 MHz clock (verify display module spec)
- DMA circular for framebuffer streaming
- Consider partial updates for better perceived performance

---

## 5. Backlight Control

| Method | Complexity | Dimming |
|--------|------------|---------|
| GPIO on/off | Simple | No |
| PWM dimming | Medium | Yes |
| LED driver IC | Higher | Yes, with current control |

**Recommendation:** PWM via TIM channel for brightness control

```
STM32H747 TIM
└── PWM output → MOSFET gate → Backlight LED+
```

Typical backlight: 20-40mA per LED string, 3-4 strings in series.

---

## 6. Graphics Considerations

### 6.1 Framebuffer Strategy

| Strategy | RAM Usage | Performance |
|----------|-----------|-------------|
| Full framebuffer | 307 KB (320×480×16bit) | Best - DMA entire frame |
| Double buffer | 614 KB | Tear-free, highest RAM |
| Partial buffer | 10-50 KB | Lower RAM, more CPU |
| Direct draw | Minimal | Slowest, tearing risk |

**STM32H747 has 1 MB SRAM** - full double-buffer is feasible.

### 6.2 Graphics Library Options

| Library | Footprint | Features | Complexity |
|---------|-----------|----------|------------|
| LVGL | 100-200 KB | Full widgets, animations | Medium |
| TouchGFX | 50-150 KB | ST optimized, designer tool | Medium |
| uGFX | 50-100 KB | Lightweight | Low |
| Custom | Variable | Minimal overhead | High |

**Recommendation:** LVGL or TouchGFX for synthetic horizon + gauges

### 6.3 Synthetic Horizon Requirements

- Smooth attitude updates: 30+ FPS target
- Horizon line rendering (trigonometry)
- Pitch ladder overlay
- Heading tape or compass rose
- Altitude/airspeed tapes (optional)

Partial screen updates can achieve 30 FPS even with SPI if only the horizon region updates.

---

## 7. Pin Budget

### 8080 Parallel (16-bit)

| Signal | STM32H747 Pin | Notes |
|--------|---------------|-------|
| D0-D15 | FMC_D0-D15 | 16 pins |
| CS | FMC_NE1 | Directly driven |
| WR | FMC_NWE | |
| RD | FMC_NOE | Optional |
| DC | FMC_A16 or GPIO | Address bit or GPIO |
| RST | GPIO | |
| BL_EN | TIM PWM | Backlight |
| **Total** | ~20 pins | |

### SPI (4-wire)

| Signal | STM32H747 Pin | Notes |
|--------|---------------|-------|
| SCK | SPIx_SCK | |
| MOSI | SPIx_MOSI | |
| CS | GPIO | |
| DC | GPIO | |
| RST | GPIO | |
| BL_EN | TIM PWM | |
| **Total** | 6 pins | |

---

## 8. Module Considerations

If using a pre-made module (common for 3.5" TFT):

**Check for:**
- Interface exposed (SPI only? Parallel available?)
- Touch controller included? (if yes, additional SPI/I2C)
- Voltage levels (5V tolerant or 3.3V only)
- Connector type (FPC, header pins)
- Mounting holes / bezel

**Common 3.5" ST7789 modules:**
- Generic SPI modules (AliExpress, etc.) - SPI only, slow
- Waveshare 3.5" displays - various interfaces
- Adafruit 3.5" TFT - SPI + optional parallel breakout

---

## 9. Power Budget

| Component | Current | Notes |
|-----------|---------|-------|
| ST7789 controller | 5-10 mA | Display active |
| Backlight | 40-120 mA | Depends on brightness |
| **Total** | ~50-130 mA | Display subsystem |

Backlight dominates - PWM dimming saves power.

---

## 10. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| SPI too slow for graphics | Medium | Use 8080 parallel or partial updates |
| Module only exposes SPI | Medium | Verify module pinout before PCB design |
| Backlight current draw | Low | PWM dimming, auto-dim in bright light |
| FMC pin conflict | Medium | Check against SDMMC, other peripherals |

---

## 11. Decision Summary

**Display:** ST7789-based 3.5" TFT (320×480)

**Interface recommendation priority:**
1. 8080 parallel via FMC (if module supports, best performance)
2. SPI with DMA (acceptable with partial updates)

**Graphics:** LVGL or TouchGFX recommended

**Backlight:** PWM dimming via timer

---

## 12. References

- docs/04-datasheets/sensors/ST7789V2-3314280.pdf
- [ST7789V2 Datasheet](https://www.newhavendisplay.com/resources_dataFiles/datasheets/LCDs/ST7789V2.pdf)
- [LVGL Graphics Library](https://lvgl.io/)
- [TouchGFX (ST)](https://www.st.com/en/development-tools/touchgfx.html)
