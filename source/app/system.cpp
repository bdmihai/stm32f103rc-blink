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

#include "stm32f1xx.h"
#include "gpio.h"
#include "system.h"

void system_init()
{
    /* enable the FLASH prefetch buffer */ 
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);

    /* enable the external High Speed Clock */
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    do {
    } while ((RCC->CR & RCC_CR_HSERDY_Msk) != RCC_CR_HSERDY);

    /* configure the PLL: no pre-external divider, with x9 multiplier (8x9 = 72 MHz) */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLMULL_Msk | RCC_CFGR_PLLXTPRE_Msk, RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

    /* enable PLL clock generation */
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    do {
    } while ((RCC->CR & RCC_CR_PLLRDY_Msk) != RCC_CR_PLLRDY);

    /* to correctly read data from FLASH memory, the number of wait states (LATENCY) 
       must be correctly programmed according to the frequency of the CPU clock 
       (HCLK) of the device. */
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_1);

    /* set the highest APBx dividers in order to ensure that we do not go through
        a non-spec phase whatever we decrease or increase HCLK. */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV16);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV16);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE_Msk, RCC_CFGR_HPRE_DIV1);

    /* set the SYSCLK to PLL */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_PLL);
    do {
    } while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

    /* disable HSI clock */
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);
    do {
    } while ((RCC->CR & RCC_CR_HSIRDY_Msk) == RCC_CR_HSIRDY);

    /* confgure the APB clocks */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV2);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV1);

    /* enable APB2 ports clock */
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); // necessary for JTAG Disable (remapping)

    /* set-up the vector table in ram */
    extern char __isr_vector_start;
    SCB->VTOR = (uintptr_t) &__isr_vector_start;
}

extern "C" unsigned int system_cpu_f()
{
    return 72000000U;
}