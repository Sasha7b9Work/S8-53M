// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Data/DataExtensions.h"
#include "FPGA/TypesFPGA.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Data/Processing.h"
#include <cstring>


namespace Averager
{
    namespace Accuracy
    {
        DataStruct &GetData();

    }

    namespace Around
    {
        DataSettings     ave_ds;                            // \-
        int              num_ave = -1;                      // |- Настройки усредняемых сейчас сигналов 
        ModeAveraging::E mode_ave = ModeAveraging::Count;   // /-

        Buffer2048<float> ave_a;        // Здесь усреднённые значения,
        Buffer2048<float> ave_b;        // рассчитанные по приблизетильному алгоритму

        int added_datas = 0;        // Столько данных учтено в измерениях

        // Действия, которые необходимо произвести перед доабавлением новых дыннах
        void ProcessBeforeAdding(const DataFrame &);

        void Append(const DataFrame &);

        DataStruct &GetData();
    }
}


namespace Limitator
{
    // Максимальные значения каналов
    uint8 lim_up[Chan::Count][FPGA::MAX_POINTS * 2];

    // Минимальные значения каналов
    uint8 lim_down[Chan::Count][FPGA::MAX_POINTS * 2];
}


void Averager::Around::ProcessBeforeAdding(const DataFrame &frame)
{
    if (ENumAveraging::ToNumber() < 2)
    {
        added_datas = 0;
    }
    else
    {
        if (!ave_ds.Equal(*frame.ds) || (num_ave != ENumAveraging::ToNumber()) || (mode_ave != ModeAveraging::Current()))
        {
            added_datas = 0;
        }

        if (added_datas == 0)
        {
            ave_ds.Set(*frame.ds);
            num_ave = ENumAveraging::ToNumber();
            mode_ave = ModeAveraging::Current();
            ave_a.Realloc(ave_ds.BytesInChanStored());
            ave_b.Realloc(ave_ds.BytesInChanStored());
        }
    }
}


void Averager::Around::Append(const DataFrame &frame)
{
    int size = frame.ds->BytesInChanStored();

    const uint8 *a = frame.DataBegin(ChA);
    const uint8 *b = frame.DataBegin(ChB);

    if (added_datas == 0)
    {
        for (int i = 0; i < size; i++)
        {
            ave_a[i] = a[i];
            ave_b[i] = b[i];
        }
    }
    else
    {
        int num_datas = Math::Limitation<int>(Storage::SameSettings::GetCount(), 0, ENumAveraging::ToNumber());

        float ave_inv = 1.0f / (float)num_datas;
        float ave_fless = 1.0f - ave_inv;

        float *out_a = ave_a.Data();
        float *out_b = ave_b.Data();

        const uint8 *in_a = a;
        const uint8 *in_b = b;

        float *end = ave_a.Last();

        while (out_a < end)
        {
            uint8 ia = *in_a;
            uint8 oa = *out_b;

            if (ia != ValueFPGA::NONE && oa != ValueFPGA::NONE)
            {
                *out_a = (*out_a) * ave_fless + ((float)(*in_a++)) * ave_inv;
                out_a++;

                *out_b = (*out_b) * ave_fless + ((float)(*in_b++)) * ave_inv;
                out_b++;
            }
            else if (ia != ValueFPGA::NONE)
            {
                *out_a++ = *in_a++;
                *out_b++ = *in_b++;
            }
            else
            {
                out_a++;
                in_a++;
                out_b++;
                in_b++;
            }
        };
    }

    added_datas++;
}


void Averager::Append(const DataFrame &frame)
{
    if (ModeAveraging::IsAround())
    {
        Around::ProcessBeforeAdding(frame);

        Around::Append(frame);
    }
}


const DataStruct &Averager::GetData()
{
    if (ModeAveraging::IsAccurately())
    {
        return Accuracy::GetData();
    }
    else if(ModeAveraging::IsAround())
    {
        return Around::GetData();
    }

    static DataStruct null_ds;
    null_ds.ds.valid = 0;

    return null_ds;
}


DataStruct &Averager::Accuracy::GetData()
{
    static DataStruct result;

    int num_datas = ENumAveraging::ToNumber();

    LIMIT_ABOVE(num_datas, Storage::SameSettings::GetCount());

    DataSettings ds = Storage::GetDataSettings(0);

    int num_bytes = ds.BytesInChanStored();

    Buffer2048<uint> sum_a(num_bytes, 0);
    Buffer2048<uint> sum_b(num_bytes, 0);

    uint *psum_a = sum_a.Data();
    uint *psum_b = sum_b.Data();

    for (int d = 0; d < num_datas; d++)
    {
        const DataStruct &data = Storage::GetData(d);

        const uint8 *in_a = data.DataConst(ChA).DataConst();
        const uint8 *in_b = data.DataConst(ChB).DataConst();

        for (int i = 0; i < num_bytes; i++)
        {
            psum_a[i] += in_a[i];
            psum_b[i] += in_b[i];
        }
    }

    result.ds.Set(ds);
    result.A.Realloc(num_bytes);
    result.B.Realloc(num_bytes);

    uint8 *out_a = result.A.Data();
    uint8 *out_b = result.B.Data();

    for (int i = 0; i < num_bytes; i++)
    {
        out_a[i] = (uint8)(psum_a[i] / num_datas);
        out_b[i] = (uint8)(psum_b[i] / num_datas);
    }

    return result;
}


DataStruct &Averager::Around::GetData()
{
    static DataStruct result;

    result.ds = ave_ds;

    int num_bytes = ave_ds.BytesInChanStored();

    result.A.Realloc(num_bytes);
    result.B.Realloc(num_bytes);

    uint8 *out_a = result.A.Data();
    uint8 *out_b = result.B.Data();

    float *in_a = ave_a.Data();
    float *in_b = ave_b.Data();

    for (int i = 0; i < num_bytes; i++)
    {
        out_a[i] = (uint8)(in_a[i] + 0.5f);
        out_b[i] = (uint8)(in_b[i] + 0.5f);
    }

    result.ds.valid = 1;

    return result;
}


void Limitator::GetLimitation(Chan ch, int direction, DataStruct &data)
{
    uint8 *buffer = 0;

    if (direction == 0)
    {
        buffer = &(lim_down[ch][0]);
    }
    else if (direction == 1)
    {
        buffer = &(lim_up[ch][0]);
    }

    data.ds.Set(Processing::out.ds);

    data.Data(ch).ReallocFromBuffer(buffer, data.ds.BytesInChanStored());
}


void Limitator::ClearLimits()
{
    std::memset(lim_up[0], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_up[1], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[0], 0xff, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[1], 0xff, FPGA::MAX_POINTS * 2);
}


void Limitator::Append(const DataStruct &in)
{
    uint numElements = (uint)in.ds.PointsInChannel();

    const uint8 *a = in.A.DataConst();
    const uint8 *b = in.B.DataConst();

    if (Storage::NumFrames() == 0 || NUM_MIN_MAX == 1 || (!Storage::GetDataSettings(0).Equal(in.ds)))
    {
        for (uint i = 0; i < numElements; i++)
        {
            lim_down[0][i] = lim_up[0][i] = a[i];
            lim_down[1][i] = lim_up[1][i] = b[i];
        }
    }
    else
    {
        int allDatas = Storage::SameSettings::GetCount();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if (Storage::SameSettings::GetCount() >= NUM_MIN_MAX)
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
            const DataStruct &data = Storage::GetData(numData);

            const uint8 *dA = data.A.DataConst();
            const uint8 *dB = data.B.DataConst();

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
