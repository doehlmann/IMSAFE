/**
    @file: d2gl_sc_fmcw_helper.c

    @brief: This file provides a set of firmware helper functions to control
            software-controlled FMCW (SC-FMCW) modulation on Distance2GoL board from Infineon.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include <DAVE.h>

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

XMC_CCU4_SHADOW_TRANSFER_t get_CCU4_shadowtransfer_slice_number(const CCU4_SLICE_CONFIG_t* ccu4_slice)
{
	XMC_CCU4_SHADOW_TRANSFER_t slice_number = XMC_CCU4_SHADOW_TRANSFER_SLICE_0;

	if(ccu4_slice->slice_num == 0)
		slice_number = XMC_CCU4_SHADOW_TRANSFER_SLICE_0;
	else if(ccu4_slice->slice_num == 1)
		slice_number = XMC_CCU4_SHADOW_TRANSFER_SLICE_1;
	else if(ccu4_slice->slice_num == 2)
		slice_number = XMC_CCU4_SHADOW_TRANSFER_SLICE_2;
	else if(ccu4_slice->slice_num == 3)
		slice_number = XMC_CCU4_SHADOW_TRANSFER_SLICE_3;

	return slice_number;
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
