/**
    @file: algorithm_config.c

    @brief: This file provides a set of firmware functions for the software
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "algorithm_config.h"

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

const float  low_gain_scaling_factor     = 31.6;  // equal to 30 dB

const uint32_t config_num_frames_for_speed_estimation = MAX_NUM_FRAMES_FOR_SPEED_ESTIMATION;
const uint32_t config_hold_window_length 		   	  = MAX_HOLD_WINDOW_LENGTH;

uint32_t algo_config_median_filter_len = 3;  // must be odd and smaller then MAX_MEDIAN_FILTER_LEN

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

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

//============================================================================

/* --- End of File -------------------------------------------------------- */
