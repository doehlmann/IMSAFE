# STM32 Peripheral Patterns

## Overview

Common peripheral initialization and usage patterns for STM32 using HAL/LL libraries with STM32CubeIDE.

---

## GPIO

### Basic Output

```c
// CubeMX: Configure pin as GPIO_Output

// Toggle LED
HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

// Set output high
HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

// Set output low
HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
```

### Basic Input

```c
// CubeMX: Configure pin as GPIO_Input (with pull-up/down as needed)

// Read input state
GPIO_PinState state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

if (state == GPIO_PIN_SET) {
    // Pin is high
}
```

### External Interrupt (EXTI)

```c
// CubeMX: Configure pin as GPIO_EXTI with rising/falling edge

// In stm32xxxx_it.c, the HAL_GPIO_EXTI_Callback is called
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BUTTON_Pin) {
        // Handle button press
        buttonPressed = 1;
    }
}

// In main.c, process the flag
if (buttonPressed) {
    buttonPressed = 0;
    // Do something
}
```

---

## UART/USART

### Polling Mode

```c
// CubeMX: Enable USART, set baud rate

uint8_t txData[] = "Hello\r\n";
uint8_t rxData[10];

// Transmit
HAL_UART_Transmit(&huart1, txData, sizeof(txData)-1, HAL_MAX_DELAY);

// Receive (blocking)
HAL_UART_Receive(&huart1, rxData, 10, 1000);  // 1000ms timeout
```

### Interrupt Mode

```c
// CubeMX: Enable USART global interrupt

uint8_t rxByte;

// Start receiving (in main, after init)
HAL_UART_Receive_IT(&huart1, &rxByte, 1);

// Callback (called when byte received)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        // Process rxByte
        ProcessByte(rxByte);

        // Restart reception
        HAL_UART_Receive_IT(&huart1, &rxByte, 1);
    }
}
```

### DMA Mode (Circular Buffer)

```c
// CubeMX: Enable USART with DMA RX, set DMA to Circular mode

#define RX_BUFFER_SIZE 64
uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxReadIndex = 0;

// Start DMA reception (once, in main after init)
HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);

// Process received data (call in main loop)
void ProcessUartData(void) {
    uint16_t writeIndex = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

    while (rxReadIndex != writeIndex) {
        uint8_t byte = rxBuffer[rxReadIndex];
        // Process byte

        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
    }
}
```

### Printf Redirect

```c
// Add to main.c or syscalls.c
#include <stdio.h>

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

// Usage
printf("Value: %d\r\n", value);
```

---

## SPI

### Basic Transfer

```c
// CubeMX: Configure SPI, set mode, clock, etc.

uint8_t txData[4] = {0x01, 0x02, 0x03, 0x04};
uint8_t rxData[4];

// Select device (CS low)
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

// Transmit and receive
HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 4, HAL_MAX_DELAY);

// Deselect device (CS high)
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
```

### Read Register Pattern

```c
uint8_t SPI_ReadRegister(uint8_t reg) {
    uint8_t txData[2] = {reg | 0x80, 0x00};  // Read flag often 0x80
    uint8_t rxData[2];

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    return rxData[1];
}

void SPI_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t txData[2] = {reg & 0x7F, value};  // Write flag often clear MSB

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, txData, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}
```

---

## I2C

### Basic Read/Write

```c
// CubeMX: Configure I2C, set speed (100kHz or 400kHz)

#define DEVICE_ADDR  (0x50 << 1)  // 7-bit address shifted

uint8_t data[4];

// Write data to device
HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDR, data, 4, HAL_MAX_DELAY);

// Read data from device
HAL_I2C_Master_Receive(&hi2c1, DEVICE_ADDR, data, 4, HAL_MAX_DELAY);
```

### Register Read/Write

```c
uint8_t I2C_ReadRegister(uint8_t devAddr, uint8_t reg) {
    uint8_t value;

    // Write register address, then read value
    HAL_I2C_Mem_Read(&hi2c1, devAddr << 1, reg, I2C_MEMADD_SIZE_8BIT,
                     &value, 1, HAL_MAX_DELAY);
    return value;
}

void I2C_WriteRegister(uint8_t devAddr, uint8_t reg, uint8_t value) {
    HAL_I2C_Mem_Write(&hi2c1, devAddr << 1, reg, I2C_MEMADD_SIZE_8BIT,
                      &value, 1, HAL_MAX_DELAY);
}

// Read multiple bytes
void I2C_ReadRegisters(uint8_t devAddr, uint8_t startReg,
                       uint8_t *buffer, uint16_t len) {
    HAL_I2C_Mem_Read(&hi2c1, devAddr << 1, startReg, I2C_MEMADD_SIZE_8BIT,
                     buffer, len, HAL_MAX_DELAY);
}
```

### I2C Scanner

```c
void I2C_Scan(void) {
    printf("Scanning I2C bus...\r\n");

    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) == HAL_OK) {
            printf("Found device at 0x%02X\r\n", addr);
        }
    }

    printf("Scan complete.\r\n");
}
```

---

## ADC

### Single Conversion (Polling)

