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
#include "error.h"
#include "terminal.h"
#include "types.h"
#include "una.h"

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
    UNA_AT_ERROR_BASE_DELAY = ERROR_BASE_STEP,
    UNA_AT_ERROR_BASE_TERMINAL = (UNA_AT_ERROR_BASE_DELAY + UNA_AT_DELAY_ERROR_BASE_LAST),
    UNA_AT_ERROR_BASE_AT = (UNA_AT_ERROR_BASE_TERMINAL + TERMINAL_ERROR_BASE_LAST),
    // Last base value.
    UNA_AT_ERROR_BASE_LAST = (UNA_AT_ERROR_BASE_AT + AT_ERROR_BASE_LAST)
} UNA_AT_status_t;

#ifndef UNA_AT_DISABLE

#ifdef UNA_AT_MODE_SLAVE
/*!******************************************************************
 * \fn UNA_AT_write_register_cb_t
 * \brief Write register command callback.
 *******************************************************************/
typedef void (*UNA_AT_process_cb_t)(void);

/*!******************************************************************
 * \fn UNA_AT_write_register_cb_t
 * \brief Write register command callback.
 *******************************************************************/
typedef AT_status_t (*UNA_AT_write_register_cb_t)(uint8_t reg_addr, uint32_t reg_value, uint32_t reg_mask);

/*!******************************************************************
 * \fn UNA_AT_read_register_cb_t
 * \brief Read register command callback.
 *******************************************************************/
typedef AT_status_t (*UNA_AT_read_register_cb_t)(uint8_t reg_addr, uint32_t* reg_value);
#endif

/*!******************************************************************
 * \fn UNA_AT_configuration_t
 * \brief UNA AT configuration structure.
 *******************************************************************/
typedef struct {
#ifdef UNA_AT_MODE_MASTER
    uint32_t baud_rate;
#endif
#ifdef UNA_AT_MODE_SLAVE
    UNA_AT_process_cb_t process_callback;
    UNA_AT_write_register_cb_t write_register_callback;
    UNA_AT_read_register_cb_t read_register_callback;
#ifdef UNA_AT_CUSTOM_COMMANDS
    PARSER_context_t** parser_context_ptr;
#endif
#endif
} UNA_AT_configuration_t;

/*** UNA functions ***/

/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_init(UNA_AT_configuration_t* configuration)
 * \brief Init UNA AT interface.
 * \param[in]   configuration: Pointer to the configuration structure.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_init(UNA_AT_configuration_t* configuration);

/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_de_init(void)
 * \brief Release UNA AT interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_de_init(void);

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

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_register_command(const AT_command_t* command)
 * \brief Register additional custom AT command.
 * \param[in]   command: Pointer to the command to register.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_register_command(const AT_command_t* command);
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn UNA_AT_status_t UNA_AT_unregister_command(const AT_command_t* command)
 * \brief Unregister additional custom AT command.
 * \param[in]   command: Pointer to the command to register.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
UNA_AT_status_t UNA_AT_unregister_command(const AT_command_t* command);
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn void UNA_AT_reply_add_string(char_t* str)
 * \brief Add a message to the reply buffer.
 * \param[in]   str: null-terminated string to print.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void UNA_AT_reply_add_string(char_t* str);
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn void UNA_AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix)
 * \brief Add an integer to the reply buffer.
 * \param[in]   value: Integer to print.
 * \param[in]   format: Format of the output string.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void UNA_AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix);
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn void UNA_AT_reply_add_byte_array(uint8_t instance, uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix)
 * \brief Add a byte array to the reply buffer.
 * \param[in]   data: Byte array to print.
 * \param[in]   data_size_bytes: Size of the input byte array.
 * \param[in]   print_prefix: Print the base prefix if non zero.
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void UNA_AT_reply_add_byte_array(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix);
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*!******************************************************************
 * \fn void UNA_AT_send_reply(void)
 * \brief Send the reply buffer.
 * \param[in]   none
 * \param[out]  none
 * \retval      none
 *******************************************************************/
void UNA_AT_send_reply(void);
#endif

/*******************************************************************/
#define UNA_AT_exit_error(base) { ERROR_check_exit(una_at_status, UNA_AT_SUCCESS, base) }

/*******************************************************************/
#define UNA_AT_stack_error(base) { ERROR_check_stack(una_at_status, UNA_AT_SUCCESS, base) }

/*******************************************************************/
#define UNA_AT_stack_exit_error(base, code) { ERROR_check_stack_exit(una_at_status, UNA_AT_SUCCESS, base, code) }

#endif /* UNA_AT_DISABLE */

#endif /* __UNA_AT_H__ */
