/**
    @file: algorithm_config.h

    @brief: This file provides a set of firmware functions for the software-
            controlled FMCW (SC-FMCW) implementation on DistanceGoL board.
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

#ifndef ALGORITHM_CONFIG_H_
#define ALGORITHM_CONFIG_H_

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/*****************************************************************************
 *
 * READ-ONLY DEFINES MAY NOT BE CHANGED!!!!!!!
 *
 * ***************************************************************************/

#define	MAX_NUM_OF_TARGETS						(1U)	/**< Maximum number of targets to be reported to Application */

#define ALGO_MAX_NUM_OF_CHIRPS					(16)	/**< Maximum number of chirps supported by Algorithm */

#define ALGO_MAX_NUM_OF_SAMPLES					(256)	/**< Maximum number of samples supported by Algorithm */

#define ALGO_MAX_LENGTH_HOLD_DATA   			(256)	/**< Maximum size of hold data provided to application */

#define ALGO_MAX_LENGTH_ACTIVATION_DATA   	    (256) 	/**< Maximum size of activation data provided to application */

#define MAX_MEDIAN_FILTER_LEN       			(13U)   /**< Maximum length of the median filter */

#define MAX_NUM_FRAMES_FOR_SPEED_ESTIMATION      (10U)	/**< Number of frames used for Speed estimation */

#define MAX_HOLD_WINDOW_LENGTH                   (20U) 	/**< Number of frames used for Hold target detection */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ALGORITHM_CONFIG_H_ */

/* --- End of File -------------------------------------------------------- */
