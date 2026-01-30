# STM32H747IIT6 Pin Allocation - IMSAFE Project

**Package:** LQFP-176
**Date:** 2026-01-30

---

## Pin Assignment by Function

### Power Pins (37 pins)

| Pin | Name | Net | Notes |
|-----|------|-----|-------|
| 6 | VBAT | VBAT | RTC backup, connect to +3V3 or battery |
| 11, 19, 28, 50, 75, 100, 106, 131, 144, 159 | VDD | +3V3 | Digital power (10 pins) |
| 10, 27, 74, 105, 130, 143, 158 | VSS | GND | Digital ground (7 pins) |
| 20 | VSSA | GND | Analog ground |
| 21 | VREF+ | +3V3 | ADC reference (can be separate) |
| 22 | VDDA | +3V3 | Analog power |
| 49 | VCAP1 | 2.2µF→GND | Internal regulator |
| 73 | VCAP2 | 2.2µF→GND | Internal regulator |
| 99 | PDR_ON | +3V3 | Power-on reset control |

### Oscillator Pins (4 pins)

| Pin | Name | Net | Notes |
|-----|------|-----|-------|
| 12 | PH0/OSC_IN | HSE_IN | 8MHz crystal input |
| 13 | PH1/OSC_OUT | HSE_OUT | 8MHz crystal output |
| 8 | PC14/OSC32_IN | LSE_IN | 32.768kHz crystal input |
| 9 | PC15/OSC32_OUT | LSE_OUT | 32.768kHz crystal output |

### Reset & Boot (2 pins)

| Pin | Name | Net | Notes |
|-----|------|-----|-------|
| 14 | NRST | NRST | Reset, 10k pull-up + 100nF filter |
| 94 | BOOT0 | BOOT0 | Boot mode, 10k pull-down |

### Debug (SWD) - 3 pins

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 72 | PA13 | — | SWDIO | Debug data I/O |
| 76 | PA14 | — | SWCLK | Debug clock |
| 89 | PB3 | — | SWO | Trace output (optional) |

### USB (4 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 68 | PA9 | — | USB_VBUS | VBUS sense (GPIO input) |
| 69 | PA10 | AF10 | USB_ID | OTG ID (optional) |
| 70 | PA11 | AF10 | USB_DM | USB D- |
| 71 | PA12 | AF10 | USB_DP | USB D+ |

### CAN-FD (2 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 81 | PD0 | AF9 | FDCAN1_RX | CAN receive |
| 82 | PD1 | AF9 | FDCAN1_TX | CAN transmit |

### I2C1 - Sensors (2 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 95 | PB8 | AF4 | I2C1_SCL | Sensor bus clock |
| 96 | PB9 | AF4 | I2C1_SDA | Sensor bus data |

### I2C4 - I/O Expander (2 pins via FFC)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 112 | PD12 | AF4 | I2C4_SCL | MCP23017 clock |
| 113 | PD13 | AF4 | I2C4_SDA | MCP23017 data |

### SPI1 - Display (4 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 30 | PA5 | AF5 | DISP_SCK | Display SPI clock |
| 90 | PB4 | AF5 | DISP_MISO | Display MISO |
| 91 | PB5 | AF5 | DISP_MOSI | Display MOSI |
| 36 | PB1 | GPIO | DISP_CS | Display chip select |

### Display Control (3 pins)

| Pin | Name | Function | Net | Notes |
|-----|------|----------|-----|-------|
| 29 | PA4 | GPIO | DISP_DC | Data/Command |
| 35 | PB0 | GPIO | DISP_RST | Display reset |
| 37 | PB2 | GPIO/PWM | DISP_BL | Backlight PWM |

### SDMMC1 - microSD (7 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 80 | PC12 | AF12 | SD_CLK | SDMMC clock |
| 83 | PD2 | AF12 | SD_CMD | SDMMC command |
| 65 | PC8 | AF12 | SD_D0 | Data bit 0 |
| 66 | PC9 | AF12 | SD_D1 | Data bit 1 |
| 78 | PC10 | AF12 | SD_D2 | Data bit 2 |
| 79 | PC11 | AF12 | SD_D3 | Data bit 3 |
| 59 | PD12 | GPIO | SD_CD | Card detect (shared w/ I2C4) |

### USART2 - BLE Module (2 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 25 | PA2 | AF7 | ESP32_RX | TX to ESP32 |
| 26 | PA3 | AF7 | ESP32_TX | RX from ESP32 |

