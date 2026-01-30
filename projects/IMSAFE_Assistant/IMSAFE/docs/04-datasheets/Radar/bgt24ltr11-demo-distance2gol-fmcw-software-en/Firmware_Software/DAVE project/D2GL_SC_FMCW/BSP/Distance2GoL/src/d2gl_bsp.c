/**
    @file: d2gl_bsp.c

    @brief: This file provides a set of firmware functions to manage BGT24LTR11
            on Distance2GoL board from Infineon.
 */

/* ===========================================================================
 ** Copyright (C) 2021 Infineon Technologies AG
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

#include "d2gl_bsp.h"
#include "d2gl_bsp_limits.h"
#include "d2gl_sc_fmcw.h"
#include "radar_api.h"
#include "radar_control.h"
#include "datastore.h"
#include "config.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

#define MIN_NORMAL 1.17549435E-38
#define MAX_VALUE  1.4E-45

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */


/**
 * @brief Used to store the DMA source addresses.
 * This symbols needs to be global, to work together with the DAVE generated functions, where they are referenced.
 */
uint32_t* pDMA_src_G0 = NULL;
uint32_t* pDMA_src_G1 = NULL;
uint32_t* pDMA_src_G2 = NULL;
uint32_t* pDMA_src_G3 = NULL;

/**
 * @brief Used to store the DMA destination addresses.
 * This symbols needs to be global, to work together with the DAVE generated functions, where they are referenced.
 */
uint32_t* pDMA_dst_G0 = NULL;
uint32_t* pDMA_dst_G1 = NULL;
uint32_t* pDMA_dst_G2 = NULL;
uint32_t* pDMA_dst_G3 = NULL;

static uint16_t data_buffer[4][BSP_MAX_NUM_SAMPLES_PER_CHIRP];

/**
 * @brief The frame count with current frame interval.
 */
volatile uint32_t bsp_frame_counter = 0;

/**
 * @brief Global structure to store all data acquisition functions callback \ref Bsp_callback_Struct_t.
 */
static Bsp_callback_Struct_t bsp_cb =
{
	.bsp_acq_started_cb = NULL,
	.bsp_acq_done_cb = NULL
};

/**
 * @brief  The current chirp counter.
 */
static uint32_t chirp_count = 0;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * @brief:  This function enables the DMA to get further DMA data transactions.
 *         Need to call after reloading the DMA configurations.
 */
static void dma_enable(void);

/**
 * @brief:  This function disables the DMA to avoid further DMA data transactions.
 *	   Need to call before reloading the DMA configurations or when data acquisition is finished.
 */
static void dma_disable(void);

/**
 * @brief:  This function resets the DMA channel DLR to kill the last over sampled sample.
 */
static void dma_reset_dlr(void);

/**
 * @brief:  This function assigns memory sections to the source pointers defined in the DMA app.
 */
static void dma_set_source_addr(void);

static void trigger_dma_start(void);

static void bgt24ltr1x_power_up(void);

static void bgt24ltr1x_power_down(void);

static void bgt24ltr1x_div_power_up(void);

static void bgt24ltr1x_div_power_down(void);

static void bgt24ltr1x_ptat_power_up(void);

static void bgt24ltr1x_ptat_power_down(void);

static void bgt24ltr1x_start_tx(void);

static void bgt24ltr1x_stop_tx(void);

static void bgt24ltr1x_bb_power_up(void);

static void bgt24ltr1x_bb_power_down(void);

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

