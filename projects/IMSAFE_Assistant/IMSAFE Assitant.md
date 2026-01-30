# Functional Specification
## Electronic Gadget: [PROJECT NAME]

| Document Info | |
|---------------|---|
| Project Code | IMSAFE flying Assistant |
| Revision | A |
| Date | 2026-1-27 |
| Author | Dean Oehlmann
| Status | Draft / Review / Approved |

---

## 1. Executive Summary

### 1.1 Product Description
This is a flying safety assistant for GA pilots and light sports pilots, it is not designed to replace approved safety devices, however it is there to enhance the safety performance of pilots, by electroniclly monitoring equipment and procedures and providing pilot prompts. The fundamental deign of the unit is a tactile device that consists of a series of switches that act as a checklist for take off and landing to ensure that procedures are followed. The checklist is complete when all switches are in either the up position for take off and the down position for landing. Above and below each switch (8 switches) are programmable LED that provides acknowledgement. The unit will have a central 3.5" TFT screen for additional information. The unit will integrate the additional following features; -carbon monoxide monitoring, carbon dioxide monitoring, altitude through barmoetric pressure, an IMU and compass for a syntehtic display, a landing radar to assist in flaring, a watch and timer, looking at 2 IMU attached to the motor and gearbox for vibration analysis. The unit will run on USB, with battery back up.

### 1.2 Key Features
- Feature 1: 8 toggle switches, up position as checklist for take off, down postion as checklist for landing. The toggle switches are an SPDT. Either all off or all on to indicate checklist complete. To saveon inputs we will look at using a MCP230178T-E/ML chip using I2C. Or equivalent chip.
- Feature 2: Above and below each toggle switch is a WS2812 programmable LED, that will indicate completetion. As the colour and brightness is programmable. This will allow alarm status to be indicated with all flashing in red at maximum brightness. It also allow for a colour change and blinking when checklist is complete.
- Feature 3: the unit will contain a pressure sensor to measure altitude based on pressure change. It will be on the I2C circuit at this stage we have a bosch BMP390.
- Feature 4: an IMU and compass. Pixracer uses dual MPU9250/6500 and HMC5983. The purpose is to create and electronic compass and a roll/yaw/pitch to create a heads up dispaly similar to a glass display.
- feature 5: a 3.5 TFT display ST7796 320xrgbx480 3/4 wire SPI, 8 parallel.
- feature 6: A rtc and timer, display
- featrue 7: A left and right encoder with integrated pushbutton PEC12R-4130F-S0012.
- feature 8: a micro SD card. to record flight and incident.
- Feature 9: a carbon monoxide monitor. Looking at a MICS-4514 which can measure both CO and NO2. May consider CO2 as well as in indication of combustion gases inside the cabin.
- Feature 10: 2x IMU that will be attached to the engine and gearbox to monitor vibration analysis. The best unit is unknown at this point. 
- Feature 11: Bluetooth for connectivity.
- Feature 12: 7.4V 16000 mAH lithium battery back up. 
- Feature 13 USB-C data conenctivity programming and charging.
- Feature 14 : Ruggedised not military grade components.
- Feature 15: landing radar. A landing radar based on the Infineon  BGT24LTR11 Shield V3.0. This will provide height of 30m down to flare height. We will utilise the Distance2goL as the platform for this.
- Feature 16: Unit wakes on USB power or button on. Unit will go to sleep after 3 hours on battery power or low battery power.  
- Feature 17: Cabin Temperature

### 1.3 Target Market / Application
GA Pilot and LSA pilots with experimental aircraft/LSA aircraft or GA aircraft.

### 1.4 Project Goals
| Goal | Metric | Target |
|------|--------|--------|
| Unit cost | BOM cost | < $250 |
| Development time | Weeks | 4 weeks |
| Power consumption | Active current | < XX mA |
| Size | PCB dimensions |as required|

---

## 2. Requirements

### 2.1 Functional Requirements

