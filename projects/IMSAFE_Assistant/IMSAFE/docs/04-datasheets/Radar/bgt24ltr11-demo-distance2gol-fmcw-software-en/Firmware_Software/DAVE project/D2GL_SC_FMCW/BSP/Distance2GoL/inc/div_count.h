/**
    @file: div_count.h

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

#ifndef DIV_COUNT_H_
#define DIV_COUNT_H_

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

#include <stdbool.h>
#include <stdint.h>
#include <DAVE.h>

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

typedef void (*div_counter_freq_cb_t)(void *p_context, float freq_Hz);

typedef struct
{
	CCU4_SLICE_CONFIG_t 	*p_div_counter;
	CAPTURE_t				*p_div_capture;
	volatile uint32_t    	measure_status;
	volatile uint32_t    	measure_value;
	uint32_t 			 	compare_count;
	div_counter_freq_cb_t	p_timer_complete_cb;
	void 					*p_timer_complete_cxt;
	uint32_t 				bgt_freq_div;
	bool 				    is_div_clk_enabled_external;
	void (*clk_enable_cb)(void);
	void (*clk_disable_cb)(void);
} div_count_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

void	div_clk_enable( div_count_t *p_inst);
void	div_clk_disable( div_count_t *p_inst);

uint32_t div_get_count_wait(div_count_t *p_inst);
float div_get_norm_count_wait(div_count_t *p_inst);
float div_get_frequency_Hz_wait(div_count_t *p_inst, uint32_t bgt_freq_div);

div_count_t *init_div_count(uint32_t compare_count);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DIV_COUNT_H_ */

/* --- End of File -------------------------------------------------------- */
