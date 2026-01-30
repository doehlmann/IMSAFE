/**
    @file: d2gl_sc_fmcw_timer.c

    @brief: This file provides a set of firmware helper functions to control
    for software controlled FMCW modulation on Distance2Go board from Infineon.

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

#include <stdint.h>
#include <DAVE.h>
#include "d2gl_sc_fmcw_helper.h"
#include "d2gl_sc_fmcw_timer.h"


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

scfmcw_timer_t *scfmcw_timer_init(scfmcw_timer_t *p_inst, const CCU4_SLICE_CONFIG_t *p_config, XMC_CCU4_SLICE_PRESCALER_t prescaler)
{
	p_inst->p_config = p_config;
	p_inst->prescaler = prescaler;

	return p_inst;
}

//============================================================================

void scfmcw_timer_config(scfmcw_timer_t *p_inst, uint16_t period, uint16_t compare_match)
{

	XMC_CCU4_SLICE_SetPrescaler(p_inst->p_config->slice_ptr, p_inst->prescaler);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(p_inst->p_config->slice_ptr, period);
	XMC_CCU4_SLICE_SetTimerCompareMatch(p_inst->p_config->slice_ptr, compare_match);
	XMC_CCU4_EnableShadowTransfer(p_inst->p_config->global->module_ptr, (uint32_t)get_CCU4_shadowtransfer_slice_number(p_inst->p_config));
}

//============================================================================

float scfmcw_timer_get_ccu_period(scfmcw_timer_t *p_inst)
{
	/* use the best resolution we have, for fast period timers: */
	return ((float) (1<<p_inst->prescaler)) /(float)XMC_SCU_CLOCK_GetCcuClockFrequency();
}

//============================================================================

void scfmcw_timer_start(scfmcw_timer_t *p_inst)
{
	XMC_CCU4_SLICE_StartTimer(p_inst->p_config->slice_ptr);
}

//============================================================================

void scfmcw_timer_stop(scfmcw_timer_t *p_inst)
{
	XMC_CCU4_SLICE_StopTimer(p_inst->p_config->slice_ptr);
	XMC_CCU4_SLICE_ClearTimer(p_inst->p_config->slice_ptr);
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
