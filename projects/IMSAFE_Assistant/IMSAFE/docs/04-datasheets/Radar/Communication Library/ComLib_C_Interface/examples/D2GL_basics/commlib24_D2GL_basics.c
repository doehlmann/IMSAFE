/**
   @file: commlib24_D2GL_basics.c

   This file implements a short test of the communication library. The library
   is used to connect to a sensor board, retrieve some raw radar data and
   print this data to the console. The raw radar data is printed to standard
   output. Information messages are printed to standard error output.
   
   If multiple boards are connected to the host, the this test connects to the
   board with the smallest COM port number.
*/


/* ===========================================================================
** Copyright (C) 2016-2021 Infineon Technologies AG
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

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// needed to retrieve the timestamp in milliseconds and Sleep
#include <Windows.h>

#include "COMPort.h"
#include "Protocol.h"
#include "EndpointRadarBaseBoard.h"
#include "EndpointRadarD2GL.h"


/*
==============================================================================
   2. LOCAL TYPES
==============================================================================
*/


/*
==============================================================================
   3. DATA
==============================================================================
*/

static int32_t protocol_handle = -1;
static uint8_t endpoint_mcu_base = 0;
static uint8_t endpoint_d2gl = 0;

volatile boolean completed = false;
uint32_t max_count = 20;
/*
==============================================================================
   4. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
* This function retrieves the list of available COM ports and tries to open
* the COM ports one after the other until a compatible port has been found.

* @return If a compatible endpoint has been found, a protocol handle to that
*         port is returned. Otherwise a negative error code is returned.
*/
static int32_t connect_to_sensor(void);

/**
* This function retrieves the list of available communication endpoints and
* assigns them to the global handles.

* @return If all needed endpoints for this demo are found positive return
* 		   code is returned. Otherwise a negative number is returned.
*/
static int32_t query_communications_endpoints(void);

/**
* This function configures the chip the the needed configuration for this
* demo. The settings are the power on reset default settings of the chip.

* @return If configuration is successful returned error code is equal to 0.
* 		   Otherwise a non zero error code is returned.
*/
static int32_t configure_sensor(void);

/**
* This function prints an error message in case something goes wrong and
* disconnects the chip.

* @param[in] err_code         This is the error code received from the
* 							operation that raised it.
*/
static void on_error(int32_t err_code);


/*
==============================================================================
   5. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t connect_to_sensor(void)
{
	char com_port_list[512];
	char* current_port;

	/* retrieve a list of all available com ports */
	com_port_list[0] = 0;
	com_get_port_list(com_port_list, sizeof(com_port_list));

	/* iterate over the port list */
	current_port = com_port_list;
	while (current_port[0])
	{
		int32_t protocol_handle;

		/*
		* find end of current port name in port list
		* (port name is delimited by other ';' or '\0'
		*/
		char* next_port = current_port;
		while (next_port[0])
		{
			if (next_port[0] == ';')
			{
				/* replace ';' by '\0' to indicate end of current port name */
				*next_port = 0;

				/* after a ';' another port name follows */
				++next_port;
				break;
			}
			++next_port;
		}

		/* try to connect to current COM port */
		printf("Trying to open COM port: %s ... ", current_port);
		protocol_handle = protocol_connect(current_port);

		if (protocol_handle >= 0)
		{
			printf("OK.\n");
			return protocol_handle;
		}
		else
		{
			printf("Failed.\n");
		}

		/* prepare for next iteration */
		current_port = next_port;
	}

	return -1;
}

