// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Text/Text.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <climits>


namespace Processing
{
    struct MeasureValue
    {
        float value[2];
    };

    namespace Marker
    {
        // »ндекс 0 имеет маркер с меньшим значением (тот который находитс€ ниже и левее на экране)
        int volt[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
        int time[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
    }

    int firstP = 0;
    int lastP = 0;
    int numP = 0;

    DataStruct out;         // «десь хран€тс€ данные после обработки, готовые к выводу и расчЄту измерений

    MeasureValue values[Measure::Count] = {{0.0f, 0.0f}};

    bool max_ready[2] = {false, false};
    bool min_ready[2] = {false, false};
    bool maxSteady_ready[2] = {false, false};
    bool minSteady_ready[2] = {false, false};
    bool ave_ready[2] = {false, false};
    bool period_ready[2] = {false, false};
    bool periodAccurate_ready[2];
    bool pic_ready[2] = {false, false};

#define EXIT_IF_ERROR_FLOAT(x)     if((x) == ERROR_VALUE_FLOAT)                             return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERRORS_FLOAT(x, y) if((x) == ERROR_VALUE_FLOAT || (y) == ERROR_VALUE_FLOAT) return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERROR_INT(x)       if((x) == ERROR_VALUE_INT)                               return ERROR_VALUE_FLOAT;

    float CalculateVoltageMax(Chan);
    float CalculateVoltageMin(Chan);
    float CalculateVoltagePic(Chan);
    float CalculateVoltageMaxSteady(Chan);
    float CalculateVoltageMinSteady(Chan);
    float CalculateVoltageAmpl(Chan);
    float CalculateVoltageAverage(Chan);
    float CalculateVoltageRMS(Chan);
    float CalculateVoltageVybrosPlus(Chan);
    float CalculateVoltageVybrosMinus(Chan);
    float CalculatePeriod(Chan);
    // “очно вычисл€ет период или целое число периодов в точках сигнала.
    int   CalculatePeriodAccurately(Chan);
    float CalculateFreq(Chan);
    float CalculateTimeNarastaniya(Chan);
    float CalculateTimeSpada(Chan);
    float CalculateDurationPlus(Chan);
    float CalculateDurationMinus(Chan);
    float CalculateSkvaznostPlus(Chan);
    float CalculateSkvaznostMinus(Chan);
    // ¬озвращает минимальное значение относительного сигнала    
    float CalculateMinRel(Chan);
    // ¬озвращает минимальное установившеес€ значение относительного сигнала
    float CalculateMinSteadyRel(Chan);
    // ¬озвращает максимальное значение относительного сигнала
    float CalculateMaxRel(Chan);
    // ¬озвращает максимальное установившеес€ значение относительного сигнала
    float CalculateMaxSteadyRel(Chan);
    // ¬озвращает среденее значение относительного сигнала
    float CalculateAverageRel(Chan);
    float CalculatePicRel(Chan);
    float CalculateDelayPlus(Chan);
    float CalculateDelayMinus(Chan);
    float CalculatePhazaPlus(Chan);
    float CalculatePhazaMinus(Chan);
    // Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый
    // номер пересечени€, начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при
    // прохождении из "-" в "+".
    float FindIntersectionWithHorLine(Chan, int numIntersection, bool downToUp, uint8 yLine);

    typedef float    (*pFuncFCh)(Chan);
    typedef String<> (*pFuncConvert)(float, bool);

    struct MeasureCalculate
    {
        char *name;
        pFuncFCh     FuncCalculate;
        pFuncConvert FucnConvertate;
        // ≈сли true, нужно показывать знак.
        bool        showSign;
    };

    const MeasureCalculate measures[Measure::Count] =
    {
        {0, 0},
        {"CalculateVoltageMax",         CalculateVoltageMax,           SU::Voltage2String,    true},
        {"CalculateVoltageMin",         CalculateVoltageMin,           SU::Voltage2String,    true},
        {"CalculateVoltagePic",         CalculateVoltagePic,           SU::Voltage2String,    false},
        {"CalculateVoltageMaxSteady",   CalculateVoltageMaxSteady,     SU::Voltage2String,    true},
        {"CalculateVoltageMinSteady",   CalculateVoltageMinSteady,     SU::Voltage2String,    true},
        {"CalculateVoltageAmpl",        CalculateVoltageAmpl,          SU::Voltage2String,    false},
        {"CalculateVoltageAverage",     CalculateVoltageAverage,       SU::Voltage2String,    true},
        {"CalculateVoltageRMS",         CalculateVoltageRMS,           SU::Voltage2String,    false},
        {"CalculateVoltageVybrosPlus",  CalculateVoltageVybrosPlus,    SU::Voltage2String,    false},
        {"CalculateVoltageVybrosMinus", CalculateVoltageVybrosMinus,   SU::Voltage2String,    false},
        {"CalculatePeriod",             CalculatePeriod,               SU::Time2String,       false},
        {"CalculateFreq",               CalculateFreq,                 SU::Freq2String,       false},
        {"CalculateTimeNarastaniya",    CalculateTimeNarastaniya,      SU::Time2String,       false},
        {"CalculateTimeSpada",          CalculateTimeSpada,            SU::Time2String,       false},
        {"CalculateDurationPlus",       CalculateDurationPlus,         SU::Time2String,       false},
        {"CalculateDurationPlus",       CalculateDurationMinus,        SU::Time2String,       false},
        {"CalculateSkvaznostPlus",      CalculateSkvaznostPlus,        SU::FloatFract2String, false},
        {"CalculateSkvaznostMinus",     CalculateSkvaznostMinus,       SU::FloatFract2String, false},
        {"CalculateDelayPlus",          CalculateDelayPlus,            SU::Time2String,       false},
        {"CalculateDelayMinus",         CalculateDelayMinus,           SU::Time2String,       false},
        {"CalculatePhazaPlus",          CalculatePhazaPlus,            SU::Phase2String,      false},
        {"CalculatePhazaMinus",         CalculatePhazaMinus,           SU::Phase2String,      false}
    };
}


void Processing::CalculateMeasures()
{
    if(!SHOW_MEASURES || !Processing::out.ds.valid)
    {
        return;
    }

    max_ready[0] = max_ready[1] = maxSteady_ready[0] = maxSteady_ready[1] = false;
    min_ready[0] = min_ready[1] = minSteady_ready[0] = minSteady_ready[1] = false;
    ave_ready[0] = ave_ready[1] = false;
    period_ready[0] = period_ready[1] = false;
    periodAccurate_ready[0] = periodAccurate_ready[1] = false;
    pic_ready[0] = pic_ready[1] = false;

    for(int str = 0; str < Measures::NumRows(); str++)
    {
        for(int elem = 0; elem < Measures::NumCols(); elem++)
        {
            Measure::E meas = Measures::Type(str, elem);
            pFuncFCh func = measures[meas].FuncCalculate;

            if(func)
            {
                if(meas == MEAS_MARKED || MEAS_MARKED_IS_NONE)
                {
                    Marker::time[Chan::A][0] = Marker::time[Chan::A][1] = Marker::time[Chan::B][0] = Marker::time[Chan::B][1] = ERROR_VALUE_INT;
                    Marker::volt[Chan::A][0] = Marker::volt[Chan::A][1] = Marker::volt[Chan::B][0] = Marker::volt[Chan::B][1] = ERROR_VALUE_INT;
                }
                if(MEAS_SOURCE_IS_A || MEAS_SOURCE_IS_A_B)
                {
                    values[meas].value[Chan::A] = func(Chan::A);
                }
                if(MEAS_SOURCE_IS_B || MEAS_SOURCE_IS_A_B)
                {
                    values[meas].value[Chan::B] = func(Chan::B);
                }
            }
        }
    }
}

float Processing::CalculateVoltageMax(Chan ch)
{
    float max = CalculateMaxRel(ch);
    
    EXIT_IF_ERROR_FLOAT(max);

    if(MEAS_MARKED == Measure::VoltageMax)
    {
        Marker::volt[ch][1] = max;                           // «десь не округл€ем, потому что max может быть только целым
    }

    return ValueFPGA::ToVoltage(max, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageMin(Chan ch)
{
    float min = CalculateMinRel(ch);
    EXIT_IF_ERROR_FLOAT(min);

    if(MEAS_MARKED == Measure::VoltageMin)
    {
        Marker::volt[ch][0] = min;                           // «десь не округл€ем, потому что min может быть только целым
    }

    return ValueFPGA::ToVoltage(min, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltagePic(Chan ch)
{
    float max = CalculateVoltageMax(ch);
    float min = CalculateVoltageMin(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure::VoltagePic)
    {
        Marker::volt[ch][0] = CalculateMaxRel(ch);
        Marker::volt[ch][1] = CalculateMinRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageMinSteady(Chan ch)
{
    float min = CalculateMinSteadyRel(ch);
    EXIT_IF_ERROR_FLOAT(min);

    if(MEAS_MARKED == Measure::VoltageMinSteady)
    {
        Marker::volt[ch][0] = ROUND(min);
    }

    return ValueFPGA::ToVoltage(min, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageMaxSteady(Chan ch)
{
    float max = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERROR_FLOAT(max);

    if(MEAS_MARKED == Measure::VoltageMaxSteady)
    {
        Marker::volt[ch][0] = max;
    }

    Range::E range = out.ds.range[ch];

    return ValueFPGA::ToVoltage(max, range, out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageVybrosPlus(Chan ch)
{
    float max = CalculateMaxRel(ch);
    float maxSteady = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(max, maxSteady);

    if (MEAS_MARKED == Measure::VoltageVybrosPlus)
    {
        Marker::volt[ch][0] = max;
        Marker::volt[ch][1] = maxSteady;
    }

    int16 rshift = out.ds.GetRShift(ch);

    return std::fabsf(ValueFPGA::ToVoltage(maxSteady, out.ds.range[ch], rshift) -
        ValueFPGA::ToVoltage(max, out.ds.range[ch], rshift)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageVybrosMinus(Chan ch)
{
    float min = CalculateMinRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);
    EXIT_IF_ERRORS_FLOAT(min, minSteady);

    if (MEAS_MARKED == Measure::VoltageVybrosMinus)
    {
        Marker::volt[ch][0] = min;
        Marker::volt[ch][1] = minSteady;
    }

    int16 rshift = out.ds.GetRShift(ch);

    return std::fabsf(ValueFPGA::ToVoltage(minSteady, out.ds.range[ch], rshift) -
        ValueFPGA::ToVoltage(min, out.ds.range[ch], rshift)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageAmpl(Chan ch)
{
    float max = CalculateVoltageMaxSteady(ch);
    float min = CalculateVoltageMinSteady(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure::VoltageAmpl)
    {
        Marker::volt[ch][0] = CalculateMaxSteadyRel(ch);
        Marker::volt[ch][1] = CalculateMinSteadyRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageAverage(Chan ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    int sum = 0;
    uint8 *data = &out.Data(ch)[firstP];

    for(int i = 0; i < period; i++)
    {
        sum += *data++;
    }

    uint8 aveRel = (float)sum / period;

    if(MEAS_MARKED == Measure::VoltageAverage)
    {
        Marker::volt[ch][0] = aveRel;
    }

    return ValueFPGA::ToVoltage(aveRel, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageRMS(Chan ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    float rms = 0.0f;
    int16 rshift = out.ds.GetRShift(ch);

    for(int i = firstP; i < firstP + period; i++)
    {
        float volts = ValueFPGA::ToVoltage(out.Data(ch)[i], out.ds.range[ch], rshift);
        rms +=  volts * volts;
    }

    if(MEAS_MARKED == Measure::VoltageRMS)
    {
        Marker::volt[ch][0] = ValueFPGA::FromVoltage(std::sqrt(rms / period), out.ds.range[ch], rshift);
    }

    return std::sqrt(rms / period) * SET_DIVIDER_ABS(ch) * 1.01f;   // \todo поправочка введена в св€зи с тем, что RMS всегда занижает
}


float Processing::CalculatePeriod(Chan ch)
{
    static float period[2] = {0.0f, 0.0f};

    if(!period_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_UINT8)
        {
            period[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            const float intersectionDownToTop = FindIntersectionWithHorLine(ch, 1, true, aveValue);
            const float intersectionTopToDown = FindIntersectionWithHorLine(ch, 1, false, aveValue);

            EXIT_IF_ERRORS_FLOAT(intersectionDownToTop, intersectionTopToDown);

            float firstIntersection = (intersectionDownToTop < intersectionTopToDown) ? intersectionDownToTop : intersectionTopToDown;
            float secondIntersection = FindIntersectionWithHorLine(ch, 2, (intersectionDownToTop < intersectionTopToDown), aveValue);

            EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

            float per = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);

            Marker::time[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
            Marker::time[ch][1] = secondIntersection - SHIFT_IN_MEMORY;

            period[ch] = per;
            period_ready[ch] = true;
        }
    }

    return period[ch];
}

#define EXIT_FROM_PERIOD_ACCURACY       \
    period[ch] = ERROR_VALUE_INT;       \
    periodAccurate_ready[ch] = true;    \
    return period[ch];

int Processing::CalculatePeriodAccurately(Chan ch)
{
    static int period[2];

    int sums[FPGA::MAX_POINTS * 2];

    if(!periodAccurate_ready[ch])
    {
        period[ch] = 0;
        float pic = CalculatePicRel(ch);

        if(pic == ERROR_VALUE_FLOAT)
        {
            EXIT_FROM_PERIOD_ACCURACY
        }
        int delta = pic * 5;
        sums[firstP] = out.Data(ch)[firstP];

        int i = firstP + 1;
        int *sum = &sums[i];
        uint8 *data = &out.Data(ch)[i];
        uint8 *end = &out.Data(ch)[lastP];

        while (data < end)
        {
            uint8 point = *data++;
            if(point < ValueFPGA::MIN || point >= ValueFPGA::MAX)
            {
                EXIT_FROM_PERIOD_ACCURACY
            }
            *sum = *(sum - 1) + point;
            sum++;
        }

        int addShift = firstP - 1;
        int maxPeriod = numP * 0.95f;

        for(int nextPeriod = 10; nextPeriod < maxPeriod; nextPeriod++)
        {
            int s = sums[addShift + nextPeriod];

            int maxDelta = 0;
            int maxStart = numP - nextPeriod;

            int *pSums = &sums[firstP + 1];
            for(int start = 1; start < maxStart; start++)
            {
                int nextSum = *(pSums + nextPeriod) - (*pSums);
                pSums++;

                int nextDelta = nextSum - s;
                if (nextSum < s)
                {
                    nextDelta = -nextDelta;
                }

                if(nextDelta > delta)
                {
                    maxDelta = delta + 1;
                    break;
                }
                else if(nextDelta > maxDelta)
                {
                    maxDelta = nextDelta;
                }
            }

            if(maxDelta < delta)
            {
                delta = maxDelta;
                period[ch] = nextPeriod;
            }
        }

        if(period[ch] == 0)
        {
            period[ch] = ERROR_VALUE_INT;
        }
        periodAccurate_ready[ch] = true;
    }

    return period[ch];
}

float Processing::CalculateFreq(Chan ch)
{
    float period = CalculatePeriod(ch);
    return period == ERROR_VALUE_FLOAT ? ERROR_VALUE_FLOAT : 1.0f / period;
}

float Processing::FindIntersectionWithHorLine(Chan ch, int numIntersection, bool downToUp, uint8 yLine)
{
    int num = 0;
    int x = firstP;
    int compValue = lastP - 1;
    int dx = 1;

    if (out.ds.peak_det)
    {
        x *= 2;
        compValue *= 2;
        dx *= 2;
    }

    uint8 *data = &out.Data(ch)[0];

    if(downToUp)
    {
        while ((num < numIntersection) && (x < compValue))
        {
            if (data[x] < yLine && data[x + dx] >= yLine)
            {
                num++;
            }
            x += dx;
        }
    }
    else
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] > yLine && data[x + dx] <= yLine)
            {
                num++;
            }
            x += dx;
        }
    }

    x -= dx;

    if (num < numIntersection)
    {
        return ERROR_VALUE_FLOAT;
    }

    return Math::GetIntersectionWithHorizontalLine(x, data[x], x + dx, data[x + dx], yLine) / dx;
}


float Processing::CalculateDurationMinus(Chan ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    Marker::time[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
    Marker::time[ch][1] = secondIntersection - SHIFT_IN_MEMORY;

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);
}


float Processing::CalculateDurationPlus(Chan ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    Marker::time[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
    Marker::time[ch][1] = secondIntersection - SHIFT_IN_MEMORY;

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);
}


float Processing::CalculateTimeNarastaniya(Chan ch)                    // WARN «десь, возможно, нужно увеличить точность - брать не целые значени рассто€ний между отсчЄтами по времени, а рассчитывать пересечени€ линий
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, min01);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, max09);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);
    
    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, max09);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);

    if (MEAS_MARKED == Measure::TimeNarastaniya)
    {
        Marker::volt[ch][0] = max09;
        Marker::volt[ch][1] = min01;
        Marker::time[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        Marker::time[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
    }

    return retValue;
}


float Processing::CalculateTimeSpada(Chan ch)                          // WARN јналогично времени нарастани€
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, max09);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, min01);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, min01);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);

    if (MEAS_MARKED == Measure::TimeSpada)
    {
        Marker::volt[ch][0] = max09;
        Marker::volt[ch][1] = min01;
        Marker::time[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        Marker::time[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
    }

    return retValue;
}


float Processing::CalculateSkvaznostPlus(Chan ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationPlus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


float Processing::CalculateSkvaznostMinus(Chan ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationMinus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}


float Processing::CalculateMinSteadyRel(Chan ch)
{
    static float min[2] = {255.0f, 255.0f};

    if(!minSteady_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            min[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;
            uint8 *data = &out.Data(ch)[firstP];
            const uint8 * const end = &out.Data(ch)[lastP];
            while(data <= end)
            {
                uint8 d = *data++;
                if(d < aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }
            min[ch] = (float)sum / numSums;
            int numMin = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);
            if (pic == ERROR_VALUE_FLOAT)
            {
                min[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &out.Data(ch)[firstP];
                float _min = min[ch];
                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d < aveValue)
                    {
                        if (d < _min)
                        {
                            if (_min - d > value)
                            {
                                sum -= d;
                                --numSums;
                                ++numDeleted;
                            }
                        }
                        else if (d - _min > value)
                        {
                            sum -= d;
                            --numSums;
                            ++numDeleted;
                        }
                    }
                }
                min[ch] = (numDeleted > numMin / 2.0f) ? CalculateMinRel(ch) : (float)sum / numSums;
            }
        }
        minSteady_ready[ch] = true;
    }

    return min[ch];
}


float Processing::CalculateMaxSteadyRel(Chan ch)
{
    static float max[2] = {255.0f, 255.0f};

    if(!maxSteady_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            max[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;
            uint8 *data = &out.Data(ch)[firstP];
            const uint8 * const end = &out.Data(ch)[lastP];

            while (data <= end)
            {
                uint8 d = *data++;
                if(d > aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }

            max[ch] = (float)sum / numSums;
            int numMax = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);

            if (pic == ERROR_VALUE_FLOAT)
            {
                max[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &out.Data(ch)[firstP];
                uint8 _max = max[ch];

                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d > aveValue)
                    {
                        if (d > _max)
                        {
                            if (d - _max > value)
                            {
                                sum -= d;
                                numSums--;
                                numDeleted++;
                            }
                        }
                        else if (_max - d > value)
                        {
                            sum -= d;
                            numSums--;
                            numDeleted++;
                        }
                    }
                }

                max[ch] = (numDeleted > numMax / 2) ? CalculateMaxRel(ch) : (float)sum / numSums;
            }
        }
        maxSteady_ready[ch] = true;
    }

    return max[ch];
}


float Processing::CalculateMaxRel(Chan ch)
{
    static float max[2] = {0.0f, 0.0f};

    if(!max_ready[ch])
    {
        uint8 val = Math::GetMaxFromArrayWithErrorCode(out.Data(ch).Data(), firstP, lastP);
        max[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        max_ready[ch] = true;
    }

    return max[ch];
}


float Processing::CalculateMinRel(Chan ch)
{
    static float min[2] = {255.0f, 255.0f};

    if (!min_ready[ch])
    {
        uint8 val = Math::GetMinFromArrayWithErrorCode(out.Data(ch).Data(), firstP, lastP);
        min[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        min_ready[ch] = true;
    }

    return min[ch];
}


float Processing::CalculateAverageRel(Chan ch)
{
    static float ave[2] = {0.0f, 0.0f};

    if(!ave_ready[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        ave[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : (min + max) / 2.0f;
        ave_ready[ch] = true;
    }
    return ave[ch];
}


float Processing::CalculatePicRel(Chan ch)
{
    static float pic[2] = {0.0f, 0.0f};

    if(!pic_ready[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        pic[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : max - min;
        pic_ready[ch] = true;
    }
    return pic[ch];
}


float Processing::CalculateDelayPlus(Chan ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);
    if(!Math::FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(Chan::A);
    float average1 = CalculateAverageRel(Chan::B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == Chan::A ? average0 : average1;
    float averageSecond = ch == Chan::A ? average1 : average0;
    Chan::E firstChannel = ch == Chan::A ? Chan::A : Chan::B;
    Chan::E secondChannel = ch == Chan::A ? Chan::B : Chan::A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, true, averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, true, averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, true, averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);
}


float Processing::CalculateDelayMinus(Chan ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);

    if(!Math::FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(Chan::A);
    float average1 = CalculateAverageRel(Chan::B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == Chan::A ? average0 : average1;
    float averageSecond = ch == Chan::A ? average1 : average0;
    Chan::E firstChannel = ch == Chan::A ? Chan::A : Chan::B;
    Chan::E secondChannel = ch == Chan::A ? Chan::B : Chan::A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, false, averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, false, averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, false, averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tbase);
}


float Processing::CalculatePhazaPlus(Chan ch)
{
    float delay = CalculateDelayPlus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f;
}


float Processing::CalculatePhazaMinus(Chan ch)
{
    float delay = CalculateDelayMinus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f; 
}


float Processing::Cursor::GetU(Chan ch, float posCurT)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    float result = 0.0f;

    LIMITATION(result, 200 - (out.Data(ch))[points.half_iword[0] + (int)posCurT] + ValueFPGA::MIN, 0, 200);

    return result;
}


float Processing::Cursor::GetT(Chan ch, float posCurU, int numCur)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    int prevData = 200 - (out.Data(ch))[points.half_iword[0]] + ValueFPGA::MIN;

    int numIntersections = 0;

    for(int i = points.half_iword[0] + 1; i < points.half_iword[1]; i++)
    {
        int curData = 200 - (out.Data(ch))[i] + ValueFPGA::MIN;

        if(curData <= posCurU && prevData > posCurU)
        {
            if(numCur == 0)
            {
                return i - points.half_iword[0];
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return i - points.half_iword[0];
                }
            }
        }

        if(curData >= posCurU && prevData < posCurU)
        {
            if(numCur == 0)
            {
                return i - points.half_iword[0];
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return i - points.half_iword[1];
                }
            }
        }
        prevData = curData;
    }
    return 0;
}

void Processing::InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tbase)
{
/*
     ѕоследовательности x в sin(x)
2    1. 50нс : pi/2, -pi/2 ...
8    2. 20нс : pi/5, pi/5 * 2, pi/5 * 3, pi/5 * 4, -pi/5 * 4, -pi/5 * 3, -pi/5 * 2, -pi/5 ...
18   3. 10нс : pi/10, pi/10 * 2 ... pi/10 * 9, -pi/10 * 9 .... -pi/10 * 2, -pi/10 ...
38   4. 5нс  : pi/20, pi/20 * 2 ... pi/20 * 19, -pi/20 * 19 ... -pi/20 * 2, -pi/20 ...
98   5. 2нс  : pi/50, pi/50 * 2 ... pi/50 * 49, -pi/50 * 49 ... -pi/50 * 2, -pi/50 ...
*/

#define MUL_SIN 1e7f
#define MUL     1e6f
#define KOEFF   (MUL / MUL_SIN)

    int deltas[5] = {50, 20, 10, 5, 2};
    int delta = deltas[tbase];

    uint8 signedData[FPGA::MAX_POINTS / 2];
    int numSignedPoints = 0;
    
    for (int pos = 0; pos < FPGA::MAX_POINTS; pos++)
    {
        if (data[pos] > 0)
        {
            signedData[numSignedPoints] = data[pos];
            numSignedPoints++;
        }
    }

    // ЌайдЄм смещение первой значащей точки

    int shift = 0;
    for (int pos = 0; pos < FPGA::MAX_POINTS; pos++)
    {
        if (data[pos] > 0)
        {
            shift = pos;
            break;
        }
    }

    float deltaX = PI;
    float stepX0 = PI / (float)delta;
    float x0 = PI - stepX0;
    int num = 0;
    
    for(int i = 0; i < FPGA::MAX_POINTS; i++)
    {
        x0 += stepX0;
        if((i % delta) == 0)
        {
            data[i] = signedData[i / delta];
        }
        else
        {
            int part = num % ((delta - 1) * 2);
            num++;
            float sinX = (part < delta - 1) ? std::sin(PI / delta * (part + 1)) : std::sin(PI / delta * (part - (delta - 1) * 2));

            if (tbase > TBase::_5ns)                 // «десь используем более быструю, но более неправильную арифметику целвых чисел
            {
                int sinXint = (int)(sinX * MUL_SIN);
                int value = 0;
                int x = (int)((x0 - deltaX) * MUL);
                int deltaXint = (int)(deltaX * MUL);

                for (int n = 0; n < numSignedPoints; n++)
                {
                    value += signedData[n] * sinXint / (x - n * deltaXint);
                    sinXint = -sinXint;
                }
                data[i] = (uint8)(value * KOEFF);
            }
            else                                    // Ќа этих развЄртках арифметика с плавающей зап€той даЄт приемлемое быстродействие
            {
                float value = 0.0f;
                float x = x0;

                for (int n = 0; n < numSignedPoints; n++)
                {
                    x -= deltaX;
                    value += signedData[n] * sinX / x;
                    sinX = -sinX;
                }
                data[i] = (uint8)value;
            }
        }
    }
    
    int pos = FPGA::MAX_POINTS - 1;
    while (pos > shift)
    {
        data[pos] = data[pos - shift];
        pos--;
    }
}

String<> Processing::GetStringMeasure(Measure::E measure, Chan ch)
{
    if (!SET_ENABLED(ch))
    {
        return String<>("");
    }

    String<> result(ch.IsA() ? "1: " : "2: ");

    if(!out.ds.valid)
    {
        result.Append("-.-");
    }
    else if(!SET_ENABLED(ch))
    {
    }
    else if(measures[measure].FuncCalculate)
    {
        pFuncConvert func = measures[measure].FucnConvertate;
        float value = values[measure].value[ch];
        String<> text = func(value, measures[measure].showSign);
        result.Append(text);
    }
    else
    {
        return result;
    }

    return result;
}

int Processing::Marker::GetU(Chan ch, int numMarker)
{
    return Marker::volt[ch][numMarker] - ValueFPGA::MIN;
}

int Processing::Marker::GetT(Chan ch, int numMarker)
{
    return Marker::time[ch][numMarker];
}


void Processing::CountedToCurrentSettings(const DataSettings &ds, const uint8 *dA, const uint8 *dB, DataStruct &out_struct)
{
    int num_bytes = ds.BytesInChanStored();

    out_struct.ds = ds;

    out_struct.A.ReallocAndFill(num_bytes, ValueFPGA::NONE);
    out_struct.B.ReallocAndFill(num_bytes, ValueFPGA::NONE);

    int dataTShift = out_struct.ds.tshift;
    int curTShift = SET_TSHIFT;

    int16 dTShift = curTShift - dataTShift;

    const uint8 *in_a = dA;
    const uint8 *in_b = dB;

    uint8 *out_a = out_struct.A.Data();
    uint8 *out_b = out_struct.B.Data();

    for (int i = 0; i < num_bytes; i++)
    {
        int index = i - dTShift;

        if (index >= 0 && index < num_bytes)
        {
            out_a[index] = in_a[i];
            out_b[index] = in_b[i];
        }
    }

    if (SET_INVERSE_A)
    {
        for (int i = 0; i < num_bytes; i++)
        {
            uint8 value = out_a[i];

            out_a[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(value, ValueFPGA::MIN, ValueFPGA::MAX));
        }
    }

    if (SET_INVERSE_B)
    {
        for (int i = 0; i < num_bytes; i++)
        {
            uint8 value = out_b[i];

            out_b[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(value, ValueFPGA::MIN, ValueFPGA::MAX));
        }
    }
 
    if((out_struct.ds.rshiftA != (uint)SET_RSHIFT_A) || (out_struct.ds.range[0] != SET_RANGE_A))
    {
        if (out_struct.ds.range[0] != SET_RANGE_A)
        {
            Range::E range = SET_RANGE_A;
            RShift rshift = SET_RSHIFT_A;

            for (int i = 0; i < num_bytes; i++)
            {
                if (out_struct.A[i] != ValueFPGA::NONE)
                {
                    float abs = ValueFPGA::ToVoltage(out_a[i], out_struct.ds.range[0], (int16)out_struct.ds.rshiftA);
                    int rel = (abs + Range::MaxOnScreen(range) + rshift.ToAbs(range)) / Range::voltsInPoint[range] + ValueFPGA::MIN;

                    if (rel < ValueFPGA::MIN) { out_a[i] = ValueFPGA::MIN; }
                    else if (rel > ValueFPGA::MAX) { out_a[i] = ValueFPGA::MAX; }
                    else { out_a[i] = (uint8)rel; }
                }
            }
        }
        else
        {
            float delta_shift = (out_struct.ds.rshiftA - SET_RSHIFT_A) / 1.6f;

            for (int i = 0; i < num_bytes; i++)
            {
                if (out_struct.A[i] != ValueFPGA::NONE)
                {
                    int value = (int)out_a[i] - delta_shift;

                    if (value < ValueFPGA::MIN)      { out_a[i] = ValueFPGA::MIN; }
                    else if (value > ValueFPGA::MAX) { out_a[i] = ValueFPGA::MAX; }
                    else                             { out_a[i] = (uint8)value;   }
                }
            }
        }
    }

    if ((out_struct.ds.rshiftB != (uint)SET_RSHIFT_B) || (out_struct.ds.range[1] != SET_RANGE_B))
    {
        if (out_struct.ds.range[1] != SET_RANGE_B)
        {
            Range::E range = SET_RANGE_B;
            RShift rshift = SET_RSHIFT_B;

            for (int i = 0; i < num_bytes; i++)
            {
                if (out_struct.B[i] != ValueFPGA::NONE)
                {
                    float abs = ValueFPGA::ToVoltage(out_b[i], out_struct.ds.range[1], (int16)out_struct.ds.rshiftB);
                    int rel = (abs + Range::MaxOnScreen(range) + rshift.ToAbs(range)) / Range::voltsInPoint[range] + ValueFPGA::MIN;

                    if (rel < ValueFPGA::MIN) { out_b[i] = ValueFPGA::MIN; }
                    else if (rel > ValueFPGA::MAX) { out_b[i] = ValueFPGA::MAX; }
                    else { out_b[i] = (uint8)rel; }
                }
            }
        }
        else
        {
            float delta_shift = (out_struct.ds.rshiftB - SET_RSHIFT_B) / 1.6f;

            for (int i = 0; i < num_bytes; i++)
            {
                if (out_struct.B[i] != ValueFPGA::NONE)
                {
                    int value = (int)out_b[i] - delta_shift;

                    if (value < ValueFPGA::MIN)      { out_b[i] = ValueFPGA::MIN; }
                    else if (value > ValueFPGA::MAX) { out_b[i] = ValueFPGA::MAX; }
                    else                             { out_b[i] = (uint8)value;   }
                }
            }
        }
    }
}


void Processing::SetData(const DataStruct &in, bool mode_p2p)
{
    out.ds.valid = 0;

    if (!in.ds.valid)
    {
        return;
    }

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    firstP = points.half_iword[0];
    lastP = points.half_iword[1];
    numP = lastP - firstP;

    int length = in.ds.BytesInChanStored();

    BufferFPGA A(length);
    BufferFPGA B(length);

    Math::CalculateFiltrArray(in.A.DataConst(), A.Data(), length, Smoothing::ToPoints());
    Math::CalculateFiltrArray(in.B.DataConst(), B.Data(), length, Smoothing::ToPoints());

    CountedToCurrentSettings(in.ds, A.Data(), B.Data(), out);

    out.ds.valid = 1;
    out.rec_points = in.rec_points;
    out.all_points = in.all_points;
    out.mode_p2p = mode_p2p;
}


void Processing::SetDataForProcessing(ModeWork::E mode, bool for_window_memory)
{
    DataSettings last_ds = Storage::GetDataSettings(0);

    out.ds.valid = 0;

    if (mode == ModeWork::Direct)
    {
        if (TBase::InModeP2P())
        {
            if (START_MODE_IS_AUTO)
            {
                if (last_ds.valid && last_ds.Equal(Storage::current.data.ds) && Storage::time_meter.ElapsedTime() < 1000)
                {
                    SetData(Storage::GetLatest());
                }
                else
                {
                    SetData(Storage::current.data, true);
                }
            }
            else if (START_MODE_IS_WAIT)
            {
                if (last_ds.valid && last_ds.Equal(Storage::current.data.ds) && !for_window_memory)
                {
                    SetData(Storage::GetLatest());
                }
                else
                {
                    SetData(Storage::current.data, true);
                }
            }
            else
            {
                if (Storage::current.data.ds.valid || for_window_memory)
                {
                    SetData(Storage::current.data, for_window_memory);
                }
                else
                {
                    SetData(Storage::GetLatest());
                }
            }
        }
        else
        {
            if (Storage::NumFrames())
            {
                SetData(Storage::GetLatest());
            }
        }
    }
    else if (mode == ModeWork::Latest)
    {
        SetData(Storage::GetData(PageMemory::Latest::current));
    }
    else if(mode == ModeWork::Internal)
    {
        if (for_window_memory)
        {
            DataStruct data;

            if (HAL_ROM::Data::Get(PageMemory::Internal::currentSignal, data))
            {
                SetData(data);
            }
        }
    }
}
