// 2022/02/11 17:49:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Data/Storage.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Data/DataExtensions.h"
#include <cstring>


namespace Storage
{
    FrameImitation current;

    TimeMeterMS time_meter;

    static const int SIZE_POOL = HAL_FMC::ADDR_RAM_DATA_END - HAL_FMC::ADDR_RAM_DATA;

    // Адрес начала памяти для хранения
    uint8 *beginPool = HAL_FMC::ADDR_RAM_DATA;

    // Адрес последнего байта памяти для хранения
    uint8 *endPool = HAL_FMC::ADDR_RAM_DATA_END;

    // Указатель на первые сохранённые данные
    DataSettings *first_ds = nullptr;

    // Указатель на последние сохранённые данные
    DataSettings *last_ds = nullptr;

    // Всего данных сохранено
    int count_data = 0;

    // Возвращает количество свободной памяти в байтах
    int MemoryFree();

    // Удалить первое (самое старое) измерение
    void RemoveFirstFrame();

    // Удалить последнее (самое новое) измерение
    void RemoveLastFrame();

    // Подготовить новый фрейм для записи в него данных. Записывает в него данные из ds
    DataSettings *PrepareNewFrame(DataSettings &ds);

    // Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);

    // Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel
    void CopyData(DataSettings *, Chan ch, BufferFPGA &);

    DataSettings *GetDataSettingsPointer(int indexFromEnd);

    namespace SameSettings
    {
        static void Calculate(const DataStruct &);

        // Количество элементов с одинаковыми (относительно последнего элемента) настройками
        static int count = 0;
    }
}


void DataSettings::PrintElement()
{
    LOG_WRITE("addr:%x, addrNext:%x, addrPrev:%x, size:%d", this, next, prev, SizeFrame());
}


void Storage::Clear()
{
    first_ds = nullptr;
    last_ds = (DataSettings *)beginPool;
    last_ds->next = last_ds->prev = nullptr;
    count_data = 0;

    Limitator::ClearLimits();

    current.data.ds.valid = 0;

    SameSettings::count = 0;
}


void Storage::Append(DataStruct &data)
{
    static int id = 0;

    SameSettings::Calculate(data);

    data.ds.time = HAL_RTC::GetPackedTime();
    data.ds.id = ++id;

    Limitator::Append(data);

    DataSettings *ds = PrepareNewFrame(data.ds);

    DataFrame frame(ds);

    frame.FillDataChannelsFromStruct(data);

    Averager::Append(frame);

    frame.ds->valid = 1;

    time_meter.Reset();
}


void Storage::SameSettings::Calculate(const DataStruct &data)
{
    DataSettings ds = GetDataSettings(0);

    if (ds.valid && data.ds.Equal(ds))
    {
        if (count < count_data)
        {
            count++;
        }
    }
    else
    {
        count = 1;
    }
}


void DataFrame::FillDataChannelFromBuffer(Chan ch, BufferFPGA &buffer)
{
    uint8 *data = (uint8 *)DataBegin(ch);

    std::memcpy(data, buffer.Data(), (uint)buffer.Size());
}


void DataFrame::FillDataChannelsFromStruct(DataStruct &data)
{
    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    uint num_bytes = (uint)data.ds.BytesInChanStored();

    std::memcpy(address, data.A.Data(), num_bytes);

    std::memcpy(address + num_bytes, data.B.Data(), num_bytes);
}


int Storage::NumFrames()
{
    return count_data;
}


int Storage::SameSettings::GetCount()
{
    return count;
}


static DataSettings *Storage::GetDataSettingsPointer(int indexFromEnd)
{
    if (first_ds == nullptr)
    {
        return nullptr;
    }

    int index = indexFromEnd;
    DataSettings *ds = last_ds;

    while (index != 0 && ((ds = (DataSettings *)ds->prev) != 0))
    {
        index--;
    }

    if (index != 0)
    {
        //        LOG_ERROR("Неправильный индекс %d, всего данных %d", indexFromEnd, NumFrames());      // \todo После сброса настроек здесь срабатывает
        return nullptr;
    }

    return ds;
}


static DataSettings Storage::GetDataSettings(int indexFromEnd)
{
    DataSettings *dp = GetDataSettingsPointer(indexFromEnd);

    if (!dp)
    {
        static DataSettings ds_null;
        ds_null.valid = 0;
        return ds_null;
    }

    return *dp;
}


