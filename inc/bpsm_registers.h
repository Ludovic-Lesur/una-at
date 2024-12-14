/*
 * bpsm_registers.h
 *
 *  Created on: 27 nov. 2022
 *      Author: Ludo
 */

#ifndef __BPSM_REGISTERS_H__
#define __BPSM_REGISTERS_H__

#include "common_registers.h"

/*** BPSM REGISTERS structures ***/

/*!******************************************************************
 * \enum BPSM_register_address_t
 * \brief BPSM registers map.
 *******************************************************************/
typedef enum {
    BPSM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    BPSM_REGISTER_ADDRESS_CONFIGURATION_1,
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
#define BPSM_REGISTER_CONFIGURATION_0_MASK_BKFH                 0x00000004
#define BPSM_REGISTER_CONFIGURATION_0_MASK_VSTR_RATIO           0x0000FF00

#define BPSM_REGISTER_CONFIGURATION_1_MASK_CHEN_THRESHOLD       0x0000FFFF
#define BPSM_REGISTER_CONFIGURATION_1_MASK_CHEN_TOGGLE_PERIOD   0x00FF0000

#define BPSM_REGISTER_STATUS_1_MASK_BKENST                      0x00000003
#define BPSM_REGISTER_STATUS_1_MASK_CHENST                      0x0000000C
#define BPSM_REGISTER_STATUS_1_MASK_CHRGST                      0x00000030

#define BPSM_REGISTER_CONTROL_1_MASK_BKEN                       0x00000001
#define BPSM_REGISTER_CONTROL_1_MASK_CHMD                       0x00000002
#define BPSM_REGISTER_CONTROL_1_MASK_CHEN                       0x00000004

#define BPSM_REGISTER_ANALOG_DATA_1_MASK_VSRC                   0x0000FFFF
#define BPSM_REGISTER_ANALOG_DATA_1_MASK_VSTR                   0xFFFF0000

#define BPSM_REGISTER_ANALOG_DATA_2_MASK_VBKP                   0x0000FFFF

#endif /* __BPSM_REGISTERS_H__ */
