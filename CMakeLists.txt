#
# CMakeLists.txt
#
#  Created on: 14 dec. 2025
#      Author: Ludo
#

# Minimum CMake version.
cmake_minimum_required(VERSION 3.23)

# Project creation.
project(una-at)

# Build mode.
if(NOT DEFINED BUILD_MODE)
    set(BUILD_MODE "STATIC")
endif()

# Create library.
add_library(${PROJECT_NAME} ${BUILD_MODE})

# Static library.
if(${BUILD_MODE} STREQUAL STATIC)

    # Macro to convert options to variables.
    macro(add_compilation_flag FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        option(FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
        set(${FLAG_NAME} ${FLAG_DEFAULT_VALUE} CACHE STRING ${FLAG_DESCRIPTION})
        list(APPEND COMPILATION_FLAGS_LIST ${FLAG_NAME})
    endmacro()
    
    # Compilation flags.
    add_compilation_flag(UNA_AT_DISABLE "Disable the UNA AT functions." OFF)
    add_compilation_flag(UNA_AT_DELAY_ERROR_BASE_LAST "Last error base of the low level delay driver." 0)
    add_compilation_flag(UNA_AT_TERMINAL_INSTANCE "Instance of the terminal used to transmit and receive commands." 0)
    add_compilation_flag(UNA_AT_MODE_MASTER "Enable master operating mode." ON)
    add_compilation_flag(UNA_AT_MODE_SLAVE "Enable slave operating mode." OFF)
    add_compilation_flag(UNA_AT_NODE_ACCESS_RETRY_MAX "Number of slave node access retries in case of failure (master mode only)." 3)
    add_compilation_flag(UNA_AT_SCAN_REGISTER_ADDRESS "Address of the common register containing the nodes address and board ID (master mode only)." 0)
    add_compilation_flag(UNA_AT_SCAN_REGISTER_MASK_NODE_ADDRESS "Mask of the node address field of the scan register (master mode only)." 0)
    add_compilation_flag(UNA_AT_SCAN_REGISTER_MASK_BOARD_ID "Mask of the board ID field of the scan register (master mode only)." 0)
    add_compilation_flag(UNA_AT_SCAN_REGISTER_TIMEOUT_MS "Scan register access timeout in milliseconds (master mode only)." 0)
    add_compilation_flag(UNA_AT_CUSTOM_COMMANDS "Enable additional commands registering (slave mode only)." OFF)
    
    # Remove OFF flags from list and keep flags set to value 0.
    foreach(FLAG ${COMPILATION_FLAGS_LIST} )
        if((NOT ${FLAG} STREQUAL OFF) AND (NOT ${FLAG} STREQUAL ON))
            set(${FLAG} "(${${FLAG}})")
        endif()
    endforeach()   
    
    # Create flags file.
    configure_file(una_at_flags.h.in una_at_flags.h @ONLY)
    
    # Fixed compilation flags of dependencies.
    if(${BUILD_MODE} STREQUAL STATIC)
        target_compile_definitions(${PROJECT_NAME}
            PRIVATE
                EMBEDDED_UTILS_DISABLE_FLAGS_FILE
                EMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST=${EMBEDDED_UTILS_HW_INTERFACE_ERROR_BASE_LAST}
                EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER=${EMBEDDED_UTILS_TERMINAL_INSTANCES_NUMBER}
                EMBEDDED_UTILS_TERMINAL_MODE_BUS
                UNA_LIB_DISABLE_FLAGS_FILE
                
        )
    endif()
    
    # Dependencies folders.
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}
            ${TYPES_PATH}
            ${EMBEDDED_UTILS_PATH}/inc
            ${UNA_LIB_PATH}/inc
    )
    
    # Print archive size.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_SIZE_UTIL} -t lib${PROJECT_NAME}.a
    )
    
endif()

# Source files list.
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/una_at_hw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/una_at.c
)

# Header files folder.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)