| ID | Requirement | Priority | Verification |
|----|-------------|----------|--------------|
| FR-001 | Device shall [action] | Must | Test |
| FR-002 | Device shall [action] | Must | Test |
| FR-003 | Device shall [action] | Should | Test |
| FR-004 | Device shall [action] | Could | Demo |

**Priority Key:**
- Must: Required for MVP
- Should: Important but not blocking
- Could: Nice to have

### 2.2 Performance Requirements

| Parameter | Min | Typical | Max | Unit | Notes |
|-----------|-----|---------|-----|------|-------|
| Supply Voltage | 3.0 | 3.3 | 3.6 | V | |
| Active Current | - | 30 | 50 | mA | All peripherals on |
| Sleep Current | - | 5 | 10 | µA | RTC running |
| Operating Temp | -20 | 25 | 70 | °C | |
| Storage Temp | -40 | - | 85 | °C | |
| Startup Time | - | - | 500 | ms | To operational |
| Measurement Accuracy | - | ±1 | ±2 | % | [of what] |
| Update Rate | - | 10 | - | Hz | |

### 2.3 Physical Requirements

| Parameter | Specification | Notes |
|-----------|---------------|-------|
| Dimensions | XX × YY × ZZ mm | Without enclosure |
| Weight | < XX g | Target |
| Enclosure | IP65 / Desktop / Handheld | |
| Mounting | DIN rail / Panel / Freestanding | |
| Connector(s) | USB-C, JST-XH | |

### 2.4 Environmental Requirements

| Parameter | Specification | Standard |
|-----------|---------------|----------|
| ESD Protection | ±4kV contact, ±8kV air | IEC 61000-4-2 |
| EMC Emissions | Class B | FCC Part 15 |
| RoHS Compliance | Required | RoHS 3 |
| Operating Humidity | 10-90% RH non-condensing | |

---

## 3. System Architecture

### 3.1 Block Diagram

```
                           ┌────────────────────────────────────────────┐
                           │              POWER MANAGEMENT              │
                           │  ┌─────────┐    ┌─────────┐    ┌───────┐  │
       USB-C ──────────────┼─►│ USB PD  ├───►│ LDO/DCDC├───►│ 3.3V  │  │
                           │  │ IC      │    │ REG     │    │ Rail  │  │
       Battery ────────────┼─►│         │    └────┬────┘    └───┬───┘  │
                           │  └────┬────┘         │             │      │
                           │       │         ┌────▼────┐        │      │
                           │       │         │ Charger │        │      │
                           │       │         │ IC      │        │      │
                           │       │         └─────────┘        │      │
                           └───────┼────────────────────────────┼──────┘
                                   │                            │
┌──────────────────────────────────┼────────────────────────────┼────────────────┐
│                                  │          MCU               │                │
│                           ┌──────▼──────┐             ┌───────▼───────┐        │
│                           │    USB      │             │   Power       │        │
│                           │  Full Speed │             │   Management  │        │
│                           └──────┬──────┘             └───────────────┘        │
│                                  │                                              │
│  ┌─────────┐    ┌───────┐   ┌────▼────┐   ┌────────┐    ┌─────────────┐        │
│  │ Sensor  │◄──►│ I2C   │◄──┤         ├──►│ SPI    │◄──►│ Flash/EEPROM│        │
│  │ 1       │    │ Bus   │   │         │   │ Bus    │    │             │        │
│  └─────────┘    └───┬───┘   │         │   └────────┘    └─────────────┘        │
│  ┌─────────┐        │       │         │                                        │
│  │ Sensor  │◄───────┘       │         │   ┌────────┐    ┌─────────────┐        │
│  │ 2       │                │         ├──►│ UART   │◄──►│ Debug/Ext   │        │
│  └─────────┘                │         │   └────────┘    └─────────────┘        │
│                             │         │                                        │
│  ┌─────────┐                │         │   ┌────────┐    ┌─────────────┐        │
│  │ Button  │───────────────►│  GPIO   │◄──┤ Timer  │    │ PWM Output  │        │
│  └─────────┘                │         │   └────────┘    └──────┬──────┘        │
│                             │         │                        │               │
│  ┌─────────┐                │         │   ┌────────┐    ┌──────▼──────┐        │
│  │ LED(s)  │◄───────────────┤         │◄──┤ ADC    │◄───┤ Analog In   │        │
│  └─────────┘                └────┬────┘   └────────┘    └─────────────┘        │
│                                  │                                              │
│                             ┌────▼────┐                                        │
│                             │ SWD     │◄───── Programming/Debug                │
│                             │ Port    │                                        │
│                             └─────────┘                                        │
└────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 MCU Selection

**Selected MCU:**
Currently we are exploring the use of the infineon xmc4700 chip as this is used in the landing radar baseboard. Schematics are available. Depending upon the headroom required for the computing requirements. It may usilize the XMC4700 for the radar management and an additional IMC for all other functions perhaps stick with the infineon range and keep the "DAVE" programming environment. The vibration analaysis may be CPU hungry. This might run only every 10 seconds. So processor management is an issue. The second is the debug and programming. having the 2 IMC will require some data management.




### 3.3 Power Architecture


```

