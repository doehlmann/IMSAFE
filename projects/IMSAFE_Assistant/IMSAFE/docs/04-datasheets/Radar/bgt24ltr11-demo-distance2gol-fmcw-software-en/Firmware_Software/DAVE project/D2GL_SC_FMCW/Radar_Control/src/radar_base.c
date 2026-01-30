/**
    @file: radar_base.c

    @brief: This file contains the basic driver API for Infineon radar sensors.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "d2gl_bsp.h"
#include "d2gl_bsp_limits.h"
#include "version.h"
#include "radar_api.h"
#include "radar_control.h"

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

/* Global Data Buffer used to store the raw IQ data samples for FMCW chirps,
   first (SAMPLES_PER_CHIRP * 2) samples are for I&Q data for RX1,
   followed by (SAMPLES_PER_CHIRP * 2) samples for I&Q data for RX2 */
uint32_t data_fifo[BSP_MAX_ADC_BUFFER_SIZE_BYTES/sizeof(uint32_t)];  
                                   					 	 	 	 	 	
static const Driver_Version_t driver_version_s =
{
  /*.uMajor    =*/ FW_VERSION_MAJOR,
  /*.uMinor    =*/ FW_VERSION_MINOR,
  /*.uRevision =*/ FW_VERSION_REVISION
};

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

const Driver_Version_t* radar_get_driver_version(void)
{
  return (&driver_version_s);
}

//============================================================================

uint16_t radar_open_device(int32_t device_number, Radar_Handle_t* device)
{
  XMC_UNUSED_ARG(device_number); /* Suppress compiler warnings of unused variable */
  
  uint16_t retValue = RADAR_ERR_OK;
  int32_t status = 0;
  acq_buf_obj acq_buf = { .status = not_initialized } ;

  /* Initialize the driver instance with NULL, until it was successfully initialized */
  if( device )
	  *device = NULL;

  /* Initialize driver data structure */
  /* -------------------------------- */
  status = ds_init();
  if(status > 0)
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }

  device_settings_t *p_dev = ds_device_get_settings();
  
  //=========================================================================
  //------------------------- General configurations ------------------------
  //=========================================================================
  
  bsp_limit_dev_settings(p_dev);
  
  /* Check frame memory limits */
  /* ------------------------- */
  size_t frame_size = p_dev->num_of_chirps_per_frame * p_dev->num_samples_per_chirp *
		  p_dev->num_of_adc_channels * sizeof(uint16_t); // complex IQ samples per antenna
  
  if (frame_size > BSP_MAX_ADC_BUFFER_SIZE_BYTES)
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* Initialize sensor hardware */
  /* -------------------------- */
  acq_buf = create_acq_data_buffer_obj((uint8_t*) data_fifo, BSP_MAX_ADC_BUFFER_SIZE_BYTES,
                                       p_dev->num_of_chirps_per_frame,
									   p_dev->num_samples_per_chirp, p_dev->num_of_adc_channels, p_dev->adc_resolution, 0);
  
  ds_set_active_acq_buf(acq_buf);
 
  
  /* BGT, DMA, Timers initialization */
  /* -------------------------------------------------- */
  retValue = radar_init(device);
  
  g_adc_sampling_completed = false;

  return (retValue);
}

//============================================================================

void radar_close_device(Radar_Handle_t device)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  bsp_bgt_shut_down();
  bsp_bb_power_down();
}

//============================================================================

uint16_t radar_get_temperature(Radar_Handle_t device, uint8_t temp_sensor, int32_t* temperature_001C)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Check temperature sensor, only single sensor supported */
  if (temp_sensor == 0)
  {
    *temperature_001C = (int32_t) (bsp_bgt_get_temp_value() * 1000.0f);	// (expected value is actual temperature in C° x 1000)
    
    return (RADAR_ERR_OK);
  }
  else
  {
    return (RADAR_ERR_SENSOR_DOES_NOT_EXIST);
  }
}

//============================================================================

uint16_t radar_get_tx_power(Radar_Handle_t device, uint8_t tx_antenna, int32_t* tx_power_001dBm)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Check number of TX antenna */
  if (tx_antenna == 0)
  {
    *tx_power_001dBm = 0; // Not supported by BGTLTR TX power
    
    return (RADAR_ERR_OK);
  }
  
  return (RADAR_ERR_ANTENNA_DOES_NOT_EXIST);
}

//============================================================================

uint16_t radar_get_min_frame_interval(Radar_Handle_t device, uint32_t* min_frame_interval_usec)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */

  device_settings_t *p_dev = ds_device_get_settings();

  *min_frame_interval_usec = bsp_calc_min_frame_interval_usec(p_dev);

  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_start_frame_timer(Radar_Handle_t device, uint32_t frame_interval_usec)
{
  /* Stop trigger timer first  */
  /* ------------------------- */
  bsp_timer_stop_clear(&TIMER_FRAME_TRIG);
  
  /* Restart trigger timer */
  /* --------------------- */
  if (frame_interval_usec > 0)
  {
    uint32_t min_frame_interval_usec;
    
    radar_get_min_frame_interval(device, &min_frame_interval_usec);

    if (frame_interval_usec < min_frame_interval_usec)
    {
      return (RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL);
    }
  
    /* Update time period of Automatic Trigger */
    if (TIMER_SetTimeInterval(&TIMER_FRAME_TRIG, frame_interval_usec * 100U) != TIMER_STATUS_SUCCESS)
    {
      return (RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL);
    }

    device_settings_t *pDev = ds_device_get_settings();
    
    pDev->frame_period_usec = frame_interval_usec;

    /* Reset the current frame counter */
    bsp_reset_frame_counter();

    g_adc_sampling_completed = false;

    bsp_timer_start(&TIMER_FRAME_TRIG);
  }
  
  return (RADAR_ERR_OK);
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
