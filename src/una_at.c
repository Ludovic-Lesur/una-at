/*
 * una_at.c
 *
 *  Created on: 07 dec. 2024
 *      Author: Ludo
 */

#include "una_at.h"

#ifndef UNA_AT_DISABLE_FLAGS_FILE
#include "una_at_flags.h"
#endif
#ifdef UNA_AT_MODE_SLAVE
#include "at.h"
#endif
#include "error.h"
#include "maths.h"
#include "parser.h"
#include "strings.h"
#include "swreg.h"
#ifdef UNA_AT_MODE_MASTER
#include "terminal.h"
#endif
#include "types.h"
#include "una.h"
#include "una_at_hw.h"

#ifndef UNA_AT_DISABLE

/*** UNA AT local macros ***/

#ifdef UNA_AT_MODE_MASTER
#define UNA_AT_COMMAND_SEPARATOR        ","
#endif
#ifdef UNA_AT_MODE_SLAVE
#define UNA_AT_COMMAND_SEPARATOR        STRING_CHAR_COMMA
#endif

#ifdef UNA_AT_MODE_MASTER
#define UNA_AT_REPLY_PARSING_DELAY_MS   20
#define UNA_AT_SEQUENCE_TIMEOUT_MS      120000

#define UNA_AT_COMMAND_WRITE_REGISTER   "AT$W="
#define UNA_AT_COMMAND_READ_REGISTER    "AT$R="
#define UNA_AT_COMMAND_END              "\r"

#define UNA_AT_REPLY_BUFFER_SIZE_BYTES  128
#define UNA_AT_REPLY_BUFFER_DEPTH       4
#define UNA_AT_REPLY_OK                 "OK"
#define UNA_AT_REPLY_ERROR              "ERROR"
#define UNA_AT_REPLY_END                STRING_CHAR_CR
#endif
#ifdef UNA_AT_MODE_SLAVE
#define UNA_AT_REPLY_DELAY_MS           5
#endif

/*** UNA AT local structures ***/

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
typedef struct {
    volatile char_t buffer[UNA_AT_REPLY_BUFFER_SIZE_BYTES];
    volatile uint32_t size;
    volatile uint8_t line_end_flag;
    PARSER_context_t parser;
} UNA_AT_reply_buffer_t;
#endif

/*******************************************************************/
typedef struct {
#ifdef UNA_AT_MODE_MASTER
    UNA_AT_reply_buffer_t reply[UNA_AT_REPLY_BUFFER_DEPTH];
    volatile uint8_t reply_write_idx;
    volatile uint8_t reply_read_idx;
#endif
#ifdef UNA_AT_MODE_SLAVE
    volatile uint8_t at_process_flag;
    PARSER_context_t* at_parser_ptr;
    UNA_AT_process_cb_t process_callback;
    UNA_AT_write_register_cb_t write_register_callback;
    UNA_AT_read_register_cb_t read_register_callback;
#endif
} UNA_AT_context_t;

/*** UNA_AT local functions declaration ***/

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
static AT_status_t _UNA_AT_write_register_callback(void);
static AT_status_t _UNA_AT_read_register_callback(void);
#endif

/*** AT local global variables ***/

#ifdef UNA_AT_MODE_SLAVE
static const AT_command_t UNA_AT_COMMANDS_LIST[] = {
    {
        .syntax = "$W=",
        .parameters = "<addr[hex],data[hex]>",
        .description = "Write node register",
        .callback = &_UNA_AT_write_register_callback
    },
    {
        .syntax = "$R=",
        .parameters = "<addr>[hex]",
        .description = "Read node register",
        .callback = &_UNA_AT_read_register_callback
    }
};
#endif
static UNA_AT_context_t una_at_ctx;