static int32_t query_communications_endpoints(void)
{
	int32_t num_endpoints;
	uint8_t i;

	/* check number of endpoints */
	num_endpoints = protocol_get_num_endpoints(protocol_handle);
	if (num_endpoints < 0)
	{
		printf("Error: Could not find any endpoint.\n");
		return -1;
	}

	printf("Number of detected endpoints: %d\n", num_endpoints);
	/* search for endpoint radar base */
	printf("Searching for endpoint \"Radar Base\" ... ");
	for (i = 1; i <= num_endpoints; ++i)
	{
		if (ep_radar_base_board_is_compatible_endpoint(protocol_handle, i) == 0)
		{
			endpoint_mcu_base = i;
			break;
		}
	}
	printf("%s\n", (endpoint_mcu_base != 0) ? "OK." : "Failed!");

	/* search for endpoint radar base */
	printf("Searching for endpoint \"%s\" ... ", "D2GL-SC-FMCW");
	for (i = 1; i <= num_endpoints; ++i)
	{
		if (ep_radar_d2gl_scfmcw_is_compatible_endpoint(protocol_handle, i) == 0)
		{
			endpoint_d2gl = i;
			break;
		}
	}
	printf("%s\n", (endpoint_d2gl != 0) ? "OK." : "Failed!");


	if (endpoint_mcu_base == 0 || endpoint_d2gl == 0)
	{
		printf("Error: Not all required communication endpoints found for this demo\n");
		return -1;
	}
	else
	{
		return 1;
	}
}

static int32_t configure_sensor(void)
{
	int32_t return_code = 0;

	return return_code;
}

static void on_error(int32_t err_code)
{
	printf("Error Code 0x%06x: %s\n", err_code,
		protocol_get_status_code_description(protocol_handle,
			err_code));

	/* Close the connection */
	/* ----------------------------- */
	printf("Connection is closed now.\n ");
	if (protocol_handle >= 0)
	{
		protocol_disconnect(protocol_handle);
	}
}

fmcw_para_values_t *log_fmcw_para_value(fmcw_para_values_t *pData, char *p_title)
{
	if( p_title)
		printf("%s\n", p_title);

	printf("\t:max_distance              : %.8f m\n", pData->max_distance_m);
	printf("\t:min_distance              : %.8f m\n", pData->min_distance_m);
	printf("\t:frame_time                : %.2f msec\n", pData->frame_time_sec*1e3);
	printf("\t:number_of_samples         : %u\n", pData->number_of_samples_nu);
	printf("\t:hold_threshold            : %u\n", pData->hold_threshold);
	printf("\t:activation_threshold      : %u\n", pData->activation_threshold);
	printf("\t:number_of_chirps_per_frame : %u\n", pData->number_of_chirps_per_frame);
	printf("\t:continuous_mode           : %s\n", (pData->continuous_mode_bool ? "true" : "false"));
	printf("\t:baseband_gain_stage       : %s\n", (pData->baseband_gain_stage_bool ? "true" : "false"));
	printf("\t:pulse_repetition_rate     : %.2f msec\n", pData->pulse_repetition_rate_sec*1e3);
	printf("\t:activate cfar             : %s\n", (pData->activate_cfar == true) ? "true" : "false");
	
	return pData;
}

void mcubase_consumption_cnf_callback(consumption_cnf_ctx_t *p_context, int32_t protocol_handle,
	uint8_t endpoint, consumption_cnf_t *p_consumption)
{
	printf("mcubase_consumption_cnf_callback called\n");
	printf("\telements: %d\n", p_consumption->elements);
	for (size_t i = 0; i < p_consumption->elements; i++)
	{
		consumption_t *pData = (p_consumption->p_consumption + i);
		printf("\t\t%d: ", pData->index);
		printf("%f %s\n", pData->value, pData->p_unit);
	}
	fflush(stdout);
}

void mcubase_consumption_def_cnf_callback(consumption_def_cnf_ctx_t *p_context, int32_t protocol_handle,
	uint8_t endpoint, consumption_def_host_t *p_consumption)
{
	printf("mcubase_consumption_def_cnf_callback called\n");
	printf("\telements: %d\n", p_consumption->consumption_label_elements);
	for (size_t i = 0; i < p_consumption->consumption_label_elements; i++)
	{
		printf("\t\t%s\n", &p_consumption->pp_consumption_labels[i][0]);
	}
	fflush(stdout);
}

void mcubase_para_store_callback(para_store_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, para_store_cnf_t *p_data)
{
	printf("mcubase_para_store_callback called\n");
	printf("\tno data is available!\n");

	fflush(stdout);
}

void mcubase_para_factory_reset_callback(para_factory_reset_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, para_factory_reset_t *p_data)
{
	printf("mcubase_para_factory_reset_callback called\n");
	printf("\tno data is available!\n");
	fflush(stdout);
}

