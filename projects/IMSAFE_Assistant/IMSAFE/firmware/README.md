# IMSAFE Firmware

## Overview

The IMSAFE firmware is split across two MCUs:
1. **Main MCU** - Handles display, sensors, user interface, logging
2. **Radar MCU (XMC4700)** - Handles radar signal processing

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                         │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   app_main.c    │   app_sensor.c  │      app_display.c          │
│   - State mgmt  │   - Sensor read │      - Graphics rendering   │
│   - Main loop   │   - Filtering   │      - Mode switching       │
│   - Power mgmt  │   - Alarms      │      - UI elements          │
├─────────────────┼─────────────────┼─────────────────────────────┤
│ app_checklist.c │  app_radar.c    │      app_vibration.c        │
│ - Switch state  │  - Radar comm   │      - FFT analysis         │
│ - LED control   │  - Distance     │      - Anomaly detection    │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                        SERVICE LAYER                             │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   svc_config.c  │   svc_log.c     │      svc_comm.c             │
│   - Settings    │   - SD logging  │      - USB CDC              │
│   - Calibration │   - Ring buffer │      - Bluetooth            │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                        DRIVER LAYER                              │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   drv_bmp390.c  │   drv_icm20948.c│      drv_st7796.c           │
│   drv_mics4514.c│   drv_mcp23017.c│      drv_ws2812.c           │
│   drv_sdcard.c  │   drv_encoder.c │      drv_i2c.c              │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                     HAL / CMSIS LAYER                            │
│                    (STM32 HAL or DAVE)                           │
└─────────────────────────────────────────────────────────────────┘
```

## Directory Structure

```
firmware/
├── README.md           # This file
├── app/                # Application layer
│   ├── inc/           # Application headers
│   └── src/           # Application source
├── drivers/           # Peripheral drivers
│   ├── inc/           # Driver headers
│   └── src/           # Driver source
├── boards/            # Board support packages
│   ├── imsafe_main/   # Main board BSP
│   └── imsafe_radar/  # Radar board BSP
└── tools/             # Build tools and scripts
```

## Build System

TBD - Depends on final MCU selection:
- **STM32:** STM32CubeIDE or CMake + arm-none-eabi-gcc
- **XMC:** Infineon DAVE or CMake

## Module Descriptions

### Application Layer

| Module | Description |
|--------|-------------|
| app_main | Main loop, state machine, power management |
| app_sensor | Sensor polling, filtering, threshold monitoring |
| app_display | Display rendering, mode management |
| app_checklist | Switch monitoring, LED control |
| app_radar | Radar MCU communication |
| app_vibration | Vibration FFT, anomaly detection |

### Driver Layer

| Driver | Hardware | Interface |
|--------|----------|-----------|
| drv_bmp390 | Altitude sensor | I2C |
| drv_icm20948 | IMU/Compass | I2C |
| drv_mics4514 | Gas sensor | ADC |
| drv_st7796 | TFT Display | SPI |
| drv_mcp23017 | I/O Expander | I2C |
| drv_ws2812 | RGB LEDs | GPIO (bitbang/DMA) |
| drv_sdcard | MicroSD | SPI/SDIO |
| drv_encoder | Rotary encoders | GPIO + Timer |

## State Machine

```
          ┌─────────────┐
          │  POWER_ON   │
          │  (init)     │
          └──────┬──────┘
                 │
                 ▼
          ┌─────────────┐
    ┌────►│    IDLE     │◄────┐
    │     │  (standby)  │     │
    │     └──────┬──────┘     │
    │            │            │
    │    Button  │  USB       │
    │            │            │
    │     ┌──────▼──────┐     │
    │     │   ACTIVE    │     │
    │     │  (running)  │     │
    │     └──────┬──────┘     │
    │            │            │
    │   Timeout  │    Error   │
    │            │            │
    │     ┌──────▼──────┐     │
    │     │   SLEEP     │─────┘
    │     │  (low pwr)  │
    │     └──────┬──────┘
    │            │
    │    Long press
    │            │
    │     ┌──────▼──────┐
    └─────┤  SHUTDOWN   │
          │  (off)      │
          └─────────────┘
```

## Configuration

Configuration stored in flash:
- Display brightness
- Alarm thresholds (CO, altitude)
- Checklist labels
- Sample rates
- Bluetooth pairing

## Development Notes

1. **Sensor Sample Rates:**
   - Altitude: 10Hz
   - IMU: 50Hz
   - Gas: 1Hz
   - Temperature: 0.5Hz

2. **Display Update:**
   - Primary mode: 30Hz
   - Settings: 10Hz

3. **Power Optimization:**
   - Use DMA for SPI transfers
   - Sleep between sensor reads
   - Reduce LED brightness when on battery

4. **Vibration Analysis:**
   - FFT run every 10 seconds (CPU intensive)
   - 1024-point FFT
   - Store baseline for comparison