void bsp_limit_dev_settings(device_settings_t *p_dev)
{
	if(p_dev->num_of_chirps_per_frame < BSP_MIN_NUM_OF_CHIRPS)
	   p_dev->num_of_chirps_per_frame = BSP_MIN_NUM_OF_CHIRPS;

	if(p_dev->num_of_chirps_per_frame > BSP_MAX_NUM_CHIRPS_PER_FRAME)
	   p_dev->num_of_chirps_per_frame = BSP_MAX_NUM_CHIRPS_PER_FRAME;

	if(p_dev->num_samples_per_chirp < BSP_MIN_NUM_SAMPLES_PER_CHIRP)
	   p_dev->num_samples_per_chirp = BSP_MIN_NUM_SAMPLES_PER_CHIRP;

	if(p_dev->num_samples_per_chirp > BSP_MAX_NUM_SAMPLES_PER_CHIRP)
	   p_dev->num_samples_per_chirp = BSP_MAX_NUM_SAMPLES_PER_CHIRP;

	if(p_dev->adc_resolution != 12)
	   p_dev->adc_resolution = 12;

	if( p_dev->num_of_chirps_per_frame > 1)
	{
		float bsp_min_prt = ((float) BSP_MIN_PRT_USEC)*1e-6f;
		float bsp_max_prt = ((float) BSP_MAX_PRT_USEC)*1e-6;

		if( !nearlyEqual(p_dev->pulse_repetition_time_sec, bsp_min_prt, 0.01 ))
		{
			if( p_dev->pulse_repetition_time_sec < bsp_min_prt )
				p_dev->pulse_repetition_time_sec = bsp_min_prt;
		}

		if( !nearlyEqual(p_dev->pulse_repetition_time_sec, bsp_max_prt, 0.01 ))
		{
			if( p_dev->pulse_repetition_time_sec > bsp_max_prt )
				p_dev->pulse_repetition_time_sec = bsp_max_prt;
		}
	}
	/* else for a single chirp, the PRT is meaningless */

	/* restrict minimum frame period to device capabilities */
	uint32_t temp_frame_period 	= bsp_calc_min_frame_interval_usec(p_dev);
	if( p_dev->frame_period_usec < temp_frame_period )
		p_dev->frame_period_usec = temp_frame_period;
}

//============================================================================

uint8_t bsp_init(device_settings_t *p_dev)
{
	uint8_t retValue = 0;
	/* Set all data acquisition functions callback to NULL */
	bsp_cb.bsp_acq_started_cb = NULL;
	bsp_cb.bsp_acq_done_cb = NULL;

	/* Disable DMA */
	dma_disable();

	/* Reset the current frame counter */
	bsp_reset_frame_counter();

	/* Initialize software controlled FMCW module */
	if( scfmcw_init( p_dev ) )
		retValue += 1;

	uint32_t temp_frame_period_usec = bsp_calc_min_frame_interval_usec(p_dev);
	if( p_dev->frame_period_usec < temp_frame_period_usec)
	{
		 retValue += 2;
	}
	return retValue;
}

//============================================================================

uint8_t bsp_update(device_settings_t *p_dev)
{

	/* Disable DMA */
	dma_disable();

	scfmcw_stop();

    /* Initialize Software controlled FMCW Modulation, with new parameters. */
	if( scfmcw_update_settings(p_dev) )
		return 1;

	return 0;
}

//============================================================================

/* Interrupt to trigger start of frame */
void triggerFrameISR(void)
{
	bsp_trigger_new_frame();
}

//============================================================================

void bsp_trigger_new_frame(void)
{
	/********** TIMING DEBUG - Frame Start *************/

	/* Get the raw ADC data */
	/* DMA Reconfiguration */
	dma_disable();

	acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();
	p_acq_buf->sysTimer_AccStart = SYSTIMER_GetTime();
	bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);

	/********** TIMING DEBUG - DMA Start *************/

	trigger_dma_start();

	/* Start the FMCW chirps */
	scfmcw_trigger_chirps();

	/* This callback is used to inform upper layers about the beginning of data acquisition process */
	if(bsp_cb.bsp_acq_started_cb != NULL)
	{

		bsp_cb.bsp_acq_started_cb(NULL);
	}
}

//============================================================================

void copyDataBuffer2AcqBuffer(acq_buf_obj *p_acq_buf, uint32_t chirpIndex)
{
	uint16_t *p_dst_Ch0 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 0, chirpIndex);
	uint16_t *p_dst_Ch1 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 1, chirpIndex);
	uint16_t *p_dst_Ch2 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 2, chirpIndex);
	uint16_t *p_dst_Ch3 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 3, chirpIndex);

	uint32_t skip_count = p_acq_buf->internals.skip_count;
	uint32_t byte_count = p_acq_buf->params.num_of_samples_per_chirp*sizeof(uint16_t);

	memcpy(p_dst_Ch0, &data_buffer[0][skip_count], byte_count);
	memcpy(p_dst_Ch1, &data_buffer[1][skip_count], byte_count);
	memcpy(p_dst_Ch2, &data_buffer[2][skip_count], byte_count);
	memcpy(p_dst_Ch3, &data_buffer[3][skip_count], byte_count);

	p_acq_buf->frame_counter = bsp_frame_counter;
}

