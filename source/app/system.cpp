/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2023 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 15.Dec.2023  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <assert.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "system.h"

void system_init()
{
    SystemInit();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    assert(HAL_Init() == HAL_OK);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    assert(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK);

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    assert(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) == HAL_OK);

    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC
                                         |RCC_PERIPHCLK_USB;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    assert(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) == HAL_OK);

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
    */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    /* set-up the vector table in ram */
    extern char __isr_vector_start;
    SCB->VTOR = (uintptr_t) &__isr_vector_start;
}

extern "C" unsigned int system_cpu_f()
{
    return SystemCoreClock;
}