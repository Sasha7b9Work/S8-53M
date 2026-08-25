// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"


// Виды измерений.
struct Measure
{
    enum E
    {
        None,
        VoltageMax,
        VoltageMin,
        VoltagePic,
        VoltageMaxSteady,
        VoltageMinSteady,
        VoltageAmpl,
        VoltageAverage,
        VoltageRMS,
        VoltageVybrosPlus,
        VoltageVybrosMinus,
        Period,
        Freq,
        TimeNarastaniya,
        TimeSpada,
        DurationPlus,
        DurationMinus,
        SkvaznostPlus,
        SkvaznostMinus,
        DelayPlus,
        DelayMinus,
        PhazaPlus,
        PhazaMinus,
        Count
    };
};


Measure::E& operator++(Measure::E &measure);


namespace Measures
{
    // Нарисовать страницу выбора измерений.
    void DrawPageChoice();

    bool IsActive(int row, int col);

    int GetDY();

    int GetDX();

    pchar Name(int row, int col);

    Measure::E Type(int row, int col);

    int NumRows();

    int NumCols();

    int GetTopTable();

    int GetDeltaGridLeft();

    int GetDeltaGridBottom();

    void RotateRegSet(int angle);

    // Вызывается при коротком нажатии на кнопку выбора измерения.
    void ShorPressOnSmallButtonSettings();

    void ShortPressOnSmallButonMarker();
}
