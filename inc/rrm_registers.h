/*
 * rrm_registers.h
 *
 *  Created on: 27 nov. 2022
 *      Author: Ludo
 */

#ifndef __RRM_REGISTERS_H__
#define __RRM_REGISTERS_H__

#include "common_registers.h"

/*** RRM REGISTERS structures ***/

/*!******************************************************************
 * \enum RRM_register_address_t
 * \brief RRM registers map.
 *******************************************************************/
typedef enum {
    RRM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    RRM_REGISTER_ADDRESS_CONFIGURATION_1,
    RRM_REGISTER_ADDRESS_STATUS_1,
    RRM_REGISTER_ADDRESS_CONTROL_1,
    RRM_REGISTER_ADDRESS_ANALOG_DATA_1,
    RRM_REGISTER_ADDRESS_ANALOG_DATA_2,
    RRM_REGISTER_ADDRESS_LAST,
} RRM_register_address_t;

/*** RRM REGISTERS macros ***/

#define RRM_NUMBER_OF_SPECIFIC_REGISTERS                (RRM_REGISTER_ADDRESS_LAST - COMMON_REGISTER_ADDRESS_LAST)

#define RRM_REGISTER_CONFIGURATION_0_MASK_RFH           0x00000001

#define RRM_REGISTER_CONFIGURATION_1_MASK_IOUT_OFFSET   0x0000FFFF

#define RRM_REGISTER_STATUS_1_MASK_RENST                0x00000003

#define RRM_REGISTER_CONTROL_1_MASK_REN                 0x00000001
#define RRM_REGISTER_CONTROL_1_MASK_ZCCT                0x00000002

#define RRM_REGISTER_ANALOG_DATA_1_MASK_VIN             0x0000FFFF
#define RRM_REGISTER_ANALOG_DATA_1_MASK_VOUT            0xFFFF0000

#define RRM_REGISTER_ANALOG_DATA_2_MASK_IOUT            0x0000FFFF

#endif /* __RRM_REGISTERS_H__ */