### 3.4 Pin Allocation

| Pin | Function | Direction | Notes |
|-----|----------|-----------|-------|

---

## 4. Detailed Design

### 4.1 Sensor Interface

**Sensor 1: Temperature *
| Parameter | Value |
|-----------|-------|
| Interface | I2C, address 0x48 |
| Range | -55°C to +150°C |
| Resolution | 0.0078°C (16-bit) |
| Accuracy | ±0.1°C (typ) |
| Sample Rate | Up to 64 Hz |
| Power | 3.5µA active, 150nA shutdown |

**Sensor 2: Humidity (SHT40)**
| Parameter | Value |
|-----------|-------|
| Interface | I2C, address 0x44 |
| RH Range | 0-100% RH |
| RH Accuracy | ±1.8% RH |
| Temp Range | -40°C to +125°C |
| Power | 400µA during measurement |

### 4.2 User Interface

**LEDs:**
| LED | Color | Function |
|-----|-------|----------|
| LED1 | Green | Power/Status |
| LED2 | Red | Error/Charging |

**LED Patterns:**
| State | LED1 (Green) | LED2 (Red) |
|-------|--------------|------------|
| Power On | Solid | Off |
| Standby | Slow blink (1Hz) | Off |
| Active | Fast blink (5Hz) | Off |
| Charging | Off | Solid |
| Charged | Solid | Off |
| Error | Off | Fast blink |
| Low Battery | Slow blink | Slow blink |

**Button:**
| Action | Duration | Function |
|--------|----------|----------|
| Short press | < 500ms | Wake / Mode toggle |
| Long press | 2-5 sec | Power on/off |
| Very long | > 10 sec | Factory reset |

### 4.3 Data Storage

**External Flash: W25Q128 (16MB)**
| Parameter | Value |
|-----------|-------|
| Interface | SPI, 104MHz max |
| Capacity | 128Mbit (16MB) |
| Page Size | 256 bytes |
| Sector Size | 4KB |
| Erase Cycles | 100,000 |
| Data Retention | 20 years |

**Memory Map:**
| Address | Size | Contents |
|---------|------|----------|
| 0x000000 | 4KB | Configuration |
| 0x001000 | 4KB | Calibration data |
| 0x002000 | 64KB | Firmware backup |
| 0x012000 | 15.9MB | Data logging |

### 4.4 Communication Protocol

**USB CDC (Virtual COM Port):**

| Baud Rate | 115200 (default, auto-detect) |
|-----------|-------------------------------|
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Flow Control | None |

**Command Format:**
```
Command:  <CMD>[:<PARAM>]\r\n
Response: <STATUS>:<DATA>\r\n

Examples:
  GET:TEMP    → OK:25.50\r\n
  GET:HUM     → OK:45.2\r\n
  GET:ALL     → OK:T=25.50,H=45.2,B=78\r\n
  SET:RATE:10 → OK\r\n
  SET:RATE:0  → ERR:INVALID_PARAM\r\n
```

