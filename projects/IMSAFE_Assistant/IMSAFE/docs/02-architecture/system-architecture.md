# IMSAFE System Architecture

## 1. System Block Diagram

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                                 POWER DOMAIN                                  │
│                                                                              │
│  ┌─────────┐    ┌─────────────┐    ┌──────────┐    ┌──────────────────────┐ │
│  │ USB-C   │───►│ Charge IC   │───►│ Battery  │───►│ Buck/Boost Converter │ │
│  │ 5V      │    │ (BQ24072)   │    │ 7.4V 2S  │    │ 3.3V / 5V Rails      │ │
│  └─────────┘    └─────────────┘    └──────────┘    └──────────────────────┘ │
│                                                              │               │
└──────────────────────────────────────────────────────────────┼───────────────┘
                                                               │
┌──────────────────────────────────────────────────────────────┼───────────────┐
│                            MAIN PROCESSOR                    │               │
│                                                              │               │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │                         MCU (TBD: STM32 / XMC)                          ││
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐    ││
│  │  │  SPI   │ │ I2C #1 │ │ I2C #2 │ │  GPIO  │ │  UART  │ │  USB   │    ││
│  │  └───┬────┘ └───┬────┘ └───┬────┘ └───┬────┘ └───┬────┘ └───┬────┘    ││
│  └──────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────┘│
│         │          │          │          │          │          │           │
└─────────┼──────────┼──────────┼──────────┼──────────┼──────────┼───────────┘
          │          │          │          │          │          │
     ┌────┴────┐     │          │          │          │     ┌────┴────┐
     │         │     │          │          │          │     │  USB-C  │
┌────▼───┐┌────▼───┐ │          │          │          │     │  Data   │
│Display ││SD Card │ │          │          │          │     └─────────┘
│ST7796  ││        │ │          │          │          │
└────────┘└────────┘ │          │          │          │
                     │          │          │          │
        ┌────────────┴───┐      │          │          │
        │    I2C Bus 1   │      │          │          │
        │                │      │          │          │
   ┌────▼───┐ ┌────▼───┐ │      │     ┌────▼───┐┌────▼───┐
   │ BMP390 │ │MPU9250 │ │      │     │WS2812  ││Encoders│
   │Altitude│ │  IMU   │ │      │     │16x LED ││ x2     │
   └────────┘ └────────┘ │      │     └────────┘└────────┘
                         │      │
   ┌────────┐ ┌────────┐ │      │
   │HMC5983 │ │MCP23017│ │      │
   │Compass │ │Switches│ │      │
   └────────┘ └────────┘ │      │
                         │      │
        ┌────────────────┴───┐  │
        │    I2C Bus 2       │  │
        │   (External)       │  │
        │                    │  │
   ┌────▼───┐ ┌────▼───┐     │  │
   │Engine  │ │Gearbox │     │  │
   │  IMU   │ │  IMU   │     │  │
   └────────┘ └────────┘     │  │
                             │  │
                        ┌────▼──┴───┐
                        │   UART    │
                        │           │
                   ┌────▼───┐  ┌────▼───┐
                   │Bluetooth│ │ Radar  │
                   │ Module │  │ MCU    │
                   └────────┘  └────┬───┘
                                    │
┌───────────────────────────────────┼───────────────────────────────────────┐
│                          RADAR SUBSYSTEM                                   │
│                                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐ │
│  │                    XMC4700 (Distance2goL)                           │ │
│  │  ┌────────────────┐    ┌────────────────┐    ┌────────────────┐    │ │
│  │  │ BGT24LTR11     │───►│ Signal         │───►│ Distance       │    │ │
│  │  │ 24GHz Radar    │    │ Processing     │    │ Output (UART)  │    │ │
│  │  └────────────────┘    └────────────────┘    └────────────────┘    │ │
│  └─────────────────────────────────────────────────────────────────────┘ │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘
```

## 2. Interface Definitions

### 2.1 I2C Bus 1 (Internal Sensors)

| Address | Device | Function |
|---------|--------|----------|
| 0x77 | BMP390 | Barometric pressure/altitude |
| 0x68 | MPU9250 | Accelerometer/Gyroscope |
| 0x1E | HMC5983 | Magnetometer |
| 0x20 | MCP23017 | Switch inputs |

**Bus Speed:** 400kHz (Fast Mode)

### 2.2 I2C Bus 2 (External Sensors)

| Address | Device | Function |
|---------|--------|----------|
| 0x68 | Engine IMU | Vibration analysis |
| 0x69 | Gearbox IMU | Vibration analysis |

**Bus Speed:** 400kHz
**Cable:** Shielded, max 1m

### 2.3 SPI Bus

| Device | CS Pin | Speed | Mode |
|--------|--------|-------|------|
| ST7796 Display | PA4 | 40MHz | Mode 0 |
| SD Card | PB12 | 25MHz | Mode 0 |

### 2.4 UART Interfaces

| Interface | Baud | Device | Notes |
|-----------|------|--------|-------|
| UART1 | 115200 | Debug | Optional |
| UART2 | 115200 | Bluetooth | AT commands |
| UART3 | 115200 | Radar MCU | Distance data |

### 2.5 GPIO

| Pin | Function | Direction | Notes |
|-----|----------|-----------|-------|
| PA0 | WS2812 Data | Output | 800kHz protocol |
| PA1 | Encoder 1 A | Input | With pull-up |
| PA2 | Encoder 1 B | Input | With pull-up |
| PA3 | Encoder 1 SW | Input | Active low |
| PB0 | Encoder 2 A | Input | With pull-up |
| PB1 | Encoder 2 B | Input | With pull-up |
| PB2 | Encoder 2 SW | Input | Active low |
| PC0 | MCP23017 INT | Input | Interrupt |

### 2.6 Analog Inputs

| Pin | Function | Range | Notes |
|-----|----------|-------|-------|
| ADC1 | MICS-4514 CO | 0-3.3V | Reducing gas |
| ADC2 | MICS-4514 NO2 | 0-3.3V | Oxidizing gas |
| ADC3 | Battery voltage | 0-3.3V | Via divider |

## 3. Data Flow

### 3.1 Sensor Data Path

```
Sensors (10-50Hz) ──► Sample Buffer ──► Filter ──► Display/Log
                                           │
                                           ▼
                                      Threshold Check
                                           │
                                           ▼
                                      Alarm Handler
```

### 3.2 Radar Data Path

```
BGT24LTR11 ──► XMC4700 DSP ──► Distance Value ──► UART ──► Main MCU ──► Display
                    │
                    ▼
              Velocity Value
```

## 4. Power Budget (Preliminary)

| Subsystem | Typical | Peak | Notes |
|-----------|---------|------|-------|
| Main MCU | 30mA | 50mA | @ 80MHz |
| Display | 50mA | 100mA | Backlight dependent |
| Sensors | 5mA | 10mA | All active |
| LEDs | 50mA | 320mA | 16x WS2812 @ 20mA |
| Radar | 200mA | 300mA | XMC4700 + BGT24LTR11 |
| Bluetooth | 20mA | 40mA | Active |
| **Total** | **355mA** | **820mA** | |

**Battery Life Estimate:**
- 16000mAh / 400mA = ~40 hours typical operation
