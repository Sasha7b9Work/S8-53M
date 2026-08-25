// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Data/Measures.h"


#define MEAS_NUM                        (gset.measures.number)
#define MEAS_NUM_IS_1                   (MEAS_NUM == MeasuresNumber::_1)
#define MEAS_NUM_IS_1_5                 (MEAS_NUM == MeasuresNumber::_1_5)
#define MEAS_NUM_IS_2_5                 (MEAS_NUM == MeasuresNumber::_2_5)
#define MEAS_NUM_IS_3_5                 (MEAS_NUM == MeasuresNumber::_3_5)
#define MEAS_NUM_IS_6_1                 (MEAS_NUM == MeasuresNumber::_6_1)
#define MEAS_NUM_IS_6_2                 (MEAS_NUM == MeasuresNumber::_6_2)

#define MEAS_SOURCE                     (gset.measures.source)
#define MEAS_SOURCE_IS_A                (MEAS_SOURCE == Chan::A)
#define MEAS_SOURCE_IS_B                (MEAS_SOURCE == Chan::B)
#define MEAS_SOURCE_IS_A_B              (MEAS_SOURCE == Chan::A_B)

#define MODE_VIEW_SIGNALS               (gset.measures.modeViewSignals)
#define MODE_VIEW_SIGNALS_IS_COMPRESS   (MODE_VIEW_SIGNALS == ModeViewSignals::Compress)

#define MEAS_MARKED                     (gset.measures.markedMeasure)
#define MEAS_MARKED_IS_NONE             (MEAS_MARKED == Measure::None)

#define MEASURE(num)                    (gset.measures.measures[num])
#define MEASURE_IS_MARKED(num)          (MEASURE(num) == MEAS_MARKED)

#define SHOW_MEASURES                   (gset.measures.show)

#define MEAS_FIELD                      (gset.measures.field)
#define MEAS_FIELD_IS_HAND              (MEAS_FIELD == MeasuresField::Hand)

#define MEAS_POS_CUR_U(num)             (gset.measures.posCurU[num])
#define MEAS_POS_CUR_U0                 (MEAS_POS_CUR_U(0))
#define MEAS_POS_CUR_U1                 (MEAS_POS_CUR_U(1))

#define MEAS_POS_CUR_T(num)             (gset.measures.posCurT[num])
#define MEAS_POS_CUR_T0                 (MEAS_POS_CUR_T(0))
#define MEAS_POS_CUR_T1                 (MEAS_POS_CUR_T(1))

#define MEAS_CURS_CNTRL_U               (gset.measures.cntrlU)

#define MEAS_CURS_CNTRL_T               (gset.measures.cntrlT)

#define MEAS_CURS_ACTIVE                (gset.measures.cursActive)
#define MEAS_CURS_ACTIVE_IS_T           (MEAS_CURS_ACTIVE == CursActive::T)




// Количество и расположение на экране результатов измерений.
struct MeasuresNumber
{
    enum E
    {
        _1,                       // 1 измерение слева внизу.
        _2,                       // 2 измерения слева внизу.
        _1_5,                     // 1 строка с 5 измерениями.
        _2_5,                     // 2 строки по 5 измерений.
        _3_5,                     // 3 строки по 5 измерений.
        _6_1,                     // 6 строк по 1 измерению.
        _6_2                      // 6 строк по 2 измерения.
    };
};


// Зона, по которой считаются измрения
struct MeasuresField
{
    enum E
    {
        Screen,       // Измерения будут производиться по той части сингала, которая видна на экране.
        AllMemory,    // Измерения будут производиться по всему сигналу.
        Hand          // Измерения будут производиться по окну, задаваемому пользователем.
    };
};


// Настройки меню ИЗМЕРЕНИЯ
struct SettingsMeasures
{
    MeasuresNumber::E  number;          // Сколько измерений выводить.
    Chan::E            source;          // Для каких каналов выводить измерения.
    ModeViewSignals::E modeViewSignals; // Сжимать ли сигналы при выводе измерений.
    Measure::E         measures[15];    // Выбранные для индикации измерения.
    bool               show;            // Показывать ли измерения.
    MeasuresField::E   field;           // Задаёт область, из которой берутся значения для расчёта измерений.
    int16              posCurU[2];      // Позиции курсоров, которые задают область, из которой берутся значения для расчёта измерений при field == MeasuresField_Hand.
    int16              posCurT[2];      // Позиции курсоров, которые задают область, из которой берутся значения для расчёта измерений при field == MeasuresField_Hand.
    CursCntrl::E       cntrlU;          // Активные курсоры напряжения.
    CursCntrl::E       cntrlT;          // Активные курсоры времени.
    CursActive::E      cursActive;      // Какие курсоры активны - по времени или напряжению.
    Measure::E         markedMeasure;   // Измерение, на которое нужно выводить маркеры.
};
