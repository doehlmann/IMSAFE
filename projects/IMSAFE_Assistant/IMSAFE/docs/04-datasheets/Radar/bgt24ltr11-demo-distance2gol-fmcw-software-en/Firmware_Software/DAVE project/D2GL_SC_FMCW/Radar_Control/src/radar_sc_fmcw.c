/**
    @file: radar_sc_fmcw.c
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

#include <inttypes.h>  /* needed for "PRIu32" as used in printf */
#include <stdint.h>
#include <stdbool.h>
#include "radar_sc_fmcw.h"
#include "EndpointRadar_d2gl_sc_fmcw.h"
#include "Protocol.h"
#include "PayloadHelper.h"
#include "datastore.h"
#include "config.h"
#include "radar_api.h"
#include "datastore_eep.h"
#include "version.h"
#include "d2gl_bsp_limits.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

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

sc_fmcw_rf_shield_info_t rf_shield_info = {
	.rf_frequency_khz = BSP_MIN_RF_FREQUENCY_KHZ,
	.bandwidth_mhz	= BANDWIDTH_MHZ,
	.major_rf_shield_version = 3,
	.minor_rf_shield_version = 0,
	.num_tx_antennas = 1,
	.num_rx_antennas = BSP_NUM_RX_ANTENNAS,
	.rf_shield_board_id = RF_BOARD_ID,
	.description = "Distance2GoL BGT24LTR11 shield V3.0",
	.modulation = "SW controlled FMCW",
	.up_chirp_time_s = (float)SC_FMCW_RAMP_UP_TIME_USEC*1.0e-6,
};

static size_t result_repeat_count = 0;
static size_t raw_data_repeat_count = 0;
static uint8_t g_endpoint = 0;
static uint8_t result_type = 0;

/*
==============================================================================
    6. EXPORTED FUNCTIONS
==============================================================================
 */

void set_result_repeat_count(uint8_t endpoint, size_t count, uint8_t type)
{
	result_repeat_count = count;
	result_type = type;
	g_endpoint = endpoint;
}

//============================================================================

void set_raw_data_repeat_count(uint8_t endpoint, size_t count)
{
	raw_data_repeat_count = count;
	g_endpoint = endpoint;
}

//============================================================================

void radar_sc_fmcw_raw_data(acq_buf_obj *p_acq_buf, device_settings_t *p_dev)
{
	if( raw_data_repeat_count > 0)
	{
		send_frame_data(g_endpoint, p_acq_buf, p_dev);
		raw_data_repeat_count -= 1;
	}
}

//============================================================================

void radar_results_updates(algo_result_t *p_algo_result)
{
	if( result_repeat_count > 0 && p_algo_result->result_status == status_ok)
	{
		sc_fmcw_result_response_t 	result;

		memset(&result, 0, sizeof(result));
		result.frame_count	= p_algo_result->frame_counter;
		result.target_status  = NOT_DETECTED;
		result.direction_of_movement = (uint8_t) DIR_NOT_DETECTED;
		result.num_activation_data = 0;
		result.num_hold_data = 0;

		for( int i = 0; i < p_algo_result->p_target_list->max_num_of_targets; i++)
		{
			target_data_t *p_target = &(p_algo_result->p_target_list->elems[i]);
			if( p_target->idx ) {

				result.range_m 		       = p_target->range_m;
				result.strength_nu 		   = p_target->strength;
				result.velocity_mps 	   = p_target->speed_mps;

				if( p_target->activated ) {

					result.direction_of_movement = (uint8_t) DIR_STATIC; // in case speed is 0.0, but not declared as static target
					if( p_target->status ==  STATIC_TARGET || p_target->status ==  UNSURE_TARGET)
					{
						result.direction_of_movement = (uint8_t) DIR_STATIC;
					}
					else if( p_target->speed_mps > 0.0)
					{
						result.direction_of_movement = (uint8_t) DIR_APPROACHING;
					}
					else if( p_target->speed_mps < 0.0 )
					{
						result.direction_of_movement = (uint8_t) DIR_DEPARTING;
					}

					/* take the status from the results */
					result.target_status = p_target->status;

				}
				/* process only one target from algo */
				break;
			}
		}
		if( result_type == 1)
		{
			result.num_activation_data = p_algo_result->num_activation_data;
			result.p_activation_data = p_algo_result->p_activation_data;
			result.num_hold_data = p_algo_result->num_hold_data;
			result.p_hold_data = p_algo_result->p_hold_data;

			send_adv_result_response(g_endpoint, &result);
		}
		else
			send_result_response(g_endpoint, &result);

		result_repeat_count -= 1;
	}
}