### ADC Channels (3 pins)

| Pin | Name | ADC | Net | Notes |
|-----|------|-----|-----|-------|
| 18 | PC3 | ADC3_INP1 | MICS_RED | Gas sensor reducing |
| 24 | PA1 | ADC1_INP1 | MICS_OX | Gas sensor oxidizing |
| 33 | PC4 | ADC1_INP4 | VBAT_SENSE | Battery voltage |

### Timer Channels - Encoders (4 pins)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 31 | PA6 | AF2 | ENC1_A | TIM3_CH1 encoder |
| 32 | PA7 | AF2 | ENC1_B | TIM3_CH2 encoder |
| 92 | PB6 | AF2 | ENC2_A | TIM4_CH1 encoder |
| 93 | PB7 | AF2 | ENC2_B | TIM4_CH2 encoder |

### Timer Channel - LED Data (1 pin)

| Pin | Name | AF | Net | Notes |
|-----|------|-----|-----|-------|
| 23 | PA0 | AF1 | LED_DATA | TIM2_CH1 WS2812B |

### GPIO - Control Signals

| Pin | Name | Function | Net | Notes |
|-----|------|----------|-----|-------|
| 7 | PC13 | GPIO In | WAKEUP | Wake from sleep |
| 15 | PC0 | GPIO In | MCP_INTA | I/O expander INT |
| 16 | PC1 | GPIO Out | ESP32_EN | BLE enable |
| 17 | PC2 | GPIO Out | ESP32_IO9 | BLE boot mode |
| 34 | PC5 | GPIO Out | MICS_HEATER | Gas heater control |
| 51 | PB12 | GPIO In | BQ_INT | Charger interrupt |
| 52 | PB13 | GPIO Out | BQ_CE | Charger enable |
| 60 | PD13 | GPIO Out | SD_CS | SD card CS (SPI mode) |
| 61 | PD14 | GPIO Out | BUZZER | Buzzer control |
| 63 | PC6 | GPIO In | ENC1_SW | Encoder 1 switch |
| 64 | PC7 | GPIO In | ENC2_SW | Encoder 2 switch |

---

## Pin Usage Summary

| Category | Count | Percentage |
|----------|-------|------------|
| Power/Ground | 37 | 21% |
| Oscillators | 4 | 2% |
| Reset/Boot | 2 | 1% |
| Debug (SWD) | 3 | 2% |
| USB | 4 | 2% |
| CAN-FD | 2 | 1% |
| I2C | 4 | 2% |
| SPI/Display | 7 | 4% |
| SDMMC | 7 | 4% |
| UART | 2 | 1% |
| ADC | 3 | 2% |
| Timers/PWM | 5 | 3% |
| GPIO | 11 | 6% |
| **Used** | **91** | **52%** |
| **Reserved/NC** | **85** | **48%** |

---

## Alternate Function Reference

### AF Mapping Used

| AF | Peripheral |
|----|------------|
| AF1 | TIM2 |
| AF2 | TIM3, TIM4 |
| AF4 | I2C1, I2C4 |
| AF5 | SPI1, SPI2 |
| AF7 | USART2 |
| AF9 | FDCAN1 |
| AF10 | USB_OTG_FS |
| AF12 | SDMMC1 |

---

## Critical Pin Notes

1. **VCAP1/VCAP2 (Pins 49, 73):** Must have 2.2µF ceramic capacitors to GND. Do not connect to any other net.

2. **VREF+ (Pin 21):** For best ADC accuracy, add LC filter from VDDA. Minimum: direct connection to +3V3.

3. **PDR_ON (Pin 99):** Tie to VDD for normal operation. Do not leave floating.

4. **BOOT0 (Pin 94):** Must be pulled low for normal flash boot. Add 10k pull-down.

5. **PA13/PA14 (Pins 72, 76):** SWDIO/SWCLK default functions. Do not use for GPIO in production without careful consideration.

6. **Unused Pins:** Configure as GPIO output low or input with pull-down in firmware to minimize power consumption and EMI.

---

## Reserved Pins for Future Use

The following pins are reserved but not currently allocated:

- PE2-PE15 (14 pins) - Potential FMC/LTDC for parallel display
- PD3-PD11 (9 pins) - Additional GPIO
- PB10, PB11 - I2C2 spare bus
- PA8, PA15, PA17 - Spare GPIO

---

*Document generated for Circuit Mind handoff*
