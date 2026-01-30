/**
    @file: div_count.c
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
#include <stdint.h>
#include <stdbool.h>

#include "d2gl_bsp.h"
#include "d2gl_bsp_limits.h"
#include "d2gl_sc_fmcw.h"
#include "div_count.h"
#include "d2gl_sc_fmcw_helper.h"

/*
==============================================================================
   4. DATA
==============================================================================
 */

static div_count_t div_count_obj;

/*
==============================================================================
  5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

static void div_frequency_Hz_callback_helper(div_count_t *p_inst, uint32_t value);
static void div_get_count_stop(div_count_t *p_inst);
static void div_get_count_start(div_count_t *p_inst);

/*
==============================================================================
  6. EXPORTED FUNCTIONS
==============================================================================
 */

div_count_t *init_div_count(uint32_t compare_count)
{
	/* init div_count_object: */
	div_count_obj.p_div_counter = (CCU4_SLICE_CONFIG_t *)&CCU4_RF_DIV_COUNTER;
	div_count_obj.p_div_capture = (CAPTURE_t *)          &CAPTURE_COUNTER_PERIOD;
	div_count_obj.measure_status = 0;
	div_count_obj.measure_value  = 0;
	div_count_obj.compare_count = compare_count;
	div_count_obj.p_timer_complete_cb = NULL;

	div_count_obj.is_div_clk_enabled_external = false;
	div_count_obj.clk_enable_cb     = bsp_bgt_div_start_up;
	div_count_obj.clk_disable_cb    = bsp_bgt_div_shut_down;

	return &div_count_obj;
}

//============================================================================

/* The following is called for measuring frequency from the divider */
void SW_IRQ_CCU4_RF_DIV_COUNT_PERIOD_handler(void)
{
	uint32_t value = 0;

	div_count_t *p_div_count_inst = &div_count_obj;

	p_div_count_inst->measure_status = 1;

	CAPTURE_STATUS_t status = CAPTURE_GetCapturedTime(p_div_count_inst->p_div_capture, (uint32_t *const)&value);

	div_get_count_stop(p_div_count_inst);

	if( status == CAPTURE_STATUS_SUCCESS)
	{
		p_div_count_inst->measure_value = value;
		// timer callback is set, so we call the helper to call the callback with the calculated frequency.
		if( p_div_count_inst->p_timer_complete_cb)
			div_frequency_Hz_callback_helper(p_div_count_inst, value);
	}
}

//============================================================================

uint32_t div_get_count_wait(div_count_t *p_inst)
{
	uint32_t timeout = 1000000;

	p_inst->measure_status = 0;
	p_inst->measure_value = 0;

	div_get_count_start(p_inst);

	/* We need termination count for this loop! Hungs, if BGT VCC is turned off, VCC_div @BGT is off, Divider is not toggling --> CCU is not triggering IRQ! */
	/* waiting until measuring the divided frequency. */
	while(timeout > 0 && !p_inst->measure_status)
	{
		timeout -= 1;
	}
	if(timeout == 0)
		return 0;

	return p_inst->measure_value;
}

//============================================================================

float div_get_norm_count_wait(div_count_t* p_inst)
{
	return ((float) p_inst->compare_count/ (float) div_get_count_wait(p_inst));
}

//============================================================================

float div_get_frequency_Hz_wait(div_count_t* p_inst, uint32_t bgt_freq_div)
{
	return div_get_norm_count_wait( p_inst ) * ((float) bgt_freq_div) * (float)XMC_SCU_CLOCK_GetCcuClockFrequency();
}

//============================================================================

void div_clk_enable( div_count_t *p_inst)
{
	if( p_inst->clk_enable_cb)
	{
		p_inst->clk_enable_cb();
		p_inst->is_div_clk_enabled_external = true;
	}
}

//============================================================================

void div_clk_disable(div_count_t *p_inst)
{
	if( p_inst->clk_disable_cb)
	{
		p_inst->clk_disable_cb();
		p_inst->is_div_clk_enabled_external = false;
	}
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

static void div_get_count_start(div_count_t *p_inst)
{
	// if clock was not enabled before, we enable it here
	if( !p_inst->is_div_clk_enabled_external && p_inst->clk_enable_cb)
	{
		p_inst->clk_enable_cb();
	}

	XMC_CCU4_SLICE_SetTimerPeriodMatch(p_inst->p_div_counter->slice_ptr, p_inst->compare_count);
	XMC_CCU4_SLICE_SetTimerCompareMatch(p_inst->p_div_counter->slice_ptr, 1);
	XMC_CCU4_EnableShadowTransfer(p_inst->p_div_counter->global->module_ptr, (uint32_t)get_CCU4_shadowtransfer_slice_number(p_inst->p_div_counter));

	/* ready for capture timer and start counter timer */
	CAPTURE_Start (p_inst->p_div_capture);
	XMC_CCU4_SLICE_StartTimer(p_inst->p_div_counter->slice_ptr);

}

//============================================================================

static void div_get_count_stop(div_count_t *p_inst )
{
	XMC_CCU4_SLICE_StopTimer(p_inst->p_div_counter->slice_ptr);
	CAPTURE_Stop(p_inst->p_div_capture);

	// if clock was not enabled before, we disable it again!
	if( !p_inst->is_div_clk_enabled_external && p_inst->clk_disable_cb)
	{
		p_inst->clk_disable_cb();
	}
}

//============================================================================

static void div_frequency_Hz_callback_helper(div_count_t* p_inst, uint32_t value)
{
	float freq_value_Hz = ((float) p_inst->compare_count/ (float) value) * ((float) p_inst->bgt_freq_div) * (float)XMC_SCU_CLOCK_GetCcuClockFrequency();

	if( p_inst->p_timer_complete_cb)
		p_inst->p_timer_complete_cb(p_inst->p_timer_complete_cxt, freq_value_Hz);

	p_inst->p_timer_complete_cb = NULL;
	p_inst->p_timer_complete_cxt = NULL;
	p_inst->bgt_freq_div = 1;
}

/* --- End of File -------------------------------------------------------- */