**Command Reference:**
| Command | Parameters | Response | Description |
|---------|------------|----------|-------------|
| `GET:TEMP` | - | `OK:<temp>` | Read temperature (°C) |
| `GET:HUM` | - | `OK:<hum>` | Read humidity (%) |
| `GET:BATT` | - | `OK:<pct>` | Read battery (%) |
| `GET:ALL` | - | `OK:T=<t>,H=<h>,B=<b>` | Read all |
| `GET:VER` | - | `OK:<major>.<minor>.<patch>` | Firmware version |
| `GET:SN` | - | `OK:<serial>` | Serial number |
| `SET:RATE` | `<ms>` | `OK` or `ERR:...` | Set sample rate (100-60000ms) |
| `SET:LOG` | `ON`/`OFF` | `OK` | Enable/disable logging |
| `CMD:RESET` | - | `OK` | Software reset |
| `CMD:DFU` | - | `OK` | Enter bootloader |
| `CMD:CAL` | `<type>` | `OK` | Run calibration |

**Error Codes:**
| Code | Description |
|------|-------------|
| `ERR:UNKNOWN_CMD` | Command not recognized |
| `ERR:INVALID_PARAM` | Parameter out of range |
| `ERR:SENSOR_FAIL` | Sensor communication error |
| `ERR:BUSY` | Operation in progress |

---

## 5. Firmware Architecture

### 5.1 Module Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                        │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   app_main.c    │   app_sensor.c  │      app_comm.c             │
│   - State mgmt  │   - Read cycle  │      - USB CDC handling     │
│   - Main loop   │   - Filtering   │      - Command parser       │
│   - Power mgmt  │   - Calibration │      - Response builder     │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                        SERVICE LAYER                            │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   svc_config.c  │   svc_log.c     │      svc_ui.c               │
│   - Load/save   │   - Data storage│      - LED control          │
│   - Defaults    │   - Ring buffer │      - Button debounce      │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                        DRIVER LAYER                             │
├─────────────────┬─────────────────┬─────────────────────────────┤
│   drv_i2c.c     │   drv_spi.c     │      drv_gpio.c             │
│   drv_tmp117.c  │   drv_w25q.c    │      drv_usb.c              │
│   drv_sht40.c   │                 │      drv_adc.c              │
├─────────────────┴─────────────────┴─────────────────────────────┤
│                     HAL / CMSIS LAYER                           │
│                    stm32l4xx_hal_*.c                            │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 State Machine

```
                    ┌─────────────┐
                    │  POWER_ON   │
                    │  (init hw)  │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
       ┌───────────►│    IDLE     │◄───────────┐
       │            │  (low power)│            │
       │            └──────┬──────┘            │
       │                   │                   │
       │           Button  │  USB Connect      │
       │                   │                   │
       │            ┌──────▼──────┐            │
       │            │   ACTIVE    │            │
       │            │  (sampling) │            │
       │            └──────┬──────┘            │
       │                   │                   │
       │   Timeout /       │        Error      │
       │   Button          │                   │
       │            ┌──────▼──────┐            │
       │            │    SLEEP    │────────────┘
       │            │  (deep low) │   Wake trigger
       │            └──────┬──────┘
       │                   │
       │           Long press (off)
       │                   │
       │            ┌──────▼──────┐
       └────────────┤  SHUTDOWN   │
                    │  (power off)│
                    └─────────────┘

            ┌─────────────┐
            │    ERROR    │ (recoverable → retry → IDLE)
            │             │ (fatal → SHUTDOWN)
            └─────────────┘
```

### 5.3 Task Scheduling (Super Loop)

```c
// Main loop pseudo-code
while (1) {
    // 1. Process USB communication (if connected)
    if (usb_connected) {
        comm_process();
    }
    
    // 2. Read sensors at configured interval
    if (timer_expired(sensor_timer)) {
        sensor_read_all();
        timer_restart(sensor_timer, sample_rate_ms);
    }
    
    // 3. Update UI (LEDs)
    ui_update();
    
    // 4. Check button
    button_process();
    
    // 5. Log data if enabled
    if (logging_enabled && new_data) {
        log_write_sample();
    }
    
    // 6. Enter low power if idle
    if (idle_timeout_expired) {
        enter_low_power_mode();
    }
}
```

