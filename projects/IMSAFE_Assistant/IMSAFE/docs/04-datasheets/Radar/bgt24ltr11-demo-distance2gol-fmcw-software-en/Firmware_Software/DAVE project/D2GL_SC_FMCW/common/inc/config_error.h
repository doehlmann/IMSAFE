/**
    @file: config_error.h

    @brief: Compilation errors check for configuration file values defined by user.
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

#ifndef CONFIG_ERROR_H_
#define CONFIG_ERROR_H_

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

#include "config.h"
#include "d2gl_bsp.h"
#include "d2gl_bsp_limits.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

//===================== General settings error check =======================//

/* Check valid range of Number of chirps */
#ifdef NUM_OF_CHIRPS
	#if (NUM_OF_CHIRPS < BSP_MIN_NUM_OF_CHIRPS) || (NUM_OF_CHIRPS > BSP_MAX_NUM_OF_CHIRPS)
		#error "NUM_OF_CHIRPS in config.h file is out of range"
	#endif
#endif

/* Check valid range of samples per chirp */
#ifdef SAMPLES_PER_CHIRP
	#if (SAMPLES_PER_CHIRP < BSP_MIN_NUM_SAMPLES_PER_CHIRP) || (SAMPLES_PER_CHIRP > BSP_MAX_NUM_SAMPLES_PER_CHIRP)
		#error "SAMPLES_PER_CHIRP in config.h file is out of range"
	#endif
#endif

/* Check valid range for frame time */
#ifdef FRAME_PERIOD_MSEC
	#if (FRAME_PERIOD_USEC > BSP_MAX_FRAME_PERIOD_MSEC)
		#error "FRAME_PERIOD_MSEC in config.h is set too high, it should be less than BSP_MAX_FRAME_PERIOD_MSEC"
	#endif
#endif

//===================== SC-FMCW settings error check =======================//

#ifdef MIN_RF_FREQUENCY_KHZ
	#if (MIN_RF_FREQUENCY_KHZ < BSP_MIN_RF_FREQUENCY_KHZ)
	#error "MIN_RF_FREQUENCY_KHZ in config.h is set too low, it should be greater than BSP_MIN_RF_FREQUENCY_KHZ"
	#endif
#endif

#ifdef MIN_RF_FREQUENCY_KHZ
	#if (MIN_RF_FREQUENCY_KHZ + (BANDWIDTH_MHZ * 1000U) > BSP_MAX_RF_FREQUENCY_KHZ)
	#error "MIN_RF_FREQUENCY_KHZ in config.h is set too high, it should be less than BSP_MIN_RF_FREQUENCY_KHZ"
	#endif
#endif

#ifdef SC_FMCW_PULSE_REPEAT_TIME_USEC
	#if((SC_FMCW_PULSE_REPEAT_TIME_USEC != 0) && (SC_FMCW_RAMP_UP_TIME_USEC + SC_FMCW_RAMP_STAY_TIME_USEC + SC_FMCW_RAMP_DOWN_TIME_USEC + EARLY_TX_ON_ADVANCE > SC_FMCW_PULSE_REPEAT_TIME_USEC))
	#error "SC_FMCW_PULSE_REPEAT_TIME_USEC in config.h is set too low"
	#endif
#endif

/* Check valid range of Number of chirps */
#ifdef SC_FMCW_PULSE_REPEAT_TIME_USEC
	#if (SC_FMCW_PULSE_REPEAT_TIME_USEC < BSP_MIN_PRT_USEC) || (SC_FMCW_PULSE_REPEAT_TIME_USEC > BSP_MAX_PRT_USEC)
		#error "SC_FMCW_PULSE_REPEAT_TIME_USEC in config.h file is out of range"
	#endif
#endif

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CONFIG_ERROR_H_ */

/* --- End of File -------------------------------------------------------- */
