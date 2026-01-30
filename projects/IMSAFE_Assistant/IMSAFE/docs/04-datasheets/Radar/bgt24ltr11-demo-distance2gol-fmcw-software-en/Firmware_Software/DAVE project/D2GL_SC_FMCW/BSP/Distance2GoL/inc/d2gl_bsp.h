/**
    @file: d2gl_bsp.h

    @brief: This file contains constants declarations and functions prototypes for
            Distance2GoL board hardware resources.
 */

/* ===========================================================================
 ** Copyright (C) 2018-2021 Infineon Technologies AG
 ** All rights reserved.
 ** ===========================================================================
 **
 ** ===========================================================================
 ** This document contains proprietary information of Infineon Technologies AG.
 ** Passing on and copying of this document, and communication of its contents
 ** is not permitted without Infineon's prior written authorization.
 ** ===========================================================================
 */

#ifndef D2GL_BSP_H_
#define D2GL_BSP_H_

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

#include "DAVE.h"
#include "radar_mcu4_bsp.h"
#include "acq_buffer.h"
#include "config.h"
#include "datastore.h"

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

/**
 * \brief Defines BSP data acquisition function callback ID.
 * @{
 */
typedef enum
{
  BSP_ACQUISITION_STARTED_CB_ID   = 1U,    /**< Start of data acquisition callback ID */
  BSP_ACQUISITION_DONE_CB_ID      = 2U     /**< End of data acquisition callback ID */

} Bsp_Callback_ID_t;
/** @} */

/**
 * \brief This is the callback types for BSP layer.
 *
 * This callback is used to inform upper layers about the data acquisition status,
 * this information will be used by radar control and algorithm to start data processing.
 *
 * The user must register the callback functions after calling BSP_init.
 *
 * Whenever a callback is issued, the following parameters are passed to the callback:
 *
 * \param[in]	parameter	Pointer that can be used as the parameters for this callback.
 *
 */
typedef struct
{
  /* User callback functions */
  void (*bsp_acq_started_cb)(void *parameter);          /**< Callback executed when starting data acquisition process,
                                                             it is used to inform upper layers about the beginning of data acquisition process */

  void (*bsp_acq_done_cb)(void *parameter);             /**< Callback executed at the end of data acquisition,
                                                             it is used to inform upper layers that data sampling is done and data is ready in user buffer */

} Bsp_callback_Struct_t;

/*
==============================================================================
   4. DATA
==============================================================================
*/

extern TIMER_t TIMER_ADC_TRIG;

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  This function limited the device settings to the valid boundaries to ensure stable operation
 */
void bsp_limit_dev_settings(device_settings_t *p_dev);

/**
 * \brief  This function configures the SC-FMCW driver forBGT24LTR11
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_init(device_settings_t *hw_settings);

/**
 * \brief  This function configures the device after a configuration update.
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_update(device_settings_t *hw_settings);

/**
 * \brief  This function is used to trigger a new frame acquisition.
 */
void bsp_trigger_new_frame(void);

/**
 * \brief  This function is used to startup the bgt device
 */
void bsp_bgt_start_up(void);

/**
 * \brief  This function is used to power-down the bgt device
 */
void bsp_bgt_shut_down(void);

/**
 * \brief  This function enables the divider output with a divider of 8192
 */
void bsp_bgt_div_start_up(void);

/**
 * \brief  This function disables the divider output
 */
void bsp_bgt_div_shut_down(void);

/**
 * \brief This function returns the current temperature info of BGT24LTR11 chip in C°.
 *
 * \return The current temperature info of BGT24LTR11 chip in C°
 */
float32_t bsp_bgt_get_temp_value(void);

/**
 * \brief  This function is used stop timer and clear its register.
 *
 * \param[in]  handle_timer  Timer handle pointer of type \ref TIMER_t
 *
 * \return  TIMER_STATUS_t  success or failure will be return
 */
TIMER_STATUS_t bsp_timer_stop_clear(TIMER_t* const handle_timer);

/**
 * \brief  This function is used start a timer.
 *
 * \param[in]  handle_timer  Timer handle pointer of type \ref TIMER_t
 *
 * \return  TIMER_STATUS_t  success or failure will be return
 */
TIMER_STATUS_t bsp_timer_start(TIMER_t* const handle_timer);

