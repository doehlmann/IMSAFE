/**
    @file: radar_sc_fmcw.h
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

#ifndef RADAR_SC_FMCW_H_
#define RADAR_SC_FMCW_H_

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include <stdint.h>
#include "ifxRadar.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define NB_OF_ELEMENTS(a)  (sizeof(a)/sizeof(a[0]))

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/* postfix "nu" indicate "no unit", postfix "bool" indicate that attribute is either false (==0) or true (!=0, non-zero)*/

typedef struct {
	float 		max_distance_m;
	float 		min_distance_m;
	float 		frame_time_sec;
	uint16_t 	number_of_samples_nu;
	uint32_t  	target_hold_threshold;
	uint32_t 	target_activation_threshold;
	uint8_t		number_of_chips_per_frame;
    uint8_t     continuous_mode_bool;
    uint8_t 	baseband_use_low_gain_input;
    float		pulse_repetition_time_sec;
    uint8_t		activate_cfar;
} sc_fmcw_para_values_t;

typedef struct {
	float max_distance_m_lower_boundary;
	float max_distance_m_upper_boundary;
	float max_distance_m_step_size;

	float min_distance_m_lower_boundary;
	float min_distance_m_upper_boundary;
	float min_distance_m_step_size;

	float frame_time_sec_lower_boundary;
	float frame_time_sec_upper_boundary;
	float frame_time_sec_step_size;

	uint8_t  num_entries_sample_list;
	uint16_t sample_list[10];  //could be adjusted, the protocol can handle variable entries!

	uint32_t target_hold_threshold_lower_boundary;
	uint32_t target_hold_threshold_upper_boundary;
	uint32_t target_hold_threshold_step_size;

	uint32_t target_activation_threshold_lower_boundary;
	uint32_t target_activation_threshold_upper_boundary;
	uint32_t target_activation_threshold_step_size;

	uint8_t	number_of_chips_per_frame_lower_boundary;
	uint8_t	number_of_chips_per_frame_upper_boundary;
	uint8_t	number_of_chips_per_frame_step_size;

	float pulse_repetition_time_sec_lower_boundary;
	float pulse_repetition_time_sec_upper_boundary;
	float pulse_repetition_time_sec_step_size;
} sc_fmcw_para_value_def_t;

typedef enum
{
	DIR_NOT_DETECTED 	= 0U,
	DIR_STATIC	  		= 1U,
	DIR_APPROACHING 	= 2U,
	DIR_DEPARTING 		= 3U,
} e_dir_status_t;

typedef struct {
	 uint32_t  frame_count;

	 float     range_m;
	 float     strength_nu;
	 float	   velocity_mps;
	 uint8_t   target_status;   			/* not detected: 0, static: 1, unsure status: 2, moving: 3 */
	 uint8_t   direction_of_movement;       /* dir: not detected: 0, dir: static: 1, dir: approaching: 2, dir: departing: 3 */

	 uint16_t  num_activation_data;
	 uint16_t  num_hold_data;

	 float 	   *p_activation_data;
	 float	   *p_hold_data;
} sc_fmcw_result_response_t;

typedef struct {
	uint32_t rf_frequency_khz;
	uint32_t bandwidth_mhz;
	uint8_t major_rf_shield_version;
	uint8_t minor_rf_shield_version;
	uint8_t num_tx_antennas;
	uint8_t num_rx_antennas;
	char *rf_shield_board_id;
	char *description;
	char *modulation;
	float up_chirp_time_s;
} sc_fmcw_rf_shield_info_t;

/*
==============================================================================
   4. DATA
==============================================================================
*/

extern sc_fmcw_rf_shield_info_t rf_shield_info;

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

bool copyComPara2Store(sc_fmcw_para_values_t *p_com_para);

void copyStore2comPara(sc_fmcw_para_values_t *p_com_para);

void radar_sc_fmcw_raw_data(acq_buf_obj *p_acq_buf, device_settings_t *p_dev);

void radar_results_updates(algo_result_t *p_algo_result);

void set_result_repeat_count(uint8_t endpoint, size_t count, uint8_t type);

void set_raw_data_repeat_count(uint8_t endpoint, size_t count);

/* defined in application scope */
void get_application_limits(sc_fmcw_para_value_def_t *p_limits, algo_settings_t *p_algo, device_settings_t *p_dev);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RADAR_SC_FMCW_H_ */

/* --- End of File -------------------------------------------------------- */

