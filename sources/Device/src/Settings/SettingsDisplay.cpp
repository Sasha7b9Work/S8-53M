// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsDisplay.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include "Display/Screen/Grid.h"


int ENumAccumulation::number_drawing = 0;


int Smoothing::ToPoints()
{
    if (SMOOTHING_IS_DISABLE)
    {
        return 0;
    }
    return (int)SMOOTHING + 1;          // WARN Здесь очень коряво реализовано
}


BitSet32 SettingsDisplay::PointsOnDisplay()
{
    BitSet32 result;

    result.half_iword[0] = SHIFT_IN_MEMORY;
    result.half_iword[1] = SHIFT_IN_MEMORY + Grid::Width() + 1;

    return result;
}


int SettingsDisplay::TimeMenuAutoHide()
{
    static const int times[] =
    {
        0, 5, 10, 15, 30, 60
    };

    return times[MENU_AUTO_HIDE] * 1000;
}


bool SettingsDisplay::IsSeparate()
{
    return (!DISABLED_DRAW_MATH && MODE_DRAW_MATH_IS_SEPARATE) || ENABLED_FFT;
}


ENumSignalsInSec::E ENumSignalsInSec::FromNum(int num)
{
    if (num == 1)
    {
        return ENumSignalsInSec::_1;
    }
    else if (num == 2)
    {
        return ENumSignalsInSec::_2;
    }
    else if (num == 5)
    {
        return ENumSignalsInSec::_5;
    }
    else if (num == 10)
    {
        return ENumSignalsInSec::_10;
    }
    else if (num == 25)
    {
        return ENumSignalsInSec::_25;
    }

    LOG_ERROR_TRACE("Число сигналов в секунду равно %d", num);

    return ENumSignalsInSec::_1;
}


int ENumSignalsInSec::ToNum(ENumSignalsInSec::E enumSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};

    if (enumSignalsInSec <= ENumSignalsInSec::_1)
    {
        return fps[enumSignalsInSec];
    }

    LOG_ERROR_TRACE("Неправильный параметр %d", (int)enumSignalsInSec);

    return 1;
}


int ENumAveraging::ToNumber()
{
    int num_ave = (1 << gset.display.enumAve);

    if (TBase::InModeRandomizer())
    {
        if (num_ave < nrst.numAveForRand)
        {
            num_ave = nrst.numAveForRand;
        }
    }

    return num_ave;
}


ModeAveraging::E ModeAveraging::Current()
{
    return TBase::InModeRandomizer() ? ModeAveraging::Around : gset.display.modeAve;
}
