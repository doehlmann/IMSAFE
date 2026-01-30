/**
    @file: main.c

    @brief: This application runs on demo Distance2GoL board with BGT24LTR11 and XMC4700 MCU.
            It consists on radar demonstration application.
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

#include "application.h"

/*
==============================================================================
   2. MAIN METHOD
==============================================================================
 */




void UART0_RX_handler( void )
{

}


int main(void)
{
  DAVE_STATUS_t status;
  
  /* Initialize DAVE APPs */
  status = DAVE_Init();
  

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");
    while(1U);
  }
  
  /* Register algorithm processing function:
     Set the algorithm processing function pointer, it will
     be used by the application for algorithm data processing */
    app_register_algo_process(algorithm_do);

  /* Initialize the application */
  app_init();
  
  /* Infinite loop */
  while(1U)
  {
    /* Main application process */
    app_process();
  }
}

void acq_completed_cb(void)
{
  /*
  The following code shows an example of how to access raw data buffer
  
  acq_buf_obj *p_acq_buf 	= ds_get_active_acq_buf();
  uint8_t *raw_data 		= p_acq_buf->p_acq_buf;
  uint32_t raw_data_size 	= p_acq_buf->used_size_of_acq_buffer;
  
  -- Add your code here --
  
  */
}

void algo_completed_cb(void)
{

}

/* --- End of File -------------------------------------------------------- */
