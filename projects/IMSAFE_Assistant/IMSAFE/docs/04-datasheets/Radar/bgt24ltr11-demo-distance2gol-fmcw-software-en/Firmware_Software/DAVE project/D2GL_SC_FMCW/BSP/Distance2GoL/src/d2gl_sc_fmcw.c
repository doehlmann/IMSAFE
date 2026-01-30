/**
    @file: d2gl_sc_fmcw.c

    @brief: This file provides a set of firmware functions for the software
    		controlled FMCW implementation on DistanceGoL board.
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
#include <stdbool.h>
#include <DAVE.h>

#include "d2gl_bsp.h"
#include "d2gl_bsp_limits.h"
#include "d2gl_sc_fmcw.h"
#include "d2gl_sc_fmcw_helper.h"
#include "div_count.h"
#include "datastore.h"
#include "d2gl_sc_fmcw_internal.h"

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

const static uint32_t adc_delay_in_calib_usec = 30U;
//  30 for    standard config for 3x 100 nF loop filter (40)
// 300 for alternative config for 2x   1 uF loop filter

const static uint32_t jump_offset = 20;
//  20 for    standard config for 3x 100 nF loop filter (84)
// 100 for alternative config for 2x   1 uF loop filter

const static uint32_t calib_dac_step = 50; // Empirical value, used by the calibration routine

/* timer objects: */
static scfmcw_timer_t prt_timer;
static scfmcw_timer_t adc_trigger;
static scfmcw_timer_t dac_trigger;

static sc_fmcw_t scfmcw_instance;

// This variable needs to be global, because it is referenced by DAVE-APP
uint32_t VCO_LUT[MAX_DAC_TABLE_COUNTS] = {0,};

volatile int32_t tx_shift_khz  = BSP_TX_ENABLE_SHIFT_KHZ;


 /*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

static void do_freq_calibration(sc_fmcw_t *p_inst, bool cal_order);

static void bootup_calibration_func(sc_fmcw_t *p_inst);

static void configure_dac_tables(sc_fmcw_t *p_inst);

static void setup_timers(sc_fmcw_t *p_inst);

static uint32_t finding_dac_value_freq_Hz(sc_fmcw_t *p_inst, float target_freq_Hz, uint32_t start_dac_value, bool round_to_higher, uint32_t initial_transition_delay, bool measure_while_tx_off);

static void scfmcw_set_middle_voltage(sc_fmcw_t *p_inst);

/*
==============================================================================
  6. EXPORTED FUNCTIONS
==============================================================================
 */

float dac_set_value_and_get_freq_Hz(sc_fmcw_t *p_inst, uint32_t new_value, uint32_t delay_factor, bool measure_while_tx_off)
{
	dac_set_single_value(p_inst->p_dac_obj, new_value);
	bsp_time_delay_usec(delay_factor * p_inst->adc_delay_in_calib_usec);

	float new_freq_Hz = div_get_frequency_Hz_wait(p_inst->p_div_count, p_inst->bgt_freq_divider);

	if( measure_while_tx_off )
	{
		// if frequency is measured, with-out TX enabled we need to consider a certain frequency shift.
		new_freq_Hz += (tx_shift_khz * 1000.0);
	}
	return new_freq_Hz;
}

//============================================================================

/* This function can be used to stop FMCW processing */
void scfmcw_stop(void)
{
	sc_fmcw_t *p_inst = &scfmcw_instance;

	scfmcw_timer_stop(p_inst->p_prt_timer);
	scfmcw_timer_stop(p_inst->p_dac_trigger);
	scfmcw_timer_stop(p_inst->p_adc_trigger);

	dac_lut_stop(p_inst->p_dac_obj);

	bsp_timer_stop_clear(p_inst->p_tx_on_timer);
}

//============================================================================

