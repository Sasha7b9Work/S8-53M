// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "SettingsTypes.h"
#include "defines.h"
#include "FPGA/SettingsFPGA.h"


#define SET_TBASE                            (gset.time.tbase)

#define SET_TSHIFT                           (gset.time.tshift_points)

#define SET_TIME_DIV_XPOS                    (gset.time.timeDivXPos)
#define SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY (SET_TIME_DIV_XPOS == FunctionTime::ShiftInMemory)

#define SET_TPOS                             (gset.time.tPos)
#define SET_TPOS_IS_LEFT                     (SET_TPOS == TPos::Left)
#define SET_TPOS_IS_CENTER                   (SET_TPOS == TPos::Center)
#define SET_TPOS_IS_RIGHT                    (SET_TPOS == TPos::Right)

#define SET_SAMPLE_TYPE                      (gset.time.sampleType)
#define SET_SAMPLE_TYPE_IS_EQUAL             (SET_SAMPLE_TYPE == SampleType::Equal)
#define SET_SAMPLE_TYPE_IS_REAL              (SET_SAMPLE_TYPE == SampleType::Real)

#define SET_PEAKDET                          (gset.time.peakDet)
#define SET_PEAKDET_IS_DISABLED              (SET_PEAKDET == PeackDetMode::Disable)
#define SET_PEAKDET_IS_ENABLED               (SET_PEAKDET == PeackDetMode::Enable)

#define SET_SELFRECORDER                     (gset.time.selfRecorder)




// Функция ручки ВРЕМЯ/ДЕЛ.
struct FunctionTime
{
    enum E
    {
        Time,              // Ручка управляет смещением по времени.
        ShiftInMemory      // Ручка управляет отображаемым на экране участком памяти.
    };
};


// Тип выборки для режима рандомизатора.
struct SampleType
{
    enum E
    {
        Real,   // реальное время - в построении участвуют только реально считанные точки, ничего не рассчитывается.
        Equal   // эквивалентная - сигнал строится по последним точкам, полученным от рандомизатора.
    };
};


// Настройки оси X.
struct SettingsTime
{ //-V802
    TBase::E            tbase;          // Масштаб по времени.
    int                 tshift_points;  // Смещение по времени в точках
    FunctionTime::E     timeDivXPos;
    TPos::E             tPos;           // Привязка синхронизации к памяти.
    SampleType::E       sampleType;     // Тип выборки для режима рандомизатора.
    PeackDetMode::E     peakDet;        // Режим работы пикового детектора
    bool                selfRecorder;   // Включен ли режим самописца.
};
