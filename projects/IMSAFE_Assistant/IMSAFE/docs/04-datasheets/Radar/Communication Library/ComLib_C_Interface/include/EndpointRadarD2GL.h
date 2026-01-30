/**
 * \file EndpointRadarD2GL.h
 *
 * \brief This file contains the API methods applicable generically to 
 *        Industrial radar demo Distance2GoL
 *
 */
 
/* ===========================================================================
** Copyright (C) 2017-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef ENDPOINTRADARD2GL_H_INCLUDED
#define ENDPOINTRADARD2GL_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>
#include <stdbool.h>
#include "EndpointRadarErrorCodes.h"
#include "radar_common.h"
/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



typedef struct {
	uint32_t rf_frequency_khz;
	uint32_t bandwidth_mhz;
	uint8_t  major_version;
	uint8_t  minor_version;
	uint8_t  num_tx_antenna;
	uint8_t  num_rx_antenna;

	char *rf_shield_type_id;
	char *description;
	char *modulation;
	float up_chirp_time_s;
} fmcw_get_shield_info_cnf_t;

typedef struct {
	void *pData;
} fmcw_get_shield_info_cnf_ctx_t;

/**
 * \defgroup EndpointRadarD2GL_SC_FMCW EndpointRadarD2GL_SC_FMCW
 *
 * \brief EndpointRadarD2GL_SC_FMCW documentation.
 *
 * @{
 */				   

typedef struct {
	float 		max_distance_m;
	float 		min_distance_m;
	float 		frame_time_sec;
	uint16_t 	number_of_samples_nu;
	uint32_t  	hold_threshold;
	uint32_t 	activation_threshold;
	uint8_t		number_of_chirps_per_frame;
    uint8_t     continuous_mode_bool;
    uint8_t 	baseband_gain_stage_bool;
    float		pulse_repetition_rate_sec;
	bool		activate_cfar;
} fmcw_para_values_t;


typedef struct {

	float max_distance_m_lower_boundary;
	float max_distance_m_upper_boundary;
	float max_distance_m_step_size;

	float min_distance_m_lower_boundary;
	float min_distance_m_upper_boundary;
	float min_distance_m_step_size;

	float frame_time_sec_lower_boundary;
	float frame_time_sec_upper_boundary;
	float frame_time_sec_step_size;

	uint8_t   size_elems;
	uint16_t   *p_sample_list;

	uint32_t   hold_threshold_lower_boundary;
	uint32_t   hold_threshold_upper_boundary;
	uint32_t   hold_threshold_step_size;

	uint32_t   activation_threshold_lower_boundary;
	uint32_t   activation_threshold_upper_boundary;
	uint32_t   activation_threshold_step_size;

	uint8_t	   number_of_chirps_per_frame_lower_boundary;
	uint8_t	   number_of_chirps_per_frame_upper_boundary;
	uint8_t	   number_of_chirps_per_frame_step_size;

	float	   pulse_repetition_time_sec_lower_boundary;
	float	   pulse_repetition_time_sec_upper_boundary;
	float	   pulse_repetition_time_sec_step_size;

} fmcw_para_value_def_host_t;


typedef struct {
	void *pData;		// currently the is nothing special to the data
} fmcw_para_values_def_cnf_ctx_t;


typedef struct {
	uint32_t  result_count;
	uint32_t  frame_number;

	float     range_m;
	float     strength;
	float	  velocity_mps;
	uint8_t   target_status;   			/* not detected: 0, static: 1, Transit: 2, moving: 3 */
	uint8_t   direction_of_movement;       /* ... */
	
} fmcw_result_cnf_t;


typedef struct {
	uint32_t  result_count;
	uint32_t  frame_number;

	float     range_m;
	float     strength;
	float	  velocity_mps;
	uint8_t   target_status;   			/* not detected: 0, static: 1, Transit: 2, moving: 3 */
	uint8_t   direction_of_movement;       /* ... */

	uint16_t  num_activation_data;
	uint16_t  num_hold_data;

	float 	   *p_activation_data;
	float	   *p_hold_data;
} fmcw_adv_result_cnf_t;