/* This function can be used to update the FMCW settings */
bool scfmcw_update_settings(device_settings_t *p_dev)
{
	/* Add here needed parameters to be updated */
	float temp_pulse_repetition_time_sec = p_dev->pulse_repetition_time_sec;

	scfmcw_instance.number_of_chirp = p_dev->num_of_chirps_per_frame;

	if (scfmcw_instance.number_of_chirp == 1)
	{
		temp_pulse_repetition_time_sec = scfmcw_instance.chirp_time_s + (float)scfmcw_instance.early_tx_on_advance_usec * 0.000001;
	}

	scfmcw_instance.number_of_sample = p_dev->num_samples_per_chirp;
	scfmcw_instance.bandwidth_mhz = p_dev->pll_bandwidth_MHz;
	scfmcw_instance.lower_rf_frequency_Hz = ((float)p_dev->pll_lower_frequency_kHz * 1000.0);
	scfmcw_instance.upper_rf_frequency_Hz = ((float)p_dev->pll_upper_frequency_kHz * 1000.0);

	scfmcw_instance.lower_rf_frequency_guard_1_Hz = scfmcw_instance.lower_rf_frequency_Hz - (BSP_CALIB_BOUNDARY_RF_FREQ_KHZ * 1000.0);
	scfmcw_instance.lower_rf_frequency_guard_2_Hz = scfmcw_instance.lower_rf_frequency_Hz + (BSP_CALIB_BOUNDARY_RF_FREQ_KHZ * 1000.0);

	scfmcw_instance.upper_rf_frequency_guard_1_Hz = scfmcw_instance.upper_rf_frequency_Hz - (BSP_CALIB_BOUNDARY_RF_FREQ_KHZ * 1000.0);
	scfmcw_instance.upper_rf_frequency_guard_2_Hz = scfmcw_instance.upper_rf_frequency_Hz + (BSP_CALIB_BOUNDARY_RF_FREQ_KHZ * 1000.0);

	/* Assign the ramp configuration and timings */
	scfmcw_instance.up_chirp_time_s = SC_FMCW_RAMP_UP_TIME_USEC * 0.000001;
	scfmcw_instance.up_chirp_stay_time_s = SC_FMCW_RAMP_STAY_TIME_USEC * 0.000001;
	scfmcw_instance.down_chirp_time_s = (SC_FMCW_RAMP_DOWN_TIME_USEC - WAIT_TIME_DAC_DMA_DONE_USEC) * 0.000001;
	scfmcw_instance.chirp_time_s = scfmcw_instance.up_chirp_time_s + scfmcw_instance.down_chirp_time_s + scfmcw_instance.up_chirp_stay_time_s;

	if (temp_pulse_repetition_time_sec < (scfmcw_instance.chirp_time_s + (float)scfmcw_instance.early_tx_on_advance_usec * 0.000001 + 500 /*usec*/ * 0.000001))
	{
		scfmcw_instance.fast_chirp_config = true;

		if (temp_pulse_repetition_time_sec > (scfmcw_instance.chirp_time_s + (float)scfmcw_instance.early_tx_on_advance_usec * 0.000001))
			scfmcw_instance.pulse_repetition_time_s = temp_pulse_repetition_time_sec;
		else
			scfmcw_instance.pulse_repetition_time_s = scfmcw_instance.chirp_time_s + (float)scfmcw_instance.early_tx_on_advance_usec * 0.000001;

		scfmcw_instance.wait_time_s = (float)scfmcw_instance.early_tx_on_advance_usec * 0.000001;
	}
	else
	{
		scfmcw_instance.fast_chirp_config = false;
		scfmcw_instance.pulse_repetition_time_s = (float)temp_pulse_repetition_time_sec;
		scfmcw_instance.wait_time_s = scfmcw_instance.pulse_repetition_time_s - scfmcw_instance.chirp_time_s;

		if ((float)scfmcw_instance.early_tx_on_advance_usec * 0.000001 > scfmcw_instance.wait_time_s)
		{
			XMC_DEBUG("FATAL: invalid chirp configuration! ");
			return true;
		}
	}

	/* not changeable per runtime!! */
	scfmcw_instance.jump_offset 				= jump_offset;
	scfmcw_instance.adc_delay_in_calib_usec  	= adc_delay_in_calib_usec;

	/* Wait until next start, to apply changes!! */
	scfmcw_instance.trigger_chirp_reconfig = true;

	return false;
}

//============================================================================

float scfmcw_get_min_frame_period_s(device_settings_t *p_dev)
{
	float min_frame_period_s = (p_dev->num_of_chirps_per_frame * p_dev->pulse_repetition_time_sec) + CALIBRATION_PERIOD_SEC;

	if( p_dev->num_of_chirps_per_frame == 1)
	{
		min_frame_period_s = (float) BSP_MIN_FRAME_RATE_SINGLE_CHIRP_MSEC*1e-3; /* include timing for calibration period */
	}
	return min_frame_period_s;
}

