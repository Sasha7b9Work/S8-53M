// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "Settings.h"
#include "SettingsChannel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/Text/Text.h"


// Структура для описания диапазона масштаба по напряжению.
struct RangeStruct
{
    pchar name[2][2];     // Название диапазона в текстовом виде, пригодном для вывода на экран.
};


// Массив структур описаний масштабов по напряжению.
static const RangeStruct ranges[Range::Count] =
{
    {{{"2\x10мВ",     "20\x10мВ"}, {"2\x10mV",  "20\x10mV"}}},
    {{{"5\x10мВ",     "50\x10мВ"}, {"5\x10mV",  "50\x10mV"}}},
    {{{"10\x10мВ",    "0.1\x10В"}, {"10\x10mV", "0.1\x10V"}}},
    {{{"20\x10мВ",    "0.2\x10В"}, {"20\x10mV", "0.2\x10V"}}},
    {{{"50\x10мВ",    "0.5\x10В"}, {"50\x10mV", "0.5\x10V"}}},
    {{{"0.1\x10В",    "1\x10В"},   {"0.1\x10V", "1\x10V"}}},
    {{{"0.2\x10В",    "2\x10В"},   {"0.2\x10V", "2\x10V"}}},
    {{{"0.5\x10В",    "5\x10В"},   {"0.5\x10V", "5\x10V"}}},
    {{{"1\x10В",      "10\x10В"},  {"1\x10V",   "10\x10V"}}},
    {{{"2\x10В",      "20\x10В"},  {"2\x10V",   "20\x10V"}}},
    {{{"5\x10В",      "50\x10В"},  {"5\x10V",   "50\x10V"}}},
    {{{"10\x10В",     "100\x10В"}, {"10\x10V",  "100\x10V"}}},
    {{{"20\x10В",     "200\x10В"}, {"20\x10V",  "200\x10V"}}}
};



int Divider::ToAbs(Divider::E multiplier)
{
    switch (multiplier)
    {
    case Divider::_10:
        return 10;

    case Divider::_1:
        return 1;
    }

    return 1;
}


bool Chan::Enabled()
{
    if (value == Chan::Math && !DISABLED_DRAW_MATH)
    {
        return true;
    }

    return (value <= ChB) ? SET_ENABLED(value) : false;
}


pchar Range::ToString(Range::E range, Divider::E multiplier)
{
    return ranges[range].name[LANG][multiplier];
}


String<> RShift::ToString(Range::E range, Divider::E multiplier)
{
    float rShiftVal = ToAbs(range) * Divider::ToAbs(multiplier);

    return SU::Voltage2String(rShiftVal, true);
};
