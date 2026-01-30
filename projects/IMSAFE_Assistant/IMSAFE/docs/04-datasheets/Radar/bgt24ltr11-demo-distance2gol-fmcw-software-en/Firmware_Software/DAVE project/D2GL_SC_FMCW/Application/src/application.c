/**
    @file: application.c

    @brief: This file includes the functions for the Distance2Go
            software-controlled FMCW Application Layer.
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

#include <inttypes.h>  /* needed for "PRIu32" as used in printf */
#include "d2gl_bsp_limits.h"
#include "EndpointRadarDeviceStatus.h"
#include "radar_device_status.h"
#include "application.h"
#include "datastore.h"
#include "acq_buffer.h"
#include "radar_sc_fmcw.h"
#include "radar_mcubase.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/* Define the number of supported endpoints */
#define NUMBER_SUPPORTED_ENDPOINTS         (3U)

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

/* Firmware version declaration */
Firmware_Information_t firmware_information =
{
	"Distance2GoL Software-Controlled FMCW - BGT24LTR11 Demo Radar Board",
	FW_VERSION_MAJOR,
	FW_VERSION_MINOR,
	FW_VERSION_REVISION
};

/* Radar handler declaration */
Radar_Handle_t h_radar_device = NULL;

/* A global buffer to hold all endpoints used by Host Communication library */
Endpoint_t endpoint_list[NUMBER_SUPPORTED_ENDPOINTS];

/* Global variable to store the state if USB initialization, used with radar GUI */
volatile uint8_t    g_usb_init = false;

/* Radar application demo global state */
static Radar_App_State_t radar_app_state = APP_IDLE;

static uint32_t acq_completed = 0;
static uint32_t algo_completed = 0;
static uint32_t acq_started = 0;
static uint32_t app_initalized = 0;

/* values for the duration_estimation */
const static float algo_duration_k0[] = { 15e-3F,	15e-3F,	  15e-3F,	15e-3F };
const static float algo_duration_k1[] = { 4.9e-3F,	5.1e-3F,	5.3e-3F,	5.5e-3F };
const static float stream_out_duration_k0[] = { 5.4e-3F,	5.4e-3F,	5.4e-3F,	5.4e-3F };
const static float stream_out_duration_k1[] = { 0.35e-3F,	0.7e-3F,	1.4e-3F,	2.8e-3F };

static uint32_t app_connected = 0;
static uint32_t delayCount = 0;

#if defined(FMCW_SUPPORTED)
static algo_result_t algo_result;
static algo_target_list_t target_list = {.max_num_of_targets = sizeof(target_list.elems) / sizeof(target_list.elems[0]), .num_of_targets = 0};
#endif /* FMCW_SUPPORTED */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

void acq_completed_cb(void);

void algo_completed_cb(void);

/* This callback function will be executed by radar control when starting data acquisition process
 *   it can be used to get information about the beginning of data acquisition
 */
void app_data_acq_started_cb(void *parameter);

/* This callback function will be executed by radar control at the end of data acquisition,
 * it is used to inform upper layers that data sampling is done and data is ready in user buffer
 */
void app_data_acq_done_cb(void *parameter);

/* This function will be executed at the end of data acquisition,
 * This function pointer should be set to the assigned algorithm before starting data processing
 */
static algorithm algo_processing_function = NULL;

#if defined(FMCW_SUPPORTED)
  static void do_algo_helper(void);
#endif /* FMCW_SUPPORTED */

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