//============================================================================

bool scfmcw_init(device_settings_t *p_dev)
{
	scfmcw_instance.p_dac_obj = dac_lut_init();

	/* init div_count_object: */
	scfmcw_instance.p_div_count = init_div_count(BGT_FREQ_MEAS_COMPARE_COUNTS);
	scfmcw_instance.bgt_freq_divider = BGT_FREQ_DIVIDER;

	scfmcw_instance.p_prt_timer   = scfmcw_timer_init(&prt_timer,   &CCU4_PRT_TRIGGER, XMC_CCU4_SLICE_PRESCALER_32);
	scfmcw_instance.p_adc_trigger = scfmcw_timer_init(&adc_trigger, &CCU4_ADC_TRIGGER, XMC_CCU4_SLICE_PRESCALER_1);
	scfmcw_instance.p_dac_trigger = scfmcw_timer_init(&dac_trigger, &CCU4_DAC_TRIGGER, XMC_CCU4_SLICE_PRESCALER_1);

	scfmcw_instance.p_tx_on_timer = (TIMER_t *)&TIMER_TX_EN;
	scfmcw_instance.early_tx_on_advance_usec = EARLY_TX_ON_ADVANCE;

	/* set target divided frequencies (lower and upper) */
	if( scfmcw_update_settings( p_dev ) == true )
	{
		XMC_DEBUG("FATAL: invalid settings!");
		return true;
	}


	/* Enable only the BGT, to calibrate the frequency, keep TX off, until frequency is calibrated! */
	bsp_bgt_power_up();

	/* Initial calibration */
	bootup_calibration_func(&scfmcw_instance);

	/* Additional regular calibration for fine tuning */
	do_freq_calibration(&scfmcw_instance, false);

	/* after calibration, set voltage back to middle voltage */
	scfmcw_set_middle_voltage(&scfmcw_instance);

	configure_dac_tables(&scfmcw_instance);
	setup_timers(&scfmcw_instance);
	scfmcw_instance.trigger_chirp_reconfig = false;

	return false;
}

//============================================================================

static void setup_timers(sc_fmcw_t *p_inst)
{

	/* use the best resolution we have, for fast period timers: */
	float period_dac_feed = scfmcw_timer_get_ccu_period(p_inst->p_dac_trigger);

	/* configure DAC trigger timer interval to generate ramp */
	uint32_t dac_timer_count = (uint32_t) ((p_inst->up_chirp_time_s / p_inst->number_of_up_counts) / period_dac_feed);
	scfmcw_timer_config(p_inst->p_dac_trigger, dac_timer_count, 1);

	float period_adc_tick = scfmcw_timer_get_ccu_period(p_inst->p_adc_trigger);

	/* configure ADC trigger timer CCU4_ADC_TRIGGER --> This is equivalent to the ADC sampling period! */
	uint32_t adc_timer_count = ((p_inst->up_chirp_time_s)/p_inst->number_of_sample)/period_adc_tick;
	scfmcw_timer_config(p_inst->p_adc_trigger, adc_timer_count, 1);

	/* configure Pulse Repeated Timer to generate multi ramps, period is longer, so we use a pre-scaler */
	float timer_resolution = period_dac_feed * (1<<p_inst->p_prt_timer->prescaler);

	uint32_t pulse_repeated_time_period_value = p_inst->pulse_repetition_time_s/timer_resolution + WAIT_TIME_DAC_DMA_DONE_TICKS;
	uint32_t pulse_repeated_time_compare_value = (p_inst->wait_time_s )/timer_resolution - 1;
	scfmcw_timer_config(p_inst->p_prt_timer, pulse_repeated_time_period_value, pulse_repeated_time_compare_value);
}

//============================================================================