/*** AT local functions ***/

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_rx_irq_callback(uint8_t data) {
    // Read current index.
    uint32_t idx = una_at_ctx.reply[una_at_ctx.reply_write_idx].size;
    // Check ending characters.
    if (data == UNA_AT_REPLY_END) {
        // Set flag on current buffer.
        una_at_ctx.reply[una_at_ctx.reply_write_idx].buffer[idx] = STRING_CHAR_NULL;
        una_at_ctx.reply[una_at_ctx.reply_write_idx].line_end_flag = 1;
        // Switch buffer.
        una_at_ctx.reply_write_idx = (uint8_t) ((una_at_ctx.reply_write_idx + 1) % UNA_AT_REPLY_BUFFER_DEPTH);
    }
    else {
        // Store incoming byte.
        una_at_ctx.reply[una_at_ctx.reply_write_idx].buffer[idx] = (char_t) data;
        // Manage index.
        idx = (uint32_t) ((idx + 1) % UNA_AT_REPLY_BUFFER_SIZE_BYTES);
        una_at_ctx.reply[una_at_ctx.reply_write_idx].size = idx;
    }
}
#endif

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
static void _UNA_AT_at_process_callback(void) {
    // Set local flag.
    una_at_ctx.at_process_flag = 1;
    // Ask for processing.
    if (una_at_ctx.process_callback != NULL) {
        una_at_ctx.process_callback();
    }
}
#endif

