/**
    @file: d2gl_sc_fmcw_internal.h

    @brief: This file provides a set of firmware functions to manage BGT24LTR11
    on Distance2GoL board from Infineon.
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

#ifndef D2GL_SC_FMCW_INTERNAL_H_
#define D2GL_SC_FMCW_INTERNAL_H_

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

#include <stdint.h>
#include <stdbool.h>
#include <DAVE.h>
#include "d2gl_dac_control.h"
#include "d2gl_sc_fmcw_timer.h"
#include "div_count.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

// Divider factor of BGT24LTR11, from via DIGITAL_IO_BGT_DIV_OUT
#define BGT_FREQ_DIVIDER					(8192)
#define BGT_FREQ_MEAS_COMPARE_COUNTS		(500)

#define MAX_DAC_TABLE_COUNTS				(2048)

/* it's calculated by PRT timer's configuration */
#define WAIT_TIME_DAC_DMA_DONE_USEC			(10)
#define WAIT_TIME_DAC_DMA_DONE_TICKS		(45)

#define CALIBRATION_PERIOD_SEC				(10e-3F) // 10 msec

/*
==============================================================================
   3. TYPE DEFINITONS
==============================================================================
 */

typedef struct
{
	float up_chirp_time_s;
	float down_chirp_time_s;
	float up_chirp_stay_time_s;
	float chirp_time_s;
	float wait_time_s;
	float pulse_repetition_time_s;

	bool fast_chirp_config;
	bool trigger_chirp_reconfig;

	dac_lut_control_t *p_dac_obj;

	div_count_t *p_div_count;

	scfmcw_timer_t *p_prt_timer;
	scfmcw_timer_t *p_adc_trigger;
	scfmcw_timer_t *p_dac_trigger;


	TIMER_t  *p_tx_on_timer;
	uint32_t early_tx_on_advance_usec;

	unsigned int target_dac_value[2];
	uint32_t number_of_chirp;
	uint32_t curr_chirp_count;
	uint32_t number_of_sample;

	uint32_t number_of_up_counts;
	uint32_t dac_stop_last_chirp_at_count;
	uint32_t dac_total_step_count;

	uint32_t jump_offset;
	uint32_t adc_delay_in_calib_usec;

	uint32_t bgt_freq_divider;
	uint32_t bandwidth_mhz;
	float lower_rf_frequency_Hz;
	float upper_rf_frequency_Hz;

	float lower_rf_frequency_guard_1_Hz;
	float lower_rf_frequency_guard_2_Hz;

	float upper_rf_frequency_guard_1_Hz;
	float upper_rf_frequency_guard_2_Hz;

	/* used to decide, to trigger Calibration, or not!
	 * enables calibration when derived freq. goes out of limit */
	bool calibration_indicator;

	uint32_t frames_since_calibration;

} sc_fmcw_t;

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_SC_FMCW_INTERNAL_H_ */

/* --- End of File -------------------------------------------------------- */

