/**
    @file: config.h

    @brief: Configuration file for Distance2GoL project.
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

#ifndef CONFIG_H_
#define CONFIG_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define MIN(a,b)     (((a)<(b))?(a):(b)) /**< Generic implementation to define MIN macro */
#define MAX(a,b)     (((a)>(b))?(a):(b)) /**< Generic implementation to define MAX macro */

//============================= HW VERSIONING ============================//

#define HW_VERSION_MAJOR				3U     	/**< Hardware major version */
#define HW_VERSION_MINOR  				0U		/**< Hardware minor version */

//============================= GENERAL CONFIG =============================//

#define NUM_OF_CHIRPS					(10U)	/**< Valid range of chirps in relation to the SAMPLES_PER_CHIRP, [1 - 16] (default 10U) */

#define SAMPLES_PER_CHIRP 				(64U)  	/**< Number of samples per chirp/frame (default 64U) */

#define FRAME_PERIOD_MSEC 				(100U) 	/**< Time period of one frame to capture data (default 100U) */

//============================= SIGNAL PROCESSING CONFIG ===================//

#define ACTIVATION_THRESHOLD			(150U)  /**< Default activation threshold */

#define HOLD_THRESHOLD					(75U)   /**< Default hold threshold */

#define MINIMUM_RANGE_M 				(1.0F)  /**< Used in algorithm to exclude targets below this range (units in m) */

#define MAXIMUM_RANGE_M 				(16.0F)	/**< Used in algorithm to exclude targets beyond this range (units in m) */

//============================= SC-FMCW CONFIG =============================//

#define FMCW_SUPPORTED					        /**< Uncomment this macro, to disable FMCW support */

#define BANDWIDTH_MHZ 			        (200U) 	/**< Bandwidth (units in MHz), [0 - 200MHz] */

#define MIN_RF_FREQUENCY_KHZ           	(24025000U)

#define SC_FMCW_RAMP_UP_TIME_USEC		(1500U) /**< Ramp up time for Chirps */

#define SC_FMCW_RAMP_STAY_TIME_USEC		(200)	/**< Ramp stay time at peak for Chirps */

#define SC_FMCW_RAMP_DOWN_TIME_USEC		(300)	/**< Ramp down time for Chirps */

#define SC_FMCW_PULSE_REPEAT_TIME_USEC	(5000U)	/**< Pulse repetition time (PRT) */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CONFIG_H */

/* --- End of File -------------------------------------------------------- */
