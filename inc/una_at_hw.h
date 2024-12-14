/*
 * una_at_hw.h
 *
 *  Created on: 07 dec. 2024
 *      Author: Ludo
 */

#ifndef __UNA_AT_HW_H__
#define __UNA_AT_HW_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "una_at.h"
#include "types.h"

#ifndef UNA_AT_DISABLE

/*** UNA AT HW functions ***/

/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_HW_delay_milliseconds(uint32_t delay_ms)
 * \brief Delay function.
 * \param[in]   delay_ms: Delay to wait in ms.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_HW_delay_milliseconds(uint32_t delay_ms);

#endif /* UNA_AT_DISABLE */

#endif /* __UNA_AT_HW_H__ */
