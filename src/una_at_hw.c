/*
 * una_at_hw.c
 *
 *  Created on: 07 dec. 2024
 *      Author: Ludo
 */

#include "una_at_hw.h"

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "una_at.h"
#include "types.h"

#ifndef UNA_AT_DISABLE

/*** UNA AT HW functions ***/

/*******************************************************************/
UNA_AT_status_t __attribute__((weak)) UNA_AT_HW_delay_milliseconds(uint32_t delay_ms) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    /* To be implemented */
    UNUSED(delay_ms);
    return status;
}

#endif /* UNA_AT_DISABLE */
