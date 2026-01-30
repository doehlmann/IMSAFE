/**
 * \file EndPointRadarD2GL.c
 *
 * \brief This file implements the API to communicate with radar Sence2GoL-Pulse
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndPointRadarD2GL.h for more information.
 */

/* ===========================================================================
** Copyright (C) 2017 - 2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include "EndpointRadarD2GL.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/**
 * \internal
 * \defgroup EndpointRadarIndustrialCommandCodes
 *
 * \brief The radar industrial endpoint defined the following commands. 
 *
 * Each payload message of the radar industrial endpoint starts with one of these
 * command codes.
 *
 * @{
 */

#define MSG_PARA_VALUES_REQ 				0x50  	/**< A message to request the parameter values. */
#define MSG_PARA_VALUES_CNF		 			0x51  	/**< A message to request the parameter values. */
#define MSG_PARA_VALUES_DEF_REQ				0x52	/**< A message to request the default parameter list values */
#define MSG_PARA_VALUES_DEF_CNF				0x53	/**< A message containing the parameters variation */
#define MSG_PARA_SET_VALUES_REQ				0x54    /**< A message to set the parameter values */
#define MSG_RESULT_REQ			    		0x55    /**< A message to set the parameter values */
#define MSG_RESULT_CNF			    		0x56    /**< A message to set the parameter values */


#define FMCW_MSG_GET_SHIELD_INFO_REQ		0x57
#define FMCW_MSG_GET_SHIELD_INFO_CNF		0x58
#define FMCW_MSG_GET_SHIELD_INFO_CNF_V2		0x63


#define FMCW_MSG_GET_RAW_DATA_REQ			0x59
#define FMCW_MSG_GET_RAW_DATA_CNF			0x6A

#define MSG_ADV_RESULT_REQ					0x61
#define MSG_ADV_RESULT_CNF					0x62


/** @} */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/
/**
 * \internal
 * See module \ref EndpointRadarErrorCodes.c for information.
 */
extern const char* ep_radar_get_error_code_description(uint16_t error_code);

/**
 * \internal
 * \brief This function is called to parse a payload message.
 *
 * Whenever a connected device sends a payload message from an endpoint that
 * is of type and version compatible to the implementation is this module,
 * this function is invoked to parse the content of that message.
 *
 * \param[in] protocol_handle  The handle to the connection, the message was
 *                             received through.
 * \param[in] endpoint         The endpoint the message was sent from.
 * \param[in] payload          A pointer to the buffer containing the payload
 *                             to parse.
 * \param[in] payload_size     The number of bytes in payload.
 */
static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size);