//============================================================================

void copyStore2comPara(sc_fmcw_para_values_t *p_com_para)
{
	algo_settings_t *p_algo 			  	= ds_algo_get_settings();
	device_settings_t *p_dev 			  	= ds_device_get_settings();
	p_com_para->max_distance_m            	= p_algo->max_distance_m;
	p_com_para->min_distance_m            	= p_algo->min_distance_m;
	p_com_para->frame_time_sec            	= ((float)p_dev->frame_period_usec)/1.0e6;
	p_com_para->number_of_samples_nu      	= p_dev->num_samples_per_chirp;
	p_com_para->target_hold_threshold     	= p_algo->target_hold_threshold;
	p_com_para->target_activation_threshold	= p_algo->target_activation_threshold;
	p_com_para->number_of_chips_per_frame 	= p_dev->num_of_chirps_per_frame;
	p_com_para->baseband_use_low_gain_input = (uint8_t) ((p_dev->baseband_use_low_gain_input == true) ? 1U: 0U);
	p_com_para->continuous_mode_bool 	  	= 0;  /* ignored, not used */
	p_com_para->pulse_repetition_time_sec   = p_dev->pulse_repetition_time_sec;
	p_com_para->activate_cfar               = (p_algo->activate_cfar == true) ? 1U : 0U;
}

//============================================================================

static bool algo_check_settings( sc_fmcw_para_value_def_t *p_limit, algo_settings_t *p_algo )
{
	if( p_algo->max_distance_m > p_limit->max_distance_m_upper_boundary)
		p_algo->max_distance_m = p_limit->max_distance_m_upper_boundary;

	if(p_algo->max_distance_m < p_limit->max_distance_m_lower_boundary)
		p_algo->max_distance_m = p_limit->max_distance_m_lower_boundary;

	if( p_algo->min_distance_m > p_limit->min_distance_m_upper_boundary)
		p_algo->min_distance_m = p_limit->min_distance_m_upper_boundary;

	if(p_algo->min_distance_m < p_limit->min_distance_m_lower_boundary)
		p_algo->min_distance_m = p_limit->min_distance_m_lower_boundary;

	if(p_algo->target_hold_threshold > p_limit->target_hold_threshold_upper_boundary)
		p_algo->target_hold_threshold = p_limit->target_hold_threshold_upper_boundary;

	if(p_algo->target_hold_threshold < p_limit->target_hold_threshold_lower_boundary)
		p_algo->target_hold_threshold = p_limit->target_hold_threshold_lower_boundary;

	if( p_algo->target_activation_threshold > p_limit->target_activation_threshold_upper_boundary)
		p_algo->target_activation_threshold = p_limit->target_activation_threshold_upper_boundary;

	if(p_algo->target_activation_threshold < p_limit->target_activation_threshold_lower_boundary)
		p_algo->target_activation_threshold = p_limit->target_activation_threshold_lower_boundary;

	return true;
}

//============================================================================

/* if staged Algo settings are with-in range 0 is return, otherwise non-zero
 * the return value could be interpreted as bit field to identify the failure, if needed!
 */