void mcubase_board_info_callback(board_info_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, board_info_cnf_t *p_data)
{
	printf("mcubase_board_info_callback called\n");
	printf("\tmajor version: %d\n", p_data->major_version);
	printf("\tminor version: %d\n", p_data->minor_version);
	printf("\trf shield type id: %s\n", p_data->rf_shield_type_id);
	printf("\tdescription: %s\n", p_data->description);
	fflush(stdout);
}




void fmcw_para_value_callback(void *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_para_values_t *ps_para_values)
{
	fmcw_para_values_t *pData = ps_para_values;

	log_fmcw_para_value(pData, "fmcw_para_value_callback:");

	fflush(stdout);
}




static void fmcw_para_value_def_callback(fmcw_para_values_def_cnf_ctx_t *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_para_value_def_host_t *ps_para_values)
{

	fmcw_para_value_def_host_t *pData = ps_para_values;

	printf("fmcw_para_value_def_callback:\n");

	printf("\t:max_distance lower_boundary               : %.2f m\n", pData->max_distance_m_lower_boundary);
	printf("\t:max_distance upper_boundary               : %.2f m\n", pData->max_distance_m_upper_boundary);
	printf("\t:max_distance step_size                    : %.3f m\n", pData->max_distance_m_step_size);

	printf("\t:min_distance lower_boundary               : %.1f m\n", pData->min_distance_m_lower_boundary);
	printf("\t:min_distance upper_boundary               : %.1f m\n", pData->min_distance_m_upper_boundary);
	printf("\t:min_distance step_size                    : %.3f m\n", pData->min_distance_m_step_size);

	printf("\t:frame time lower boundary                 : %f msec\n", pData->frame_time_sec_lower_boundary*1e3);
	printf("\t:frame time upper boundary                 : %f msec\n", pData->frame_time_sec_upper_boundary*1e3);
	printf("\t:frame_time step_size                      : %.0f usec\n", pData->frame_time_sec_step_size*1e6);

	printf("\t:frame rate upper boundary                 : %f Hz\n", 1.0/pData->frame_time_sec_lower_boundary);
	printf("\t:frame rate lower boundary                 : %f Hz\n", 1.0/pData->frame_time_sec_upper_boundary);

	printf("\t:size_elems: %d \n", pData->size_elems);
	for (int i = 0; i < pData->size_elems; i++)
	{
		printf("\t\t%d", pData->p_sample_list[i]);
	}
	printf("\n");

	printf("\t:hold_threshold_lower_boundary             : %u\n", pData->hold_threshold_lower_boundary);
	printf("\t:hold_threshold_upper_boundary             : %u\n", pData->hold_threshold_upper_boundary);
	printf("\t:hold_threshold_step_size                  : %u\n", pData->hold_threshold_step_size);
	printf("\t:activation_threshold_lower_boundary       : %u\n", pData->activation_threshold_lower_boundary);
	printf("\t:activation_threshold_upper_boundary       : %u\n", pData->activation_threshold_upper_boundary);
	printf("\t:activation_threshold_step_size            : %u\n", pData->activation_threshold_step_size);

	printf("\t:number_of_chirps_per_frame_lower_boundary : %u\n", pData->number_of_chirps_per_frame_lower_boundary);
	printf("\t:number_of_chirps_per_frame_upper_boundary : %u\n", pData->number_of_chirps_per_frame_upper_boundary);
	printf("\t:number_of_chirps_per_frame_step_size       : %u\n", pData->number_of_chirps_per_frame_step_size);


	printf("\t:pulse_repetition_rate lower_boundary      : %.2f msec\n", pData->pulse_repetition_time_sec_lower_boundary*1e3);
	printf("\t:pulse_repetition_rate upper_boundary      : %.2f msec\n", pData->pulse_repetition_time_sec_upper_boundary*1e3);
	printf("\t:pulse_repetition_rate step_size           : %.0f usec\n", pData->pulse_repetition_time_sec_step_size*1e6);

	fflush(stdout);

}



