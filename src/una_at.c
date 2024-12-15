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
#include "common_registers.h"
#include "error.h"
#include "math.h"
#include "parser.h"
#include "string.h"
#include "swreg.h"
#ifdef UNA_AT_MODE_MASTER
#include "terminal.h"
#endif
#include "una.h"
#include "una_at_hw.h"

#ifndef UNA_AT_DISABLE

/*** UNA AT local macros ***/

#ifdef UNA_AT_MODE_MASTER
#define UNA_AT_REPLY_PARSING_DELAY_MS   20
#define UNA_AT_SEQUENCE_TIMEOUT_MS      120000

#define UNA_AT_COMMAND_WRITE_REGISTER   "AT$W="
#define UNA_AT_COMMAND_READ_REGISTER    "AT$R="
#define UNA_AT_COMMAND_SEPARATOR        ","
#define UNA_AT_COMMAND_END              "\r"

#define UNA_AT_REPLY_BUFFER_SIZE_BYTES  128
#define UNA_AT_REPLY_BUFFER_DEPTH       16
#define UNA_AT_REPLY_OK                 "OK"
#define UNA_AT_REPLY_ERROR              "ERROR"
#define UNA_AT_REPLY_END                STRING_CHAR_CR
#endif

/*** UNA AT local structures ***/

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
typedef struct {
    volatile char_t buffer[UNA_AT_REPLY_BUFFER_SIZE_BYTES];
    volatile uint8_t size;
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
} UNA_AT_context_t;

/*** AT local global variables ***/

#ifdef UNA_AT_MODE_MASTER
static UNA_AT_context_t at_bus_ctx;
#endif

