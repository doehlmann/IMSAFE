/**
    @file: radar_mcu4_bsp.c

    @brief: This file provides a set of firmware functions to manage all peripherals,
            LED, Button and current sensor available on Radar_MCU4 board from Infineon.
 */

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "DIGITAL_IO/digital_io.h"
#include "radar_mcu4_bsp.h"

/*
==============================================================================
   4. DATA
==============================================================================
 */

/* SCL IO: In case of software reset this pin will be used to generate 9 clock cycles */
const DIGITAL_IO_t DIGITAL_IO_EEPROM_SW_RESET =
{
  .gpio_port = XMC_GPIO_PORT4,
  .gpio_pin = 2U,
  .gpio_config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,

  },
  .hwctrl = XMC_GPIO_HWCTRL_DISABLED
};

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

/* Function to write data into EEPROM in case of write operation in multi-page, within more then one page */
/* ==> For many write operations we have to wait at least 5ms between each operation
       This is needed only for write operation, there is no need to have this delay for the read operation */
EEPROM_STATUS_t bsp_eeprom_24cw128x_write_data(const uint16_t mem_address, uint8_t *tx_data, uint32_t tx_size)
{
	EEPROM_STATUS_t status			= EEPROM_STATUS_SUCCESS;
	I2C_MASTER_STATUS_t i2c_status 	= I2C_MASTER_STATUS_FAILURE;
	uint16_t current_mem_address 	= mem_address;
	uint8_t *current_tx_address		= tx_data;
	uint32_t page_count		 		= (tx_size / EEPROM_24CW128X_PAGE_SIZE);
	uint32_t remaining_tx_size 		= (tx_size % EEPROM_24CW128X_PAGE_SIZE);
	uint32_t date_size_to_write		= 0;
	uint8_t mem_add[2];

	/* Read all pages */
	while(page_count >= 0)
	{
		/* calc how much data to read */
		if (page_count > 0)
		{
			date_size_to_write = EEPROM_24CW128X_PAGE_SIZE;
			page_count--;
		}
		else if(page_count == 0 && remaining_tx_size > 0)
		{
			date_size_to_write = remaining_tx_size ;
			remaining_tx_size = 0;
		}
		else if(page_count == 0 && remaining_tx_size == 0)
		{
			/* no data to write */
			break;
		}
		else
		{
			status = EEPROM_STATUS_LENGTH_MISMATCH;
			break;
		}

		/* sanity check */
		if ((current_mem_address >= EEPROM_24CW128X_START_MEM_ADDR) &&
		((current_mem_address + date_size_to_write) < EEPROM_24CW128X_MAX_SIZE))
		{
			/* EEPROM start read address */
			mem_add[0] = (current_mem_address & 0x3F00) >> 8;
			mem_add[1] = current_mem_address & 0x00FF;

			/* Send memory address */
			i2c_status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
			while (I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

			if (i2c_status == I2C_MASTER_STATUS_SUCCESS)
			{
				/* Start reading data */
				i2c_status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, current_tx_address, date_size_to_write, true);
				while (I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));
			}

			if (i2c_status != I2C_MASTER_STATUS_SUCCESS)
			{
				status = EEPROM_STATUS_WRITE_ERROR;
				break;
			}
			else
			{
				current_mem_address += date_size_to_write;
				current_tx_address 	+= date_size_to_write;

				bsp_eeprom_time_delay(6, BSP_TIMER_DELAY_MILLISECOND);
			}
		}
		else
		{
			status = EEPROM_STATUS_LENGTH_MISMATCH;
		}
	}

	return status;
}

//============================================================================

