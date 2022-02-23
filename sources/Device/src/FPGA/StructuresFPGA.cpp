// 2022/02/23 08:43:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include <climits>


namespace FPGA
{
    namespace Launch
    {
        static int pred = 0;    // ����- � �����- ������� �������� � ������.
        static int post = 0;    // ����� �������� �� ����� ��������� � ��� ����, ������ ��� ��� ����� ����������� �� ���

        static const int8 d_pred[TBase::Count] =   // �������������� �������� ��� �����������
        {//  2    5   10   20   50  100  200
            10,  10,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

        static const int8 d_post[TBase::Count] =   // �������������� �������� ��� ������������
        {//  2    5   10   20   50  100  200
            10,  20,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

//        static const int8 d_read[TBase::Count] =   // �������������� �������� ��� ������ ������
//        {// 2    5   10   20   50  100  200
//            0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//        };

        void Calculate();

        void CalculateReal();

        void CalculateRandomize();

        // ���������� ��������, ������� ��� ������ � ����
        uint16 PostForWrite();
    }
}


void FPGA::Launch::Load()
{
    if (!FPGA::Reader::mutex_read.IsLocked())
    {
        Calculate();

        BUS_FPGA::Write(WR_POST, PostForWrite(), true);
        BUS_FPGA::Write(WR_PRED, PredForWrite(), true);
    }
}


uint16 FPGA::Launch::PostForWrite()
{
    int result = post + d_post[SET_TBASE];

    if (PEAKDET_IS_ENABLE)
    {
        result *= 2;
    }

    return (uint16)(~result);
}


uint16 FPGA::Launch::PredForWrite()
{
    int result = pred + d_pred[SET_TBASE];

    if (PEAKDET_IS_ENABLE)
    {
        result *= 2;
    }

    return (uint16)(~result);
}


void FPGA::Launch::Calculate()
{
    if (TBase::InRandomizeMode())
    {
        CalculateRandomize();
    }
    else
    {
        CalculateReal();
    }
}


void FPGA::Launch::CalculateReal()
{
    int num_points = ENUM_POINTS_FPGA::ToNumBytes();

    int values[TPos::Count] = {num_points, num_points / 2, 0};

    pred = post = values[SET_TPOS];

    pred = Math::Limitation(pred - TSHIFT * 2, 0, 65535);

    post = Math::Limitation(post + TSHIFT * 2, 0, 65535);
}


void FPGA::Launch::CalculateRandomize()
{
    CalculateReal();

    pred++;
    post++;
}