/* Initialize all firmware and algorithm parameters and start communication protocol */
void app_init(void)
{
	/* Initialize USB */
	if (bsp_usb_init(BSP_USB_TIMEOUT) == 0)
	{
		g_usb_init = true;
	}

	/* Register all endpoints to the communication protocol */
	Endpoint_t ep_list[NUMBER_SUPPORTED_ENDPOINTS] =
	{
			EP_RADAR_MCUBASE_LIST_ENTRY(h_radar_device),
			EP_RADAR_D2GL_SC_FMCW_LIST_ENTRY(h_radar_device),
	        EP_RADAR_DEVICE_STATUS_LIST_ENTRY(h_radar_device)
    };

	/* Register all endpoints in case of init error to the communication protocol */
	Endpoint_t ep_err_list[2] =
	{
			EP_RADAR_MCUBASE_LIST_ENTRY(h_radar_device),
  	        EP_RADAR_DEVICE_STATUS_LIST_ENTRY(h_radar_device)
    };

	app_initalized = 0;
	uint8_t num_of_ep = sizeof(ep_err_list) / sizeof(ep_err_list[0]);
	Endpoint_t *pEndPoints = &ep_err_list[0];

	/* Open the available radar device */
	if (radar_open_device(0, &h_radar_device) == RADAR_ERR_OK)
	{
		app_initalized = 1;
		/* Register user callback functions */
		radar_register_callback(RADAR_ACQUISITION_STARTED_CB_ID, app_data_acq_started_cb);
		radar_register_callback(RADAR_ACQUISITION_DONE_CB_ID, app_data_acq_done_cb);

		num_of_ep = sizeof(ep_list) / sizeof(ep_list[0]);
		pEndPoints = &ep_list[0];
	}
	memcpy(endpoint_list, pEndPoints, num_of_ep * sizeof(Endpoint_t));

	/* Initialize the internal protocol state and the communication interface */
	protocol_init(endpoint_list, num_of_ep, NULL, BSP_HOST_TIMEOUT_USEC, system_reset);

	/* Initialize the internal protocol state and the communication interface */
	if (app_initalized)
	{
		/* Start data acquisition at the defined frame interval */
		if (radar_start_acquisition(h_radar_device) != RADAR_ERR_OK)
		{
			XMC_DEBUG("Starting Radar data acquisition failed\n");
			while (1U)
				;
		}
	}
}

//============================================================================

/* Register algorithm processing function */
void app_register_algo_process(algorithm algo_processor)
{
	/* Register algo processing function */
	algo_processing_function = algo_processor;
}

//============================================================================

float calcCurrentConsumption_mA( device_settings_t *pDev )
{
	float retValue = 0.0F;

	float chirp_time_bb_msec   = 2.1;
	float calibration_time_msec = 3.0;

	float  chirp_time_bgt_msec = (float) pDev->pulse_repetition_time_sec*1000.0;
	if( pDev->pulse_repetition_time_sec >= 3.0e-6 )
	{
		chirp_time_bgt_msec = 2.5;  // duty cycle chirp mode
	}
	float dc_bgt_on = pDev->num_of_chirps_per_frame * chirp_time_bgt_msec / ((float)pDev->frame_period_usec / 1000.0);
	float dc_bb_on  = pDev->num_of_chirps_per_frame * chirp_time_bb_msec / ((float)pDev->frame_period_usec / 1000.0);
	float dc_ptat_on = 0.0;
	float dc_calib_on = calibration_time_msec / ((float)pDev->frame_period_usec / 1000.0);

	float pc_bgt_mA  = 45;
	float pc_div_mA  = 19;
	float pc_ptat_mA = 1.5;
	float pc_calib_mA = pc_bgt_mA + pc_div_mA + pc_ptat_mA;
	float pc_bb_mA    = 0.335; // TLV9002 (OPA) + Voltage divider

	retValue = ( dc_bgt_on*pc_bgt_mA + dc_calib_on * pc_calib_mA + dc_ptat_on * pc_ptat_mA + dc_bb_on * pc_bb_mA);

	return retValue;
}

//============================================================================

/* Main application process */
void app_process(void)
{
  if (g_usb_init == true)
  {
    /* Run communication protocol */
    protocol_run();
  }
  else
  {
    /* Initialize the USB low layer driver and start USB data communication */
    if(bsp_usb_init(BSP_USB_TIMEOUT) == 0)
    {
      g_usb_init = true;
    }
  }
  /* Main radar application process */
  radar_app_process();
}

//============================================================================