void fmcw_result_cnf_callback(void *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_result_cnf_t *p_result)
{
	static uint32_t result_count = 0;
	if (result_count > max_count)
		completed = true;
	result_count++;

	char statusArray[][20] = { "NOT_DETECTED", "STATIC_TARGET", "UNSURE_TARGET", "MOVING_TARGET" };
	char dirArray[][20] = { "NOT_DETECTED", "STATIC", "APPROACHING", "DEPARTING" };
	printf("fmcw_result_cnf_callback called\n");

	printf("\tresult cnt:                %d\n", p_result->result_count);
	printf("\tframe number:              %d\n", p_result->frame_number);

	printf("\trange:                     %f m\n", p_result->range_m);
	printf("\tstrength:                  %f\n", p_result->strength);
	printf("\tvelocity:                  %f m/s\n", p_result->velocity_mps);

	printf("\ttarget_status:             %s\n", statusArray[p_result->target_status]);
	printf("\tdirection_of_movement:     %s\n", dirArray[p_result->direction_of_movement]);

}



typedef struct {
	bool show_output_on_target;
	bool show_output;
	bool show_activation_data;
	bool show_hold_data;
} show_result_options_t;

typedef struct {
	bool show_raw_data_header;
	bool show_raw_data;
} show_raw_data_options_t;



void fmcw_adv_result_cnf_callback(void *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_adv_result_cnf_t *p_result)
{

	char statusArray[][20] = { "NOT_DETECTED", "STATIC_TARGET", "UNSURE_TARGET", "MOVING_TARGET" };
	char dirArray[][20] = { "NOT_DETECTED", "STATIC", "APPROACHING", "DEPARTING" };

	show_result_options_t *p_options = (show_result_options_t *) p_context;
	if (p_options->show_output)
	{

		printf("fmcw_adv_result_cnf_callback called\n");

		if (p_options->show_output)
		{

			bool show_details = true;
			if (p_options->show_output_on_target && p_result->target_status == 0)
			{
				show_details = false;
			}

			if (show_details)
			{
				printf("\tresult cnt:             %d\n", p_result->result_count);
				printf("\tframe number:           %d\n", p_result->frame_number);

				printf("\trange [m]:              %f\n", p_result->range_m);
				printf("\tstrength:               %f\n", p_result->strength);
				printf("\tvelocity_mps:           %f\n", p_result->velocity_mps);

				printf("\ttarget_status:          %s\n", statusArray[p_result->target_status]);
				printf("\tdirection_of_movement:  %s\n", dirArray[p_result->direction_of_movement]);
				printf("\tfft size:               256\n");			// Hard coded in device!
			}

			if (p_options->show_activation_data)
			{
				printf("\tnum_activation_data :  %u\n", p_result->num_activation_data);
				for (uint32_t i = 0; i < p_result->num_activation_data; i++)
				{
					printf("activation threshhold data[%d]: %f\n", i, p_result->p_activation_data[i]);
				}
			}

			if (p_options->show_activation_data)
			{
				printf("\tnum_hold_data :  %u\n", p_result->num_hold_data);
				for (uint32_t i = 0; i < p_result->num_hold_data; i++)
				{
					printf("hold treshhold data[%d]: %f\n", i, p_result->p_hold_data[i]);
				}
			}
		}
		fflush(stdout);
	}
}

void fmcw_get_shield_info_cnf_callback(fmcw_get_shield_info_cnf_ctx_t*p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_get_shield_info_cnf_t *p_result)
{
	printf("\trf_frequency:                  %d kHz\n", p_result->rf_frequency_khz);
	printf("\tbandwidth:                     %d MHz\n", p_result->bandwidth_mhz);
	printf("\tmajor version:                 %d\n", p_result->major_version);
	printf("\tminor version:                 %d\n", p_result->minor_version);
	printf("\tnum_tx_antenna:                %d\n", p_result->num_tx_antenna);
	printf("\tnum_rx_antenna:                %d\n", p_result->num_rx_antenna);
	printf("\trf shield type id:             %s\n", p_result->rf_shield_type_id);
	printf("\tdescription:                   %s\n", p_result->description);
	printf("\tmodulation:                    %s\n", p_result->modulation);
	printf("\tup_chirp_time:                 %.0f usec\n", p_result->up_chirp_time_s*1e6);

	fflush(stdout);

}

typedef struct
{
	uint8_t used_rx[8];
	uint8_t used_rx_count;

}RX_Info_t;

