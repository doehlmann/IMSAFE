/**
    @file  datastore.c
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

#include "algorithm_exports.h"
#include "d2gl_bsp_limits.h"
#include "datastore.h"
#include "config.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*--------------------------------------------
 * NOTE: it is assumed that the storage and the consumption of the data is executed in the same execution context. (=Thread or task) otherwise counter measures against data corruption need to be considered!
 *
 *
 */

/* this would be the data structure that is persistent in flash memory! (done via scatter load linker file, once implemented! */
algo_settings_t ds_default_algo_settings = {
  .struct_length				  = sizeof( algo_settings_t ),
  .struct_version                 = DS_ALGO_SETTINGS_STRUCT_VERSION,

  .min_distance_m                 = MINIMUM_RANGE_M,
  .max_distance_m                 = MAXIMUM_RANGE_M,

  .target_hold_threshold	      = HOLD_THRESHOLD,
  .target_activation_threshold    = ACTIVATION_THRESHOLD,
  .activate_cfar				  = false,
};

const device_settings_t ds_default_device_settings =
{
  .struct_length			     = sizeof( device_settings_t ),
  .struct_version          		 = DS_HW_SETTINGS_STRUCT_VERSION,

  .bsp_max_num_of_chirps		 = BSP_MAX_NUM_OF_CHIRPS,
  /* ADC Settings */
  .adc_resolution                = 12,

  /* Frame Settings */
  .pulse_repetition_time_sec	 = (float) SC_FMCW_PULSE_REPEAT_TIME_USEC*1.0e-6,
  .num_of_chirps_per_frame       = NUM_OF_CHIRPS,
  .frame_period_usec             = FRAME_PERIOD_MSEC *1000,
  .num_samples_per_chirp         = SAMPLES_PER_CHIRP,
  .rx_antenna_mask               = BSP_RX_ANTENNA_MASK,

  .num_sample_skip_count		 = 0,  /* all samples are taken in our case! */

  /* RF Settings */
  .pll_lower_frequency_kHz       = MIN_RF_FREQUENCY_KHZ,
  .pll_upper_frequency_kHz       = MIN_RF_FREQUENCY_KHZ + (BANDWIDTH_MHZ * 1000U),
  .pll_bandwidth_MHz             = BANDWIDTH_MHZ,
  .pll_modulation_mode			 = 1, /* 0: Constant frequency (Doppler), 1: FMCW-modulation */

  .baseband_use_low_gain_input   = false,
  .num_of_adc_channels			 = BSP_NUM_OF_ADC_CHANNELS,
 };


/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
