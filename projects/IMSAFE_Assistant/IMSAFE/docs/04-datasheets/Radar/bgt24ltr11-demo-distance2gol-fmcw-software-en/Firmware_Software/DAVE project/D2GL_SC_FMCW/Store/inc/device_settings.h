/**
    @file  device_settings.h
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

#ifndef DEVICE_SETTINGS_H_
#define DEVICE_SETTINGS_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define DS_HW_SETTINGS_STRUCT_VERSION   210312U /* device settings structure version --> date of last change */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
 *  Hardware configuration structure
 */
typedef struct device_settings_TAG
{
  uint32_t    struct_length;  /* length of this structure */
  uint32_t    struct_version; /* Hardware structure version */

  uint32_t    bsp_max_num_of_chirps;

  /* Frequency Settings */
  uint32_t    pll_modulation_mode; /* 0: Constant frequency (doppler), 1: FMCW-modulation */

  /* ADC Settings */
  uint8_t     adc_resolution;
  uint32_t    num_of_adc_channels;

  /* Frame Settings */
  float       pulse_repetition_time_sec;
  uint32_t    num_of_chirps_per_frame;
  uint32_t    frame_period_usec;
  uint32_t    num_samples_per_chirp;
  uint8_t     rx_antenna_mask;

  uint32_t 	  num_sample_skip_count;	/* The skip count that is applied, before actual samples are taken */

  /* PLL Settings */
  float       pll_lower_frequency_kHz;
  float       pll_upper_frequency_kHz;
  float       pll_bandwidth_MHz;

  bool     	  baseband_use_low_gain_input;

} device_settings_t;


/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DEVICE_SETTINGS_H_  */

/* --- End of File -------------------------------------------------------- */