static void proces_rx_info(uint8_t rx_mask, RX_Info_t *rx_info)
{

	int i = 0;
	while (i < 8)
	{
		if (((rx_mask >> i) & 0x01) == 1)
		{
			rx_info->used_rx[rx_info->used_rx_count] = i + 1;
			rx_info->used_rx_count++;
		}
		i++;
	}
}

// called every time ep_radar_base_get_frame_data method is called to return measured time domain signals
static void process_received_data(const get_raw_data_cnf_t* frame_info)
{
	// Print the sampled data which can be found in frame_info->sample_data

	/**
	* for complex data format (default):
	* 		-------------------------------------------------------------------------------------------------------------
	* 		| 	ADC 1				|	ADC 2				| 	ADC 1					|	ADC 2						|
	*     	| 	Samples per Chirp	| 	Samples per Chirp 	| 	Samples per Chirp 		|  	Samples per Chirp	 		|
	* 		-------------------------------------------------------------------------------------------------------------
	* 		| 				RX 1							| 				RX2											|
	* 		-------------------------------------------------------------------------------------------------------------
	* 		| 				One Chirp data size  = Sample per Chirp * Num of RX Antenes * Number of channels			|
	* 	 	-------------------------------------------------------------------------------------------------------------
	* */
	RX_Info_t rx_info = { { 0 }, 0 };

	proces_rx_info(frame_info->rx_mask, &rx_info);

	uint32_t num_chan_per_ant = (frame_info->data_format == EP_RADAR_BASE_RX_DATA_REAL ? 1 : 2);
	/* TODO:
	I think in the  the follwoing two line it should be rx_info.used_rx_count instead of the frame_info->num_rx_antennas.
	In the EndPointRadarBase.c of FW it should actually also be rx_info.used_rx_count. Need Discussion.
	*/
	uint32_t chrip_inc = frame_info->num_samples_per_chirp * num_chan_per_ant * frame_info->num_rx_antennas;
	uint32_t total_samle_count = frame_info->num_chirps *
		frame_info->num_samples_per_chirp *
		frame_info->num_rx_antennas *
		num_chan_per_ant;
	uint8_t chrip_nr = 0;
	uint8_t rx_nr = 0;
	uint8_t adc_nr = 0;
	uint8_t sample_nr = 0;

	for (uint32_t i = 0; i < total_samle_count; i++)
	{
		chrip_nr = (i / chrip_inc);
		rx_nr = ((i % chrip_inc) / (num_chan_per_ant * frame_info->num_samples_per_chirp));
		adc_nr = ((i % (chrip_inc / rx_info.used_rx_count)) / (frame_info->num_samples_per_chirp));
		sample_nr = (i % frame_info->num_samples_per_chirp);
		printf("\t\t");
		printf("%d, ", frame_info->frame_number);
		printf("%d, ", chrip_nr);
		printf("%d, ", rx_info.used_rx[rx_nr]);
		printf("%d, ", adc_nr);
		printf("%d, ", sample_nr);
		printf("%f\n", frame_info->sample_data[i]);

	}
	return;
}


void fmcw_get_raw_data_cnf_callback(get_raw_data_cnf_ctx_t *p_context, int32_t protocol_handle,
	uint8_t endpoint, get_raw_data_cnf_t *p_raw_data)
{

	show_raw_data_options_t *p_show_raw_data_options = (show_raw_data_options_t*)p_context;
	static int count = 0;
	printf("fmcw_get_raw_data_cnf_callback called: %d, %d\r\n", count, p_raw_data->frame_number);

	if (p_show_raw_data_options && p_show_raw_data_options->show_raw_data_header)
	{
		printf("\tframe number:          %d\n", p_raw_data->frame_number);
		printf("\tnum_chirps:            %d\n", p_raw_data->num_chirps);
		printf("\tnum_samples_per_chirp: %d\n", p_raw_data->num_samples_per_chirp);
		printf("\trx_mask:               %d\n", p_raw_data->rx_mask);
	}

	if(p_show_raw_data_options && p_show_raw_data_options->show_raw_data)
	{
		printf(" ----------------- raw data -------------------------\n");
		process_received_data(p_raw_data);
		printf(" ----------------------------------------------------\n");
	}
	count++;

}


