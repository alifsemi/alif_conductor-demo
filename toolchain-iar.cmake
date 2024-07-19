#/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */
set(TOOLKIT arm)

set(CMAKE_C_COMPILER                iccarm)
set(CMAKE_CXX_COMPILER              iccarm)
set(CMAKE_ASM_COMPILER              iasmarm)
SET(CMAKE_LINKER                    ilinkarm)
set(CMAKE_CROSSCOMPILING            true)
set(CMAKE_SYSTEM_NAME               Generic)

if ("${ENSEMBLE_CORE}" STREQUAL "M55_HE" OR "${ENSEMBLE_CORE}" STREQUAL "M55_HP")
    set(CMAKE_SYSTEM_PROCESSOR          "cortex-m55")
    set(CPU_COMPILE_DEF                 CPU_CORTEX_M55)
endif()
if ("${ENSEMBLE_CORE}" STREQUAL "A32")
    set(CMAKE_SYSTEM_PROCESSOR          "cortex-a32")
endif()

set(CPU_NAME                        ${CMAKE_SYSTEM_PROCESSOR})

set(BINARY_EXTENSION                ".elf")

find_program(CMAKE_C_COMPILER       iccarm)
find_program(CMAKE_CXX_COMPILER     iccarm)
find_program(CMAKE_ASM_COMPILER     iasmarm)
find_program(CMAKE_LINKER           ilinkarm)

# Do not link when testing compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (DEFINED XIP)
    set(ALIF_LINKER_FILE                ${CMAKE_CURRENT_SOURCE_DIR}/linker/iar_${ENSEMBLE_CORE}_MRAM.icf)
    set (XIP ${XIP}) # To avoid CMake warning about unused XIP variable
else()
    # Default is TCM
    set(ALIF_LINKER_FILE                ${CMAKE_CURRENT_SOURCE_DIR}/linker/iar_${ENSEMBLE_CORE}_TCM.icf)
endif()

set(CMAKE_C_FLAGS_DEBUG            "--no_cse --no_unroll --no_inline --no_code_motion --no_tbaa --no_clustering --no_scheduling --debug" CACHE STRING "Flags used by the C compiler during DEBUG builds.")
set(CMAKE_C_FLAGS_MINSIZEREL       "-Ohz -DNDEBUG" CACHE STRING "Flags used by the C compiler during MINSIZEREL builds.")
set(CMAKE_C_FLAGS_RELEASE          "-Ohs -DNDEBUG" CACHE STRING "Flags used by the C compiler during RELEASE builds.")

set(CMAKE_CXX_FLAGS_DEBUG          "--no_cse --no_unroll --no_inline --no_code_motion --no_tbaa --no_clustering --no_scheduling --debug" CACHE STRING "Flags used by the CXX compiler during DEBUG builds.")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Ohz -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during MINSIZEREL builds.")
set(CMAKE_CXX_FLAGS_RELEASE        "-Oh -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during RELEASE builds.")


if ("${ENSEMBLE_CORE}" STREQUAL "M55_HE" OR "${ENSEMBLE_CORE}" STREQUAL "M55_HP")
    add_compile_options(
        --cpu=Cortex-M55
        --fpu=VFPv5_d16
        --cpu_mode=thumb
    )
    add_link_options(
        --cpu=Cortex-M55
        --fpu=VFPv5_d16
    )
endif()
if ("${ENSEMBLE_CORE}" STREQUAL "A32")
    add_compile_options(
        --cpu=Cortex-A32
        --fpu=VFPv5
    )
    add_link_options(
        --cpu=Cortex-A32
        --fpu=VFPv5
    )
endif()

add_compile_options(
    -DCLOCKS_PER_SEC=1000
    -e
    --vectorize
    --char-is-unsigned
    --dlib_config normal
    $<$<COMPILE_LANGUAGE:CXX>:--c++>
)
add_link_options(
    --entry=Reset_Handler
)

macro(set_warnings_as_errors target)
    target_compile_options(${target} PRIVATE
        --warnings_are_errors
    )
endmacro()

#Note: IAR require prototypes is stricter than GCCs implicit function declaration
#      For example CMSIS interrupt handlers do not have prototypes and build fails
#      --> treat as warning for now
macro(set_werror_implicit_function_declaration target)
    target_compile_options(${target} PRIVATE
        --require_prototypes
        --diag_warning Pa045
    )
endmacro()

macro(add_elf_to_bin_phase target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ielftool --bin --verbose ${target}.elf ${target}.bin
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endmacro()

macro(add_linker_file_to_target target linker_file)
    message(STATUS "Using ICF file: ${linker_file} for target ${target}")

    target_link_options(${target}
        PRIVATE
        --config=${linker_file}
        --map=${target}.map
    )

endmacro()
