#/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

# Get rid of warnings when running cmake ..
cmake_policy(SET CMP0123 NEW)

set(ENSEMBLE_CORE "M55_HE" CACHE STRING "Set ENSEMBLE_CORE to: M55_HE or M55_HP.")
message(STATUS "Using Alif Ensemble core: ${ENSEMBLE_CORE}")

# These can be also set via command line
set(ALIF_COMMON_UTILS_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/common_app_utils)
set(ALIF_BOARD_DIR              ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/Board)
set(ALIF_ENSEMBLE_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/Ensemble)
set(ALIF_DRIVER_DIR             ${ALIF_ENSEMBLE_DIR}/Alif_CMSIS)
set(ALIF_DEVICE_DIR             ${ALIF_ENSEMBLE_DIR}/Device)
set(ALIF_ENSEMBLE_DRIVERS_DIR   ${ALIF_ENSEMBLE_DIR}/drivers)
set(ALIF_RTE_DIR                ${ALIF_DEVICE_DIR}/E7/AE722F80F55D5XX)

# Set up the project
project(conductor_tool-demo-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

if (${ENSEMBLE_CORE} STREQUAL "M55_HP")
    add_compile_definitions(
        _RTE_
        M55_HP
    )
elseif (${ENSEMBLE_CORE} STREQUAL "M55_HE")
    add_compile_definitions(
        _RTE_
        M55_HE
    )
else()
    message(FATAL_ERROR "Unsupported ENSEMBLE_CORE value!")
endif()

if (DEFINED XIP)
    add_compile_definitions(
        XIP_MODE
    )
endif()

set(MEMORY_STITCHING OFF CACHE BOOL "Run memory stitching example.")
message(STATUS "Memory stitching: ${MEMORY_STITCHING}")
if (MEMORY_STITCHING)
    add_compile_definitions(MEMORY_STITCHING)
endif()

set(ENSEMBLE_BOARD "devkit_gen2" CACHE STRING "Set ENSEMBLE_BOARD to: devkit_gen1/2, appkit_alpha1/2, appkit_gen2 or devkit_b0_cob.")
message(STATUS "Using Alif Ensemble board: ${ENSEMBLE_BOARD}")

if (${ENSEMBLE_BOARD} STREQUAL "devkit_gen1")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=0
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "appkit_alpha1")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=1
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "appkit_alpha2")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=2
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "devkit_b0_cob")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=3
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "devkit_gen2")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=4
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "appkit_gen2")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=5
    )
else()
    message(FATAL_ERROR "Unsupported ENSEMBLE_BOARD value!")
endif()

add_executable(${PROJECT_NAME})

# Generate .axf or .elf depending on the compiler
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    SUFFIX ${BINARY_EXTENSION}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# add bin generation from axf/elf file
add_elf_to_bin_phase(${PROJECT_NAME})

# Add sources
target_sources(${PROJECT_NAME} PRIVATE
    ${ALIF_COMMON_UTILS_DIR}/logging/retarget.c
    ${ALIF_COMMON_UTILS_DIR}/logging/uart_tracelib.c
    ${ALIF_COMMON_UTILS_DIR}/fault_handler/fault_handler.c
    conductor_tool_demo.c
)

add_library(armcmsis_interface INTERFACE)
target_include_directories(armcmsis_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/packs/ARM/CMSIS/5.9.0/CMSIS/Driver/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/packs/ARM/CMSIS/5.9.0/CMSIS/Core/Include
)

add_library(ensemblecmsis_interface INTERFACE)
target_include_directories(ensemblecmsis_interface INTERFACE
    ${ALIF_DRIVER_DIR}/Include
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/include
    ${ALIF_DEVICE_DIR}/common/include
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/include
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/config
)

add_library(ensembleboard_interface INTERFACE)
target_include_directories(ensembleboard_interface INTERFACE
    ${ALIF_BOARD_DIR}
    ${ALIF_BOARD_DIR}/${ENSEMBLE_BOARD}
    ${ALIF_DEVICE_DIR}/common/config
)

###################################################
##  Add core startup files                       ##
###################################################
add_library(corestartup STATIC)
target_sources(corestartup PRIVATE
    RTE/mpu_M55.c     # Own variant of MPU needed for memory stitching
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/source/startup_${ENSEMBLE_CORE}.c
    ${ALIF_DEVICE_DIR}/common/source/system_M55.c
    ${ALIF_DEVICE_DIR}/common/source/system_utils.c
    ${ALIF_DEVICE_DIR}/common/source/clk.c
    ${ALIF_DEVICE_DIR}/common/source/tgu_M55.c
    ${ALIF_DEVICE_DIR}/common/source/pm.c
    ${ALIF_DEVICE_DIR}/common/source/tcm_partition.c
)

target_link_libraries(corestartup PRIVATE
    ensemblecmsis_interface
    armcmsis_interface
    ensembleboard_interface
    rte_interface
)


###################################################
##  Add CMSIS driver library                     ##
###################################################
add_library(ensemblecmsis)

target_sources(ensemblecmsis PRIVATE
    ${ALIF_BOARD_DIR}/${ENSEMBLE_BOARD}/board_init.c
    ${ALIF_DRIVER_DIR}/Source/Driver_USART.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/uart.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/pinconf.c
    ${ALIF_DRIVER_DIR}/Source/Driver_GPIO.c
)

set_werror_implicit_function_declaration(ensemblecmsis)

target_link_libraries(ensemblecmsis
PRIVATE
    armcmsis_interface
    rte_interface
    corestartup
PUBLIC
    ensembleboard_interface
    ensemblecmsis_interface
)

###################################################
##  Add Conductor Tool Demo interface            ##
###################################################
add_library(conductor_tool_demo_interface INTERFACE)
target_include_directories(conductor_tool_demo_interface INTERFACE
    ${ALIF_COMMON_UTILS_DIR}/logging
    ${ALIF_COMMON_UTILS_DIR}/profiling
    ${ALIF_COMMON_UTILS_DIR}/fault_handler
)
target_link_libraries(conductor_tool_demo_interface INTERFACE
    ensembleboard_interface
    ensemblecmsis_interface
    armcmsis_interface
)

add_library(rte_interface INTERFACE)
target_include_directories(rte_interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/RTE
    ${ALIF_RTE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    rte_interface
    conductor_tool_demo_interface
    ensemblecmsis
)

# Tell linker that reset interrupt handler is our entry point
add_link_options(--entry=Reset_Handler)

# Warnings as errors
set_warnings_as_errors(${PROJECT_NAME})
add_linker_file_to_target(${PROJECT_NAME} ${ALIF_LINKER_FILE})