void get_application_limits(sc_fmcw_para_value_def_t *p_limits, algo_settings_t *p_algo, device_settings_t *p_dev)
{
	if( p_limits == NULL)
		return;

	p_limits->max_distance_m_lower_boundary = BSP_MIN_MAXIMUM_RANGE_M;
	p_limits->max_distance_m_upper_boundary = BSP_MAX_MAXIMUM_RANGE_M;
	p_limits->max_distance_m_step_size	    = BSP_RANGE_STEP_SIZE;

	p_limits->min_distance_m_lower_boundary = BSP_MIN_MINIMUM_RANGE_M;
	p_limits->min_distance_m_upper_boundary = BSP_MAX_MINIMUM_RANGE_M;
	p_limits->min_distance_m_step_size      = BSP_RANGE_STEP_SIZE;

	/* calculate minimum possible frame period */
	float min_frame_period_sec 	= (float) bsp_calc_min_frame_interval_usec(p_dev)*1.0e-6;
	if( p_dev->num_of_chirps_per_frame > 1)
	{
		if( min_frame_period_sec < (float) BSP_MIN_FRAME_PERIOD_MSEC* 1.0e-3)
		{
			min_frame_period_sec = BSP_MIN_FRAME_PERIOD_MSEC * 1.0e-3;
		}

		/* calculate processing time of algo (just considering the upper boundary of 256 samples per chirp) */
		uint32_t index = 0;
		if( p_dev->num_samples_per_chirp == 64)
			index = 1;
		else if( p_dev->num_samples_per_chirp == 128)
			index = 2;
		else if( p_dev->num_samples_per_chirp == 256)
			index = 3;

		float duration_of_algo_calculation_sec = algo_duration_k0[index] + algo_duration_k1[index] * (p_dev->num_of_chirps_per_frame - 1);

		/* calculated time needed to stream-out the raw data to the host*/
		float duration_of_stream_out_sec = stream_out_duration_k0[index] + stream_out_duration_k1[index] * (p_dev->num_of_chirps_per_frame - 1);

		if( duration_of_algo_calculation_sec + duration_of_stream_out_sec > min_frame_period_sec)
			min_frame_period_sec = duration_of_algo_calculation_sec + duration_of_stream_out_sec;
	}
	else
	{
		min_frame_period_sec = BSP_MIN_FRAME_RATE_SINGLE_CHIRP_MSEC * 1.0e-3;
	}

	p_limits->frame_time_sec_lower_boundary = min_frame_period_sec;
	p_limits->frame_time_sec_upper_boundary = (float) BSP_MAX_FRAME_PERIOD_MSEC*1.0e-3F;
	p_limits->frame_time_sec_step_size      = 10.0*1.0e-6;

	p_limits->sample_list[0] = BSP_MIN_NUM_SAMPLES_PER_CHIRP;
	p_limits->sample_list[1] = 64;
	p_limits->sample_list[2] = 128;
	p_limits->sample_list[3] = BSP_MAX_NUM_SAMPLES_PER_CHIRP;
	p_limits->num_entries_sample_list = 4;

	p_limits->target_hold_threshold_lower_boundary = 1;
	p_limits->target_hold_threshold_upper_boundary = 1000;
	p_limits->target_hold_threshold_step_size      = 5;

	p_limits->target_activation_threshold_lower_boundary = 1;
	p_limits->target_activation_threshold_upper_boundary = 1000;
	p_limits->target_activation_threshold_step_size      = 5;

	p_limits->number_of_chips_per_frame_lower_boundary = BSP_MIN_NUM_OF_CHIRPS;
	p_limits->number_of_chips_per_frame_upper_boundary = BSP_MAX_NUM_OF_CHIRPS;
	p_limits->number_of_chips_per_frame_step_size       = 1;

	p_limits->pulse_repetition_time_sec_lower_boundary = (float)BSP_MIN_PRT_USEC*1e-6; 	// 2.5msec
	p_limits->pulse_repetition_time_sec_upper_boundary = (float)BSP_MAX_PRT_USEC*1e-6;  // 10 msec
	p_limits->pulse_repetition_time_sec_step_size 	   = (float)10/1e6;        		// in steps of 10 usec
}

//============================================================================

static uint32_t helper_do_hw_settings(void)
{
	device_settings_t *p_dev_settings = ds_device_get_settings();

	uint32_t error = radar_apply_hw_settings(h_radar_device, p_dev_settings);
	if(error == RADAR_ERR_OK)
	{
	   ds_device_settings_copy_to_shadow();
	   /* add com-lib callback to send-out updated parameters!!! */
	}
	else
	{
	  ds_device_settings_revert_from_shadow();
	  // current and new parameter differs, if an error occurred, tell to the application
	}
	radar_app_state = APP_IDLE;
	ds_clear_device_parameter_changed();

	error = radar_start_acquisition(h_radar_device);
    return error;
}

