// 2022/02/11 17:51:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Timer.h"
#include "Log.h"
#include <stm32f4xx_hal.h>


namespace Timer
{
    struct TimerStruct
    {
        void (*f)();
        int  reactionTimeMS;
        int  currentTimeMS;
        bool isRun;

        void Update(uint dT);
    };

    TimerStruct timers[TypeTimer::Count];

    uint timeStartLogging = 0;
    uint timePrevPoint = 0;

    void Pause(TypeTimer);
    void Continue(TypeTimer);
    bool IsRun(TypeTimer);
}


void Timer::Init()
{
    for (int i = 0; i < TypeTimer::Count; i++)
    {
        timers[i].isRun = false;
    }
}


void Timer::PauseOnTime(uint timeMS)
{
    HAL_Delay(timeMS);
};


void Timer::PauseOnTicks(uint numTicks)
{
    uint startTicks = TIM2->CNT;
    while (TIM2->CNT - startTicks < numTicks) {};
}


uint Timer::GetMS()
{
    return HAL_GetTick();
}


uint Timer::GetTicks()
{
    return TIM2->CNT;
}


void Timer::StartMultiMeasurement()
{
    TIM2->CR1 &= (uint16)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN; 
}


void Timer::StartLogging()
{
    timeStartLogging = TIM2->CNT;
    timePrevPoint = timeStartLogging;
}


uint Timer::LogPointUS(char *name)
{
    uint interval = TIM2->CNT - timePrevPoint;
    timePrevPoint = TIM2->CNT;
    LOG_WRITE("%s %.2f us", name, interval / (float)TICKS_IN_US);
    return interval;
}


uint Timer::LogPointMS(char *name)
{
    uint interval = TIM2->CNT - timePrevPoint;
    timePrevPoint = TIM2->CNT;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}


void Timer::Enable(TypeTimer::E type, int timeInMS, pFuncVV eF)
{
    timers[type].f = eF;
    timers[type].reactionTimeMS = timeInMS;
    timers[type].currentTimeMS = timeInMS - 1;
    timers[type].isRun = true;
}


void Timer::Disable(TypeTimer::E type)
{
    timers[type].isRun = false;
}


void Timer::Pause(TypeTimer::E type)
{
    Disable(type);
}


void Timer::Continue(TypeTimer::E type)
{
    timers[type].isRun = true;
}


bool Timer::IsRun(TypeTimer::E type)
{
    return timers[type].isRun;
};


void Timer::TimerStruct::Update(uint dT)
{
    if (isRun)
    {
        currentTimeMS -= dT;

        if (currentTimeMS < 0)
        {
            f();
            currentTimeMS = reactionTimeMS - 1;
        }
    }
}


void Timer::Update1ms()
{
    for(int num = 0; num < TypeTimer::Count; num++)
    {
        timers[num].Update(1);
    }
}


void Timer::Update10ms()
{
    for(int num = 0; num < TypeTimer::Count; num++)
    {
        timers[num].Update(10);
    }
}


TimeMeterMS::TimeMeterMS()
{
    Reset();
}


void TimeMeterMS::Reset()
{
    time_reset = TIME_MS;
    time_pause = 0;
}


void TimeMeterMS::Pause()
{
    time_pause = TIME_MS;
}


void TimeMeterMS::Continue()
{
    time_reset += (TIME_MS - time_pause);
}


uint TimeMeterMS::ElapsedTime()
{
    return TIME_MS - time_reset;
}


TimeMeterUS::TimeMeterUS() : ticks_reset(TIM2->CNT) { }


void TimeMeterUS::Reset()
{
    ticks_reset = TIM2->CNT;
}


uint TimeMeterUS::ElapsedUS()
{
    return (TIM2->CNT - ticks_reset) / TICKS_IN_US;
}


uint TimeMeterUS::ElapsedTicks()
{
    return (TIM2->CNT - ticks_reset);
}
