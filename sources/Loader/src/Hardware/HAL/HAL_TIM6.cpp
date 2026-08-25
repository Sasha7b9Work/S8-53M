// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


static TIM_HandleTypeDef handleTIM6 =
{
    TIM6,
    {
        119,                    // Init.Prescaler
        TIM_COUNTERMODE_UP,     // Init.CounterMode
        500,                    // Init.Period
        TIM_CLOCKDIVISION_DIV1  // Init.ClockDivision
    }
};


void HAL_TIM6::Init()
{
    // Таймер для мс
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    HAL_TIM_Base_Init(&handleTIM6);

    HAL_TIM_Base_Start_IT(&handleTIM6);
}


bool HAL_TIM6::CallbackOnIRQ()
{
    if (__HAL_TIM_GET_FLAG(&handleTIM6, TIM_FLAG_UPDATE) == SET &&
        __HAL_TIM_GET_ITSTATUS(&handleTIM6, TIM_IT_UPDATE))
    {
        Timer::Callback1ms();
        __HAL_TIM_CLEAR_FLAG(&handleTIM6, TIM_FLAG_UPDATE);
        __HAL_TIM_CLEAR_IT(&handleTIM6, TIM_IT_UPDATE);

        return true;
    }

    return false;
}