### 5.4 Interrupt Priority

| IRQ | Priority | Handler | Purpose |
|-----|----------|---------|---------|
| SysTick | 0 | SysTick_Handler | 1ms tick |
| USB_LP | 1 | USB_LP_IRQHandler | USB events |
| EXTI0 | 2 | EXTI0_IRQHandler | Button press |
| I2C1_EV | 3 | I2C1_EV_IRQHandler | Sensor I2C |
| DMA1_CH | 4 | DMA1_Channel_IRQHandler | SPI DMA |
| TIM2 | 5 | TIM2_IRQHandler | General timing |

### 5.5 Memory Budget

| Region | Size | Used | Free | Purpose |
|--------|------|------|------|---------|
| Flash | 1024KB | ~64KB | ~960KB | Code + const |
| RAM | 128KB | ~16KB | ~112KB | Stack + heap + data |
| EEPROM | 0KB | - | - | (emulated in flash) |

---

## 6. Power Management

### 6.1 Power Modes

| Mode | MCU State | Current | Wake Sources | Entry |
|------|-----------|---------|--------------|-------|
| Active | Run 80MHz | ~15mA | N/A | Normal operation |
| LP Run | Run 2MHz | ~200µA | Any | Idle > 1s |
| Stop 1 | Stopped | ~5µA | Button, USB, RTC | Idle > 30s |
| Stop 2 | Stopped | ~1µA | Button, RTC | Idle > 5min |
| Shutdown | Off | ~30nA | Wakeup pin only | User power off |

### 6.2 Power Budget

**Active Mode (worst case):**
| Component | Current | Notes |
|-----------|---------|-------|
| MCU @ 80MHz | 8 mA | With peripherals |
| USB PHY | 5 mA | When connected |
| TMP117 | 3.5 µA | Continuous |
| SHT40 | 400 µA | During measure |
| Flash (active) | 15 mA | During write |
| LEDs (1 on) | 5 mA | 5mA each |
| **Total (typ)** | **~20 mA** | |
| **Total (max)** | **~35 mA** | |

**Sleep Mode:**
| Component | Current | Notes |
|-----------|---------|-------|
| MCU Stop 2 | 1 µA | |
| TMP117 shutdown | 150 nA | |
| SHT40 idle | 100 nA | |
| Flash standby | 1 µA | |
| LDO quiescent | 1 µA | |
| **Total** | **~4 µA** | |

### 6.3 Battery Life Estimation

**Battery: 500mAh Li-Po**

| Usage Pattern | Average Current | Runtime |
|---------------|-----------------|---------|
| Always active | 20 mA | 25 hours |
| 10% active / 90% sleep | ~3 mA | 7 days |
| 1% active / 99% sleep | ~200 µA | 3 months |
| Deep sleep only | 4 µA | 14 years* |

*Theoretical, limited by battery self-discharge

---

## 7. Testing

### 7.1 Unit Tests

| Module | Test | Method |
|--------|------|--------|


### 7.2 Integration Tests

| Test ID | Description | Pass Criteria |
|---------|-------------|---------------|
| IT-001 | USB enumeration | Appears as COM port |
| IT-002 | GET:TEMP command | Valid response ±5°C of room temp |
| IT-003 | GET:HUM command | Valid response 20-80% typical |
| IT-004 | SET:RATE | Rate changes, acknowledged |
| IT-005 | Button short press | Mode changes |
| IT-006 | Button long press | Power off |
| IT-007 | Low battery | Warning at 20%, shutdown at 5% |
| IT-008 | Charging | Indicator correct, charges to full |
| IT-009 | Data logging | Data stored, retrievable |
| IT-010 | Factory reset | Config cleared, defaults restored |

### 7.3 Environmental Tests