/*
==============================================================================
   4. DATA
==============================================================================
*/
/**
 * \internal
 * \brief This structure contains information about this module needed by the
 *        module Protocol.c
 *
 * The structure contains the endpoint type and version range that is
 * supported by this module. The number identifying the endpoint type is the
 * ASCII code for 'RIND' (= Radar Industrial).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and to retrieve human readable status/error messages from status
 * and error codes. Those function are called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_RadarD2GL_definition =
{
    /*.type             =*/ 0x544A5346, /* ASCII code 'TJSF' = D2GL-SC-FMCW (via "software controlled fmcw")*/
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar D2GL SC FMCW",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointRadarP2GRegisteredCallbacks
 *
 * \brief The registered callback function are stored here.
 *
 * For each message type that is expected from the connected device a callback
 * function can be registered. That registered callback function is stored
 * here along with a data pointer that is forwarded the callback function each
 * time it is issued.
 *
 * @{
 */

/**
* \brief The callback function to handle the parameter value indication.
*/
static fmcw_para_values_cnf_callback_t para_values_cnf_cb = NULL;
static void *para_values_cnf_ctx = NULL;

static fmcw_para_values_def_cnf_callback_t para_values_def_cnf_cb = NULL;
static fmcw_para_values_def_cnf_ctx_t *para_values_def_cnf_ctx = NULL;

static fmcw_result_cnf_callback_t result_cnf_cb = NULL;
static void *result_cnf_ctx = NULL;

static fmcw_adv_result_cnf_callback_t adv_result_cnf_cb = NULL;
static void *adv_result_cnf_ctx = NULL;

static fmcw_get_shield_info_cnf_callback_t fmcw_get_shield_info_cnf_cb = NULL;
static fmcw_get_shield_info_cnf_ctx_t *fmcw_get_shield_info_cnf_ctx = NULL;

static get_raw_data_cnf_callback_t get_raw_data_cnf_cb = NULL;
static get_raw_data_cnf_ctx_t *get_raw_data_cnf_ctx = NULL;

/** @} */



/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t parse_frame_info(int32_t protocol_handle, uint8_t endpoint,
                                const uint8_t* payload,
                                uint16_t payload_size)
{
	get_raw_data_cnf_t frame_info;
	uint32_t total_samples;
	uint32_t expected_message_size;

	uint8_t header_size = 18;

	/* read header info from payload */
	frame_info.frame_number =
		protocol_read_payload_uint32(payload, 1);
	frame_info.num_chirps = protocol_read_payload_uint32(payload, 5);
	frame_info.num_rx_antennas =
		protocol_read_payload_uint8(payload, 9);
	frame_info.num_samples_per_chirp =
		protocol_read_payload_uint32(payload, 10);
	frame_info.rx_mask = protocol_read_payload_uint8(payload, 14);
	frame_info.data_format =
		(Rx_Data_Format_t)protocol_read_payload_uint8(payload, 15);
	frame_info.adc_resolution =
		protocol_read_payload_uint8(payload, 16);
	frame_info.interleaved_rx =
		protocol_read_payload_uint8(payload, 17);

	/* calculate expected message size */
	total_samples = frame_info.num_chirps *
					frame_info.num_samples_per_chirp *
					frame_info.num_rx_antennas *
					(frame_info.data_format == EP_RADAR_BASE_RX_DATA_REAL ? 1 : 2);

	expected_message_size = total_samples * frame_info.adc_resolution;
	expected_message_size = header_size +
							(expected_message_size >> 3) +
							((expected_message_size & 0x07) ? 1 : 0);

	if (payload_size == expected_message_size)
	{
		uint32_t sample_bit_mask = (1 << frame_info.adc_resolution) - 1;
		const float norm_factor = 1.f / (float)sample_bit_mask;
		float *sample_data = (float *)malloc(total_samples *
											 sizeof(float));
		float *write_ptr = sample_data;

		uint16_t read_idx = header_size;
		uint16_t read_bit_position = 0;

		while (total_samples)
		{
			uint32_t current_sample =
				protocol_read_payload_uint16(payload, read_idx) >>
				read_bit_position;
			*write_ptr++ = (current_sample & sample_bit_mask) *
						   norm_factor;

			read_bit_position += frame_info.adc_resolution;
			read_idx += read_bit_position >> 3;
			read_bit_position &= 0x07;

			--total_samples;
		}

		/* send frame info to callback */
		frame_info.sample_data = sample_data;
		get_raw_data_cnf_cb(get_raw_data_cnf_ctx, protocol_handle,
							endpoint, &frame_info);
		free(sample_data);
	}

	return 1;
}

static int32_t parse_para_values_cnf(int32_t protocol_handle, uint8_t endpoint,
								const uint8_t* payload, uint16_t payload_size)
{
	uint8_t payloadId = protocol_read_payload_uint8(payload, 0);

	if ((payloadId == MSG_PARA_VALUES_CNF) &&
		(payload_size >= 26))
	{
		if (para_values_cnf_cb)
		{
			fmcw_para_values_t para_values;
			uint16_t offset = 1;

			para_values.max_distance_m    = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.min_distance_m = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.frame_time_sec = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.number_of_samples_nu    = protocol_read_payload_uint16(&payload[0], offset);
			offset += sizeof(uint16_t);
			para_values.hold_threshold = protocol_read_payload_uint32(&payload[0], offset);
			offset += sizeof(uint32_t);
			para_values.activation_threshold = protocol_read_payload_uint32(&payload[0], offset);
			offset += sizeof(uint32_t);
			para_values.number_of_chirps_per_frame    = protocol_read_payload_uint8(&payload[0], offset);
			offset += sizeof(uint8_t);
			para_values.continuous_mode_bool = protocol_read_payload_uint8(&payload[0], offset);
			offset += sizeof(uint8_t);
			para_values.baseband_gain_stage_bool = protocol_read_payload_uint8(&payload[0], offset);
			offset += sizeof(uint8_t);
			if(payload_size >= offset + sizeof( float) )
			{
				para_values.pulse_repetition_rate_sec = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
			}
			else
				para_values.pulse_repetition_rate_sec = 0.0; // don't leave value uninitialized!
			if (payload_size >= offset + sizeof(uint8_t))
			{
				uint8_t u8_activate_cfar = protocol_read_payload_uint8(&payload[0], offset);
				para_values.activate_cfar = (u8_activate_cfar > 0 ? true : false);
				offset += sizeof(uint8_t);
			}
			else
				para_values.activate_cfar = false; // don't leave value uninitialized!

			/* send frame format to callback */
			para_values_cnf_cb(para_values_cnf_ctx, protocol_handle,
									   endpoint, &para_values);
		}
		return 1;
	}
	else if ((payloadId == MSG_PARA_VALUES_DEF_CNF) &&
			(payload_size > 1))
		{
			if (para_values_def_cnf_cb)
			{
				// TODO: move this out into a separate function!
				fmcw_para_value_def_host_t para_value_def;

				// pre-init optional attribute
				para_value_def.pulse_repetition_time_sec_lower_boundary = 0.0;
				para_value_def.pulse_repetition_time_sec_upper_boundary = 0.0;
				para_value_def.pulse_repetition_time_sec_step_size = 0.0;


				uint16_t offset = 1; // skip message id
				(void)protocol_read_payload_uint16(&payload[0], offset); // last_message_size not used 
				offset += sizeof(uint16_t);

				para_value_def.max_distance_m_lower_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.max_distance_m_upper_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.max_distance_m_step_size = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);

				para_value_def.min_distance_m_lower_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.min_distance_m_upper_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.min_distance_m_step_size = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);

				para_value_def.frame_time_sec_lower_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.frame_time_sec_upper_boundary = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				para_value_def.frame_time_sec_step_size = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);

				para_value_def.size_elems = protocol_read_payload_uint8( &payload[0], offset);
				offset += sizeof(uint8_t);
				para_value_def.p_sample_list = (uint16_t *)malloc(para_value_def.size_elems*sizeof(uint16_t));
				for(uint16_t n= 0; n < para_value_def.size_elems; n++)
				{
					para_value_def.p_sample_list[n] = protocol_read_payload_uint16( &payload[0], offset);
					offset += sizeof(uint16_t);
				}


				para_value_def.hold_threshold_lower_boundary = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.hold_threshold_upper_boundary = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.hold_threshold_step_size = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);

				para_value_def.activation_threshold_lower_boundary = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.activation_threshold_upper_boundary = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.activation_threshold_step_size = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);

				para_value_def.number_of_chirps_per_frame_lower_boundary = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				para_value_def.number_of_chirps_per_frame_upper_boundary = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				para_value_def.number_of_chirps_per_frame_step_size = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				if( payload_size >= offset + sizeof(float) )
				{
					para_value_def.pulse_repetition_time_sec_lower_boundary = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);
				}

				if( payload_size >= offset + sizeof(float) )
				{
					para_value_def.pulse_repetition_time_sec_upper_boundary = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);
				}

				if( payload_size >= offset + sizeof(float) )
				{
					para_value_def.pulse_repetition_time_sec_step_size = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);
				}

				/* send frame format to callback */
				para_values_def_cnf_cb(para_values_def_cnf_ctx, protocol_handle,
										   endpoint, &para_value_def);

				// free-up memory!!
				free(para_value_def.p_sample_list);

			}
			return 1;
		}
		else  if ((payloadId == MSG_RESULT_CNF) &&
				(payload_size == 23))
		{
			if (result_cnf_cb)
			{
				fmcw_result_cnf_t result;

				uint16_t offset = 1;

				result.result_count = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				result.frame_number = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				result.range_m = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				result.strength = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				result.velocity_mps = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				result.target_status = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				result.direction_of_movement = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				/* send result to callback */
				result_cnf_cb(result_cnf_ctx, protocol_handle,
										   endpoint, &result);

			}
			return 1;
		}
		else  if ((payloadId == MSG_ADV_RESULT_CNF) &&
				(payload_size >1))
		{
			if (adv_result_cnf_cb)
			{
				fmcw_adv_result_cnf_t adv_result;

				uint16_t offset = 1;

				adv_result.result_count = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				adv_result.frame_number = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				adv_result.range_m = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				adv_result.strength = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				adv_result.velocity_mps = protocol_read_payload_float32(&payload[0], offset);
				offset += sizeof(float);
				adv_result.target_status = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				adv_result.direction_of_movement = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				adv_result.num_activation_data = protocol_read_payload_uint16(&payload[0], offset);
				offset += sizeof(uint16_t);
				adv_result.p_activation_data = (float *)malloc(adv_result.num_activation_data*sizeof(float));
				if (adv_result.num_activation_data > 0)
				{
					for (uint32_t i = 0; i < adv_result.num_activation_data; i++)
					{
						adv_result.p_activation_data[i] = protocol_read_payload_float32(&payload[0], offset);
						offset += sizeof(float);
					}
				}

				adv_result.num_hold_data = protocol_read_payload_uint16(&payload[0], offset);
				offset += sizeof(uint16_t);
				adv_result.p_hold_data = (float *)malloc(adv_result.num_hold_data*sizeof(float));
				if (adv_result.num_hold_data > 0)
				{
					for (uint32_t i = 0; i < adv_result.num_hold_data; i++)
					{
						adv_result.p_hold_data[i] = protocol_read_payload_float32(&payload[0], offset);
						offset += sizeof(float);
					}
				}

				/* send result to callback */
				adv_result_cnf_cb(adv_result_cnf_ctx, protocol_handle,
										   endpoint, &adv_result);
										   
				if (adv_result.p_activation_data != NULL)
				{
					free(adv_result.p_activation_data);
				}
				if (adv_result.p_hold_data != NULL)
				{
					free(adv_result.p_hold_data);
				}

			}
			return 1;
		}
		else if((payloadId == FMCW_MSG_GET_SHIELD_INFO_CNF) && (payload_size >1))
		{
			fmcw_get_shield_info_cnf_t shield_info = {0};
			uint16_t offset = 1;

			if( fmcw_get_shield_info_cnf_cb ) 
            {

				shield_info.rf_frequency_khz  = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				shield_info.major_version  = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.minor_version  = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.num_tx_antenna  = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.num_rx_antenna  = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				shield_info.bandwidth_mhz = 200; // Assume 200 value for old firmware

				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.rf_shield_type_id); // NOTE: Called board id in FW
	
				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.description);

				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.modulation);

				fmcw_get_shield_info_cnf_cb(fmcw_get_shield_info_cnf_ctx, protocol_handle, endpoint, &shield_info);

				if (shield_info.rf_shield_type_id != NULL)
				{
						free(shield_info.rf_shield_type_id);
				}
				if (shield_info.description != NULL)
				{
						free(shield_info.description);
				}
				if (shield_info.modulation != NULL)
				{
						free(shield_info.modulation);
				}
			}
		}
		else if ((payloadId == FMCW_MSG_GET_SHIELD_INFO_CNF_V2) && (payload_size > 1))
		{
			fmcw_get_shield_info_cnf_t shield_info = { 0 };
			uint16_t offset = 1;

			if (fmcw_get_shield_info_cnf_cb) 
            {
				shield_info.rf_frequency_khz = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				shield_info.bandwidth_mhz = protocol_read_payload_uint32(&payload[0], offset);
				offset += sizeof(uint32_t);
				shield_info.major_version = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.minor_version = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.num_tx_antenna = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);
				shield_info.num_rx_antenna = protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.rf_shield_type_id); // NOTE: Called board id in FW

				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.description);

				offset += protocol_read_payload_string(&payload[0], offset, &shield_info.modulation);
				if (payload_size >= offset + sizeof(float))
				{
					shield_info.up_chirp_time_s = protocol_read_payload_float32(&payload[0], offset );
					offset += sizeof(float);
				}

				fmcw_get_shield_info_cnf_cb(fmcw_get_shield_info_cnf_ctx, protocol_handle, endpoint, &shield_info);
				
				if (shield_info.rf_shield_type_id != NULL)
				{
					free(shield_info.rf_shield_type_id);
				}
				if (shield_info.description != NULL)
				{
					free(shield_info.description);
				}
				if (shield_info.modulation != NULL)
				{
					free(shield_info.modulation);
				}

			}
		}
		else if ((payloadId == FMCW_MSG_GET_RAW_DATA_CNF) && (payload_size >= 18))
		{
			parse_frame_info(protocol_handle, endpoint, payload, payload_size);
		}
		else
		{
			printf("not found\n");
		}

	return 0;
}


