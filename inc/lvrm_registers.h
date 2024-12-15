/*
 * lvrm_registers.h
 *
 *  Created on: 12 mar. 2022
 *      Author: Ludo
 */

#ifndef __LVRM_REGISTERS_H__
#define __LVRM_REGISTERS_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "common_registers.h"
#include "types.h"
#include "una.h"

/*** LVRM REGISTERS structures ***/

/*!******************************************************************
 * \enum LVRM_register_address_t
 * \brief LVRM registers map.
 *******************************************************************/
typedef enum {
    LVRM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    LVRM_REGISTER_ADDRESS_CONFIGURATION_1,
    LVRM_REGISTER_ADDRESS_CONFIGURATION_2,
    LVRM_REGISTER_ADDRESS_STATUS_1,
    LVRM_REGISTER_ADDRESS_CONTROL_1,
    LVRM_REGISTER_ADDRESS_ANALOG_DATA_1,
    LVRM_REGISTER_ADDRESS_ANALOG_DATA_2,
    LVRM_REGISTER_ADDRESS_LAST,
} LVRM_register_address_t;

/*** LVRM REGISTERS macros ***/

#define LVRM_NUMBER_OF_SPECIFIC_REGISTERS                       (LVRM_REGISTER_ADDRESS_LAST - COMMON_REGISTER_ADDRESS_LAST)

#define LVRM_REGISTER_CONFIGURATION_0_MASK_BMSF                 0x00000001
#define LVRM_REGISTER_CONFIGURATION_0_MASK_RLFH                 0x00000002

#define LVRM_REGISTER_CONFIGURATION_1_MASK_VBATT_LOW_THRESHOLD  0x0000FFFF
#define LVRM_REGISTER_CONFIGURATION_1_MASK_VBATT_HIGH_THRESHOLD 0xFFFF0000

#define LVRM_REGISTER_CONFIGURATION_2_MASK_IOUT_OFFSET          0x0000FFFF

#define LVRM_REGISTER_STATUS_1_MASK_RLSTST                      0x00000003

#define LVRM_REGISTER_CONTROL_1_MASK_RLST                       0x00000001
#define LVRM_REGISTER_CONTROL_1_MASK_ZCCT                       0x00000002

#define LVRM_REGISTER_ANALOG_DATA_1_MASK_VCOM                   0x0000FFFF
#define LVRM_REGISTER_ANALOG_DATA_1_MASK_VOUT                   0xFFFF0000

#define LVRM_REGISTER_ANALOG_DATA_2_MASK_IOUT                   0x0000FFFF

/*** LVRM REGISTERS global variables ***/

#ifdef UNA_AT_USE_REGISTER_ACCESS
extern const UNA_register_access_t LVRM_REGISTER_ACCESS[LVRM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ACCESS_TIMEOUT
extern const uint32_t LVRM_REGISTER_ACCESS_TIMEOUT_MS[LVRM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ERROR_VALUE
extern const uint32_t LVRM_REGISTER_ERROR_VALUE[LVRM_REGISTER_ADDRESS_LAST];
#endif

#endif /* __LVRM_REGISTERS_H__ */
