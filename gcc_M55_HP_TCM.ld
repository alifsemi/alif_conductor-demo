/* The file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
* M55-HP application specific settings. These need to changed based on
* application and system memory requirements
*/
__STACK_SIZE = 0x00001000;
__HEAP_SIZE  = 0x00004000;

/*
* This linker script is for application to be executed by the
* M55-HP core. For global shared memory, make sure the memory base and size
* does not conflict with that in the M55-HE or other cores linker scripts
*/

MEMORY
{
  ITCM  (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00040000  /* 256 KB per core */
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00080000  /* 512 KB for HP core */
  SRAM0 (rwx) : ORIGIN = 0x02040000, LENGTH = 0x00040000  /* 256kB */
  SRAM1 (rwx) : ORIGIN = 0x08040000, LENGTH = 0x00040000  /* 256kB */
  MRAM  (rx)  : ORIGIN = 0x80120000, LENGTH = 0x00100000  /* 1MB */
}


ENTRY(Reset_Handler)

SECTIONS
{
  .text.at_itcm : ALIGN(8)
  {
    KEEP(*(.vectors))
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)
    *(startup_ro_data)

    KEEP(*(.eh_frame*))
  } > ITCM

  .ARM.extab : ALIGN(4)
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > ITCM

  .ARM.exidx : ALIGN(4)
  {
  __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  __exidx_end = .;
  } > ITCM

  .copy.table : ALIGN(4)
  {
    __copy_table_start__ = .;
    LONG (LOADADDR(.data.at_dtcm))
    LONG (ADDR(.data.at_dtcm))
    LONG (SIZEOF(.data.at_dtcm)/4)
    /* Add each additional data section here */
/*
    LONG (LOADADDR(.data.at_itcm))
    LONG (ADDR(.data.at_itcm))
    LONG (SIZEOF(.data.at_itcm)/4)

    LONG (LOADADDR(.data.at_sram0))
    LONG (ADDR(.data.at_sram0))
    LONG (SIZEOF(.data.at_sram0)/4)

    LONG (LOADADDR(.data.at_sram1))
    LONG (ADDR(.data.at_sram1))
    LONG (SIZEOF(.data.at_sram1)/4)
*/
    __copy_table_end__ = .;
  } > ITCM

  .zero.table : ALIGN(4)
  {
    __zero_table_start__ = .;
    /* Add each additional bss section here */
    LONG (ADDR(.bss))
    LONG (SIZEOF(.bss)/4)
    /*
    LONG (ADDR(.bss.at_sram0))
    LONG (SIZEOF(.bss.at_sram0)/4)
    LONG (ADDR(.bss.at_sram1))
    LONG (SIZEOF(.bss.at_sram1)/4)
    */
    __zero_table_end__ = .;
  } > ITCM

  __etext = .;

  .data.at_dtcm : ALIGN(8)
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))

    __data_end__ = .;
  } > DTCM AT> ITCM

  /*
   * Secondary data section, optional
   *
   * Each additional data section must be in
   * the .copy.table above to asure proper
   * initialization during startup.
   */

  /* Place non-zero code/data here if required in SRAM0 */
  /*
  .bss.at_sram0 (NOLOAD) : ALIGN(8)
  {
  } > SRAM0

  .bss.at_sram1 (NOLOAD) : ALIGN(8)
  {
  } > SRAM1
  */

   /*
   Secondary bss sections, optional

   Each additional bss section must be in
   the .zero.table above to asure proper
   initialization during startup.

   NOTE! .bss.noinit areas are not initialized with zeroes.
   They are used to test retention memories in SoC STOP
   so they must not be zeroed. Other .bss areas are zeroed.
   */
  .bss (NOLOAD) : ALIGN(8)
  {
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)

    __bss_end__ = .;
  } > DTCM

  .heap (NOLOAD) : ALIGN(8)
  {
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > DTCM

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (NOLOAD) : ALIGN(8)
  {
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region DTCM overflowed with stack")

  /* Force alignment of load region as SETools requires multiple of 16 */
  .padding : ALIGN(16)
  {
    LONG (0)
    LONG (0)
    LONG (0)
    LONG (0)
  } > ITCM
}
