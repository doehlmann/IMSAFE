/**
    @file: d2gl_sc_fmcw_timer.h

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

#ifndef D2GL_SC_FMCW_TIMER_H_
#define D2GL_SC_FMCW_TIMER_H_

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

/*
==============================================================================
   3. TYPES
==============================================================================
 */

typedef struct
{
   const CCU4_SLICE_CONFIG_t *p_config;
   XMC_CCU4_SLICE_PRESCALER_t prescaler;
} scfmcw_timer_t;

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

scfmcw_timer_t *scfmcw_timer_init(scfmcw_timer_t *p_inst, const CCU4_SLICE_CONFIG_t *p_config, XMC_CCU4_SLICE_PRESCALER_t prescaler);

void scfmcw_timer_config(scfmcw_timer_t *p_inst, uint16_t period, uint16_t compare_match);

float scfmcw_timer_get_ccu_period(scfmcw_timer_t *p_inst);

void scfmcw_timer_start(scfmcw_timer_t *p_inst);

void scfmcw_timer_stop(scfmcw_timer_t *p_inst);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_SC_FMCW_TIMER_H_ */

/* --- End of File -------------------------------------------------------- */
