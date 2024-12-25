/*
 * una_at.h
 *
 *  Created on: 07 dec. 2024
 *      Author: Ludo
 */

#ifndef __UNA_AT_H__
#define __UNA_AT_H__

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#include "at.h"
#include "terminal.h"
#include "types.h"
#include "una.h"

/*** UNA AT macros ***/

#define UNA_AT_DEFAULT_TIMEOUT_MS   200

/*** UNA AT structures ***/

/*!******************************************************************
 * \enum UNA_AT_status_t
 * \brief UNA driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    UNA_AT_SUCCESS = 0,
    UNA_AT_ERROR_NULL_PARAMETER,
    UNA_AT_ERROR_REPLY_TYPE,
    // Low level drivers errors.
    UNA_AT_ERROR_BASE_DELAY = 0x0100,
    UNA_AT_ERROR_BASE_TERMINAL = (UNA_AT_ERROR_BASE_DELAY + UNA_AT_DELAY_ERROR_BASE_LAST),
    UNA_AT_ERROR_BASE_AT = (UNA_AT_ERROR_BASE_TERMINAL + TERMINAL_ERROR_BASE_LAST),
    // Last base value.
    UNA_AT_ERROR_BASE_LAST = (UNA_AT_ERROR_BASE_AT + AT_ERROR_BASE_LAST)
} UNA_AT_status_t;

#ifndef UNA_AT_DISABLE

/*** UNA functions ***/

/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_init(uint32_t baud_rate)
 * \brief Init UNA AT interface.
 * \param[in]   baud_rate: Bus baud rate to use.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_init(uint32_t baud_rate);

/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_de_init(void)
 * \brief Release UNA AT interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_de_init(void);

#ifdef UNA_AT_MODE_SLAVE
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_process(void)
 * \brief Process UNA interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_process(void);
#endif

#ifdef UNA_AT_MODE_MASTER
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_send_command(UNA_command_parameters_t* command_parameters)
 * \brief Send a command over UNA AT interface.
 * \param[in]   command_parameters: Pointer to the command parameters.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_send_command(UNA_command_parameters_t* command_parameters);
#endif

#ifdef UNA_AT_MODE_MASTER
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_write_register(UNA_access_parameters_t* write_parameters, uint32_t reg_value, uint32_t reg_mask, UNA_access_status_t* write_status)
 * \brief Write node register through UNA AT interface.
 * \param[in]   write_parameters: Pointer to the write operation parameters.
 * \param[in]   reg_value: Register value to write.
 * \param[in]   reg_mask: Writing operation mask.
 * \param[out]  write_status: Pointer to the writing operation status.
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_write_register(UNA_access_parameters_t* write_parameters, uint32_t reg_value, uint32_t reg_mask, UNA_access_status_t* write_status);
#endif

#ifdef UNA_AT_MODE_MASTER
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_read_register(UNA_access_parameters_t* read_parameters, uint32_t* reg_value, UNA_access_status_t* read_status)
 * \brief Read node register through UNA AT interface.
 * \param[in]   read_parameters: Pointer to the read operation parameters.
 * \param[out]  reg_value: Pointer to the read register value.
 * \param[out]  read_status: Pointer to the read operation status.
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_read_register(UNA_access_parameters_t* read_parameters, uint32_t* reg_value, UNA_access_status_t* read_status);
#endif

#ifdef UNA_AT_MODE_MASTER
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_scan(UNA_node_t* nodes_list, uint8_t nodes_list_size, uint8_t* nodes_count)
 * \brief Scan all AT nodes connected to the bus.
 * \param[in]   nodes_list_size: Maximum size of the node list.
 * \param[out]  nodes_list: Pointer to the list where to store the nodes.
 * \param[out]  nodes_count: Pointer to the number of nodes detected.
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_scan(UNA_node_t* nodes_list, uint8_t nodes_list_size, uint8_t* nodes_count);
#endif

/*******************************************************************/
#define UNA_AT_exit_error(base) { ERROR_check_exit(una_at_status, UNA_AT_SUCCESS, base) }

/*******************************************************************/
#define UNA_AT_stack_error(base) { ERROR_check_stack(una_at_status, UNA_AT_SUCCESS, base) }

/*******************************************************************/
#define UNA_AT_stack_exit_error(base, code) { ERROR_check_stack_exit(una_at_status, UNA_AT_SUCCESS, base, code) }

#endif /* UNA_AT_DISABLE */

#endif /* __UNA_AT_H__ */