static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful
     */
	if (parse_para_values_cnf(protocol_handle, endpoint,
		payload, payload_size))
		return;
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_radar_d2gl_scfmcw_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_RadarD2GL_definition);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

fmcw_para_values_cnf_callback_t ep_radar_d2gl_scfmcw_set_para_value_cb(fmcw_para_values_cnf_callback_t callback,
								void* p_context, void **prev_context)
{
	/* save and return previous cb and context, so that it could be restore aferwards! */
	fmcw_para_values_cnf_callback_t previous_cb = para_values_cnf_cb;
	if (prev_context != NULL)
		*prev_context = para_values_cnf_ctx;

	para_values_cnf_cb 		 = callback;
	para_values_cnf_ctx 	 = p_context;
	return previous_cb;
}


void ep_radar_d2gl_scfmcw_set_para_def_cb(fmcw_para_values_def_cnf_callback_t callback,
								fmcw_para_values_def_cnf_ctx_t* p_context)
{
	para_values_def_cnf_cb 		 = callback;
	para_values_def_cnf_ctx 	 = p_context;
}

void ep_radar_d2gl_scfmcw_set_result_cb(fmcw_result_cnf_callback_t callback,
								void* p_context)
{
	result_cnf_cb 	 = callback;
	result_cnf_ctx 	 = p_context;
}

