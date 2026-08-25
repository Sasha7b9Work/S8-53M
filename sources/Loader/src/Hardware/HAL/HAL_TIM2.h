// 2022/02/01 11:50:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


/**
  * TIME_TICKS - количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement().
  * В одной секунде 60.000.000 тиков. Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
  * Количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement(). Не более (1 << 32).
***/

#define TIMER_TICKS HAL_TIM2::GetTicks()
#define TICKS_IN_MS 60000               // Столько тиков содержится в 1 мс
#define TICKS_IN_US 60                  // Столько тиков содержится в 1 мкс

#ifdef GUI
    #define TIMER_MS HAL_TIM::TimeMS()
#else
    extern volatile unsigned int uwTick;
    #define TIMER_MS              uwTick
#endif


namespace HAL_TIM2
{
    void Init();

    uint GetTicks();

    // Запускает счётчик для измерения малых отрезков времени.
    void StartMultiMeasurement();

    void DelayTicks(uint numTicks);

    void DelayUS(uint timeUS);

    // Задержка на timeMS миллисекунд
    void DelayMS(uint timeMS);

    namespace Logging
    {
        // Устанавливает стартовую точку логгирования.
        // Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
        void Start();

        uint PointUS(char *name);

        uint PointMS(char *name);
    }
};