int32_t fmcw_set_parameter_helper(uint32_t protocol_handle, uint8_t endpoint, fmcw_para_values_t *p_para, char *text)
{
	int32_t status_code;
	
	log_fmcw_para_value(p_para, text);

	status_code = ep_radar_d2gl_scfmcw_para_set_value_req(protocol_handle, endpoint, p_para);

	printf("req_result: 0x%04x, %s\n", status_code & 0xFFFF,
		protocol_get_status_code_description(protocol_handle, status_code));

	if ((status_code & 0xFFFF) == PROTOCOL_STATUS_OK)
	{
		Sleep(200);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);
	}
	return status_code;
}

static bool signal_parameter_receive = false;

void fmcw_para_value_copy_callback(void *p_context, int32_t protocol_handle,
	uint8_t endpoint, fmcw_para_values_t *ps_para_values)
{
	fmcw_para_values_t *pData = ps_para_values;
	if (p_context != NULL)
	{
		memcpy(p_context, ps_para_values, sizeof(fmcw_para_values_t));
	}
	signal_parameter_receive = true;
}


bool read_parameter_helper(fmcw_para_values_t *p_para)
{
	uint32_t timeoutCount = 20; 
	void *p_prev_context = NULL;
	
	signal_parameter_receive = false;
	fmcw_para_values_cnf_callback_t prev_cb = ep_radar_d2gl_scfmcw_set_para_value_cb(fmcw_para_value_copy_callback, p_para, &p_prev_context);
	ep_radar_d2gl_scfmcw_para_value_req(protocol_handle, endpoint_d2gl);
	while(!signal_parameter_receive && timeoutCount > 0)
	{
		Sleep(50);
		timeoutCount--;
	}
	/* register the old callback again! */
	ep_radar_d2gl_scfmcw_set_para_value_cb(prev_cb, p_prev_context, NULL);

	if (timeoutCount == 0)
	{
		printf("***** ERROR: No response!!\n");
		fflush(stdout);
		return false;
	}
	return true;
}

/*
==============================================================================
4. EXPORTED FUNCTIONS
==============================================================================
*/


