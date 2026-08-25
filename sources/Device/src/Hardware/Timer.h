// 2022/02/11 17:52:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once 
#include "defines.h"


#define TIME_MS     Timer::GetMS()
#define TIME_TICKS  Timer::GetTicks()
#define TICKS_IN_US 60


struct TypeTimer
{
    enum E
    {
        ShowLevelRShiftA,       // Нужно устанавливать, когда изменяется положение ручки смещения канала 1.
        ShowLevelRShiftB,       // Нужно устанавливать, когда изменяется положение ручки смещения канала 2.
        ShowLevelTrigLev,       // Нужно устанавливать, когда изменяется положение ручки уровня синхронизации
        NumSignalsInSec,        // Для установки количества считываний сигнала в секунду.
        FlashDisplay,           // Таймер для мерцающих участков экрана чёрно-белого дисплея.
        P2P,                    // Таймер для чтения данных в режиме поточечного вывода.
        ShowMessages,           // Таймер для засекания время показа информационных и предупреждающих сообщений.
        MenuAutoHide,           // Таймер для отсчёта времени скрывания меню.
        RShiftMarkersAutoHide,  // Таймер для отсчёта времени скрывания дополнительных боковых меток.
        Temp,                   // Вспомогательный, для общих нужд.
        StopSound,              // Выключить звук
        UpdateDisplay,          // Этот таймер могут использовать функции, использующие свою процедуру рисования
        CDC,                    // Вспомогательный таймер для CDC
        DisableTrigLED,         // Таймер для выключения лампочки синхронизации
        Count                   // Общее количество таймеров.
    };
};


namespace Timer
{
    void Init();

    // Задержка на timeMS миллисекунд
    void PauseOnTime(uint timeMS);

    void PauseOnTicks(uint numTicks);

    uint GetMS();

    // Количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement().
    // В одной секунде 120.000.000 тиков.Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
    // Количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement().Не более(1 << 32).
    uint GetTicks();

    // Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутренее время таймеров на 1мс.
    void Update1ms();

    // Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутреннее время таймеров на 10мс.
    void Update10ms();

    void Enable(TypeTimer::E, int timeInMS, pFuncVV);

    // Остановить таймер.
    void Disable(TypeTimer::E);

    // Приостановить таймер. Запустить снова можно функцией Timer_Continue().
    void Pause(TypeTimer::E);

    // Продолжить работу таймера, ранее приостановленного функцией Timer_Pause().
    void Continue(TypeTimer::E);

    // С помощью этой функции можно узнать, работает ли таймер.
    bool IsRun(TypeTimer::E);

    // Запускает счётчик для измерения малых отрезков времени.
    void StartMultiMeasurement();

    // Устанавливает стартовую точку логгирования. Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
    void StartLogging();

    uint LogPointUS(char *name);

    uint LogPointMS(char *name);
};


// Измеритель отрезков времени
struct TimeMeterUS
{
    TimeMeterUS();

    void Reset();

    uint ElapsedUS();

    uint ElapsedTicks();

private:

    uint ticks_reset;
};


// Структура для отсчёта времени
struct TimeMeterMS
{
    TimeMeterMS();

    // Установить момент отсчёта
    void Reset();

    void Pause();

    void Continue();

    // Столько миллисекунд прошло с момента вызова Reset()
    uint ElapsedTime();

private:

    uint time_reset;        // От этого времени отсчитывается ElapsedTime()
    uint time_pause;        // В этот момент поставили на паузу
};