/*******************************************************************/
#ifdef UNA_AT_MODE_MASTER
static UNA_AT_status_t _UNA_AT_tx_buffer_add_register(uint32_t reg_value) {
#endif
#ifdef UNA_AT_MODE_SLAVE
static void _UNA_AT_tx_buffer_add_register(uint32_t reg_value) {
#endif
    // Local variables.
#ifdef UNA_AT_MODE_MASTER
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
#endif
    uint8_t byte = 0;
    uint8_t idx = 0;
    uint8_t first_non_zero_found = 0;
    // Convert 32-bits value to byte array.
    for (idx = 0; idx < UNA_REGISTER_SIZE_BYTES; idx++) {
        // Compute byte.
        byte = (uint8_t) ((reg_value >> ((UNA_REGISTER_SIZE_BYTES - 1 - idx) << 3)) & 0xFF);
        // Update flag.
        if (byte != 0) {
            first_non_zero_found = 1;
        }
        // Check flag and index.
        if ((first_non_zero_found != 0) || (idx == (UNA_REGISTER_SIZE_BYTES - 1))) {
            // Convert byte.
#ifdef UNA_AT_MODE_MASTER
            terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (int32_t) byte, STRING_FORMAT_HEXADECIMAL, 0);
            TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
#endif
#ifdef UNA_AT_MODE_SLAVE
            AT_reply_add_integer((int32_t) byte, STRING_FORMAT_HEXADECIMAL, 0);
#endif
        }
    }
#ifdef UNA_AT_MODE_MASTER
errors:
    return status;
#endif
}

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_flush_reply(uint8_t reply_index) {
    // Local variables.
    uint32_t idx = 0;
    // Flush buffer.
    for (idx = 0; idx < UNA_AT_REPLY_BUFFER_SIZE_BYTES; idx++) {
        una_at_ctx.reply[reply_index].buffer[idx] = STRING_CHAR_NULL;
    }
    una_at_ctx.reply[reply_index].size = 0;
    // Reset flag.
    una_at_ctx.reply[reply_index].line_end_flag = 0;
    // Reset parser.
    una_at_ctx.reply[reply_index].parser.buffer = (char_t*) una_at_ctx.reply[reply_index].buffer;
    una_at_ctx.reply[reply_index].parser.buffer_size = 0;
    una_at_ctx.reply[reply_index].parser.separator_index = 0;
    una_at_ctx.reply[reply_index].parser.start_index = 0;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_flush_replies(void) {
    // Local variables.
    uint8_t idx = 0;
    // Reset replies buffers.
    for (idx = 0; idx < UNA_AT_REPLY_BUFFER_DEPTH; idx++) {
        _UNA_AT_flush_reply(idx);
    }
    // Reset index and count.
    una_at_ctx.reply_write_idx = 0;
    una_at_ctx.reply_read_idx = 0;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static UNA_AT_status_t _UNA_AT_wait_reply(UNA_reply_parameters_t* reply_params, uint32_t* reg_value, UNA_access_status_t* reply_status) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    uint32_t reply_time_ms = 0;
    uint32_t sequence_time_ms = 0;
    uint8_t reply_count = 0;
    // Check parameters.
    if ((reg_value == NULL) || (reply_status == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if ((reply_params->type) >= UNA_REPLY_TYPE_LAST) {
        status = UNA_AT_ERROR_REPLY_TYPE;
        goto errors;
    }
    // Reset status.
    (reply_status->flags) = 0;
    // Directly exit function with success status for none reply type.
    if ((reply_params->type) == UNA_REPLY_TYPE_NONE) goto errors;
    // Enable receiver.
    terminal_status = TERMINAL_enable_rx(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Main reception loop.
    while (1) {
        // Delay.
        status = UNA_AT_HW_delay_milliseconds(UNA_AT_REPLY_PARSING_DELAY_MS);
        if (status != UNA_AT_SUCCESS) goto errors;
        // Update timers.
        reply_time_ms += UNA_AT_REPLY_PARSING_DELAY_MS;
        sequence_time_ms += UNA_AT_REPLY_PARSING_DELAY_MS;
        // Check write index.
        if (una_at_ctx.reply_write_idx != una_at_ctx.reply_read_idx) {
            // Check line end flag.
            if (una_at_ctx.reply[una_at_ctx.reply_read_idx].line_end_flag != 0) {
                // Increment parsing count.
                reply_count++;
                // Reset time and flag.
                reply_time_ms = 0;
                una_at_ctx.reply[una_at_ctx.reply_read_idx].line_end_flag = 0;
                // Update buffer length.
                una_at_ctx.reply[una_at_ctx.reply_read_idx].parser.buffer_size = una_at_ctx.reply[una_at_ctx.reply_read_idx].size;
                // Parse reply.
                switch (reply_params->type) {
                case UNA_REPLY_TYPE_OK:
                    // Compare to reference string.
                    parser_status = PARSER_compare(&una_at_ctx.reply[una_at_ctx.reply_read_idx].parser, PARSER_MODE_STRICT, UNA_AT_REPLY_OK);
                    break;
                case UNA_REPLY_TYPE_VALUE:
                    // Parse register.
                    parser_status = SWREG_parse_register(&una_at_ctx.reply[una_at_ctx.reply_read_idx].parser, STRING_CHAR_NULL, reg_value);
                    break;
                default:
                    status = UNA_AT_ERROR_REPLY_TYPE;
                    break;
                }
                // Check status.
                if (parser_status == PARSER_SUCCESS) {
                    // Update raw pointer, status and exit.
                    (reply_status->flags) = 0;
                    break;
                }
                // Check error.
                parser_status = PARSER_compare(&una_at_ctx.reply[una_at_ctx.reply_read_idx].parser, PARSER_MODE_HEADER, UNA_AT_REPLY_ERROR);
                if (parser_status == PARSER_SUCCESS) {
                    // Update output data.
                    (reply_status->error_received) = 1;
                    break;
                }
            }
            // Update read index.
            _UNA_AT_flush_reply(una_at_ctx.reply_read_idx);
            una_at_ctx.reply_read_idx = (uint8_t) ((una_at_ctx.reply_read_idx + 1) % UNA_AT_REPLY_BUFFER_DEPTH);
        }
        // Exit if timeout.
        if (reply_time_ms > (reply_params->timeout_ms)) {
            // Set status to timeout if none reply has been received, otherwise the parser error code is returned.
            if (reply_count == 0) {
                (reply_status->reply_timeout) = 1;
            }
            else {
                (reply_status->parser_error) = 1;
            }
            break;
        }
        if (sequence_time_ms > UNA_AT_SEQUENCE_TIMEOUT_MS) {
            // Set status to timeout in any case.
            (reply_status->sequence_timeout) = 1;
            break;
        }
    }
errors:
    TERMINAL_disable_rx(UNA_AT_TERMINAL_INSTANCE);
    return status;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static UNA_AT_status_t _UNA_AT_send(UNA_node_address_t node_address) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Reset replies.
    _UNA_AT_flush_replies();
    // Send command.
    terminal_status = TERMINAL_set_destination_address(UNA_AT_TERMINAL_INSTANCE, node_address);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_send_tx_buffer(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
static AT_status_t _UNA_AT_write_register_callback(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    uint32_t reg_addr = 0;
    uint32_t reg_value = 0;
    uint32_t reg_mask = 0;
    // Delay to ensure that the master node has switched to RX.
    UNA_AT_HW_delay_milliseconds(UNA_AT_REPLY_DELAY_MS);
    // Read address parameter.
    parser_status = SWREG_parse_register(una_at_ctx.at_parser_ptr, UNA_AT_COMMAND_SEPARATOR, &reg_addr);
    PARSER_exit_error(AT_ERROR_BASE_PARSER);
    // First try with 3 parameters.
    parser_status = SWREG_parse_register(una_at_ctx.at_parser_ptr, UNA_AT_COMMAND_SEPARATOR, &reg_value);
    if (parser_status == PARSER_SUCCESS) {
        // Try parsing register mask parameter.
        parser_status = SWREG_parse_register(una_at_ctx.at_parser_ptr, STRING_CHAR_NULL, &reg_mask);
        PARSER_exit_error(AT_ERROR_BASE_PARSER);
    }
    else {
        // Try with only 2 parameters.
        parser_status = SWREG_parse_register(una_at_ctx.at_parser_ptr, STRING_CHAR_NULL, &reg_value);
        PARSER_exit_error(AT_ERROR_BASE_PARSER);
        // Perform full write operation since mask is not given.
        reg_mask = UNA_REGISTER_MASK_ALL;
    }
    // Write register.
    if (una_at_ctx.write_register_callback != NULL) {
        // Execute write callback.
        status = una_at_ctx.write_register_callback((uint8_t) reg_addr, reg_value, reg_mask);
        if (status != AT_SUCCESS) goto errors;
    }
    else {
        status = AT_ERROR_COMMAND_EXECUTION;
        goto errors;
    }
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
static AT_status_t _UNA_AT_read_register_callback(void) {
    // Local variables.
    AT_status_t status = AT_SUCCESS;
    PARSER_status_t parser_status = PARSER_SUCCESS;
    uint32_t reg_addr = 0;
    uint32_t reg_value = 0;
    // Delay to ensure that the master node has switched to RX.
    UNA_AT_HW_delay_milliseconds(UNA_AT_REPLY_DELAY_MS);
    // Read address parameter.
    parser_status = SWREG_parse_register(una_at_ctx.at_parser_ptr, STRING_CHAR_NULL, &reg_addr);
    PARSER_exit_error(AT_ERROR_BASE_PARSER);
    // Read register.
    if (una_at_ctx.read_register_callback != NULL) {
        // Execute read callback.
        status = una_at_ctx.read_register_callback((uint8_t) reg_addr, &reg_value);
        if (status != AT_SUCCESS) goto errors;
    }
    else {
        status = AT_ERROR_COMMAND_EXECUTION;
        goto errors;
    }
    // Send reply.
    _UNA_AT_tx_buffer_add_register(reg_value);
    AT_send_reply();
errors:
    return status;
}
#endif

/*** AT functions ***/

/*******************************************************************/
UNA_AT_status_t UNA_AT_init(UNA_AT_configuration_t* configuration) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
#ifdef UNA_AT_MODE_MASTER
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
#endif
#ifdef UNA_AT_MODE_SLAVE
    AT_status_t at_status = AT_SUCCESS;
    AT_configuration_t at_config;
    uint8_t idx = 0;
#endif
    // Check parameter.
    if (configuration == NULL) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Init context.
#ifdef UNA_AT_MODE_MASTER
    _UNA_AT_flush_replies();
#endif
#ifdef UNA_AT_MODE_SLAVE
    una_at_ctx.at_process_flag = 0;
    una_at_ctx.process_callback = (configuration->process_callback);
    una_at_ctx.write_register_callback = (configuration->write_register_callback);
    una_at_ctx.read_register_callback = (configuration->read_register_callback);
#endif
    // Init low level interface.
#ifdef UNA_AT_MODE_MASTER
    // Init bus terminal.
    terminal_status = TERMINAL_open(UNA_AT_TERMINAL_INSTANCE, (configuration->baud_rate), &_UNA_AT_rx_irq_callback);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
#endif
#ifdef UNA_AT_MODE_SLAVE
    // Init AT receiver.
    at_config.process_callback = &_UNA_AT_at_process_callback;
    at_config.terminal_instance = UNA_AT_TERMINAL_INSTANCE;
    at_status = AT_init(&at_config, &(una_at_ctx.at_parser_ptr));
    AT_exit_error(UNA_AT_ERROR_BASE_AT);
    // Register commands.
    for (idx = 0; idx < (sizeof(UNA_AT_COMMANDS_LIST) / sizeof(AT_command_t)); idx++) {
        at_status = AT_register_command(&(UNA_AT_COMMANDS_LIST[idx]));
        AT_exit_error(UNA_AT_ERROR_BASE_AT);
    }
#ifdef UNA_AT_CUSTOM_COMMANDS
    *(configuration->parser_context_ptr) = una_at_ctx.at_parser_ptr;
#endif
#endif
errors:
    return status;
}

/*******************************************************************/
UNA_AT_status_t UNA_AT_de_init(void) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
#ifdef UNA_AT_MODE_MASTER
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
#endif
#ifdef UNA_AT_MODE_SLAVE
    AT_status_t at_status = AT_SUCCESS;
    uint8_t idx = 0;
#endif
    // Release low level interface.
#ifdef UNA_AT_MODE_MASTER
    // Release bus terminal.
    terminal_status = TERMINAL_close(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
#endif
#ifdef UNA_AT_MODE_SLAVE
    // Unregister commands.
    for (idx = 0; idx < (sizeof(UNA_AT_COMMANDS_LIST) / sizeof(AT_command_t)); idx++) {
        AT_unregister_command(&(UNA_AT_COMMANDS_LIST[idx]));
    }
    // Init AT receiver.
    at_status = AT_de_init();
    AT_exit_error(UNA_AT_ERROR_BASE_AT);
#endif
errors:
    return status;
}

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
UNA_AT_status_t UNA_AT_send_command(UNA_command_parameters_t* command_params) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Add command.
    terminal_status = TERMINAL_flush_tx_buffer(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, (command_params->command));
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Add ending marker.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_END);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Send command.
    status = _UNA_AT_send(command_params->node_addr);
    if (status != UNA_AT_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
UNA_AT_status_t UNA_AT_write_register(UNA_access_parameters_t* write_params, uint32_t reg_value, uint32_t reg_mask, UNA_access_status_t* write_status) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    uint32_t unused_reg_value = 0;
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
    uint32_t retry_count = 0;
#endif
    // Check parameters.
    if ((write_params == NULL) || (write_status == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset access status.
    (write_status->all) = 0;
    (write_status->type) = UNA_ACCESS_TYPE_WRITE;
    // Build write command.
    terminal_status = TERMINAL_flush_tx_buffer(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_WRITE_REGISTER);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (int32_t) (write_params->reg_addr), STRING_FORMAT_HEXADECIMAL, 0);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_SEPARATOR);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    status = _UNA_AT_tx_buffer_add_register(reg_value);
    if (status != UNA_AT_SUCCESS) goto errors;
    // Add mask if needed.
    if (reg_mask != UNA_REGISTER_MASK_ALL) {
        terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_SEPARATOR);
        TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
        status = _UNA_AT_tx_buffer_add_register(reg_mask);
        if (status != UNA_AT_SUCCESS) goto errors;
    }
    // Add ending marker.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_END);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
    for (retry_count = 0; retry_count < UNA_AT_NODE_ACCESS_RETRY_MAX; retry_count++) {
#endif
        // Send command.
        status = _UNA_AT_send(write_params->node_addr);
        if (status != UNA_AT_SUCCESS) goto errors;
        // Wait reply.
        status = _UNA_AT_wait_reply(&(write_params->reply_params), &unused_reg_value, write_status);
        if (status != UNA_AT_SUCCESS) goto errors;
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
        // Exit on first success.
        if (write_status->flags == 0) break;
    }
#endif
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
UNA_AT_status_t UNA_AT_read_register(UNA_access_parameters_t* read_params, uint32_t* reg_value, UNA_access_status_t* read_status) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
    uint32_t retry_count = 0;
#endif
    // Check parameters.
    if ((read_params == NULL) || (read_status == NULL) || (reg_value == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset access status.
    (read_status->all) = 0;
    (read_status->type) = UNA_ACCESS_TYPE_READ;
    // Build read command.
    terminal_status = TERMINAL_flush_tx_buffer(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_READ_REGISTER);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (uint32_t) (read_params->reg_addr), STRING_FORMAT_HEXADECIMAL, 0);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Add ending marker.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_END);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
    for (retry_count = 0; retry_count < UNA_AT_NODE_ACCESS_RETRY_MAX; retry_count++) {
#endif
        // Send command.
        status = _UNA_AT_send(read_params->node_addr);
        if (status != UNA_AT_SUCCESS) goto errors;
        // Wait reply.
        status = _UNA_AT_wait_reply(&(read_params->reply_params), reg_value, read_status);
        if (status != UNA_AT_SUCCESS) goto errors;
#if (UNA_AT_NODE_ACCESS_RETRY_MAX > 1)
        // Exit on first success.
        if (read_status->flags == 0) break;
    }
#endif
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
UNA_AT_status_t UNA_AT_scan(UNA_node_t* nodes_list, uint8_t nodes_list_size, uint8_t* nodes_count) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    UNA_access_parameters_t read_params;
    UNA_access_status_t read_status;
    UNA_node_address_t node_addr = 0;
    uint32_t reg_value = 0;
    // Check parameters.
    if ((nodes_list == NULL) || (nodes_count == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset count.
    (*nodes_count) = 0;
    // Build read input common parameters.
    read_params.reg_addr = UNA_AT_SCAN_REGISTER_ADDRESS;
    read_params.reply_params.timeout_ms = UNA_AT_SCAN_REGISTER_TIMEOUT_MS;
    read_params.reply_params.type = UNA_REPLY_TYPE_VALUE;
    // Loop on all addresses.
    for (node_addr = 0; node_addr < UNA_NODE_ADDRESS_R4S8CR_START; node_addr++) {
        // Update address.
        read_params.node_addr = node_addr;
        // Read UNA_AT_ID register.
        status = UNA_AT_read_register(&read_params, &reg_value, &read_status);
        if (status != UNA_AT_SUCCESS) goto errors;
        // Check reply status.
        if (read_status.flags == 0) {
            // Check node address consistency.
            if (SWREG_read_field(reg_value, UNA_AT_SCAN_REGISTER_MASK_NODE_ADDRESS) == node_addr) {
                // Update board ID.
                nodes_list[(*nodes_count)].address = SWREG_read_field(reg_value, UNA_AT_SCAN_REGISTER_MASK_NODE_ADDRESS);
                nodes_list[(*nodes_count)].board_id = SWREG_read_field(reg_value, UNA_AT_SCAN_REGISTER_MASK_BOARD_ID);
                // Update nodes count.
                (*nodes_count)++;
            }
        }
        // Check index.
        if ((*nodes_count) >= nodes_list_size) break;
    }
errors:
    return status;
}
#endif

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
UNA_AT_status_t UNA_AT_process(void) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    AT_status_t at_status = AT_SUCCESS;
    // Check AT flag.
    if (una_at_ctx.at_process_flag != 0) {
        // Clear flag.
        una_at_ctx.at_process_flag = 0;
        // Process AT parser.
        at_status = AT_process();
        AT_exit_error(UNA_AT_ERROR_BASE_AT);
    }
errors:
    return status;
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
UNA_AT_status_t UNA_AT_register_command(const AT_command_t* command) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    AT_status_t at_status = AT_SUCCESS;
    // Register command.
    at_status = AT_register_command(command);
    AT_exit_error(UNA_AT_ERROR_BASE_AT);
errors:
    return status;
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
UNA_AT_status_t UNA_AT_unregister_command(const AT_command_t* command) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    AT_status_t at_status = AT_SUCCESS;
    // Unregister command.
    at_status = AT_unregister_command(command);
    AT_exit_error(UNA_AT_ERROR_BASE_AT);
errors:
    return status;
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
void UNA_AT_reply_add_string(char_t* str) {
    AT_reply_add_string(str);
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
void UNA_AT_reply_add_integer(int32_t value, STRING_format_t format, uint8_t print_prefix) {
    AT_reply_add_integer(value, format, print_prefix);
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
void UNA_AT_reply_add_byte_array(uint8_t* data, uint32_t data_size_bytes, uint8_t print_prefix) {
    AT_reply_add_byte_array(data, data_size_bytes, print_prefix);
}
#endif

#if ((defined UNA_AT_MODE_SLAVE) && (defined UNA_AT_CUSTOM_COMMANDS))
/*******************************************************************/
void UNA_AT_send_reply(void) {
    AT_send_reply();
}
#endif

/*** UNA AT compilation flags check ***/

#if (!(defined UNA_AT_MODE_MASTER) && !(defined UNA_AT_MODE_SLAVE))
#error "una-at: None mode selected"
#endif

#endif /* UNA_AT_DISABLE */
