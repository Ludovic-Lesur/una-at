/*
 * dmm_registers.h
 *
 *  Created on: 14 jun. 2023
 *      Author: Ludo
 */

#ifndef __DMM_REGISTERS_H__
#define __DMM_REGISTERS_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "common_registers.h"
#include "types.h"
#include "una.h"

/*** DMM REGISTERS structures ***/

/*!******************************************************************
 * \enum DMM_register_address_t
 * \brief DMM registers map.
 *******************************************************************/
typedef enum {
    DMM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    DMM_REGISTER_ADDRESS_STATUS_1,
    DMM_REGISTER_ADDRESS_CONTROL_1,
    DMM_REGISTER_ADDRESS_ANALOG_DATA_1,
    DMM_REGISTER_ADDRESS_ANALOG_DATA_2,
    DMM_REGISTER_ADDRESS_LAST,
} DMM_register_address_t;

/*** DMM REGISTERS macros ***/

#define DMM_NUMBER_OF_SPECIFIC_REGISTERS                    (DMM_REGISTER_ADDRESS_LAST - COMMON_REGISTER_ADDRESS_LAST)

#define DMM_REGISTER_CONFIGURATION_0_MASK_NODES_SCAN_PERIOD 0x000000FF
#define DMM_REGISTER_CONFIGURATION_0_MASK_UL_PERIOD         0x0000FF00
#define DMM_REGISTER_CONFIGURATION_0_MASK_DL_PERIOD         0x00FF0000

#define DMM_REGISTER_STATUS_1_MASK_NODES_COUNT              0x000000FF

#define DMM_REGISTER_CONTROL_1_MASK_STRG                    0x00000001

#define DMM_REGISTER_ANALOG_DATA_1_MASK_VRS                 0x0000FFFF
#define DMM_REGISTER_ANALOG_DATA_1_MASK_VHMI                0xFFFF0000

#define DMM_REGISTER_ANALOG_DATA_2_MASK_VUSB                0x0000FFFF

/*** DMM REGISTERS global variables ***/

#ifdef UNA_AT_USE_REGISTER_ACCESS
extern const UNA_register_access_t DMM_REGISTER_ACCESS[DMM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ACCESS_TIMEOUT
extern const uint32_t DMM_REGISTER_ACCESS_TIMEOUT_MS[DMM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ERROR_VALUE
extern const uint32_t DMM_REGISTER_ERROR_VALUE[DMM_REGISTER_ADDRESS_LAST];
#endif

#endif /* __DMM_REGISTERS_H__ */