/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a Radar D2GL
*        endpoint.
*
* This function checks type and version of the specified endpoint in a
* connected device and returns a code that indicates if that endpoint is
* compatible to the radar endpoint implementation in this module.
*
* \param[in] protocol_handle  A handle to the connection to the sensor
*                             device.
* \param[in] endpoint         The endpoint in the connected device to be
*                             checked for compatibility.
*
* \return If the specified endpoint is compatible to this implementation the
*         function returns 0. If the endpoint is not compatible, a negative
*         error code is returned.
*/
int32_t ep_radar_d2gl_scfmcw_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint);


/**
* \brief This is the callback type executed on a parameter value confirmation event,
*        triggered, by a parameter value request command.
*
* *
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] ps_para_value     pointer to static memory of type fmcw_para_values_t, holding
* 							   the parameter values.
*/
typedef void(*fmcw_para_values_cnf_callback_t)(void *p_context, int32_t protocol_handle,
											uint8_t endpoint, fmcw_para_values_t *ps_para_values);


/**
* \brief This is the callback type executed on a parameter value confirmation event,
*        triggered, by a parameter value request command.
*
* *
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] ps_para_value     pointer to static memory of type fmcw_para_values_t, holding
* 							   the parameter values.
*/
typedef void(*fmcw_para_values_def_cnf_callback_t)(fmcw_para_values_def_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, fmcw_para_value_def_host_t *ps_para_values);


typedef void(*fmcw_result_cnf_callback_t)(void *p_context, int32_t protocol_handle,
											uint8_t endpoint, fmcw_result_cnf_t *ps_para_values);

typedef void(*fmcw_adv_result_cnf_callback_t)(void *p_context, int32_t protocol_handle,
											uint8_t endpoint, fmcw_adv_result_cnf_t *ps_para_values);

typedef void(*fmcw_get_shield_info_cnf_callback_t)(fmcw_get_shield_info_cnf_ctx_t *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_get_shield_info_cnf_t *ps_temperature);


/**
* \brief This function is used to request the parameter values,
*
* results in callback of type fmcw_para_values_cnf_callback_t
*
* \param[in] protocol_handle  A handle to the connection to the sensor
*                             device.
* \param[in] endpoint         The endpoint in the connected device to be
*                             checked for compatibility.
*
* \return If the specified endpoint is compatible to this implementation the
*         function returns 0. If the endpoint is not compatible, a negative
*         error code is returned.
*/
int32_t ep_radar_d2gl_scfmcw_para_value_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_d2gl_scfmcw_para_value_def_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_d2gl_scfmcw_para_set_value_req(int32_t protocol_handle, uint8_t endpoint, fmcw_para_values_t *p_data);

int32_t ep_radar_d2gl_scfmcw_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);

int32_t ep_radar_d2gl_scfmcw_adv_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);

int32_t ep_radar_d2gl_scfmcw_get_shield_info_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_d2gl_scfmcw_get_raw_data_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);


fmcw_para_values_cnf_callback_t ep_radar_d2gl_scfmcw_set_para_value_cb(fmcw_para_values_cnf_callback_t callback,
								void* p_context, void **prev_context);

void ep_radar_d2gl_scfmcw_set_para_def_cb(fmcw_para_values_def_cnf_callback_t callback,
								fmcw_para_values_def_cnf_ctx_t* p_context);

void ep_radar_d2gl_scfmcw_set_result_cb(fmcw_result_cnf_callback_t callback,
								void* p_context);

void ep_radar_d2gl_scfmcw_set_adv_result_cb(fmcw_adv_result_cnf_callback_t callback,
								void* p_context);

void ep_radar_d2gl_scfmcw_set_shield_info_cb(fmcw_get_shield_info_cnf_callback_t callback,
					fmcw_get_shield_info_cnf_ctx_t* p_context);

void ep_radar_d2gl_scfmcw_set_raw_data_cb(get_raw_data_cnf_callback_t callback,
					void* p_context);


/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARD2GL_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
