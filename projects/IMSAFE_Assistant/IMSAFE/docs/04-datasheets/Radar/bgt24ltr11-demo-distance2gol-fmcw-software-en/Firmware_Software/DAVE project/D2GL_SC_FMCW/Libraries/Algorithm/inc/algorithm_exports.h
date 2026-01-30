/**
    @file: algorithm_exports.h

    @brief: This file provides a set of firmware functions for the software-
            controlled FMCW (SC-FMCW) implementation on DistanceGoL board.

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

#ifndef ALGORITHM_EXPORTS_H_
#define ALGORITHM_EXPORTS_H_

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
#include <stdbool.h>
#include "acq_buffer.h"
#include "device_settings.h"
#include "algorithm_config.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define DS_ALGO_SETTINGS_STRUCT_VERSION  210308U /* algo settings structure version --> date of last change */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

typedef enum
{
	NOT_DETECTED   = 0U,
	STATIC_TARGET  = 1U,
	UNSURE_TARGET  = 2U,
	MOVING_TARGET  = 3U,
} e_target_status_t;

typedef struct targetData
{
  uint32_t idx;
  e_target_status_t status;
  uint8_t activated;
  float strength;
  float range_m;
  float speed_mps;
} target_data_t;

typedef enum
{
  unknown,
  target_mode,
  tracking_mode
} e_target_mode_t;

typedef struct algo_target_list_TAG
{
  uint32_t num_of_targets;
  uint32_t max_num_of_targets;
  e_target_mode_t mode;
  target_data_t elems[MAX_NUM_OF_TARGETS];
} algo_target_list_t;

typedef enum
{
  status_unknown = -1,
  status_ok = 0,
  status_error_wrong_parameter_window_gen = 256U,
  status_error_wrong_parameter_median_filter_size_too_big,
  status_error_wrong_parameter_median_size_must_be_odd,
  status_error_wrong_parameter_num_frames_for_speed_estimation,
  status_error_wrong_parameter_max_hold_window_length,
  status_error_wrong_parameter_num_of_tracks_too_big,
  status_error_frame_missed = 512U,
} e_result_status_t;

/*
 * Algorithm result structure
 */
typedef struct
{
	e_result_status_t result_status;
	algo_target_list_t *p_target_list;
	uint32_t frame_counter;
	uint32_t max_num_hold_data;
	uint32_t max_num_activation_data;

	uint32_t num_hold_data;
	uint32_t num_activation_data;

	float *p_activation_data;
	float *p_hold_data;
} algo_result_t;

/*
 * Algorithm settings structure
 */
typedef struct algo_settings_TAG
{
  uint32_t struct_length;
  uint32_t struct_version;

  float min_distance_m;
  float max_distance_m;

  uint32_t target_hold_threshold;
  uint32_t target_activation_threshold;

  bool activate_cfar;

} algo_settings_t;

/*
==============================================================================
   4. DATA
==============================================================================
*/

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  Top-level wrapper function for SC-FMCW algorithm.
 */
void algorithm_do(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ALGORITHM_EXPORTS_H_ */

/* --- End of File -------------------------------------------------------- */