static bool dev_check_settings(sc_fmcw_para_value_def_t *p_limit, device_settings_t *p_dev)
{
	bsp_limit_dev_settings(p_dev);

	if(p_limit != NULL)
	{
		if( p_dev->frame_period_usec < (uint32_t) floorf(p_limit->frame_time_sec_lower_boundary*1e6))
			p_dev->frame_period_usec = (uint32_t) floorf(p_limit->frame_time_sec_lower_boundary*1e6);

		if( p_dev->num_of_chirps_per_frame > 1)
		{
			if( !nearlyEqual(p_dev->pulse_repetition_time_sec, p_limit->pulse_repetition_time_sec_lower_boundary, 0.01 ))
			{
				if( p_dev->pulse_repetition_time_sec < p_limit->pulse_repetition_time_sec_lower_boundary )
					p_dev->pulse_repetition_time_sec = p_limit->pulse_repetition_time_sec_lower_boundary;
			}

			if( !nearlyEqual(p_dev->pulse_repetition_time_sec, p_limit->pulse_repetition_time_sec_upper_boundary, 0.01 ))
			{

				if( p_dev->pulse_repetition_time_sec > p_limit->pulse_repetition_time_sec_upper_boundary )
					p_dev->pulse_repetition_time_sec = p_limit->pulse_repetition_time_sec_upper_boundary;
			}
		}
		/* else for a single chirp, the PRT is meaningless */
	}

	uint32_t valid_count = 0;
	for(int i = 0; i < p_limit->num_entries_sample_list; i++)
	{
		if( p_dev->num_samples_per_chirp == p_limit->sample_list[i])
			valid_count += 1;
	}
	if( valid_count == 0)	/* no match for samples per chirp!! */
		return false;

	return true;
}

//============================================================================

bool copyComPara2Store(sc_fmcw_para_values_t *p_com_para)
{
	algo_settings_t temp_algo_settings;
	device_settings_t temp_device_settings;
	sc_fmcw_para_value_def_t temp_limits;

	algo_settings_t *p_algo  = ds_algo_get_settings();
	device_settings_t *p_dev = ds_device_get_settings();

	/* build temporary copies of the the setting to be checked for ranges and limites */
	memcpy(&temp_algo_settings,p_algo, sizeof(temp_algo_settings));
	memcpy(&temp_device_settings, p_dev, sizeof(temp_device_settings));

	temp_algo_settings.max_distance_m	       			= p_com_para->max_distance_m;
	temp_algo_settings.min_distance_m	       			= p_com_para->min_distance_m;
	temp_device_settings.frame_period_usec		   		= p_com_para->frame_time_sec * 1.0e6;
	temp_device_settings.num_samples_per_chirp      	= p_com_para->number_of_samples_nu;
	temp_algo_settings.target_hold_threshold     		= p_com_para->target_hold_threshold;
	temp_algo_settings.target_activation_threshold      = p_com_para->target_activation_threshold;
	temp_device_settings.num_of_chirps_per_frame    	= p_com_para->number_of_chips_per_frame;
	temp_device_settings.baseband_use_low_gain_input    = (p_com_para->baseband_use_low_gain_input > 0U) ? true: false;
	temp_device_settings.pulse_repetition_time_sec      = p_com_para->pulse_repetition_time_sec;
	temp_algo_settings.activate_cfar                    = (p_com_para->activate_cfar > 0) ? true : false;
	/* p_com_para->continuous_mode_bool is ignored, and not used */

	/* check and apply limits to the setting to ensure stable operation */
	get_application_limits(&temp_limits, &temp_algo_settings, &temp_device_settings);
	if( dev_check_settings(&temp_limits, &temp_device_settings) != true)
		return false;

	get_application_limits(&temp_limits, &temp_algo_settings, &temp_device_settings);
	if( algo_check_settings(&temp_limits, &temp_algo_settings) != true )
		return false;

	ds_set_device_parameter_changed();

	memcpy(p_algo, &temp_algo_settings, sizeof(temp_algo_settings));
	memcpy(p_dev, &temp_device_settings, sizeof(temp_device_settings));

	return true;

}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