//============================================================================

void DMA_ADC_Event_Handler(XMC_DMA_CH_EVENT_t dma_event)
{
	if((dma_event & XMC_DMA_CH_EVENT_BLOCK_TRANSFER_COMPLETE) != 0)
	{
		/* Once DMA issue interrupts, we'd better stop the ADC Trigger Timer,
		 * we don't need to care which interrupts are triggered by DMA.
		 * this timer is source peripheral's trigger to request DMA transaction.
		 * Even though DMA error interrupt, we'd better stop the this timer. */
		scfmcw_stop_adc_sampling();

		acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

		/* copy data buffer into target buffer, if skip count is active*/
		copyDataBuffer2AcqBuffer(p_acq_buf, chirp_count);

		chirp_count++;

		dma_disable();

		/* Clear the request from source peripheral even though DMA was disabled by itself.
		 * We should consider the pending request from external peripheral. */
		dma_reset_dlr();

		if (chirp_count < p_acq_buf->params.num_of_chirps_per_frame)
		{
			/* DMA Reconfiguration, do we really need it? */
			bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);

			/* Enable DMA */
			dma_enable();
		}
		else
		{
			bsp_frame_counter++;

			p_acq_buf->frames_since_calibration = scfmcw_get_frames_since_calibration();

			/********** TIMING DEBUG - DMA END *************/
			p_acq_buf->sysTimer_AccStop = SYSTIMER_GetTime();
			p_acq_buf->frame_counter = bsp_frame_counter;
			chirp_count = 0;

			g_adc_sampling_completed = true;

			/* This callback is used to inform upper layers that a valid data is available and ADC sampling is completed */
			if(bsp_cb.bsp_acq_done_cb != NULL)
			{
				bsp_cb.bsp_acq_done_cb(NULL);
			}
		}
	}
}

//============================================================================

TIMER_STATUS_t bsp_timer_stop_clear(TIMER_t* const handle_timer)
{
	TIMER_STATUS_t  status;

	status = TIMER_Stop(handle_timer);

	status |= TIMER_Clear(handle_timer);

	return (status);
}

//============================================================================

TIMER_STATUS_t bsp_timer_start(TIMER_t* const handle_timer)
{
	TIMER_STATUS_t  status;

	status = TIMER_Start(handle_timer);

	return (status);
}

//============================================================================

void bsp_time_delay_usec(uint32_t delay_val_usec)
{
	uint32_t delay_cnt;

	TIMER_ClearEvent(&TIMER_DELAY);

	/* time in in units of microseconds */
	delay_cnt = delay_val_usec * 100U;

	TIMER_SetTimeInterval(&TIMER_DELAY,delay_cnt);

	bsp_timer_start(&TIMER_DELAY);

	while(!TIMER_GetInterruptStatus(&TIMER_DELAY));

	TIMER_Stop(&TIMER_DELAY);

	TIMER_Clear(&TIMER_DELAY);
}

//============================================================================

void bsp_dma_set_adc_control_from_acq_buf_obj(acq_buf_obj *acq_buf)
{
	dma_set_source_addr();

	/* We use a 2nd buffer here, to be able to run the signal processing in parallel to the data acquisition */
	pDMA_dst_G0 = (uint32_t*)&data_buffer[0][0];
	pDMA_dst_G1 = (uint32_t*)&data_buffer[1][0];
	pDMA_dst_G2 = (uint32_t*)&data_buffer[2][0];
	pDMA_dst_G3 = (uint32_t*)&data_buffer[3][0];

	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num, (uint32_t) pDMA_dst_G0);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num, (uint32_t) pDMA_dst_G1);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num, (uint32_t) pDMA_dst_G2);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num, (uint32_t) pDMA_dst_G3);

	uint32_t block_size = acq_buf->internals.skip_count + acq_buf->params.num_of_samples_per_chirp;
	DMA_CH_SetBlockSize(&DMA_CH_G0, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G1, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G2, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G3, block_size);
}

//============================================================================

uint8_t bsp_usb_init(uint32_t timeout_delay)
{
	uint32_t time_out_cnt = 0;

	if (USBD_VCOM_Connect() != USBD_VCOM_STATUS_SUCCESS)
	{
		return (1); /* Fail */
	}

	while (time_out_cnt < timeout_delay)
	{
		if (USBD_VCOM_IsEnumDone())
		{
			return (0); /* Success */
		}

		time_out_cnt++;
	}

	return (1); /* Fail */
}

