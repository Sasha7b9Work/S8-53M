// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Hardware/Timer.h"


struct FrameImitation
{
    DataStruct    data;

    // Подготовить к новому циклу чтения.
    // В режиме рандомизатора - переписать последние данные из Storage.
    // В режиме поточечного вывода - сбросить точки
    void PrepareForNewCycle();

    void AppendPoints(BitSet16, BitSet16);

    // Создать для хранения данных при текущих настройках
    void CreateForCurrent();

private:
    void AppendByte(uint8, uint8);
};


namespace Storage
{
    extern FrameImitation current;      // Сюда считываем непосредственно из FPGA. Рандомизатор, точки там. Чтобы потом переложить в хранилище
    extern TimeMeterMS time_meter;      // Для засекания времени последнего сохранения данных

    // Удаление всех сохранённых измерений
    void Clear();

    // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void Append(DataStruct &);

    // Сколько всего измерений сохранено в памяти.
    int NumFrames();

    // Получить указатель на данные
    const DataStruct &GetData(int fromEnd);

    // Данные, которые нужно выводить в обычном режиме. Они всегда хранятся в конце Storage (GetData(0))
    // Если включено усреднение (или режим рандомизатора, в котором всегда усреднение), то берутся данные из Averager
    const DataStruct &GetLatest();

    // Возвращает указатель на данные, отстоящие на fromEnd oт последнего сохранённого
    DataSettings GetDataSettings(int fromEnd);

    int NumberAvailableEntries();

    namespace SameSettings
    {
        // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
        int GetCount();
    }
};
