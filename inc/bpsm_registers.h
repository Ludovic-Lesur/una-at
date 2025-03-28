/*
 * bpsm_registers.h
 *
 *  Created on: 27 nov. 2022
 *      Author: Ludo
 */

#ifndef __BPSM_REGISTERS_H__
#define __BPSM_REGISTERS_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "common_registers.h"
#include "types.h"
#include "una.h"

/*** BPSM REGISTERS structures ***/

/*!******************************************************************
 * \enum BPSM_register_address_t
 * \brief BPSM registers map.
 *******************************************************************/
typedef enum {
    BPSM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    BPSM_REGISTER_ADDRESS_CONFIGURATION_1,
    BPSM_REGISTER_ADDRESS_CONFIGURATION_2,
    BPSM_REGISTER_ADDRESS_STATUS_1,
    BPSM_REGISTER_ADDRESS_CONTROL_1,
    BPSM_REGISTER_ADDRESS_ANALOG_DATA_1,
    BPSM_REGISTER_ADDRESS_ANALOG_DATA_2,
    BPSM_REGISTER_ADDRESS_LAST,
} BPSM_register_address_t;

/*** BPSM REGISTERS macros ***/

#define BPSM_NUMBER_OF_SPECIFIC_REGISTERS                       (BPSM_REGISTER_ADDRESS_LAST - COMMON_REGISTER_ADDRESS_LAST)

#define BPSM_REGISTER_CONFIGURATION_0_MASK_CEFH                 0x00000001
#define BPSM_REGISTER_CONFIGURATION_0_MASK_CSFH                 0x00000002
#define BPSM_REGISTER_CONFIGURATION_0_MASK_BKFH                 0x00000008

#define BPSM_REGISTER_CONFIGURATION_1_MASK_CHEN_THRESHOLD       0x0000FFFF
#define BPSM_REGISTER_CONFIGURATION_1_MASK_CHEN_TOGGLE_PERIOD   0x00FF0000

#define BPSM_REGISTER_CONFIGURATION_2_MASK_LVF_LOW_THRESHOLD    0x0000FFFF
#define BPSM_REGISTER_CONFIGURATION_2_MASK_LVF_HIGH_THRESHOLD   0xFFFF0000

#define BPSM_REGISTER_STATUS_1_MASK_BKENST                      0x00000003
#define BPSM_REGISTER_STATUS_1_MASK_CHENST                      0x0000000C
#define BPSM_REGISTER_STATUS_1_MASK_CHRGST                      0x00000030
#define BPSM_REGISTER_STATUS_1_MASK_LVF                         0x00000100

#define BPSM_REGISTER_CONTROL_1_MASK_BKEN                       0x00000001
#define BPSM_REGISTER_CONTROL_1_MASK_CHMD                       0x00000002
#define BPSM_REGISTER_CONTROL_1_MASK_CHEN                       0x00000004

#define BPSM_REGISTER_ANALOG_DATA_1_MASK_VSRC                   0x0000FFFF
#define BPSM_REGISTER_ANALOG_DATA_1_MASK_VSTR                   0xFFFF0000

#define BPSM_REGISTER_ANALOG_DATA_2_MASK_VBKP                   0x0000FFFF

/*** BPSM REGISTERS global variables ***/

#ifdef UNA_AT_USE_REGISTER_ACCESS
extern const UNA_register_access_t BPSM_REGISTER_ACCESS[BPSM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ACCESS_TIMEOUT
extern const uint32_t BPSM_REGISTER_ACCESS_TIMEOUT_MS[BPSM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ERROR_VALUE
extern const uint32_t BPSM_REGISTER_ERROR_VALUE[BPSM_REGISTER_ADDRESS_LAST];
#endif

#endif /* __BPSM_REGISTERS_H__ */