/**
 * \brief  This function sets the destination address pointers for each DMA transfer channel I1, Q1, I2 and Q2.
 *
 * \param[in]  DMA_dst_I1  Unsigned 32-bit pointer for DMA destination address for ADC_I1 channel
 * \param[in]  DMA_dst_Q1  Unsigned 32-bit pointer for DMA destination address for ADC_Q1 channel
 * \param[in]  DMA_dst_I2  Unsigned 32-bit pointer for DMA destination address for ADC_I2 channel
 * \param[in]  DMA_dst_Q2  Unsigned 32-bit pointer for DMA destination address for ADC_Q2 channel
 */
void bsp_dma_set_destination_addr(uint32_t* DMA_dst_I1, uint32_t* DMA_dst_Q1, uint32_t* DMA_dst_I2, uint32_t* DMA_dst_Q2);


void bsp_dma_set_adc_control_from_acq_buf_obj(acq_buf_obj *acq_buf);


uint32_t bsp_calc_min_frame_interval_usec(device_settings_t *hw_settings);

/**
 * \brief  This function initializes the USB low layer driver and starts USB data communication.
 *
 * \param[in]  timeout_delay  Unsigned 32-bit counter value, representing number of attempts to be made to connect to the VCOM at the Host
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_usb_init(uint32_t timeout_delay);

/**
 * \brief  This function returns the number of available BGT24 radar devices.
 *
 * \return  Returns "0" if no devices otherwise >= 1.
 */
uint8_t bsp_get_number_of_available_radar_devices(void);

/**
 * \brief This function sets the number of samples to captured during each chirp
 *        This value will be used to configure the DMA block size and destination address
 *
 * \param[in]  num_samples_per_dma_transfer   The number of samples to be captured during each chirp
 */
void bsp_set_num_samples_per_dma_transfer(uint32_t num_samples_per_dma_transfer);

/**
 * \brief This function gets the frame count with current frame interval.
 *
 * \return The current frame counter
 */
uint32_t bsp_get_frame_counter(void);

/**
 * \brief This function sets the frame count to default value 0.
 */
void bsp_reset_frame_counter(void);

/**
 * \brief	This function reads the output voltage of V_PTAT pin.
 *
 * \return  Return The voltage output on V_PTAT pin in mV.
 */
float bsp_get_vptat_voltage(void);
uint16_t bsp_get_vptat_voltage_u16(void);

/**
 * \brief	This function registers user callback functions in BSP callback structure \ref Bsp_callback_Struct_t.
 *          Note: This function must be called after bsp_init() function call
 *
 * \param[in] 	cb_id		BSP callback ID \ref Bsp_Callback_ID_t
 * \param[in] 	pcallback	pointer to BSP callback function, can be one of \ref Bsp_callback_Struct_t
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_register_callback(Bsp_Callback_ID_t cb_id, void (*pcallback)(void *parameter));

/**
 * \brief	This function unregisters user callback functions from radar object structure \ref Bsp_callback_Struct_t.
 *
 * \param[in] 	cb_id		BSP callback ID \ref Bsp_Callback_ID_t
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_unregister_callback(Bsp_Callback_ID_t cb_id);

/**
 * \brief   This function powers up the baseband.
 */
void bsp_bb_power_up(void);

/**
 * \brief   This function powers down the baseband.
 */
void bsp_bb_power_down(void);

/**
 * \brief 	This function powers up the BGT.
 */
void bsp_bgt_power_up(void);

/**
 * \brief 	This function powers down the BGT.
 */
void bsp_bgt_power_down(void);

/**
 * \brief 	This function powers up the BGT divider.
 */
void bsp_bgt_div_power_up(void);

/**
 * \brief 	This function powers down the BGT divider.
 */
void bsp_bgt_div_power_down(void);

/**
 * \brief 	This function powers up the BGT PTAT.
 */
void bsp_bgt_ptat_power_up(void);

/**
 * \brief 	This function powers down the BGT PTAT.
 */
void bsp_bgt_ptat_power_down(void);

/**
 * \brief   This function enables BGT TX transmission.
 */
void bsp_bgt_start_tx(void);

/**
 * \brief   This function disables BGT TX transmission.
 */
void bsp_bgt_stop_tx(void);

void bsp_time_delay_usec(uint32_t delay_val_usec);

bool nearlyEqual(float a, float b, float epsilon);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* D2GL_BSP_H_ */

/* --- End of File -------------------------------------------------------- */
