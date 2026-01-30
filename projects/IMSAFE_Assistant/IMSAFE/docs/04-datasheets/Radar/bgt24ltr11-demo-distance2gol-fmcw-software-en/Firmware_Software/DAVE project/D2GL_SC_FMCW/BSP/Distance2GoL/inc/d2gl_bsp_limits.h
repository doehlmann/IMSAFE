/**
    @file: d2gl_bsp_limits.h

    @brief: This file contains constants declarations and functions prototypes for
            Distance2GoL board hardware resources.
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

#ifndef D2GL_BSP_LIMITS_H_
#define D2GL_BSP_LIMITS_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/**
  * @brief  Define for Distance2GoL board
  */

/**
 * \addtogroup BGT24LTR11 sensor definitions
 * @{
 */
#define BSP_NUM_TX_ANTENNAS      (uint8_t)1U /**< TX antennas in BGT24LTR11 */

#define BSP_NUM_RX_ANTENNAS      (uint8_t)2U /**< RX antennas in BGT24LTR11 */

#define BSP_MAX_NUM_ADC_CHANNELS	(uint8_t)4U /**< The maximum number of ADC channels that are captured */

#define BSP_NUM_TEMP_SENSORS     (uint8_t)0U /**< Number of temperature sensor in BGT24LTR11 */

#define BSP_MAX_TX_POWER_LEVEL   (uint8_t)0U /**< Maximum BGT24LTR11 TX output power */

#define BSP_TX_ENABLE_SHIFT_KHZ  (3000)      /**< Frequency shift due to enabling TX */

#define BSP_MIN_RF_FREQUENCY_KHZ (24025000U)
#define BSP_MAX_RF_FREQUENCY_KHZ (24225000U)

#define EARLY_TX_ON_ADVANCE 		(500)

/* Guards to trigger new Calibration, if boundaries are violated */
#define BSP_CALIB_BOUNDARY_RF_FREQ_KHZ (5000U)

#define BSP_MAX_NUM_OF_CHIRPS				(16U)
#define BSP_MIN_NUM_OF_CHIRPS				(1)

#define BSP_MIN_MAXIMUM_RANGE_M  		((float) 2.0F)
#define BSP_MAX_MAXIMUM_RANGE_M  		((float) 30.0F)
#define BSP_RANGE_STEP_SIZE      		((float) 0.125F)

#define BSP_MIN_MINIMUM_RANGE_M  		((float) 1.0F)
#define BSP_MAX_MINIMUM_RANGE_M  		((float) 25.0F)
/** @} */

/**
 * \addtogroup Distance2GoL Hardware specific constants
 * @{
 */

#define BSP_NUM_OF_ADC_CHANNELS				   (uint32_t) 4U /**< Number of ADC channels to be acquired in RAM */
 
#define BSP_MAX_NUM_CHIRPS_PER_FRAME         (uint32_t) 16U /**< Maximum supports up to 16 ramps, but a single cycle uses up to 2 ramp sections */

#define BSP_MIN_NUM_SAMPLES_PER_CHIRP        32U /**< Minimum Samples per chirp supported */

#define BSP_MAX_NUM_SAMPLES_PER_CHIRP        256U /**< Maximum Samples per chirp supported */

#define BSP_MAX_ADC_BUFFER_SIZE_BYTES        (uint32_t)(BSP_NUM_OF_ADC_CHANNELS * BSP_MAX_NUM_CHIRPS_PER_FRAME * BSP_MAX_NUM_SAMPLES_PER_CHIRP * sizeof(uint16_t))    /**< Maximum size of buffer to hold the raw ADC samples in bytes */

#define BSP_RX_ANTENNA_MASK                  (uint8_t) 0x03U /**< Selection of RX Antenna */

#define BSP_ADC_RESOLUTION                   (uint8_t) 12U /**< Number of bits per ADC data sample */

#define BSP_MAX_FRAME_PERIOD_MSEC				(200U) /**< Max frame period in msec */
#define BSP_MIN_FRAME_PERIOD_MSEC				(50U)	 /**< Min frame period in msec */


#define BSP_MIN_PRT_USEC						   (2500U)  /**< minimum Pulse repetition rate (2.5 msec) */
#define BSP_MAX_PRT_USEC						   (10000U) /**< maximum Pulse repetition rate (10.0 msec) */

#define BSP_MIN_FRAME_RATE_SINGLE_CHIRP_MSEC (20U)	   /**< Min. frame period for single chirp config (value should be including calibration duration) */

/** @} */

/**
 * \addtogroup Timeouts Definitions
 * @{
 */
#define BSP_HOST_TIMEOUT_USEC    (uint32_t) (10*1000000U) /**< 10s timeout period for Host Application (in units of us) */

#define BSP_USB_TIMEOUT          (uint32_t) 0x0FFFU /**< Maximum attempt count to connect with the Host USB */
/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_BSP_LIMITS_H_ */

/* --- End of File -------------------------------------------------------- */