const DataStruct &Storage::GetData(int from_end)
{
    static FrameImitation result;

    DataSettings *dp = GetDataSettingsPointer(from_end);

    if (!dp)
    {
        result.data.ds.valid = 0;
        return result.data;
    }

    DataSettings ds = *dp;

    if (ds.id != result.data.ds.id)
    {
        result.data.ds = ds;
        result.data.ds.valid = 1;

        int num_bytes = ds.BytesInChanStored();

        if (result.data.A.Size() != num_bytes)
        {
            result.data.A.Realloc(num_bytes);
            result.data.B.Realloc(num_bytes);
        }

        uint8 *address = (uint8 *)dp + sizeof(DataSettings);

        result.data.A.FillFromBuffer(address, num_bytes);
        result.data.B.FillFromBuffer(address + num_bytes, num_bytes);
    }

    return result.data;
}


const DataStruct &Storage::GetLatest()
{
    if (ENumAveraging::ToNumber() > 1)
    {
        return Averager::GetData();
    }

    return GetData(0);
}


void Storage::CopyData(DataSettings *ds, Chan ch, BufferFPGA &data)
{
    uint8 *address = ((uint8 *)ds + sizeof(DataSettings));

    uint length = (uint)ds->BytesInChanStored();

    if (ch.IsB())
    {
        address += length;
    }

    data.ReallocFromBuffer(address, (int)length);
}


int Storage::NumberAvailableEntries()
{
    if (first_ds == nullptr)
    {
        return 0;
    }

    return SIZE_POOL / last_ds->SizeFrame();
}


DataSettings *Storage::PrepareNewFrame(DataSettings &ds)
{
    int required = ds.SizeFrame();

    while (MemoryFree() < required)
    {
        RemoveFirstFrame();
    }

    uint8 *addrRecord = nullptr;

    if (first_ds == nullptr)
    {
        first_ds = (DataSettings *)beginPool;
        addrRecord = beginPool;
        ds.prev = nullptr;
        ds.next = nullptr;
    }
    else
    {
        addrRecord = (uint8 *)last_ds + last_ds->SizeFrame();

        if (addrRecord + ds.SizeFrame() > endPool)
        {
            addrRecord = beginPool;
        }

        ds.prev = last_ds;
        last_ds->next = addrRecord;
        ds.next = nullptr;
    }

    last_ds = (DataSettings *)addrRecord;

    std::memcpy(addrRecord, &ds, sizeof(DataSettings));

    count_data++;

    return last_ds;
}


int Storage::MemoryFree()
{
    if (first_ds == nullptr)
    {
        return SIZE_POOL;
    }
    else if (first_ds == last_ds)
    {
        return (endPool - (uint8 *)first_ds - (int)first_ds->SizeFrame());
    }
    else if (first_ds < last_ds)
    {
        if ((uint8 *)first_ds == beginPool)
        {
            return (endPool - (uint8 *)last_ds - last_ds->SizeFrame());
        }
        else
        {
            return (uint8 *)first_ds - beginPool;
        }
    }
    else if (last_ds < first_ds)
    {
        return (uint8 *)first_ds - (uint8 *)last_ds - last_ds->SizeFrame();
    }
    return 0;
}


int DataSettings::SizeFrame() const
{
    return (int)sizeof(DataSettings) + 2 * BytesInChanStored();
}


void Storage::RemoveFirstFrame()
{
    if (first_ds)
    {
        first_ds = (DataSettings *)first_ds->next;
        first_ds->prev = nullptr;
        count_data--;
    }
}


void Storage::RemoveLastFrame()
{
    if (last_ds)
    {
        if (last_ds->prev)
        {
            DataSettings *ds = (DataSettings *)last_ds->prev;
            ds->next = nullptr;
        }
        else
        {
            last_ds = nullptr;
            first_ds = nullptr;
        }

        count_data--;
    }
}


bool Storage::SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings dp0 = GetDataSettings(elemFromEnd0);
    DataSettings dp1 = GetDataSettings(elemFromEnd1);

    if (!dp0.valid || !dp1.valid)
    {
        return false;
    }

    return dp0.Equal(dp1);
}
