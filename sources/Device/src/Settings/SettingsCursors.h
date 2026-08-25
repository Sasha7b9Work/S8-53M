// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Settings/SettingsTypes.h"


#define CURSORS_SHOW_FREQ           (gset.cursors.showFreq)
#define CURS_MOVEMENT               (gset.cursors.movement)
#define CURS_MOVEMENT_IS_PERCENTS   (CURS_MOVEMENT == CursMovement::Percents)

#define CURS_SOURCE                 (gset.cursors.source)
#define CURS_SOURCE_A               (CURS_SOURCE == Chan::A)

#define CURsU_CNTRL_CH(ch)          (gset.cursors.cntrlU[ch])
#define CURsU_CNTRL                 (CURsU_CNTRL_CH(CURS_SOURCE))
#define CURS_CNTRL_U_IS_DISABLE(ch) (CURsU_CNTRL_CH(ch) == CursCntrl::Disable)

#define CURS_CNTRL_T(ch)            (gset.cursors.cntrlT[ch])
#define CURS_CNTRL_T_IS_DISABLE(ch) (CURS_CNTRL_T(ch) == CursCntrl::Disable)

#define CURS_POS_U(ch, num)         (gset.cursors.posCurU[ch][num])
#define CURS_POS_U0(ch)             (CURS_POS_U(ch, 0))
#define CURS_POS_U1(ch)             (CURS_POS_U(ch, 1))

#define CURS_POS_T(ch, num)         (gset.cursors.posCurT[ch][num])
#define CURS_POS_T0(ch)             (CURS_POS_T(ch, 0))
#define CURS_POS_T1(ch)             (CURS_POS_T(ch, 1))

#define DELTA_U100(ch)              (gset.cursors.deltaU100percents[ch])

#define DELTA_T100(ch)              (gset.cursors.deltaT100percents[ch])

#define CURS_ACTIVE                 (gset.cursors.active)
#define CURS_ACTIVE_IS_T            (CURS_ACTIVE == CursActive::T)
#define CURS_ACTIVE_IS_U            (CURS_ACTIVE == CursActive::U)

#define CURS_LOOKMODE(num)          (gset.cursors.lookMode[num])
#define CURS_LOOKMODE_0             (CURS_LOOKMODE(0))
#define CURS_LOOKMODE_1             (CURS_LOOKMODE(1))

#define CURS_SHOW                   (gset.cursors.showCursors)


// Ќастройки курсорных измерений.
struct SettingsCursors
{
    CursCntrl::E    cntrlU[Chan::Count];        // јктивные курсоры напр€жени€.
    CursCntrl::E    cntrlT[Chan::Count];        // јктивные курсоры напр€жени€.
    Chan::E         source;                     // »сточник - к какому каналу относ€тс€ курсоры.
    float           posCurU[Chan::Count][2];    // “екущие позиции курсоров напр€жени€ обоих каналов.
    float           posCurT[Chan::Count][2];    // “екущие позиции курсоров времени обоих каналов.
    float           deltaU100percents[2];       // –ассто€ние между курсорами напр€жени€ дл€ 100%, дл€ обоих каналов.
    float           deltaT100percents[2];       // –ассто€ние между курсорами времени дл€ 100%, дл€ обоих каналов.
    CursMovement::E movement;                   //  ак перемещатьс€ курсорам - по точкам или по процентам.
    CursActive::E   active;                     //  акие курсоры сейчас активны.
    CursLookMode::E lookMode[2];                // –ежимы слежени€ за курсорами дл€ двух пар курсоров.
    bool            showFreq;                   // ”становленное в true значение, что нужно показывать на экране значение 1/dT между курсорами.
    bool            showCursors;                // ѕоказывать ли курсоры.
};
