// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_TIM2
{
    namespace Logging
    {
        static uint timeStart = 0;
        static uint timePrevPoint = 0;
    }
}


void HAL_TIM2::Init()
{
    // Таймер для тиков
    TIM_HandleTypeDef handleTIM2 =
    {
        TIM2,
        {
            0,
            TIM_COUNTERMODE_UP,
            0xffffffffU,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    HAL_TIM_Base_Init(&handleTIM2);

    HAL_TIM_Base_Start(&handleTIM2);
}


uint HAL_TIM2::GetTicks()
{
#ifdef GUI
    return HAL_TIM::TimeMS();
#else
    return TIM2->CNT;
#endif
}


void HAL_TIM2::StartMultiMeasurement()
{
#ifndef GUI
    TIM2->CR1 &= (uint)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
#endif
}


void HAL_TIM2::DelayUS(uint timeUS)
{
    uint startTicks = TIMER_TICKS;

    uint num_tics = timeUS * TICKS_IN_US;

    while (TIMER_TICKS - startTicks < num_tics) {};
}


void HAL_TIM2::DelayMS(uint timeMS)
{
    HAL_Delay(timeMS);
}


void HAL_TIM2::DelayTicks(uint numTicks)
{
    uint startTicks = TIMER_TICKS;
    while (TIMER_TICKS - startTicks < numTicks) {};
}


uint HAL_TIM::TimeMS()
{
    return HAL_GetTick();
}


void HAL_TIM2::Logging::Start()
{
    timeStart = TIMER_TICKS;
    timePrevPoint = timeStart;
}


uint HAL_TIM2::Logging::PointUS(char *name)
{
    uint interval = TIMER_TICKS - timePrevPoint;
    timePrevPoint = TIMER_TICKS;
    LOG_WRITE("%s %.2f us", name, (float)(interval) / 120.0F);
    return interval;
}


uint HAL_TIM2::Logging::PointMS(char *name)
{
    uint interval = TIMER_TICKS - timePrevPoint;
    timePrevPoint = TIMER_TICKS;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}