/*** AT local functions ***/

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_rx_irq_callback(uint8_t data) {
    // Read current index.
    uint8_t idx = at_bus_ctx.reply[at_bus_ctx.reply_write_idx].size;
    // Check ending characters.
    if (data == UNA_AT_REPLY_END) {
        // Set flag on current buffer.
        at_bus_ctx.reply[at_bus_ctx.reply_write_idx].buffer[idx] = STRING_CHAR_NULL;
        at_bus_ctx.reply[at_bus_ctx.reply_write_idx].line_end_flag = 1;
        // Switch buffer.
        at_bus_ctx.reply_write_idx = (uint8_t) ((at_bus_ctx.reply_write_idx + 1) % UNA_AT_REPLY_BUFFER_DEPTH);
    }
    else {
        // Store incoming byte.
        at_bus_ctx.reply[at_bus_ctx.reply_write_idx].buffer[idx] = (char_t) data;
        // Manage index.
        idx = (uint8_t) ((idx + 1) % UNA_AT_REPLY_BUFFER_SIZE_BYTES);
        at_bus_ctx.reply[at_bus_ctx.reply_write_idx].size = idx;
    }
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_flush_reply(uint8_t reply_index) {
    // Flush buffer.
    at_bus_ctx.reply[reply_index].size = 0;
    // Reset flag.
    at_bus_ctx.reply[reply_index].line_end_flag = 0;
    // Reset parser.
    at_bus_ctx.reply[reply_index].parser.buffer = (char_t*) at_bus_ctx.reply[reply_index].buffer;
    at_bus_ctx.reply[reply_index].parser.buffer_size = 0;
    at_bus_ctx.reply[reply_index].parser.separator_index = 0;
    at_bus_ctx.reply[reply_index].parser.start_index = 0;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
static void _UNA_AT_flush_replies(void) {
    // Local variables.
    uint8_t rep_idx = 0;
    // Reset replies buffers.
    for (rep_idx = 0; rep_idx < UNA_AT_REPLY_BUFFER_DEPTH; rep_idx++) {
        _UNA_AT_flush_reply(rep_idx);
    }
    // Reset index and count.
    at_bus_ctx.reply_write_idx = 0;
    at_bus_ctx.reply_read_idx = 0;
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
        if (at_bus_ctx.reply_write_idx != at_bus_ctx.reply_read_idx) {
            // Check line end flag.
            if (at_bus_ctx.reply[at_bus_ctx.reply_read_idx].line_end_flag != 0) {
                // Increment parsing count.
                reply_count++;
                // Reset time and flag.
                reply_time_ms = 0;
                at_bus_ctx.reply[at_bus_ctx.reply_read_idx].line_end_flag = 0;
                // Update buffer length.
                at_bus_ctx.reply[at_bus_ctx.reply_read_idx].parser.buffer_size = at_bus_ctx.reply[at_bus_ctx.reply_read_idx].size;
                // Parse reply.
                switch (reply_params->type) {
                case UNA_REPLY_TYPE_OK:
                    // Compare to reference string.
                    parser_status = PARSER_compare(&at_bus_ctx.reply[at_bus_ctx.reply_read_idx].parser, PARSER_MODE_STRICT, UNA_AT_REPLY_OK);
                    break;
                case UNA_REPLY_TYPE_VALUE:
                    // Parse register.
                    parser_status = SWREG_parse_register(&at_bus_ctx.reply[at_bus_ctx.reply_read_idx].parser, STRING_CHAR_NULL, reg_value);
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
                parser_status = PARSER_compare(&at_bus_ctx.reply[at_bus_ctx.reply_read_idx].parser, PARSER_MODE_HEADER, UNA_AT_REPLY_ERROR);
                if (parser_status == PARSER_SUCCESS) {
                    // Update output data.
                    (reply_status->error_received) = 1;
                    break;
                }
            }
            // Update read index.
            _UNA_AT_flush_reply(at_bus_ctx.reply_read_idx);
            at_bus_ctx.reply_read_idx = (uint8_t) ((at_bus_ctx.reply_read_idx + 1) % UNA_AT_REPLY_BUFFER_DEPTH);
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
    // Add ending marker.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_END);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Send command.
    terminal_status = TERMINAL_set_destination_address(UNA_AT_TERMINAL_INSTANCE, node_address);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_send_tx_buffer(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
errors:
    return status;
}
#endif

/*** AT functions ***/

/*******************************************************************/
UNA_AT_status_t UNA_AT_init(uint32_t baud_rate) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
#ifdef UNA_AT_MODE_MASTER
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Init context.
    _UNA_AT_flush_replies();
    // Init bus terminal.
    terminal_status = TERMINAL_open(UNA_AT_TERMINAL_INSTANCE, baud_rate, &_UNA_AT_rx_irq_callback);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
errors:
#else
    UNUSED(baud_rate);
#endif
    return status;
}

/*******************************************************************/
UNA_AT_status_t UNA_AT_de_init(void) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
#ifdef UNA_AT_MODE_MASTER
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Release bus terminal.
    terminal_status = TERMINAL_close(UNA_AT_TERMINAL_INSTANCE);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
errors:
#endif
    return status;
}

#ifdef UNA_AT_MODE_SLAVE
/*******************************************************************/
UNA_AT_status_t UNA_AT_process(void) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    return status;
}
#endif

#ifdef UNA_AT_MODE_MASTER
/*******************************************************************/
UNA_AT_status_t UNA_AT_send_command(UNA_command_parameters_t* command_params) {
    // Local variables.
    UNA_AT_status_t status = UNA_AT_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Add command.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, (command_params->command));
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
    // Check parameters.
    if ((write_params == NULL) || (write_status == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset access status.
    (write_status->all) = 0;
    (write_status->type) = UNA_ACCESS_TYPE_WRITE;
    // Build write command.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_WRITE_REGISTER);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (uint32_t) (write_params->reg_addr), STRING_FORMAT_HEXADECIMAL, 0);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_SEPARATOR);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (int32_t) reg_value, STRING_FORMAT_HEXADECIMAL, 0);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Add mask if needed.
    if (reg_mask != UNA_REGISTER_MASK_ALL) {
        terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_SEPARATOR);
        TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
        terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (int32_t) reg_mask, STRING_FORMAT_HEXADECIMAL, 0);
        TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    }
    // Send command.
    status = _UNA_AT_send(write_params->node_addr);
    if (status != UNA_AT_SUCCESS) goto errors;
    // Wait reply.
    status = _UNA_AT_wait_reply(&(write_params->reply_params), &unused_reg_value, write_status);
    if (status != UNA_AT_SUCCESS) goto errors;
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
    // Check parameters.
    if ((read_params == NULL) || (read_status == NULL) || (reg_value == NULL)) {
        status = UNA_AT_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Reset access status.
    (read_status->all) = 0;
    (read_status->type) = UNA_ACCESS_TYPE_READ;
    // Build read command.
    terminal_status = TERMINAL_tx_buffer_add_string(UNA_AT_TERMINAL_INSTANCE, UNA_AT_COMMAND_READ_REGISTER);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(UNA_AT_TERMINAL_INSTANCE, (uint32_t) (read_params->reg_addr), STRING_FORMAT_HEXADECIMAL, 0);
    TERMINAL_exit_error(UNA_AT_ERROR_BASE_TERMINAL);
    // Send command.
    status = _UNA_AT_send(read_params->node_addr);
    if (status != UNA_AT_SUCCESS) goto errors;
    // Wait reply.
    status = _UNA_AT_wait_reply(&(read_params->reply_params), reg_value, read_status);
    if (status != UNA_AT_SUCCESS) goto errors;
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
    read_params.reg_addr = COMMON_REGISTER_ADDRESS_NODE_ID;
    read_params.reply_params.timeout_ms = UNA_AT_DEFAULT_TIMEOUT_MS;
    read_params.reply_params.type = UNA_REPLY_TYPE_VALUE;
    // Loop on all addresses.
    for (node_addr = 0; node_addr <= UNA_AT_NODE_ADDRESS_LAST; node_addr++) {
        // Update address.
        read_params.node_addr = node_addr;
        // Read UNA_AT_ID register.
        status = UNA_AT_read_register(&read_params, &reg_value, &read_status);
        if (status != UNA_AT_SUCCESS) goto errors;
        // Check reply status.
        if (read_status.flags == 0) {
            // Check node address consistency.
            if (SWREG_read_field(reg_value, COMMON_REGISTER_NODE_ID_MASK_NODE_ADDR) == node_addr) {
                // Update board ID.
                nodes_list[(*nodes_count)].address = SWREG_read_field(reg_value, COMMON_REGISTER_NODE_ID_MASK_NODE_ADDR);
                nodes_list[(*nodes_count)].board_id = SWREG_read_field(reg_value, COMMON_REGISTER_NODE_ID_MASK_BOARD_ID);
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

/*** UNA AT compilation flags check ***/

#if (!(defined UNA_AT_MODE_MASTER) && !(defined UNA_AT_MODE_SLAVE))
#error "una-at: None mode selected"
#endif

#endif /* UNA_AT_DISABLE */