static void configure_dac_tables(sc_fmcw_t *p_inst)
{
	uint32_t index;
	uint32_t stay_index;
	uint32_t down_index;
	uint32_t dac_value;
	uint32_t temp_lower_freq = 0;

	uint32_t current_dac_for_lower_freq = p_inst->target_dac_value[0];
	uint32_t current_dac_for_upper_freq = p_inst->target_dac_value[1];

	p_inst->number_of_up_counts = current_dac_for_upper_freq - current_dac_for_lower_freq;

	float avg_up_step_size = (float)(current_dac_for_upper_freq - current_dac_for_lower_freq) / scfmcw_instance.number_of_up_counts;

	// ramp down only to the middle frequency, in the down chirp
	float dac_decrement_factor = p_inst->down_chirp_time_s / (p_inst->up_chirp_time_s / 2.0);
	uint32_t number_of_down_counts = (roundf(dac_decrement_factor * p_inst->number_of_up_counts) / 2); // decrement only to half!

	if (p_inst->fast_chirp_config)
	{
		dac_decrement_factor = p_inst->down_chirp_time_s / (p_inst->up_chirp_time_s);
		number_of_down_counts = roundf(dac_decrement_factor * p_inst->number_of_up_counts);
	}

	float dac_stay_factor = p_inst->up_chirp_stay_time_s / p_inst->up_chirp_time_s;
	uint32_t number_of_stay_counts = roundf(dac_stay_factor * p_inst->number_of_up_counts);

	temp_lower_freq = current_dac_for_lower_freq + p_inst->jump_offset;
	for (index = 0; index < p_inst->number_of_up_counts; index++)
	{
		VCO_LUT[index] = temp_lower_freq + index;
	}

	/* fill up stay-time */
	stay_index = index;
	for (index = 0; index < number_of_stay_counts; index++)
	{
		VCO_LUT[stay_index + index] = current_dac_for_upper_freq;
	}

	/* fill up down-chirp only half */
	down_index = stay_index + index;
	for (index = 1; index <= number_of_down_counts; index++, down_index++)
	{
		dac_value = current_dac_for_upper_freq - roundf(index * avg_up_step_size / dac_decrement_factor);
		VCO_LUT[down_index] = dac_value;
	}

	uint32_t mid_value = (current_dac_for_upper_freq + current_dac_for_lower_freq) / 2;
	VCO_LUT[down_index] = mid_value;

	/* configure DAC's DMA block size */
	/* There exist no setter function for the wavegen_lut_length! so we need to set the internal attribute manually here! */
	p_inst->dac_total_step_count = down_index;
	p_inst->dac_stop_last_chirp_at_count = p_inst->dac_total_step_count - (number_of_down_counts / 2);

	/* set new DAC table */
	dac_lut_set_table(p_inst->p_dac_obj, &VCO_LUT[0], p_inst->dac_total_step_count);
}

//============================================================================

static void start_pre_frame_timer( sc_fmcw_t *p_inst)
{

	TIMER_SetTimeInterval(p_inst->p_tx_on_timer, (p_inst->wait_time_s * 1000000.0 - p_inst->early_tx_on_advance_usec)*100U);
	bsp_timer_start(p_inst->p_tx_on_timer);
}

//============================================================================

/* This function is used from external (with-in ADC DMA handler!), so the instance of the pll is not known!  */
void scfmcw_stop_adc_sampling(void)
{
	sc_fmcw_t *p_inst = &scfmcw_instance;

	scfmcw_timer_stop(p_inst->p_adc_trigger);

	bsp_bb_power_down();
}

//============================================================================

static void trigger_next_chirp( sc_fmcw_t *p_inst )
{
	if( p_inst->fast_chirp_config == true)
	{
		bsp_bb_power_up();  // enable baseband

		if( p_inst->curr_chirp_count == 0 )
		{
			bsp_bgt_start_up();
		}

		/* set start frequency with dac value. (24.025Ghz) */
		dac_set_single_value(p_inst->p_dac_obj, p_inst->target_dac_value[0]);

		if( p_inst->curr_chirp_count == p_inst->number_of_chirp - 1)
		{
			// reached forlast chirp, reduce DAC-lut length
			dac_lut_set_table(p_inst->p_dac_obj, &VCO_LUT[0], p_inst->dac_stop_last_chirp_at_count);
		}
		else
		{
			dac_lut_set_table(p_inst->p_dac_obj, &VCO_LUT[0], p_inst->dac_total_step_count);
		}
		dac_lut_trigger_auto_start(p_inst->p_dac_obj);
	}
	else
	{
		start_pre_frame_timer(p_inst);
	}
}

