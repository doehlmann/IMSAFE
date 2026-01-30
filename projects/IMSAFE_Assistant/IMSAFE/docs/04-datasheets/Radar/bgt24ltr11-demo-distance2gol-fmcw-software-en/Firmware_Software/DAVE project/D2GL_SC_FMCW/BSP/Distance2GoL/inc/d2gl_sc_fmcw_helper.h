/**
    @file: d2gl_sc_fmcw_helper.h

    @brief: This file provides a set of firmware functions for the software
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

#ifndef D2GL_SC_FMCW_HELPER_H_
#define D2GL_SC_FMCW_HELPER_H_

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

XMC_CCU4_SHADOW_TRANSFER_t get_CCU4_shadowtransfer_slice_number(const CCU4_SLICE_CONFIG_t* ccu4_slice);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_SC_FMCW_HELPER_H_ */

/* --- End of File -------------------------------------------------------- */
