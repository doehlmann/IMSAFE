/**
    @file: d2gl_dac_control.h

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

#ifndef D2GL_DAC_CONTROL_H_
#define D2GL_DAC_CONTROL_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

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

typedef struct
{
	XMC_DAC_t*	mDAC;
	XMC_DMA_t*	mDMA;
	uint8_t		dma_channel;
	uint8_t		dac_channel;
	DAC_LUT_t*	mDAC_LUT;
} dac_lut_control_t;

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

/**
 * \brief  This function creates a dac_lut object.
 *
 * \return  Returns dac_lut_object.
 */
dac_lut_control_t *dac_lut_init(void);

/**
 * \brief  This function sets a value on the DAC output.
 *
 */

void dac_set_single_value(dac_lut_control_t *p_inst, uint32_t new_value);

/**
 * \brief   This function arms the trigger to allow the automatic start of
 * 			the dac value stream-out.
 *
 */
void dac_lut_trigger_auto_start(dac_lut_control_t *p_inst);

/**
 * \brief   This function stops the dac control.
 *
 */
void dac_lut_stop(dac_lut_control_t *p_inst);

/**
 * \brief   This function sets a new table to be streamed out, next time
 * 			the trigger is issued.
 *
 */
void dac_lut_set_table( dac_lut_control_t *p_inst, uint32_t *p_table, uint16_t length);

#endif /* D2GL_DAC_CONTROL_H_ */

/* --- End of File -------------------------------------------------------- */