//============================================================================

void scfmcw_trigger_chirps(void)
{
	sc_fmcw_t *p_inst = &scfmcw_instance;

	/* ensure to reset everything */
	scfmcw_stop();

	if( p_inst->trigger_chirp_reconfig )
	{
		// Sync reconfiguration by parameter change to new Frame startup.
		configure_dac_tables(&scfmcw_instance);
		setup_timers(&scfmcw_instance);
		scfmcw_instance.trigger_chirp_reconfig = false;
		/* changing chirp format, does not change the rf frequencies, but the termal conditions are different, so we consider to calibration rf frequencyies soon */
		scfmcw_instance.calibration_indicator = true;

	}
	p_inst->curr_chirp_count = 0;

	/* trigger ramp, start PRT timer, until the last chirp is reached */
	scfmcw_timer_start(p_inst->p_prt_timer);

	trigger_next_chirp(p_inst);
}

//============================================================================

/* SW_IRQ_CCU4_ADC_TRIGGER_EVT_CMP_handler compare match interrupt (it is called every time after ADC samples a point during up chirp)
 * called at every ADC sampling */
void SW_IRQ_CCU4_ADC_TRIGGER_EVT_CMP_handler(void)
{
	/* functionally not used */
	/* for debugging and integration purpose, useful to toggle a pin her, like DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D4); */
}

//============================================================================

/* SW_IRQ_CCU4_DAC_TRIGGER_EVT_CMP_handler compare match interrupt (it is called after each DAC step during complete chirp) */
void SW_IRQ_CCU4_DAC_TRIGGER_EVT_CMP_handler(void)
{
	/* functionally not used */

}

//============================================================================

/* SW_IRQ_TIMER_TX_EN_handler period match interrupt: Handler is only called in duty cycling mode! */
void SW_IRQ_TIMER_TX_EN_handler(void)    //
{

	sc_fmcw_t *p_inst = &scfmcw_instance;
	bsp_timer_stop_clear(p_inst->p_tx_on_timer);

	bsp_bb_power_up();  // enable baseband
	bsp_bgt_start_up();

	// add small delay, before the the VCO voltage is dropped to lower frequency value.
	bsp_time_delay_usec(20);

	/* set start frequency with dac value. (24.025Ghz) */
	dac_set_single_value(p_inst->p_dac_obj, p_inst->target_dac_value[0]);

	dac_lut_set_table(p_inst->p_dac_obj, &VCO_LUT[0], p_inst->dac_total_step_count);

	dac_lut_trigger_auto_start(p_inst->p_dac_obj);

}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

static bool do_rf_measurement( sc_fmcw_t *p_inst, bool *p_calibration_order_low_first )
{
	bool calibration_order_low_first = true;
	bool is_calibration_needed = false;

	if( scfmcw_instance.calibration_indicator == 0 )
	{
		bool measure_while_tx_off = true;
		uint32_t current_dac_for_lower_freq = p_inst->target_dac_value[0];
		uint32_t current_dac_for_upper_freq = p_inst->target_dac_value[1];
		float current_lower_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, current_dac_for_lower_freq, 8, measure_while_tx_off);
		float current_upper_freq_Hz = p_inst->upper_rf_frequency_Hz;

		/* Check, to enable calibration, if target frequencies run out certain tolerance! */
		if(current_lower_freq_Hz < p_inst->lower_rf_frequency_guard_1_Hz || current_lower_freq_Hz > p_inst->lower_rf_frequency_guard_2_Hz )
		{
			p_inst->calibration_indicator += 1;
		}
		else
		{
			current_upper_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, current_dac_for_upper_freq, 8, measure_while_tx_off);
			/* Check, to enable calibration, if target frequencies run out certain tolerance! */
			if (current_upper_freq_Hz < p_inst->upper_rf_frequency_guard_1_Hz || current_upper_freq_Hz > p_inst->upper_rf_frequency_guard_2_Hz)
			{
				p_inst->calibration_indicator += 100;
				calibration_order_low_first = false;
			}
		}

	}

	is_calibration_needed = (p_inst->calibration_indicator > 0 ) ? true : false;

	if( p_calibration_order_low_first )
		*p_calibration_order_low_first = calibration_order_low_first;

	return is_calibration_needed;
}

