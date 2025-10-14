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

set(ALIF_CORE "M55_HE" CACHE STRING "Set ALIF_CORE to: M55_HE or M55_HP.")
message(STATUS "Using Alif core: ${ALIF_CORE}")

# Set board. Can be given with cmake command, for example: -DTARGET_BOARD=DevKit-e8
set(TARGET_BOARD  "DevKit-e7" CACHE STRING "Alif Board type")
set_property(CACHE TARGET_BOARD PROPERTY STRINGS "AppKit-e7" "DevKit-e1c" "DevKit-e4" "DevKit-e7" "DevKit-e8")

# Don't change this it is resolved via ALIF_BOARD
set(ALIF_DEVICE_SKU "AE722F80F55D5" CACHE STRING "Specify Alif SKU part number")
set_property(CACHE ALIF_DEVICE_SKU PROPERTY STRINGS "AE1C1F4051920" "AE722F80F55D5" "AE822FA0E5597")

 # Sanity check TARGET_BOARD
if (NOT ((TARGET_BOARD STREQUAL "AppKit-e7") OR (TARGET_BOARD STREQUAL "DevKit-e1c") OR (TARGET_BOARD STREQUAL "DevKit-e4") OR (TARGET_BOARD STREQUAL "DevKit-e7")
    OR (TARGET_BOARD STREQUAL "DevKit-e8")))
    message(FATAL_ERROR "Possible TARGET_BOARD values are: AppKit-e7, DevKit-e1c, DevKit-e4, DevKit-e7 and DevKit-e8 but given value was ${TARGET_BOARD}")
endif()

set(ENSEMBLE_BOARD "devkit_gen2" CACHE STRING "Set ENSEMBLE_BOARD to: devkit_gen2, appkit_gen2, devkit_e8 or devkit_e1c.")

# resolve correct SKU
if ((TARGET_BOARD STREQUAL "DevKit-e8") OR (TARGET_BOARD STREQUAL "DevKit-e4")) # Add other Devkits and AppKits which are Ensemble devices with Striping support
    set(ALIF_DEVICE_SKU "AE822FA0E5597" CACHE STRING "" FORCE)
    add_compile_definitions(ENSEMBLE_SOC_GEN2)
    set(ENSEMBLE_BOARD "devkit_e8" CACHE STRING "")
elseif ((TARGET_BOARD STREQUAL "AppKit-e7") OR (TARGET_BOARD STREQUAL "DevKit-e7")) # Add other Devkit and AppKits which are Ensemble devices
    set(ALIF_DEVICE_SKU "AE722F80F55D5" CACHE STRING "" FORCE)
    if (TARGET_BOARD STREQUAL "AppKit-e7")
        set(ENSEMBLE_BOARD "appkit_gen2" CACHE STRING "")
    endif()
elseif (TARGET_BOARD STREQUAL "DevKit-e1c") # Add other Devkit and AppKits which are Balletto devices
    set(ALIF_DEVICE_SKU "AE1C1F4051920" CACHE STRING "" FORCE)
    add_compile_definitions(ENSEMBLE_SOC_E1C)
    set(ENSEMBLE_BOARD "devkit_e1c" CACHE STRING "")
endif()

set(ALIF_COMMON_UTILS_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/common_app_utils)
set(ALIF_BOARD_DIR              ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/Board)
set(ALIF_CMSIS_PACK_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/packs/AlifSemiconductor/cmsis-dfp)
set(ALIF_CMSIS_DIR              ${ALIF_CMSIS_PACK_DIR}/Alif_CMSIS)
set(ALIF_DEVICE_DIR             ${ALIF_CMSIS_PACK_DIR}/Device)
set(ALIF_DRIVERS_DIR            ${ALIF_CMSIS_PACK_DIR}/drivers)

# Set up the project
project(conductor_tool-demo-${ALIF_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

if (${ALIF_CORE} STREQUAL "M55_HP")
    set(ALIF_CORE_STRING "rtss_hp")
    add_compile_definitions(
        M55_HP
        RTSS_HP
    )
elseif (${ALIF_CORE} STREQUAL "M55_HE")
    set(ALIF_CORE_STRING "rtss_he")
    add_compile_definitions(
        M55_HE
        RTSS_HE
    )
else()
    message(FATAL_ERROR "Unsupported ALIF_CORE value!")
endif()

add_compile_definitions(
    _RTE_
    XIP_MODE
)

set(MEMORY_STITCHING OFF CACHE BOOL "Run memory stitching example.")
message(STATUS "Memory stitching: ${MEMORY_STITCHING}")
if (MEMORY_STITCHING)
    add_compile_definitions(MEMORY_STITCHING)
endif()

message(STATUS "Using Alif Ensemble board: ${ENSEMBLE_BOARD}")
if (${ENSEMBLE_BOARD} STREQUAL "devkit_gen2")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=4
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "appkit_gen2")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=5
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "devkit_e1c")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=6
    )
elseif (${ENSEMBLE_BOARD} STREQUAL "devkit_e8")
    add_compile_definitions(
        BOARD_ALIF_DEVKIT_VARIANT=7
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
        ${CMAKE_CURRENT_SOURCE_DIR}/packs/ARM/CMSIS/CMSIS/Driver/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/packs/ARM/CMSIS/CMSIS/Core/Include
)

add_library(ensemblecmsis_interface INTERFACE)
target_include_directories(ensemblecmsis_interface INTERFACE
    ${ALIF_CMSIS_DIR}/Include
    ${ALIF_CMSIS_DIR}/Include/config
    ${ALIF_DRIVERS_DIR}/include
    ${ALIF_DEVICE_DIR}/system/include
    ${ALIF_DEVICE_DIR}/core/common/config
    ${ALIF_DEVICE_DIR}/core/common/include
    ${ALIF_DEVICE_DIR}/core/${ALIF_CORE_STRING}/config
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/config
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/include
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/include/${ALIF_CORE_STRING}
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
    ${ALIF_DEVICE_DIR}/core/common/source/cache.c
    ${ALIF_DEVICE_DIR}/core/common/source/mpu.c
    ${ALIF_DEVICE_DIR}/core/common/source/pm.c
    ${ALIF_DEVICE_DIR}/core/common/source/sau_tcm_ns_setup.c
    ${ALIF_DEVICE_DIR}/core/common/source/startup.c
    ${ALIF_DEVICE_DIR}/core/common/source/system.c
    ${ALIF_DEVICE_DIR}/core/common/source/tgu.c
    ${ALIF_DEVICE_DIR}/core/common/source/vectors.c
    ${ALIF_DEVICE_DIR}/system/source/sys_utils.c
    ${ALIF_DEVICE_DIR}/system/source/sys_clocks.c
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
    ${ALIF_CMSIS_DIR}/Source/Driver_USART.c
    ${ALIF_DRIVERS_DIR}/source/uart.c
    ${ALIF_DRIVERS_DIR}/source/pinconf.c
    ${ALIF_CMSIS_DIR}/Source/Driver_IO.c
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
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/rte
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
