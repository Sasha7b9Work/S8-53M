// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


void HAL_TIM7::Init()
{
    __TIM7_CLK_ENABLE();
}


void HAL_TIM7::Config(uint16 prescaler, uint16 period)
{
    static TIM_HandleTypeDef htim =
    {
        TIM7,
        {
            0,
            TIM_COUNTERMODE_UP,
        }
    };

    htim.Init.Prescaler = prescaler;
    htim.Init.Period = period;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&htim) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    TIM_MasterConfigTypeDef masterConfig =
    {
        TIM_TRGO_UPDATE,
        TIM_MASTERSLAVEMODE_DISABLE
    };

    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &masterConfig) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_TIM_Base_Stop(&htim) != HAL_OK)
    {
        ERROR_HANDLER();
    }


    if (HAL_TIM_Base_Start(&htim) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}