//============================================================================

void IRQ_ERROR_TOGGLE_Handler(void)
{
	bsp_led_red_toggle();
	bsp_timer_start(&TIMER_ERROR_TOGGLE);
}

//============================================================================

/* Main radar application process */
void radar_app_process(void)
{
	if (app_initalized)
	{
	}
	if (acq_started)
	{
		acq_started = 0;
	}

	if (acq_completed)
	{
#if defined(FMCW_SUPPORTED)
		/* Start the processing of the algorithm assigned in the function pointer 'algo_processor' */
		if (algo_processing_function != NULL)
		{
			do_algo_helper();
			algo_completed += 1;
		}
#endif /* FMCW_SUPPORTED */
		acq_completed = 0;
	}

	if (algo_completed)
	{
		algo_completed_cb();
		algo_completed = 0;
	}

	if (app_initalized)
	{
		switch (radar_app_state)
		{
		case APP_IDLE:
			radar_app_state = APP_CHECK_FOR_SETTINGS_UPDATE;
			break;

		case APP_CHECK_FOR_SETTINGS_UPDATE:
			/* Check if a new hardware settings are required */
			if (ds_is_device_parameter_changed() || ds_is_device_settings_updated())
			{
				radar_stop_acquisition(h_radar_device);
				radar_app_state = APP_APPLY_DEVICE_SETTINGS;
			}
			else
			{
				radar_app_state = APP_CHECK_FOR_ERROR;
			}
			break;

		case APP_APPLY_DEVICE_SETTINGS:
			helper_do_hw_settings();
			radar_app_state = APP_CHECK_FOR_ERROR;
			break;

		case APP_CHECK_FOR_ERROR:
		{
			int status = device_status_check();
			if (status < 0)
			{
				radar_app_state = APP_HANDLE_ERROR;
			}
			else
			{
				radar_app_state = APP_IDLE;
			}
		}
		break;

		case APP_HANDLE_ERROR:
			send_device_status_response();
			radar_app_state = APP_IDLE;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (radar_app_state)
		{
		case APP_IDLE:
		case APP_CHECK_FOR_ERROR:
		{
			int status = device_status_check();
			if (status < 0)
			{
				radar_app_state = APP_HANDLE_ERROR;
			}
			else
			{
				radar_app_state = APP_IDLE;
			}
		}
		break;

		case APP_HANDLE_ERROR:
		{
			/* Send out error only once, if PC app is not requesting it! */
			if (!app_connected)
			{
				delayCount += 1;
				if (delayCount == 100)
				{
					send_device_status_response();
					app_connected = 1;
					delayCount = 0;
				}
				bsp_time_delay_usec(20);
			}
			radar_app_state = APP_CHECK_FOR_ERROR;
		}
		break;

		default:
			break;
		}
	}
}

//============================================================================

/* De-initialize and free all resources used by the application */
void app_destructor(void)
{
  /* Unregister user callback functions */
  radar_unregister_callback(RADAR_ACQUISITION_STARTED_CB_ID);
  radar_unregister_callback(RADAR_ACQUISITION_DONE_CB_ID);
}

//============================================================================

void app_reset_protocol(void)
{
	set_consumption_repeat_count(0);
	set_raw_data_repeat_count(0, 0);
	set_result_repeat_count(0, 0, 0);
}

//============================================================================

/* This callback function will be executed by radar control when starting data acquisition process
 *   it can be used to get information about the beginning of data acquisition
 */
void app_data_acq_started_cb(void *parameter)
{
	acq_started += 1;
}

//============================================================================

/* This callback function will be executed by radar control at the end of data acquisition,
 * it is used to inform upper layers that data sampling is done and data is ready in user buffer
 */
void app_data_acq_done_cb(void *parameter)
{
  acq_completed += 1;
  acq_completed_cb();
}

//============================================================================

void update_leds( algo_result_t *p_result )
{
	/*
	UNSURE_TARGET --> blue,
	MOVING_TARGET --> red,
	STATIC_TARGET  --> green:
	*/
	uint32_t led_green = 0;
	uint32_t led_blue = 0;
	uint32_t led_red = 0;

	/* signal an error condition! */
	if( p_result->result_status != status_ok )
	{
		static int blink = 0;

		if( blink == 0)
		{
			bsp_led_red_on();
			bsp_led_blue_on();
			bsp_led_green_off();
			blink = 1;
		}
		else
		{
			bsp_led_red_off();
			bsp_led_blue_off();
			bsp_led_green_off();
			blink = 0;
		}
		return;
	}

	for (int i = 0; i < p_result->p_target_list->max_num_of_targets; i++)
	{
		target_data_t *p_target = &(p_result->p_target_list->elems[i]);
		if (p_target->idx ) 
		{
			if( p_target->activated)
			{
				if (p_target->status == STATIC_TARGET)
				{
				led_green = 1;
				} 
				else if (p_target->status == MOVING_TARGET)
				{
					led_red = 1;
				} 
				else if(p_target->status == UNSURE_TARGET)
				{
					led_blue = 1;
				}
			}
		}
	}

	if(led_green)
	{
		bsp_led_red_off();
		bsp_led_blue_off();
		bsp_led_green_on();

		/* Arduino IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
	else if(led_red)
	{
		bsp_led_red_on();
		bsp_led_blue_off();
		bsp_led_green_off();

		/* Arduino IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D7);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
	}
	else if(led_blue)
	{
		bsp_led_red_off();
		bsp_led_blue_on();
		bsp_led_green_off();

		/* Arduino IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
	else
	{
		bsp_led_red_off();
		bsp_led_blue_off();
		bsp_led_green_off();

		/* Arduino IO */
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
}

//============================================================================

/* This function yet to be filled in case of software reset */
void system_reset(void)
{

}

//============================================================================

/* Run the selected algorithm */
#if defined(FMCW_SUPPORTED)

void do_algo_helper(void)
{
  static uint32_t application_buffer[BSP_MAX_ADC_BUFFER_SIZE_BYTES/sizeof(uint32_t)];

  acq_buf_obj *p_acq_buf 			= ds_get_active_acq_buf();
  algo_settings_t *p_algo_settings  = ds_algo_get_settings();
  device_settings_t *p_dev_settings = ds_device_get_shadow_settings();

  /* get acquisition timing from acq_buffer object */
  acq_buf_obj appl_acq_buf 		= deep_copy_acq_buffer_obj(p_acq_buf, (uint8_t *)&application_buffer[0], sizeof(application_buffer));
  acq_buf_obj *p_appl_acq_buf 	= &appl_acq_buf;

  algo_result_t *p_algo_result 	= &algo_result;
  memset(p_algo_result, 0, sizeof(algo_result_t));
  target_list.num_of_targets = 0;
  target_list.max_num_of_targets = sizeof( target_list.elems )/sizeof(target_list.elems[0]);
  algo_result.p_target_list  = &target_list;

  static float holdDataBuffer[ALGO_MAX_LENGTH_HOLD_DATA];
  static float activationDataBuffer[ALGO_MAX_LENGTH_ACTIVATION_DATA];

  algo_result.max_num_hold_data = sizeof(holdDataBuffer)/sizeof(holdDataBuffer[0]);
  algo_result.max_num_activation_data = sizeof(activationDataBuffer)/sizeof(activationDataBuffer[0]);
  algo_result.p_activation_data = &activationDataBuffer[0];  // if NULL, no output buffer is provided, do not copy!
  algo_result.p_hold_data = &holdDataBuffer[0];

  /* this is the async raw data streaming to host device */
  if (algo_processing_function)
	  algo_processing_function(p_appl_acq_buf, p_algo_settings, p_dev_settings, p_algo_result);

  radar_sc_fmcw_raw_data(p_appl_acq_buf, p_dev_settings);

  ds_store_algo_result_ptr(p_algo_result);
  radar_results_updates(p_algo_result);

  mcu_base_updates();

  update_leds(p_algo_result);

  delete_acq_buffer_obj(p_appl_acq_buf);
}
#endif /* FMCW_SUPPORTED */

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