void ep_radar_d2gl_scfmcw_set_adv_result_cb(fmcw_adv_result_cnf_callback_t callback,
								void* p_context)
{
	adv_result_cnf_cb 	 = callback;
	adv_result_cnf_ctx 	 = p_context;
}

void ep_radar_d2gl_scfmcw_set_shield_info_cb(fmcw_get_shield_info_cnf_callback_t callback,
					fmcw_get_shield_info_cnf_ctx_t* p_context)
{
	fmcw_get_shield_info_cnf_cb 	 = callback;
	fmcw_get_shield_info_cnf_ctx  = p_context;
}


void ep_radar_d2gl_scfmcw_set_raw_data_cb(get_raw_data_cnf_callback_t callback,
		void* p_context)
{
	get_raw_data_cnf_cb 	 = callback;
	get_raw_data_cnf_ctx 	 = p_context;
}



int32_t ep_radar_d2gl_scfmcw_para_value_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_VALUES_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_d2gl_scfmcw_para_value_def_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_VALUES_DEF_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_d2gl_scfmcw_para_set_value_req(int32_t protocol_handle, uint8_t endpoint, fmcw_para_values_t *p_data)
{
	/* build message to send */
	uint8_t cmd_message[100];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_PARA_SET_VALUES_REQ);

	offset += protocol_write_payload_float32(cmd_message, offset, p_data->max_distance_m);
	offset += protocol_write_payload_float32(cmd_message, offset, p_data->min_distance_m);
	offset += protocol_write_payload_float32(cmd_message, offset, p_data->frame_time_sec);
	offset += protocol_write_payload_uint16(cmd_message, offset, p_data->number_of_samples_nu);
	offset += protocol_write_payload_uint32(cmd_message, offset, p_data->hold_threshold);
	offset += protocol_write_payload_uint32(cmd_message, offset, p_data->activation_threshold);
	offset += protocol_write_payload_uint8(cmd_message, offset, p_data->number_of_chirps_per_frame);
	offset += protocol_write_payload_uint8(cmd_message, offset, p_data->continuous_mode_bool);
	offset += protocol_write_payload_uint8(cmd_message, offset, p_data->baseband_gain_stage_bool);
	offset += protocol_write_payload_float32(cmd_message, offset, p_data->pulse_repetition_rate_sec);
	offset += protocol_write_payload_uint8(cmd_message, offset, p_data->activate_cfar);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, offset);
}

/*
 *  if repeat_count is 0xFFFF.FFFF the results will be continuously provided, unless stopped (with repeat_count set to 0)
 *  if repeat_count is 1, only a single result is returned, and the next result needs to be retriggerd
 *  in case you like to avoid extra latency, by the round trip request, set the result cnt to  0xfffffffff, and a continuous stream of results will be received.
 */

int32_t ep_radar_d2gl_scfmcw_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_RESULT_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, offset);
}

int32_t ep_radar_d2gl_scfmcw_adv_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_ADV_RESULT_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, offset);
}

int32_t ep_radar_d2gl_scfmcw_get_shield_info_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, FMCW_MSG_GET_SHIELD_INFO_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, offset);
}



int32_t ep_radar_d2gl_scfmcw_get_raw_data_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, FMCW_MSG_GET_RAW_DATA_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarD2GL_definition,
									cmd_message, offset);
}



/* --- End of File -------------------------------------------------------- */
