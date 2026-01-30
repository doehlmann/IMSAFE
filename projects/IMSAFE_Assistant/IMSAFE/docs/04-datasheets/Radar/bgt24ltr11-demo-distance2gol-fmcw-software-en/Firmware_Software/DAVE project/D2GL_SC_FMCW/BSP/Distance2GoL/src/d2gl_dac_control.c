/**
    @file: d2gl_dac_control.c

    @brief: This file provides a set of firmware functions to control the DAC in
    for Software-Controlled FMCW (SC-FMCW) modulation on Distance2GoL board from Infineon.

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
#include "d2gl_dac_control.h"

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

uint32_t dummy[32]; // needed by DAVE app, but not used!
static dac_lut_control_t dac_lut_inst;

/*
==============================================================================
  5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/* This handler is called after each DAC LUT DMA transfer, sometimes useful for debugging */
static void VCO_Table_TransferEvntHandler(XMC_DMA_CH_EVENT_t event)
{
	/* functionally not used */
}

/*
==============================================================================
  6. EXPORTED FUNCTIONS
==============================================================================
 */

dac_lut_control_t *dac_lut_init(void)
{
	/* set DAC and DMA for generating Look Up table */
	dac_lut_inst.mDAC = (XMC_DAC_t*)DAC_LUT.module_ptr;
	dac_lut_inst.mDMA = (XMC_DMA_t*)DAC_LUT.dma_config_ptr->global_dma->dma;
	dac_lut_inst.dma_channel = DAC_LUT.dma_config_ptr->dma_channel;
	dac_lut_inst.dac_channel = DAC_LUT.ch;
	dac_lut_inst.mDAC_LUT = (DAC_LUT_t*)&DAC_LUT;

	/* set VCO DMA handler */
	XMC_DMA_CH_EnableEvent(dac_lut_inst.mDMA, dac_lut_inst.dma_channel, ((uint8_t)XMC_DMA_CH_EVENT_BLOCK_TRANSFER_COMPLETE | (uint8_t)XMC_DMA_CH_EVENT_ERROR));
	XMC_DMA_CH_SetEventHandler(dac_lut_inst.mDMA, dac_lut_inst.dma_channel, VCO_Table_TransferEvntHandler);

	return &dac_lut_inst;
}

//============================================================================

void dac_set_single_value(dac_lut_control_t *p_inst, uint32_t new_value)
{
	/* initialize DAC's configuration for ramping */
	XMC_DAC_CH_SetMode(p_inst->mDAC, p_inst->dac_channel, XMC_DAC_CH_MODE_IDLE);
	XMC_DAC_CH_SetMode(p_inst->mDAC, p_inst->dac_channel, XMC_DAC_CH_MODE_SINGLE);

	/* Set DAC output back to lower frequency value */
	XMC_DAC_CH_Write(p_inst->mDAC, p_inst->dac_channel,(uint16_t)new_value);
}

//============================================================================

void dac_lut_trigger_auto_start( dac_lut_control_t *p_inst )
{
	/* set DAC mode as Look up table mode */
	XMC_DAC_CH_SetMode(p_inst->mDAC, p_inst->dac_channel, XMC_DAC_CH_MODE_IDLE);
	DAC_LUT_ResetIndex(p_inst->mDAC_LUT);
	DAC_LUT_SetRepetition(p_inst->mDAC_LUT, p_inst->dac_channel, 1U);

	/* Enable LUT Mode */
	DAC_LUT_Start(p_inst->mDAC_LUT);
}

//============================================================================

void dac_lut_stop(dac_lut_control_t *p_inst )
{
	DAC_LUT_Stop(p_inst->mDAC_LUT);

	/* Disable/switch-off DAC */
	XMC_DAC_CH_SetMode(p_inst->mDAC, p_inst->dac_channel, XMC_DAC_CH_MODE_IDLE);
	DAC_LUT_ResetIndex(p_inst->mDAC_LUT);
	DAC_LUT_SetRepetition(p_inst->mDAC_LUT, p_inst->dac_channel, 1U);
}

//============================================================================

void dac_lut_set_table( dac_lut_control_t *p_inst, uint32_t *p_table, uint16_t length)
{
	/* adjust internal structures */
	p_inst->mDAC_LUT->wave_lut_ptr = p_table;
	XMC_DMA_CH_SetSourceAddress(p_inst->mDMA, p_inst->dma_channel, (uint32_t) p_inst->mDAC_LUT->wave_lut_ptr);
	p_inst->mDAC_LUT->wavegen_lut_lenght = length;
	XMC_DMA_CH_SetBlockSize(p_inst->mDMA, p_inst->dma_channel, p_inst->mDAC_LUT->wavegen_lut_lenght);
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */


/* --- End of File -------------------------------------------------------- */