```c
// CubeMX: Configure ADC channel

uint32_t adcValue;

// Start conversion
HAL_ADC_Start(&hadc1);

// Wait for conversion
HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

// Read value
adcValue = HAL_ADC_GetValue(&hadc1);

// Stop ADC
HAL_ADC_Stop(&hadc1);

// Convert to voltage (assuming 3.3V reference, 12-bit ADC)
float voltage = (adcValue * 3.3f) / 4096.0f;
```

### Continuous with DMA

```c
// CubeMX: Configure ADC with DMA, Continuous Conversion, multiple channels

#define NUM_CHANNELS 4
uint16_t adcBuffer[NUM_CHANNELS];

// Start ADC with DMA (once, after init)
HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, NUM_CHANNELS);

// Read values anytime (DMA updates buffer continuously)
uint16_t ch0 = adcBuffer[0];
uint16_t ch1 = adcBuffer[1];
// etc.
```

---

## Timers

### Basic Delay Timer

```c
// CubeMX: Configure timer with prescaler and period for desired timebase

// Start timer
HAL_TIM_Base_Start(&htim2);

// Get counter value
uint32_t count = __HAL_TIM_GET_COUNTER(&htim2);

// Simple delay (not recommended for production)
__HAL_TIM_SET_COUNTER(&htim2, 0);
while (__HAL_TIM_GET_COUNTER(&htim2) < 1000);  // Wait for count
```

### Timer Interrupt (Periodic)

```c
// CubeMX: Configure timer with interrupt, set period for desired interval
// Example: 84MHz clock, Prescaler=8400-1, Period=10000-1 → 1 second

// Start timer interrupt (after init)
HAL_TIM_Base_Start_IT(&htim2);

// Callback (called every period)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        // Called every 1 second
        secondCounter++;
    }
}
```

### PWM Output

```c
// CubeMX: Configure timer channel as PWM Generation

// Start PWM
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

// Set duty cycle (0 to Period)
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500);  // 50% if period=1000

// Change duty cycle dynamically
void SetPWMDuty(uint16_t duty) {
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
}

// Percent-based
void SetPWMPercent(float percent) {
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim3);
    uint32_t compare = (uint32_t)(percent * period / 100.0f);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, compare);
}
```

### Input Capture (Frequency Measurement)

```c
// CubeMX: Configure timer channel as Input Capture

volatile uint32_t icValue1 = 0;
volatile uint32_t icValue2 = 0;
volatile uint32_t frequency = 0;
volatile uint8_t captureState = 0;

// Start input capture interrupt
HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        if (captureState == 0) {
            icValue1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            captureState = 1;
        } else {
            icValue2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            uint32_t diff;
            if (icValue2 > icValue1) {
                diff = icValue2 - icValue1;
            } else {
                diff = (0xFFFF - icValue1) + icValue2;
            }

            // frequency = timer_clock / diff
            frequency = HAL_RCC_GetPCLK1Freq() / diff;
            captureState = 0;
        }
    }
}
```

---

## Watchdog

### Independent Watchdog (IWDG)

```c
// CubeMX: Enable IWDG, set prescaler and reload for desired timeout

// IWDG starts automatically after init (if configured in CubeMX)

// Refresh watchdog in main loop
while (1) {
    // Application code

    HAL_IWDG_Refresh(&hiwdg);  // Must call before timeout
}
```

---

## Low Power Modes

### Sleep Mode

```c
// Enter sleep mode (wake on any interrupt)
HAL_SuspendTick();
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
HAL_ResumeTick();
```

### Stop Mode

```c
// Enter stop mode (wake on EXTI)
HAL_SuspendTick();
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
// After wake, reconfigure system clock
SystemClock_Config();
HAL_ResumeTick();
```

---

## Common Patterns

### Non-blocking State Machine

```c
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_COMPLETE,
    STATE_ERROR
} State_t;

State_t state = STATE_IDLE;
uint32_t stateTimer = 0;

void StateMachine_Update(void) {
    switch (state) {
        case STATE_IDLE:
            if (startCondition) {
                state = STATE_RUNNING;
                stateTimer = HAL_GetTick();
            }
            break;

        case STATE_RUNNING:
            if (HAL_GetTick() - stateTimer > 1000) {
                state = STATE_COMPLETE;
            }
            break;

        case STATE_COMPLETE:
            // Handle completion
            state = STATE_IDLE;
            break;

        case STATE_ERROR:
            // Handle error
            break;
    }
}
```

### Software Timer

```c
typedef struct {
    uint32_t startTime;
    uint32_t interval;
    uint8_t running;
} SoftTimer_t;

void SoftTimer_Start(SoftTimer_t *timer, uint32_t interval) {
    timer->startTime = HAL_GetTick();
    timer->interval = interval;
    timer->running = 1;
}

uint8_t SoftTimer_Elapsed(SoftTimer_t *timer) {
    if (!timer->running) return 0;
    return (HAL_GetTick() - timer->startTime) >= timer->interval;
}

// Usage
SoftTimer_t ledTimer;
SoftTimer_Start(&ledTimer, 500);

if (SoftTimer_Elapsed(&ledTimer)) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    SoftTimer_Start(&ledTimer, 500);  // Restart
}
```
