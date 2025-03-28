/*
 * bcm_registers.h
 *
 *  Created on: 27 mar. 2025
 *      Author: Ludo
 */

#ifndef __BCM_REGISTERS_H__
#define __BCM_REGISTERS_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "common_registers.h"
#include "types.h"
#include "una.h"

/*** BCM REGISTERS structures ***/

/*!******************************************************************
 * \enum BCM_register_address_t
 * \brief BCM registers map.
 *******************************************************************/
typedef enum {
    BCM_REGISTER_ADDRESS_CONFIGURATION_0 = COMMON_REGISTER_ADDRESS_LAST,
    BCM_REGISTER_ADDRESS_CONFIGURATION_1,
    BCM_REGISTER_ADDRESS_CONFIGURATION_2,
    BCM_REGISTER_ADDRESS_STATUS_1,
    BCM_REGISTER_ADDRESS_CONTROL_1,
    BCM_REGISTER_ADDRESS_ANALOG_DATA_1,
    BCM_REGISTER_ADDRESS_ANALOG_DATA_2,
    BCM_REGISTER_ADDRESS_LAST,
} BCM_register_address_t;

/*** BCM REGISTERS macros ***/

#define BCM_NUMBER_OF_SPECIFIC_REGISTERS                        (BCM_REGISTER_ADDRESS_LAST - COMMON_REGISTER_ADDRESS_LAST)

#define BCM_REGISTER_CONFIGURATION_0_MASK_CEFH                  0x00000001
#define BCM_REGISTER_CONFIGURATION_0_MASK_CSFH                  0x00000002
#define BCM_REGISTER_CONFIGURATION_0_MASK_CLFH                  0x00000004
#define BCM_REGISTER_CONFIGURATION_0_MASK_BKFH                  0x00000008

#define BCM_REGISTER_CONFIGURATION_1_MASK_CHEN_THRESHOLD        0x0000FFFF
#define BCM_REGISTER_CONFIGURATION_1_MASK_CHEN_TOGGLE_PERIOD    0x00FF0000

#define BCM_REGISTER_CONFIGURATION_2_MASK_LVF_LOW_THRESHOLD     0x0000FFFF
#define BCM_REGISTER_CONFIGURATION_2_MASK_LVF_HIGH_THRESHOLD    0xFFFF0000

#define BCM_REGISTER_STATUS_1_MASK_BKENST                       0x00000003
#define BCM_REGISTER_STATUS_1_MASK_CHENST                       0x0000000C
#define BCM_REGISTER_STATUS_1_MASK_CHRGST0                      0x00000030
#define BCM_REGISTER_STATUS_1_MASK_CHRGST1                      0x000000C0
#define BCM_REGISTER_STATUS_1_MASK_LVF                          0x00000100

#define BCM_REGISTER_CONTROL_1_MASK_BKEN                        0x00000001
#define BCM_REGISTER_CONTROL_1_MASK_CHMD                        0x00000002
#define BCM_REGISTER_CONTROL_1_MASK_CHEN                        0x00000004

#define BCM_REGISTER_ANALOG_DATA_1_MASK_VSRC                    0x0000FFFF
#define BCM_REGISTER_ANALOG_DATA_1_MASK_VSTR                    0xFFFF0000

#define BCM_REGISTER_ANALOG_DATA_2_MASK_VBKP                    0x0000FFFF
#define BCM_REGISTER_ANALOG_DATA_2_MASK_ISTR                    0xFFFF0000

/*** BCM REGISTERS global variables ***/

#ifdef UNA_AT_USE_REGISTER_ACCESS
extern const UNA_register_access_t BCM_REGISTER_ACCESS[BCM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ACCESS_TIMEOUT
extern const uint32_t BCM_REGISTER_ACCESS_TIMEOUT_MS[BCM_REGISTER_ADDRESS_LAST];
#endif
#ifdef UNA_AT_USE_REGISTER_ERROR_VALUE
extern const uint32_t BCM_REGISTER_ERROR_VALUE[BCM_REGISTER_ADDRESS_LAST];
#endif

#endif /* __BCM_REGISTERS_H__ */