int main()
{
	int32_t return_code = 0;


	/* connect to a sensor board */
	/* ------------------------- */
	protocol_handle = connect_to_sensor();
	if (protocol_handle < 0)
	{
		printf("Error: Could not find a sensor board.\n");
		return protocol_handle;
	}

	/* search for relevant endpoints */
	/* ----------------------------- */
	if (query_communications_endpoints() == 1)
	{
		/* configure chip to power on reset parameters */
		return_code = configure_sensor();
		if (return_code & 0xFFFF)
		{
			on_error(return_code);
			return (return_code & 0xFFFF);
		}

		/* callback settings: */

		show_result_options_t show_result_options = {
			.show_output = false,
			.show_output_on_target = false,
			.show_activation_data = false,
		    .show_hold_data = false,
		};


		printf("---- register callbacks ----\n");

		ep_radar_d2gl_scfmcw_set_shield_info_cb(fmcw_get_shield_info_cnf_callback, NULL);
		ep_radar_d2gl_scfmcw_set_raw_data_cb(fmcw_get_raw_data_cnf_callback, NULL);
		ep_radar_d2gl_scfmcw_set_para_value_cb(fmcw_para_value_callback, NULL, NULL);
		ep_radar_d2gl_scfmcw_set_para_def_cb(fmcw_para_value_def_callback, NULL);
		ep_radar_d2gl_scfmcw_set_result_cb(fmcw_result_cnf_callback, NULL);
		ep_radar_d2gl_scfmcw_set_adv_result_cb(fmcw_adv_result_cnf_callback, 
			&(show_result_options_t) { 	.show_output = false, .show_output_on_target = false, .show_activation_data = false, .show_hold_data = false } );

		ep_radar_base_board_consumption_cb(mcubase_consumption_cnf_callback, NULL);
		ep_radar_base_board_consumption_def_cb(mcubase_consumption_def_cnf_callback, NULL);
		ep_radar_base_board_para_store_cb(mcubase_para_store_callback, NULL);
		ep_radar_base_board_para_factory_reset_cb(mcubase_para_factory_reset_callback, NULL);
		ep_radar_base_board_board_info_cb(mcubase_board_info_callback, NULL);

		{
			printf("Trying to get a device info ...");

			Firmware_Information_t fw_info;
			return_code = protocol_get_firmware_information(protocol_handle, &fw_info);

			printf("%s\n", ((return_code & 0xFFFF) == 0) ? "OK." : "Failed!");
			if ((return_code & 0xFFFF))
			{
				goto ON_ERROR;
			}

			printf("FW info: \"%s\": %d.%d.%d\n", fw_info.description, fw_info.version_major, fw_info.version_minor, fw_info.version_build);
			fflush(stdout);
		}


		printf("---- fmcw: get shield info ----\n");
		ep_radar_d2gl_scfmcw_get_shield_info_req(protocol_handle, endpoint_d2gl);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);





		{
			printf("---- mcu base: factory reset ----\n");
			ep_radar_base_board_para_factory_reset_req(protocol_handle, endpoint_mcu_base);
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}

		{
			printf("---- fmcw: get parameter definition ----\n");
			ep_radar_d2gl_scfmcw_para_value_def_req(protocol_handle, endpoint_d2gl);
			fflush(stdout);
			Sleep(500);
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}

		fmcw_para_values_t settings; 
		if (read_parameter_helper(&settings))
		{
			log_fmcw_para_value(&settings, "read parameter: ");
		}
		else
		{
			printf("**** can't read parameter! \n");
			
		}
		fflush(stdout);

		settings.number_of_chirps_per_frame = 5;
		settings.frame_time_sec = 100 * 1e-3F;
				
		printf("**** apply new settings now: ... \n");
		fflush(stdout);

		return_code = fmcw_set_parameter_helper(protocol_handle, endpoint_d2gl, &settings, "settings for the test");
		if ((return_code & 0xFFFF) > 0)
			goto ON_ERROR;

		ep_radar_d2gl_scfmcw_set_adv_result_cb(fmcw_adv_result_cnf_callback, 
				&(show_result_options_t) { 	.show_output = true, .show_output_on_target = true, .show_activation_data = true, .show_hold_data = true } );

		return_code = ep_radar_d2gl_scfmcw_adv_result_req(protocol_handle, endpoint_d2gl, 50);
		if ((return_code & 0xFFFF) > 0)
			goto ON_ERROR;
		
		ep_radar_d2gl_scfmcw_set_raw_data_cb(fmcw_get_raw_data_cnf_callback, 
			    &(show_raw_data_options_t) { .show_raw_data_header = true, .show_raw_data = true } );

		return_code = ep_radar_d2gl_scfmcw_get_raw_data_req(protocol_handle, endpoint_d2gl, 50);
		if ((return_code & 0xFFFF) > 0)
			goto ON_ERROR;

		printf("start getting data....");
		fflush(stdout);
		uint32_t count = 20;
		while (count)
		{
			printf("count: %d\n", count);
			Sleep(50);
			protocol_poll_receive_once(protocol_handle);
			count--;
		}

		printf("stop getting data....\n");
		fflush(stdout);
		return_code = ep_radar_d2gl_scfmcw_adv_result_req(protocol_handle, endpoint_d2gl, 0);
		if ((return_code & 0xFFFF) > 0)
			goto ON_ERROR;

		return_code = ep_radar_d2gl_scfmcw_get_raw_data_req(protocol_handle, endpoint_d2gl, 0);
		if ((return_code & 0xFFFF) > 0)
			goto ON_ERROR;
	} //  if (query_communications_endpoints() == 1)
	printf("Wait for termination!\n");
	fflush(stdout);
	Sleep(5000);

ON_ERROR:
	if ((return_code & 0xFFFF))
	{
		printf("Error Code 0x%06x: %d, %s\n", return_code, return_code,
			protocol_get_status_code_description(protocol_handle, return_code));
	}

	/* finally, close the connection */
	/* ----------------------------- */
	printf("Connection is closed now.\n ");
	protocol_disconnect(protocol_handle);
	fflush(stdout);

	return (return_code & 0xFFFF);
}