//============================================================================

float32_t bsp_bgt_get_temp_value(void)
{
	/* It is not supported in BGT24LTR11 */
	return 0;
}

//============================================================================

uint32_t bsp_get_frame_counter(void)
{
	return (bsp_frame_counter);
}

//============================================================================

void bsp_reset_frame_counter(void)
{
	bsp_frame_counter = 0;
}

//============================================================================

float bsp_get_vptat_voltage(void)
{
	uint32_t  vptat_value;

	/* Read the voltage on V_PTAT pin */
	ADC_MEASUREMENT_ADV_SoftwareTrigger(&ADC_MEASUREMENT_SCAN);
	vptat_value = ((ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_SCAN_BGT_V_PTAT_handle)) & 0xFFF);

	return (vptat_value * 3300.0f / 4095.0f);
}

//============================================================================

uint16_t bsp_get_vptat_voltage_u16(void)
{
	uint32_t  vptat_value;

	/* Read the voltage on V_PTAT pin */
	ADC_MEASUREMENT_ADV_SoftwareTrigger(&ADC_MEASUREMENT_SCAN);
	vptat_value = (ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_SCAN_BGT_V_PTAT_handle));

	return (uint16_t) (vptat_value & 0xFFFF);
}

//============================================================================

uint8_t bsp_register_callback(Bsp_Callback_ID_t cb_id, void (*pcallback)(void *parameter))
{
	uint8_t status = 0;

	/* Check if he callback function is valid */
	if(pcallback == NULL)
	{
		return(1);
	}

	/* Register user callback function based on callback ID */
	switch (cb_id)
	{
	case BSP_ACQUISITION_STARTED_CB_ID:
		bsp_cb.bsp_acq_started_cb = pcallback;
		break;

	case BSP_ACQUISITION_DONE_CB_ID:
		bsp_cb.bsp_acq_done_cb = pcallback;
		break;

	default:
		/* Return error status */
		status =  1;
		break;
	}

	return (status);
}

//============================================================================

uint8_t bsp_unregister_callback(Bsp_Callback_ID_t cb_id)
{
	uint8_t status = 0;

	/* Unregister user callback function based on callback ID */
	switch (cb_id)
	{
	case BSP_ACQUISITION_STARTED_CB_ID:
		bsp_cb.bsp_acq_started_cb = NULL;
		break;

	case BSP_ACQUISITION_DONE_CB_ID:
		bsp_cb.bsp_acq_done_cb = NULL;
		break;

	default:
		/* Return error status */
		status =  1;
		break;
	}

	return (status);
}

//============================================================================

uint32_t bsp_calc_min_frame_interval_usec(device_settings_t *p_dev)
{

	float min_frame_period_s = scfmcw_get_min_frame_period_s(p_dev);

	uint32_t min_frame_interval_usec = (uint32_t) (min_frame_period_s * 1000000.0); // units in us

	return min_frame_interval_usec;
}

//============================================================================

void bsp_bgt_start_up(void)
{
	/* Power-up BGT */
	bsp_bgt_power_up();

	/* Delay for Tx ON */
	bsp_time_delay_usec(1);

	/* Start BGT Tx */
	bsp_bgt_start_tx();
}

//============================================================================

void bsp_bgt_shut_down(void)
{
	/* Stop BGT Tx */
	bsp_bgt_stop_tx();

	/* Delay for Tx OFF */
	bsp_time_delay_usec(1);

	/* Power-down BGT */
	bsp_bgt_power_down();
}

//============================================================================

void bsp_bgt_div_start_up(void)
{
	/* Enable VCC_PTAT to set divider of 8192 */
	bsp_bgt_ptat_power_up();

	/* Delay for VCC_PTAT ON */
	bsp_time_delay_usec(1);

	/* Enable divider output */
	bsp_bgt_div_power_up();
}

//============================================================================

void bsp_bgt_div_shut_down(void)
{
	/* Disable divider output */
	bsp_bgt_div_power_down();

	/* Disable VCC_PTAT */
	bsp_bgt_ptat_power_down();
}

//============================================================================

void bsp_bb_power_up(void)
{
	bgt24ltr1x_bb_power_up();
}

//============================================================================

void bsp_bb_power_down(void)
{
	bgt24ltr1x_bb_power_down();
}

