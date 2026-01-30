/**
    @file: d2gol_sc_fmcw.h

    @brief: This file contains constants declarations and functions prototypes for
            Distance2GoL Software Controlled FMCW.
 */

/* ===========================================================================
** Copyright (C) 2021-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

#ifndef D2GL_SC_FMCW_H_
#define D2GL_SC_FMCW_H_

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

#include "DAVE.h"
#include "config.h"
#include "datastore.h"

/*
==============================================================================
   2. DEFINITIONS
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
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
* \brief  This function is called, when device settings are updated.
*
* \return  Returns false, it values are accepted and valid. Returns true, if there is a problem with the new values
*/

bool scfmcw_update_settings(device_settings_t *p_dev);  // called by BSP!

/**
* \brief  This function is called, to initialize the sc_fmcw module.
*
* \return  Returns false, it values are accepted and valid. Returns true, if there is a problem with the new values
*/

bool scfmcw_init(device_settings_t *p_dev); // called by BSP!

/**
* \brief  This function is called, to start the actual chirps.
*
*/
void scfmcw_trigger_chirps(void);

/**
* \brief  This function is called, to stop the adc_sampling
*
*/
void scfmcw_stop_adc_sampling(void);

/**
* \brief  This function is called, to get the minimum possible frame period
*
*/
float scfmcw_get_min_frame_period_s(device_settings_t *p_dev);

/**
* \brief  This function is called, to stop the chirp generation
*
*/
void scfmcw_stop(void);

/**
* \brief    This function is called, to get the number of frames, since the last
* 			calibration was performed. This is useful to know by the algorithm,
* 			to reset e.g. internal states.
*
*/
uint32_t scfmcw_get_frames_since_calibration(void);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_SC_FMCW_H_ */

/* --- End of File -------------------------------------------------------- */