| Test | Condition | Duration | Pass Criteria |
|------|-----------|----------|---------------|
| Temperature | -20°C to +70°C | 2 hrs each | Functions correctly |
| Humidity | 90% RH | 24 hrs | No condensation issues |
| ESD | ±4kV contact | 10 discharges | No damage, recovers |
| Vibration | Random, 2G | 1 hr | No failures |

### 7.4 Production Tests (Automated)

| Step | Test | Limit | Time |
|------|------|-------|------|
| 1 | Program firmware | Verify checksum | 30s |
| 2 | Power on | Current < 50mA | 2s |
| 3 | LED test | All LEDs light | 1s |
| 4 | USB enum | Device detected | 5s |
| 5 | Temp sensor | 15°C < T < 35°C | 2s |
| 6 | Humidity sensor | 20% < RH < 80% | 2s |
| 7 | Button test | Press detected | Manual |
| 8 | Write serial # | EEPROM programmed | 2s |
| 9 | Final current | < 25mA active | 1s |
| | **Total** | | ~45s |

---

## 8. Manufacturing

### 8.1 Bill of Materials (Key Components)

| Ref | Qty | Part Number | Description | Supplier | Unit Cost |
|-----|-----|-------------|-------------|----------|-----------|

| | | | **BOM Total** | | **~$25** |

### 8.2 PCB Specifications

| Parameter | Specification |
|-----------|---------------|
| Layers | 4 |
| Dimensions | 40 × 30 mm |
| Thickness | 1.6 mm |
| Copper Weight | 1 oz outer, 0.5 oz inner |
| Surface Finish | ENIG |
| Solder Mask | Green |
| Silkscreen | White |
| Min Trace/Space | 0.15 mm / 0.15 mm |
| Min Via | 0.3 mm drill, 0.6 mm pad |

### 8.3 Assembly Notes

- All components are top-side only (except battery connector)
- Reflow solder, lead-free (SAC305)
- No-clean flux
- Components temperature rated for 260°C reflow
- Conformal coating optional for harsh environments

---

## 9. Regulatory Compliance

### 9.1 Required Certifications

| Certification | Region | Status |
|---------------|--------|--------|
| FCC Part 15 Class B | USA | Required |
| CE Mark | EU | Required |
| RoHS 3 | EU | Required |
| WEEE | EU | Required |
| IC | Canada | If sold in CA |
| MIC | Japan | If sold in JP |

### 9.2 Compliance Strategy

| Requirement | Implementation |
|-------------|----------------|
| EMC Emissions | Proper PCB layout, shielding if needed |
| ESD Protection | TVS diodes on USB, proper grounding |
| RoHS | Lead-free components only |
| Battery Safety | UN38.3 tested cells, protection circuit |

---

## 10. Documentation Deliverables

| Document | Format | Status |
|----------|--------|--------|
| Functional Specification | Markdown/PDF | This document |
| Hardware Design Spec | Markdown/PDF | Pending |
| Schematic | KiCad / PDF | Pending |
| PCB Layout | KiCad / Gerbers | Pending |
| BOM | CSV / Excel | Pending |
| Firmware Source | C, Git repo | Pending |
| User Manual | PDF | Pending |
| Test Report | PDF | Pending |

---

## Appendix A: Mechanical Drawing


---

## Appendix B: Wire Protocol (Binary Option)

For applications requiring higher throughput, binary protocol available:

| Byte | Field | Description |
|------|-------|-------------|
| 0 | SYNC | 0xAA |
| 1 | CMD | Command code |
| 2-3 | LEN | Payload length (little-endian) |
| 4..n | DATA | Payload |
| n+1-n+2 | CRC16 | CCITT CRC |

---

## Revision History

| Rev | Date | Author | Description |
|-----|------|--------|-------------|
| A | YYYY-MM-DD | | Initial release |
| | | | |
| | | | |

---

## Approval

| Role | Name | Signature | Date |
|------|------|-----------|------|
| Author | | | |
| Hardware Lead | | | |
| Firmware Lead | | | |
| Project Manager | | | |
