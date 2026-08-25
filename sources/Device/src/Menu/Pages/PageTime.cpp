// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "Utils/Math.h"
#include "Display/Screen/Grid.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Text/Warnings.h"


extern const Page pTime;


static bool IsActive_Sample()
{
    return TBase::InModeRandomizer();
}


static const Choice mcSample =
{
    TypeItem::Choice, &pTime, IsActive_Sample,
    {
        "Выборка", "Sampling"
        ,
        "\"Реальная\" - \n"
        "\"Эквивалентная\" -"
        ,
        "\"Real\" - \n"
        "\"Equals\" - "
    },
    {
        {"Реальное время",  "Real"},
        {"Эквивалентная",   "Equals"}
    },
    (int8*)&SET_SAMPLE_TYPE
};


static bool IsActive_PeakDet()
{
    return (SET_TBASE >= TBase::MIN_PEC_DEAT);
}


void OnChanged_PeakDet(bool active)
{
    if (active)
    {
        bool running = FPGA::IsRunning();
        FPGA::Stop();

        PeackDetMode::Set(SET_PEAKDET);
        TBase::Set(SET_TBASE);

        if (SET_PEAKDET_IS_DISABLED)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            SHIFT_IN_MEMORY = centerX * 2 - Grid::Width() / 2;
            PageMemory::OnChanged_NumPoints(true);
        }
        else if (SET_PEAKDET_IS_ENABLED)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            LIMITATION(SHIFT_IN_MEMORY, centerX / 2 - Grid::Width() / 2, 0, ENUM_POINTS_FPGA::ToNumPoints() - Grid::Width());
            PageMemory::OnChanged_NumPoints(true);
        }

        if (running)
        {
            FPGA::Start();
        }
    }
    else
    {
        Warning::ShowBad(Warning::TooSmallSweepForPeakDet);
    }
}


static const Choice mcPeakDet =
{
    TypeItem::Choice, &pTime, IsActive_PeakDet,
    {
        "Пик дет", "Pic deat",
        "Включает/выключает пиковый детектор.",
        "Turns on/off peak detector."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_PEAKDET, OnChanged_PeakDet
};


void PageTime::OnChanged_TPos(bool)
{
    TPos::Set(SET_TPOS);
}


static const Choice mcTPos =
{
    TypeItem::Choice, &pTime, 0,
    {
        "\x7b", "\x7b",
        "Задаёт точку привязки нулевого смещения по времени к экрану - левый край, центр, правый край.",
        "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge."
    },
    {
        {"Лево",    "Left"},
        {"Центр",   "Center"},
        {"Право",   "Right"}
    },
    (int8*)&SET_TPOS, PageTime::OnChanged_TPos
};


static bool IsActive_SelfRecorder()
{
    return TBase::InModeP2P();
}


static void OnChanged_SelfRecorder(bool active)
{
    if (active)
    {
        FPGA::LoadRegUPR();
    }
    else
    {
        Warning::ShowBad(Warning::TooFastScanForSelfRecorder);
    }
}


static const Choice mcSelfRecorder =
{
    TypeItem::Choice, &pTime, IsActive_SelfRecorder,
    {
        "Самописец", "Self-Recorder",
        "Включает/выключает режим самописца. Этот режим доступен на развёртках 20мс/дел и более медленных.",
        "Turn on/off the recorder. This mode is available for scanning 20ms/div and slower."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_SELFRECORDER, OnChanged_SelfRecorder
};


static const Choice mcDivRole =
{
    TypeItem::Choice, &pTime, 0,
    {
        "Ф-ция ВР/ДЕЛ", "Func Time/DIV"
        ,
        "Задаёт функцию для ручки ВРЕМЯ/ДЕЛ: в режиме сбора информации (ПУСК/СТОП в положении ПУСК):\n"
        "1. \"Время\" - изменение смещения по времени.\n"
        "2. \"Память\" - перемещение по памяти."
        ,
        "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
        "1. \"Time\" - change the time shift.\n"
        "2. \"Memory\" - moving from memory."
    },
    {
        {"Время",   "Time"},
        {"Память",  "Memory"}
    },
    (int8*)&SET_TIME_DIV_XPOS
};


static const arrayItems itemsTime =
{
    (void *)&mcSample,       // РАЗВЕРТКА - Выборка
    (void *)&mcPeakDet,      // РАЗВЕРТКА - Пик дет
    (void *)&mcTPos,         // РАЗВЕРТКА - To
    (void *)&mcSelfRecorder, // РАЗВЕРТКА - Самописец
    (void *)&mcDivRole       // РАЗВЕРТКА - Ф-ция ВР/ДЕЛ    
};

static const Page pTime            // РАЗВЕРТКА
(
    PageMain::self, 0,
    "РАЗВЕРТКА", "SCAN",
    "Содержит настройки развёртки.",
    "Contains scan settings.",
    NamePage::Time, &itemsTime
);

const Page *PageTime::self = &pTime;