//============================================================================

/* SW_IRQ_CCU4_PRT_TRIG_EVT_MATCH_handler is called every time after PRT period expires*/
void SW_IRQ_CCU4_PRT_TRIG_EVT_MATCH_handler(void)
{

	sc_fmcw_t *p_inst = &scfmcw_instance;

	p_inst->curr_chirp_count += 1;

	if(p_inst->curr_chirp_count >= p_inst->number_of_chirp)
	{
		bool calibration_order_low_first = true;

		// Reached end count of chirps, prepare for pause
		p_inst->curr_chirp_count = 0;

		bsp_bb_power_down();

		bsp_bgt_stop_tx();
		scfmcw_stop();

		bool is_calibration_needed = do_rf_measurement(p_inst, &calibration_order_low_first);

		if( is_calibration_needed )
		{


			do_freq_calibration(p_inst, calibration_order_low_first);

			/* after calibration, set voltage back to middle voltage */
			scfmcw_set_middle_voltage(p_inst);


			/* configure chirp after re-calibration */
			configure_dac_tables(&scfmcw_instance);

			setup_timers(&scfmcw_instance);

			p_inst->frames_since_calibration = 0;
		}
		else
		{
			p_inst->frames_since_calibration += 1;

			/* after calibration, set voltage back to middle voltage */
			scfmcw_set_middle_voltage(p_inst);
		}


		bsp_bgt_power_down();

	}
	else
	{
		if ( p_inst->fast_chirp_config == false)
		{
			bsp_bgt_shut_down();
		}

		// not the last chirp
		/* Continue with the next chirp */
		trigger_next_chirp(p_inst);
	}
}

//============================================================================

void bootup_calibration_func(sc_fmcw_t *p_inst)
{
	float ptat_voltage;
	float new_freq_Hz;
	bool measure_while_tx_off = true;
	uint32_t current_dac_value;

	/* read V_PTAT and then disable VCC_PTAT */
	ptat_voltage = ((float) bsp_get_vptat_voltage_u16() / 4092.0f) * 3.3;

	/* enable divider output */
	div_clk_enable(p_inst->p_div_count);

	/* set corresponding DAC value for V_TUNE */
	current_dac_value = (uint32_t) roundf(4095 * (ptat_voltage - 0.3) / (2.5 - 0.3));
	new_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, current_dac_value, 1, measure_while_tx_off);

	/* find lower DAC value */
	while(new_freq_Hz > p_inst->lower_rf_frequency_guard_2_Hz )
	{
		current_dac_value -= calib_dac_step;
		new_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, current_dac_value, 1, measure_while_tx_off);
	}
	p_inst->target_dac_value[0] = current_dac_value;

	/* find higher DAC value */
	while(new_freq_Hz < p_inst->upper_rf_frequency_guard_1_Hz )
	{
		current_dac_value += calib_dac_step;
		new_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, current_dac_value, 1, measure_while_tx_off);
	}
	p_inst->target_dac_value[1] = current_dac_value;

	/* disable divider output */
	div_clk_disable(p_inst->p_div_count);
}

//============================================================================

void do_freq_calibration(sc_fmcw_t *p_inst, bool cal_order_low_first)
{
	uint32_t new_dac_value;
	bool measure_while_tx_off = true;

	/* Enable divider output */
	div_clk_enable(p_inst->p_div_count);

	if (cal_order_low_first == true )
	{
		float new_lower_freq_Hz;
		// search lower frequency first
		new_dac_value = finding_dac_value_freq_Hz(p_inst, p_inst->lower_rf_frequency_Hz, p_inst->target_dac_value[0], true, 5, measure_while_tx_off);
		p_inst->target_dac_value[0] = new_dac_value;

		new_lower_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, new_dac_value, 5, measure_while_tx_off);

		new_dac_value = finding_dac_value_freq_Hz(p_inst, new_lower_freq_Hz + (p_inst->bandwidth_mhz * 1000000.0f), p_inst->target_dac_value[1], false, 8, measure_while_tx_off);
		p_inst->target_dac_value[1] = new_dac_value;
	}
	else
	{
		float new_upper_freq_Hz;
		// search upper frequency first
		new_dac_value = finding_dac_value_freq_Hz(p_inst, p_inst->upper_rf_frequency_Hz, p_inst->target_dac_value[1], false, 5, measure_while_tx_off);
		p_inst->target_dac_value[1] = new_dac_value;

		new_upper_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, new_dac_value, 5, measure_while_tx_off);

		new_dac_value = finding_dac_value_freq_Hz(p_inst, new_upper_freq_Hz - (p_inst->bandwidth_mhz * 1000000.0f), p_inst->target_dac_value[0], true, 8, measure_while_tx_off);
		p_inst->target_dac_value[0] = new_dac_value;
	}

	/* just re-calibrated! */
	p_inst->calibration_indicator = 0;

	/* Disable divider output */
	div_clk_disable(p_inst->p_div_count);

}

