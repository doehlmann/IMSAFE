/**
    @file  datastore.h
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

#ifndef DATASTORE_H_
#define DATASTORE_H_

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

#include <stdint.h>
#include <stdbool.h>
#include "acq_buffer.h"
#include "device_settings.h"
#include "algorithm_exports.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(test_for_true) _Static_assert((test_for_true), "(" #test_for_true ") failed")
#endif

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

typedef struct default_settings_check
{
	uint8_t def_dev_settings_crc;
	uint8_t def_algo_settings_crc;
}eep_default_settings_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

uint32_t ds_default_settings_check(void);

void ds_algo_store_settings(algo_settings_t *p_algo_settings);

void ds_algo_fetch_settings(algo_settings_t *p_algo_settings);

uint8_t ds_algo_apply_staged_settings(void);

algo_settings_t *ds_algo_get_settings(void);

uint32_t ds_init(void);

device_settings_t *ds_device_get_settings(void);

device_settings_t *ds_device_get_shadow_settings(void);

uint8_t ds_device_apply_staged_settings(void);

uint32_t ds_is_device_settings_updated(void);

void ds_device_settings_copy_to_shadow(void);

void ds_device_settings_revert_from_shadow(void);

acq_buf_obj *ds_get_active_acq_buf(void);

void ds_set_active_acq_buf(const acq_buf_obj acq_buf);

/*
 * setter and getter for algo results
 */
void ds_store_algo_result_ptr(algo_result_t *p_result);

algo_result_t *ds_get_algo_result_ptr(void);

/*
 * trigger a factory reset of the parameters
 */
void ds_hw_para_factory_reset(void);

void ds_hw_para_store_persistent(void);

void ds_algo_para_factory_reset(void);

void ds_algo_para_store_persistent(void);

uint32_t ds_current_device_settings_is_default(void);

uint32_t ds_current_algo_settings_is_default(void);

void ds_set_device_parameter_changed(void);

void ds_clear_device_parameter_changed(void);

uint32_t ds_is_device_parameter_changed(void);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DATASTORE_H_ */

/* --- End of File -------------------------------------------------------- */