//============================================================================

void bsp_bgt_power_up(void)
{
	bgt24ltr1x_power_up();
}

//============================================================================

void bsp_bgt_power_down(void)
{
	bgt24ltr1x_power_down();
}

//============================================================================

void bsp_bgt_div_power_up(void)
{
	bgt24ltr1x_div_power_up();
}

//============================================================================

void bsp_bgt_div_power_down(void)
{
	bgt24ltr1x_div_power_down();
}

//============================================================================

void bsp_bgt_ptat_power_up(void)
{
	bgt24ltr1x_ptat_power_up();
}

//============================================================================

void bsp_bgt_ptat_power_down(void)
{
	bgt24ltr1x_ptat_power_down();
}

//============================================================================

void bsp_bgt_start_tx(void)
{
	bgt24ltr1x_start_tx();
}

//============================================================================

void bsp_bgt_stop_tx(void)
{
	bgt24ltr1x_stop_tx();
}

//============================================================================

bool nearlyEqual(float a, float b, float epsilon)
{
	float absA = abs(a);
	float absB = abs(b);
	float diff = abs(a - b);

	if (a == b) { // shortcut, handles infinities
		return true;
	} else if (a == 0 || b == 0 || (absA + absB < MIN_NORMAL)) {
		// a or b is zero or both are extremely close to it
		// relative error is less meaningful here
		return diff < (epsilon * MIN_NORMAL);
	} else { // use relative error
		return diff / MIN((absA + absB), MAX_VALUE) < epsilon;
	}
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

static void trigger_dma_start(void)
{
	/* DMA Reconfiguration */
	dma_disable();

	acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

	bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);

	/* Enable DMA */
	dma_enable();
}

//============================================================================

static void dma_enable(void)
{
	DMA_CH_Enable(&DMA_CH_G0);
	DMA_CH_Enable(&DMA_CH_G1);
	DMA_CH_Enable(&DMA_CH_G2);
	DMA_CH_Enable(&DMA_CH_G3);
}

//============================================================================

static void dma_disable(void)
{
	DMA_CH_Disable(&DMA_CH_G0);
	DMA_CH_Disable(&DMA_CH_G1);
	DMA_CH_Disable(&DMA_CH_G2);
	DMA_CH_Disable(&DMA_CH_G3);
}

//============================================================================

static void dma_reset_dlr(void)
{
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num);
}

//============================================================================

static void dma_set_source_addr(void)
{
	/* DMA Source/Destination Address setup */
	pDMA_src_G0 = (uint32_t*)&ADC_MEASUREMENT_ADV_G0_CH_G0_RES;
	pDMA_src_G1 = (uint32_t*)&ADC_MEASUREMENT_ADV_G1_CH_G1_RES;
	pDMA_src_G2 = (uint32_t*)&ADC_MEASUREMENT_ADV_G2_CH_G2_RES;
	pDMA_src_G3 = (uint32_t*)&ADC_MEASUREMENT_ADV_G3_CH_G3_RES;

	/* Set the source address pointers for each DMA transfer */
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num, (uint32_t)pDMA_src_G0);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num, (uint32_t)pDMA_src_G1);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num, (uint32_t)pDMA_src_G2);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num, (uint32_t)pDMA_src_G3);
}

//============================================================================

static void bgt24ltr1x_power_up(void)
{
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_BGT_EN);
}

//============================================================================

static void bgt24ltr1x_power_down(void)
{
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_BGT_EN);
}

//============================================================================

static void bgt24ltr1x_div_power_up(void)
{
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_DIV_EN);
}

//============================================================================

static void bgt24ltr1x_div_power_down(void)
{
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_DIV_EN);
}

//============================================================================

static void bgt24ltr1x_ptat_power_up(void)
{
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);
}

//============================================================================

static void bgt24ltr1x_ptat_power_down(void)
{
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_PTAT);
}

//============================================================================

static void bgt24ltr1x_start_tx(void)
{
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_TX_EN);
}

//============================================================================

static void bgt24ltr1x_stop_tx(void)
{
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_TX_EN);
}

//============================================================================

static void bgt24ltr1x_bb_power_up(void)
{
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB1_EN);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB2_EN);
}

//============================================================================

static void bgt24ltr1x_bb_power_down(void)
{
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BB1_EN);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BB2_EN);
}

/* --- End of File -------------------------------------------------------- */
