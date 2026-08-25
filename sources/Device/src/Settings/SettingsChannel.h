// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define SET_INVERSE(ch)         (gset.chan[ch].inverse)
#define SET_INVERSE_A           (SET_INVERSE(Chan::A))
#define SET_INVERSE_B           (SET_INVERSE(Chan::B))
#define SET_RSHIFT_MATH         (gset.math.rshift)
#define SET_RANGE_MATH          (gset.math.range)

#define SET_RANGE(ch)           (gset.chan[ch].range)
#define SET_RANGE_A             (SET_RANGE(Chan::A))
#define SET_RANGE_B             (SET_RANGE(Chan::B))

#define SET_RSHIFT(ch)          (gset.chan[ch].rshift)
#define SET_RSHIFT_A            (SET_RSHIFT(Chan::A))
#define SET_RSHIFT_B            (SET_RSHIFT(Chan::B))

#define SET_COUPLE(ch)          (gset.chan[ch].modeCouple)
#define SET_COUPLE_A            (SET_COUPLE(Chan::A))
#define SET_COUPLE_B            (SET_COUPLE(Chan::B))

#define SET_DIVIDER(ch)         (gset.chan[ch].divider)
#define SET_DIVIDER_A           SET_DIVIDER(Chan::A)
#define SET_DIVIDER_B           SET_DIVIDER(Chan::B)
#define SET_DIVIDER_ABS(ch)     (Divider::ToAbs(SET_DIVIDER(ch)))

#define SET_ENABLED(ch)         (gset.chan[ch].enable)
#define SET_ENABLED_A           SET_ENABLED(Chan::A)
#define SET_ENABLED_B           SET_ENABLED(Chan::B)

#define SET_FILTR(ch)           (gset.chan[ch].filtr)
#define SET_FILTR_A             (SET_FILTR(Chan::A))
#define SET_FILTR_B             (SET_FILTR(Chan::B))

#define CAL_STRETCH(ch)         (gset.chan[ch].cal_stretch)
#define CAL_STRETCH_A           (CAL_STRETCH(ChA))
#define CAL_STRETCH_B           (CAL_STRETCH(ChB))

#define CAL_RSHIFT(ch)          (gset.chan[ch].cal_rshift[SET_RANGE(ch)][SET_COUPLE(ch)])
#define CAL_RSHIFT_A            (CAL_RSHIFT(ChA))
#define CAL_RSHIFT_B            (CAL_RSHIFT(ChB))


// Настройки каналов
struct SettingsChannel
{ //-V802
    RShift          rshift;
    ModeCouple::E   modeCouple;     // Режим по входу.
    Divider::E      divider;        // Множитель.
    Range::E        range;          // Масштаб по напряжению.
    bool            enable;         // Включён ли канал.
    bool            inverse;        // Инвертирован ли канал.
    bool            filtr;          // Фильтр

    float           cal_stretch;    // Коэффицент растяжки (на него умножается сигнал)
    int8            cal_rshift[Range::Count][ModeCouple::Count];   // Добавочное смещение, вычисляемое при калибровке
};
