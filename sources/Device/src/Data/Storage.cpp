// 2022/02/11 17:49:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/Storage.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Strings.h"
#include "Utils/Containers/Buffer.h"
#include "Data/DataExtensions.h"
#include <cstring>


namespace Storage
{
    // ���������� ��������� ��� ��������� ������.
    const int SIZE_POOL = (30 * 1024);

    // ����� �������� ������.
    uint8 pool[SIZE_POOL] = {0};

    // ����� ������ ������ ��� ��������
    uint8 *beginPool = &(pool[0]);

    // ����� ���������� ����� ������ ��� ��������
    uint8 *endPool = &(pool[SIZE_POOL - 1]);

    // ������������ �������� �������
    uint8 lim_up[Chan::Count][FPGA::MAX_POINTS * 2];

    // ����������� �������� �������
    uint8 lim_down[Chan::Count][FPGA::MAX_POINTS * 2];

    // ��������� �� ������ ����������� ������
    DataSettings *first_ds = nullptr;

    // ��������� �� ��������� ����������� ������
    DataSettings *last_ds = nullptr;

    // ����� ������ ���������
    int num_frames = 0;

    // ���������� ���������� ��������� ������ � ������
    int MemoryFree();

    // ������� ������ (����� ������) ���������
    void RemoveFirstFrame();

    // ������� ��������� (����� �����) ���������
    void RemoveLastFrame();

    // ����������� ����� ����� ��� ������ � ���� ������. ���������� � ���� ������ �� ds
    DataSettings *PrepareNewFrame(DataSettings &ds);

    // ���������� true, ���� ��������� ��������� � ��������� elemFromEnd0 � elemFromEnd1 ���������, � false � ���� ������.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);

    // ������� �������� ���, ���� � ����
    void ClearLimitsAndSums();

    void CalculateLimits(const DataSettings *, const uint8 *dataA, const uint8 *dataB);

    // ���� ��������� ����� �����
    void AppendFrameP2P(DataSettings);
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
    ClearLimitsAndSums();
}


void Storage::ClearLimitsAndSums()
{
    std::memset(lim_up[0], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_up[1], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[0], 0xff, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[1], 0xff, FPGA::MAX_POINTS * 2);
}


void Storage::OpenFrame()
{
    DataSettings ds;
    ds.Init();

    PrepareNewFrame(ds);
}


int Storage::NumFrames()
{
    return num_frames;
}


void Storage::CalculateLimits(const DataSettings *dss, const uint8 *a, const uint8 *b)
{
    uint numElements = (uint)dss->PointsInChannel();

    if (NumFrames() == 0 || NUM_MIN_MAX == 1 || (!GetDataSettings(0)->Equal(*dss)))
    {
        for (uint i = 0; i < numElements; i++)
        {
            lim_down[0][i] = lim_up[0][i] = a[i];
            lim_down[1][i] = lim_up[1][i] = b[i];
        }
    }
    else
    {
        int allDatas = NumFramesWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if (NumFramesWithSameSettings() >= NUM_MIN_MAX)
        {
            for (uint i = 0; i < numElements; i++)
            {
                lim_down[0][i] = lim_up[0][i] = a[i];
                lim_down[1][i] = lim_up[1][i] = b[i];
            }
            allDatas--;
        }

        for (int numData = 0; numData < allDatas; numData++)
        {
            const uint8 *dA = GetData(Chan::A, numData);
            const uint8 *dB = GetData(Chan::B, numData);

            for (uint i = 0; i < numElements; i++)
            {
                if (dA[i] < lim_down[0][i]) lim_down[0][i] = dA[i];
                if (dA[i] > lim_up[0][i])   lim_up[0][i] = dA[i];
                if (dB[i] < lim_down[1][i]) lim_down[1][i] = dB[i];
                if (dB[i] > lim_up[1][i])   lim_up[1][i] = dB[i];
            }
        }
    }
}


int Storage::NumFramesWithSameSettings()
{
    int retValue = 0;
    int numElements = NumFrames();

    for (retValue = 1; retValue < numElements; retValue++)
    {
        if (!SettingsIsIdentical(retValue, retValue - 1))
        {
            break;
        }
    }
    return retValue;
}


int Storage::NumFramesWithCurrentSettings()
{
    DataSettings dp;
    dp.Init();
    int retValue = 0;
    int numElements = NumFrames();

    for (retValue = 0; retValue < numElements; retValue++)
    {
        if (!GetDataSettings(retValue)->Equal(dp))
        {
            break;
        }
    }
    return retValue;
}


void Storage::CloseFrame()
{
    DataSettings *ds = GetDataSettings(0);

    ds->time = HAL_RTC::GetPackedTime();

    CalculateLimits(ds, ds->DataBegin(ChA), ds->DataEnd(ChB));

    Averager::Append(ds);
}


bool Storage::GetData(int fromEnd, DataStruct &data)
{
    DataSettings *ds = GetDataSettings(fromEnd);

    if (ds == nullptr)
    {
        data.ds.valid = 0;

        LOG_WRITE("��� ������");

        return false;
    }

    data.ds.Set(*ds);

    data.A.FromBuffer(ds->DataBegin(ChA), ds->BytesInChannel());
    data.B.FromBuffer(ds->DataBegin(ChB), ds->BytesInChannel());

    return true;
}


uint8 *Storage::GetData(Chan ch, int fromEnd)
{
    DataSettings *ds = GetDataSettings(fromEnd);

    if (ds == 0)
    {
        return nullptr;
    }

    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    return ch.IsA() ? address : (address + ds->BytesInChannel());
}


uint8 *Storage::GetLimitation(Chan ch, int direction)
{
    uint8 *retValue = 0;

    if (direction == 0)
    {
        retValue = &(lim_down[ch][0]);
    }
    else if (direction == 1)
    {
        retValue = &(lim_up[ch][0]);
    }

    return retValue;
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

    last_ds->closed = 0;

    num_frames++;

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


int DataSettings::SizeFrame()
{
    return (int)sizeof(DataSettings) + 2 * BytesInChannel();
}


void Storage::RemoveFirstFrame()
{
    if (first_ds)
    {
        first_ds = (DataSettings *)first_ds->next;
        first_ds->prev = nullptr;
        num_frames--;
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

        num_frames--;
    }
}


DataSettings *Storage::GetDataSettings(int indexFromEnd)
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
//        LOG_ERROR("������������ ������ %d, ����� ������ %d", indexFromEnd, NumElements());      // \todo ����� ������ �������� ����� �����������
        return nullptr;
    }

    return ds;
}


bool Storage::SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings *dp0 = GetDataSettings(elemFromEnd0);
    DataSettings *dp1 = GetDataSettings(elemFromEnd1);

    return dp0->Equal(*dp1);
}


void Storage::CreateFrameP2P(const DataSettings &_ds)
{
    if (Storage::NumFrames() == 0)
    {
        AppendFrameP2P(_ds);
    }
    else
    {
        DataSettings *ds = GetDataSettings(0);

        if (ds->InModeP2P() && ds->Equal(_ds))
        {
            ds->ResetP2P();
        }
        else
        {
            AppendFrameP2P(_ds);
        }
    }
}


void Storage::AppendFrameP2P(DataSettings ds)
{
    ds.ResetP2P();

    PrepareNewFrame(ds);
}
