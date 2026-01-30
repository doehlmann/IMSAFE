/**
 * \file EndpointRadar_d2gl_sc_fmcw.h
 *
 * \brief Radar endpoint containing functionality pertinent to Industrial D2GL Radar Demo kit.
 *
 */

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_ENDPOINTRADAR_D2GL_SC_FMCW_H_
#define HOSTCOMMUNICATION_ENDPOINTRADAR_D2GL_SC_FMCW_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "Protocol.h"
#include "ifxRadar.h"
#include "radar_sc_fmcw.h"

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

#define EP_RADAR_D2GL_SC_FMCW_LIST_ENTRY(context) \
{ \
    /*.endpoint_type    = */ 0x544A5346, /* ASCII code 'TJSF' */ \
    									 /*  = Distance2GoL SC-FMCW*/ \
    /*.endpoint_version = */ 1, \
    /*.handle_message   = */ ep_radar_d2gl_sc_fmcw_handle_message, \
    /*.context          = */ context, \
    /*.handle_change    = */ ep_radar_d2gl_sc_fmcw_handle_change \
}


#define MSG_PARA_VALUES_IND 			0x50  	/**< A message to request the parameter values. */
#define MSG_PARA_VALUES_RES		 		0x51  	/**< A message to confirm the parameter values. */

#define MSG_PARA_VALUES_DEF_IND			0x52	/**< A message to request the default parameter list values */
#define MSG_PARA_VALUES_DEF_RES			0x53	/**< A message containing the parameters variation */
#define MSG_PARA_SET_VALUES_IND			0x54    /**< A message to set the parameter values */


#define MSG_RESULT_IND					0x55
#define MSG_RESULT_RES					0x56	/**< Result response, was started via MSG_RESULT_REQ_IND */

#define MSG_GET_SHIELD_INFO_IND			0x57
#define MSG_GET_SHIELD_INFO_RES         0x58

#define MSG_GET_RAW_DATA_IND			0x59
#define MSG_GET_RAW_DATA_RES			0x6A

#define MSG_ADV_RESULT_IND				0x61
#define MSG_ADV_RESULT_RES				0x62

#define MSG_GET_SHIELD_INFO_RES_V2      0x63

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

uint16_t ep_radar_d2gl_sc_fmcw_handle_message(uint8_t endpoint, uint8_t* message_data,
											uint16_t num_bytes,	void* context);

void ep_radar_d2gl_sc_fmcw_handle_change(uint8_t endpoint, void* context,
                                  	   uint32_t what);

uint16_t send_parameter_values(uint8_t endpoint);

void send_frame_data(uint8_t endpoint, acq_buf_obj *p_acq_buf, device_settings_t *p_dev);

void send_result_response( uint8_t endpoint, sc_fmcw_result_response_t *p_pulse_result  );

void send_adv_result_response( uint8_t endpoint, sc_fmcw_result_response_t *p_pulse_result  );

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_ENDPOINTRADAR_D2GL_SC_FMCW_H_ */

/* --- End of File -------------------------------------------------------- */
