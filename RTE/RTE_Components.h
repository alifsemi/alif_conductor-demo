/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

/*
 * Define the Device Header File:
*/
#if defined(M55_HP)
    #define CMSIS_device_header "M55_HP.h"
#elif defined(M55_HE)
    #define CMSIS_device_header "M55_HE.h"
#elif defined(A32)
    #define CMSIS_device_header "a32_device.h"
#else
    #error "Undefined M55 CPU!"
#endif

/* AlifSemiconductor::CMSIS Driver.USART.USART */
#define RTE_Drivers_USART0   1           /* Driver UART0 */
#define RTE_Drivers_USART2   1           /* Driver UART1 */
#define RTE_Drivers_USART4   1           /* Driver UART1 */
#define RTE_Drivers_GPIO     1           /* Driver GPIO */
#define RTE_Drivers_PINCONF  1           /* Driver PinPAD and PinMux */

#endif /* RTE_COMPONENTS_H */
