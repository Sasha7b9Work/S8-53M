// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"


namespace Processing
{
    extern DataStruct out;      // Здесь хранятся данные, готовые для вывода - преобразованные из in

    // Привести к текущим настройкам данные, из inA, inB. Данные сохраняются в out
    void CountedToCurrentSettings(const DataSettings &, const uint8 *inA, const uint8 *inB, DataStruct &out);

    // Установить данные для обработки в соотетствии с текущими настройками и состоянием
    // Если for_windows_memory - установить данные для окна памяти
    void SetDataForProcessing(ModeWork::E, DataStruct &, bool for_window_memory);

    // Обработать сигнал из in. Результат сохранён в Data::out
    // Если mode_p2p == true, то будет установлен признак поточечного фрейма
    void SetData(const DataStruct &in, bool mode_p2p = false);

    // Расчитать все измерения.
    void CalculateMeasures();

    // Возвращает строку автоматического измерения.
    String<> GetStringMeasure(Measure::E, Chan);

    namespace Cursor
    {
        // Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT.
        float GetU(Chan, float posCurT);

        // Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU.
        float GetT(Chan, float posCurU, int numCur);
    }

    namespace Marker
    {
        // Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
        int GetU(Chan, int numMarker);

        // Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
        int GetT(Chan, int numMarker);
    }
};