/* Function to read data from EEPROM in case of read operation in multi-page, within more then one page */
EEPROM_STATUS_t bsp_eeprom_24cw128x_read_data(const uint16_t mem_address, uint8_t *rx_data, uint32_t rx_size)
{
	EEPROM_STATUS_t status			= EEPROM_STATUS_SUCCESS;
	I2C_MASTER_STATUS_t i2c_status 	= I2C_MASTER_STATUS_FAILURE;
	uint16_t current_mem_address 	= mem_address;
	uint8_t *current_rx_address		= rx_data;
	uint32_t page_count		 		= rx_size / EEPROM_24CW128X_PAGE_SIZE;
	uint32_t remaining_rx_size 		= rx_size % EEPROM_24CW128X_PAGE_SIZE;
	uint32_t date_size_to_read 		= 0;
	uint8_t mem_add[2];

	/* Read all pages */
	while(page_count >= 0)
	{
		/* calc how much data to read*/
		if (page_count > 0)
		{
			date_size_to_read = EEPROM_24CW128X_PAGE_SIZE;
			page_count--;
		}
		else if(page_count == 0 && remaining_rx_size > 0)
		{
			date_size_to_read = remaining_rx_size;
			remaining_rx_size = 0;
		}
		else if(page_count == 0 && remaining_rx_size == 0)
		{
			/* no data to read */
			break;
		}
		else
		{
			status = EEPROM_STATUS_LENGTH_MISMATCH;
			break;
		}

		/* sanity check*/
		if ((current_mem_address >= EEPROM_24CW128X_START_MEM_ADDR) &&
		((current_mem_address + date_size_to_read) < EEPROM_24CW128X_MAX_SIZE))
		{
			/* EEPROM start read address */
			mem_add[0] = (current_mem_address & 0x3F00) >> 8;
			mem_add[1] = current_mem_address & 0x00FF;

			/* Send memory address */
			i2c_status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
			while (I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

			if (i2c_status == I2C_MASTER_STATUS_SUCCESS)
			{
				/* Start reading data */
				i2c_status = I2C_MASTER_Receive(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, current_rx_address, date_size_to_read, true, true);
				while (I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));
			}

			if (i2c_status != I2C_MASTER_STATUS_SUCCESS)
			{
				status = EEPROM_STATUS_READ_ERROR;
				break;
			}
			else
			{
				/*if i2c read successful, increase the address to the amount of data that has been read */
				current_mem_address += date_size_to_read;
				current_rx_address 	+= date_size_to_read;
			}
		}
		else
		{
			status = EEPROM_STATUS_LENGTH_MISMATCH;
		}
	}

	return status;
}

//============================================================================

/* Function to perform a software rest in case of I2C communication error */
EEPROM_STATUS_t bsp_eeprom_24cw128x_software_reset(void)
{
  DAVE_STATUS_t init_status = DAVE_STATUS_SUCCESS;
  EEPROM_STATUS_t status = EEPROM_STATUS_FAILURE;

  /**  Re-initialization SCL pin as output to perform the Software Reset */
  init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_EEPROM_SW_RESET);

  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_EEPROM_SW_RESET);
  if(init_status == DAVE_STATUS_SUCCESS)
  {
    /* Generate 9 clock cycles on SCL pin to perform a software reset */
    for(uint32_t index = 0; index < 9; index++)
    {
       DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_EEPROM_SW_RESET);
       bsp_eeprom_time_delay(1, BSP_TIMER_DELAY_MICROSECOND);
       DIGITAL_IO_SetOutputLow(&DIGITAL_IO_EEPROM_SW_RESET);
       bsp_eeprom_time_delay(1, BSP_TIMER_DELAY_MICROSECOND);
    }

    /* Re-initialization the I2C_MASTER APP instance I2C_MASTER_EEPROM */
	init_status = (DAVE_STATUS_t)I2C_MASTER_Init(&I2C_MASTER_EEPROM);

	/* Software Reset successfully performed */
	if (init_status == DAVE_STATUS_SUCCESS)
	{
	  status = EEPROM_STATUS_SUCCESS;
    }
  }

  return (status);
}

//============================================================================

/* Function used to generate a time in units of microseconds or milliseconds */
void bsp_eeprom_time_delay(uint32_t delay_value, BSP_TIMER_DELAY_t time_unit)
{
  uint32_t delay_cnt;

  TIMER_ClearEvent(&TIMER_EEPROM_DELAY);

  if(time_unit == BSP_TIMER_DELAY_MILLISECOND)
  {
	/* Time in units of milliseconds */
	delay_cnt = (1000U * delay_value) * 100U;
  }
  else
  {
	/* Time in units of microseconds, according to documentation
	 * e.g. : required timer interval value = 30.45 micro second
	 * Input value to the API = 30.45 * 100 = 3045*/
    delay_cnt = delay_value * 100U;
  }

  TIMER_SetTimeInterval(&TIMER_EEPROM_DELAY, delay_cnt);

  TIMER_Start(&TIMER_EEPROM_DELAY);

  while(!TIMER_GetInterruptStatus(&TIMER_EEPROM_DELAY));

  TIMER_Stop(&TIMER_EEPROM_DELAY);

  TIMER_Clear(&TIMER_EEPROM_DELAY);
}

#if !defined(BUILD_FOR_ARDUINO)

//============================================================================

void bsp_led_green_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_GREEN_LED);
}

//============================================================================

void bsp_led_green_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_GREEN_LED);
}

//============================================================================

void bsp_led_green_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_GREEN_LED);
}
//============================================================================

void bsp_led_blue_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_blue_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_blue_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_red_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_RED_LED);
}

//============================================================================

void bsp_led_red_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_RED_LED);
}

//============================================================================

void bsp_led_red_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_RED_LED);
}
#endif

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
*/

/* --- End of File -------------------------------------------------------- */
