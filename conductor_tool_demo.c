/* Copyright (C) 2023-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "uart_tracelib.h"
#include "global_map.h"
#include "board.h"
#include "Driver_GPIO.h"
#include "pinconf.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdatomic.h>
#include <time.h>

#include "RTE_Components.h"
#include CMSIS_device_header

/*
 * Version info
*/
#define VERSION_MAJOR   (1)
#define VERSION_MINOR   (0)

/* LED0 gpio pins */
#define GPIO12_PORT                     12  /*< Use LED0_R,LED0_B GPIO port >*/
#define GPIO7_PORT                      7   /*< Use LED0_G GPIO port >*/
#define PIN3                            3   /*< LED0_R gpio pin >*/
#define PIN4                            4   /*< LED0_G gpio pin >*/
#define PIN0                            0   /*< LED0_B gpio pin >*/

/* LED1 gpio pins */
#define GPIO6_PORT                      6   /*< Use LED1_R,LED1_B,LED1_R GPIO port >*/
#define PIN2                            2   /*< LED1_R gpio pin >*/
#define PIN4                            4   /*< LED1_G gpio pin >*/
#define PIN6                            6   /*< LED1_B gpio pin >*/

#if M55_HE
/* GPIO port used for LED0_R & LED0_B */
extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO12_PORT);
ARM_DRIVER_GPIO *gpioDrv12 = &ARM_Driver_GPIO_(GPIO12_PORT);

/* GPIO port used for LED0_G */
extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO7_PORT);
ARM_DRIVER_GPIO *gpioDrv7 = &ARM_Driver_GPIO_(GPIO7_PORT);
#else
/* GPIO port used for LED1_R, LED1_B & LED1_G */
extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO6_PORT);
ARM_DRIVER_GPIO *gpioDrv6 = &ARM_Driver_GPIO_(GPIO6_PORT);
#endif

void wait_second()
{
    for (int i = 0; i < 10; i++) {
        sys_busy_loop_us(100*1000);
    }
}

/**
  \fn         void led_blink_app(void)
  \brief      LED blinky function
  \param[in]  none
  \return     none
*/
void led_blink_app (void)
{
  /*
   * gpio12 pin3 can be used as Red LED of LED0.
   * gpio7 pin4 can be used as Green LED of LED0.
   * gpio12 pin0 can be used as Blue LED of LED0.
   *
   * gpio6 pin2 can be used as Red LED of LED1.
   * gpio6 pin4 can be used as Green LED of LED1.
   * gpio6 pin6 can be used as Blue LED of LED1.
   *
   * This demo application is about.
   *   - HE Core blinks LED0, HP Core blink LED1
   *   - Blink LED0_R and LED1_R, then LED0_B and LED1_B, then LED0_G and LED1_G simultaneously in rotation.
   */

    int32_t ret = 0;

#if M55_HE
    uint8_t LED0_R = PIN3;
    uint8_t LED0_G = PIN4;
    uint8_t LED0_B = PIN0;
    ret = gpioDrv12->Initialize(LED0_R, NULL);
    ret |= gpioDrv7->Initialize(LED0_G, NULL);
    ret |= gpioDrv12->Initialize(LED0_B, NULL);
#else
    uint8_t LED1_R = PIN2;
    uint8_t LED1_G = PIN4;
    uint8_t LED1_B = PIN6;
    ret = gpioDrv6->Initialize(LED1_R, NULL);
    ret |= gpioDrv6->Initialize(LED1_G, NULL);
    ret |= gpioDrv6->Initialize(LED1_B, NULL);
#endif
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Failed to initialize\n");
        goto error_uninitialize;
    }

#if M55_HE
    ret = gpioDrv12->PowerControl(LED0_R, ARM_POWER_FULL);
    ret |= gpioDrv7->PowerControl(LED0_G, ARM_POWER_FULL);
    ret |= gpioDrv12->PowerControl(LED0_B, ARM_POWER_FULL);
#else
    ret = gpioDrv6->PowerControl(LED1_R, ARM_POWER_FULL);
    ret |= gpioDrv6->PowerControl(LED1_G, ARM_POWER_FULL);
    ret |= gpioDrv6->PowerControl(LED1_B, ARM_POWER_FULL);
#endif
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Failed to powered full\n");
        return;
    }

#if M55_HE
    ret = gpioDrv12->SetDirection(LED0_R, GPIO_PIN_DIRECTION_OUTPUT);
    ret |= gpioDrv7->SetDirection(LED0_G, GPIO_PIN_DIRECTION_OUTPUT);
    ret |= gpioDrv12->SetDirection(LED0_B, GPIO_PIN_DIRECTION_OUTPUT);
#else
    ret = gpioDrv6->SetDirection(LED1_R, GPIO_PIN_DIRECTION_OUTPUT);
    ret |= gpioDrv6->SetDirection(LED1_G, GPIO_PIN_DIRECTION_OUTPUT);
    ret |= gpioDrv6->SetDirection(LED1_B, GPIO_PIN_DIRECTION_OUTPUT);
#endif
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Failed to configure\n");
        return;
    }

    while (1)
    {
        /* Toggle Red LED */
#if M55_HE
        ret = gpioDrv12->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_HIGH);
#else
        ret = gpioDrv6->SetValue(LED1_R, GPIO_PIN_OUTPUT_STATE_HIGH);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();
#if M55_HE
        ret = gpioDrv12->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_LOW);
#else
        ret = gpioDrv6->SetValue(LED1_R, GPIO_PIN_OUTPUT_STATE_LOW);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();

        /* Toggle Green LED */
#if M55_HE
        ret = gpioDrv7->SetValue(LED0_G, GPIO_PIN_OUTPUT_STATE_HIGH);