//============================================================================

uint32_t finding_dac_value_freq_Hz(sc_fmcw_t *p_inst, float target_freq_Hz, uint32_t start_dac_value, bool round_to_higher, uint32_t initial_transition_delay, bool measure_while_tx_off)
{
	/* calculate number of DAC steps per MHz */
	float temp_freq_Hz = 0;
	uint32_t temp_dac_value = 0;
	float start_freq_Hz = 0;

	do
	{
		/* set DAC and check current frequency */
		start_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, start_dac_value, initial_transition_delay, measure_while_tx_off);
		/* calculate temporary DAC value */

		if ((target_freq_Hz - start_freq_Hz) > 0)
		{
			temp_dac_value = start_dac_value + calib_dac_step;
		}
		else
		{
			temp_dac_value = start_dac_value - calib_dac_step;
		}

		/* set and check temporary frequency */
		temp_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, temp_dac_value, 5, measure_while_tx_off);
		if(temp_freq_Hz == start_freq_Hz )
		{
			/* repeat, if freq is equal to start frequency due to rapid temperature change */
			temp_freq_Hz = dac_set_value_and_get_freq_Hz(p_inst, temp_dac_value, 5, measure_while_tx_off);

		}
	}
	while(temp_freq_Hz == start_freq_Hz);

	float lower_dac_value = 0;
	float upper_dac_value = 0;
	float rf_freq_lower_Hz = 0;
	float rf_freq_upper_Hz = 0;

	if ((target_freq_Hz - start_freq_Hz) > 0)
	{
		lower_dac_value = (float) start_dac_value;
		upper_dac_value = (float) temp_dac_value;
		rf_freq_lower_Hz = start_freq_Hz;
		rf_freq_upper_Hz = temp_freq_Hz;
	}
	else
	{
		lower_dac_value = (float) temp_dac_value;
		upper_dac_value = (float) start_dac_value;
		rf_freq_lower_Hz = temp_freq_Hz;
		rf_freq_upper_Hz = start_freq_Hz;
	}

	float rf_diff = rf_freq_upper_Hz - rf_freq_lower_Hz;
	float dac_diff = upper_dac_value - lower_dac_value;

	float dac_steps_per_Hz = dac_diff / fabs(rf_diff);

	/* calculate new DAC value */
	float temp_dac_offset = (target_freq_Hz - temp_freq_Hz) * dac_steps_per_Hz;
	int32_t dac_offset = 0;
	if (round_to_higher == true)  // round up
		dac_offset = (int32_t) ceilf(temp_dac_offset);
	else // round down
		dac_offset = (int32_t) floorf(temp_dac_offset);
	uint32_t new_dac_value = (uint32_t) ((int32_t) temp_dac_value + dac_offset);

	return new_dac_value;
}

//============================================================================

uint32_t scfmcw_get_frames_since_calibration(void)
{
	return scfmcw_instance.frames_since_calibration;
}

//============================================================================
static void scfmcw_set_middle_voltage( sc_fmcw_t *p_inst)
{

	uint32_t current_dac_for_lower_freq = p_inst->target_dac_value[0];
	uint32_t current_dac_for_upper_freq = p_inst->target_dac_value[1];
	uint32_t mid_value = (current_dac_for_upper_freq + current_dac_for_lower_freq)/2;

	dac_set_single_value(p_inst->p_dac_obj, mid_value);
}


/* --- End of File -------------------------------------------------------- */