#else
        ret = gpioDrv6->SetValue(LED1_G, GPIO_PIN_OUTPUT_STATE_HIGH);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();
#if M55_HE
        ret = gpioDrv7->SetValue(LED0_G, GPIO_PIN_OUTPUT_STATE_LOW);
#else
        ret = gpioDrv6->SetValue(LED1_G, GPIO_PIN_OUTPUT_STATE_LOW);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();

        /* Toggle Blue LED */
#if M55_HE
        ret = gpioDrv12->SetValue(LED0_B, GPIO_PIN_OUTPUT_STATE_HIGH);
#else
        ret = gpioDrv6->SetValue(LED1_B, GPIO_PIN_OUTPUT_STATE_HIGH);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();
#if M55_HE
        ret = gpioDrv12->SetValue(LED0_B, GPIO_PIN_OUTPUT_STATE_LOW);
#else
        ret = gpioDrv6->SetValue(LED1_B, GPIO_PIN_OUTPUT_STATE_LOW);
#endif
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }

        /* wait for 1 Sec */
        wait_second();
    }

error_power_off:
#if M55_HE
    ret = gpioDrv12->PowerControl(LED0_R, ARM_POWER_OFF);
    ret |= gpioDrv7->PowerControl(LED0_G, ARM_POWER_OFF);
    ret |= gpioDrv12->PowerControl(LED0_B, ARM_POWER_OFF);
#else
    ret = gpioDrv6->PowerControl(LED1_R, ARM_POWER_OFF);
    ret |= gpioDrv6->PowerControl(LED1_G, ARM_POWER_OFF);
    ret |= gpioDrv6->PowerControl(LED1_B, ARM_POWER_OFF);
#endif
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Failed to power off \n");
    } else {
        printf("LEDs power off \n");
    }

error_uninitialize:
#if M55_HE
    ret = gpioDrv12->Uninitialize(LED0_R);
    ret |= gpioDrv7->Uninitialize(LED0_G);
    ret |= gpioDrv12->Uninitialize(LED0_B);
#else
    ret = gpioDrv6->Uninitialize(LED1_R);
    ret |= gpioDrv6->Uninitialize(LED1_G);
    ret |= gpioDrv6->Uninitialize(LED1_B);
#endif
    if (ret != ARM_DRIVER_OK) {
        printf("Failed to Un-initialize \n");
    } else {
        printf("Un-initialized \n");
    }
}

static void uart_callback(uint32_t event)
{
    (void)event;
}

static void init_system()
{
    BOARD_Pinmux_Init();
}

/*  ESCLK_SEL Register: 0x10, R/W, Clock Select Register for M55-HP and M55-HE
    bits 1-0 (0x03): Select PLL clock frequency for RTSS_HP_CLK
    0x0: Select 100 MHz PLL clock
    0x1: Select 200 MHz PLL clock
    0x2: Select 300 MHz PLL clock
    0x3: Select 400 MHz PLL clock

    bits 5-4 (0x30): Select PLL clock frequency for RTSS_HE_CLK
    0x0: Select 60 MHz PLL clock
    0x1: Select 80 MHz PLL clock
    0x2: Select 120 MHz PLL clock
    0x3: Select 160 MHz PLL clock */
void print_clocks()
{
    uint32_t *clk_reg = (uint32_t*)(CGU_BASE + 0x10);
    uint32_t he_clk;
    uint32_t hp_clk;

    switch ((*clk_reg) & 0x03)
    {
    case 0:
        hp_clk = 100;
        break;
    case 1:
        hp_clk = 200;
        break;
    case 2:
        hp_clk = 300;
        break;
    case 3:
    default:
        hp_clk = 400;
        break;
    }

    switch ((*clk_reg) & 0x30)
    {
    case 0x00:
        he_clk = 60;
        break;
    case 0x10:
        he_clk = 80;
        break;
    case 0x20:
        he_clk = 120;
        break;
    case 0x30:
    default:
        he_clk = 160;
        break;
    }

    printf("\nHP clock frequency = %" PRIu32 "Mhz\n", hp_clk);
    printf("HE clock frequency = %" PRIu32 "Mhz\n", he_clk);
}

int main()
{
    init_system();

    tracelib_init(NULL, uart_callback);

#if M55_HE
    printf("\nConductor Tool Demo app v%" PRIu32 ".%" PRIu32 " on M55_HE\n\n", (uint32_t)VERSION_MAJOR, (uint32_t)VERSION_MINOR);

#if MEMORY_STITCHING
    printf("Trying to read from relocated SRAM1 (0x%08" PRIx32 ")\n", (uint32_t)0x02400000);
    volatile uint32_t *data = (uint32_t*)0x02400000;
    printf("Data from SRAM1: 0x%08" PRIx32 "\n", *data);
#else
    printf("Trying to read SRAM1 (0x%08" PRIx32 ")\n", (uint32_t)SRAM1_BASE);
    volatile uint32_t *data = (uint32_t*)SRAM1_BASE;
    printf("Data from SRAM1: 0x%08" PRIx32 "\n", *data);
#endif

#else
    printf("\nConductor Tool Demo app v%" PRIu32 ".%" PRIu32 " on M55_HP\n\n", (uint32_t)VERSION_MAJOR, (uint32_t)VERSION_MINOR);
    printf("Trying to read SRAM0 (0x%08" PRIx32 ")\n", (uint32_t)SRAM0_BASE);
    volatile uint32_t *data = (uint32_t*)SRAM0_BASE;
    printf("Data from SRAM0: 0x%08" PRIx32 "\n", *data);
#endif

    print_clocks();

    led_blink_app();

    return 0;
}
