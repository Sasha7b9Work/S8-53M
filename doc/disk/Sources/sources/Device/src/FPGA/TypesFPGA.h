#pragma once


/** @file FPGA_Types.h
    @brief ���� �������� �������, ������ � � ���������, ����������� ��� ������ � ��������
    @verbatim
    ����� ������� ��������� ������� ���������� ��������� ������� 2-15
    ������� WR_START ����������� ������ ��� ��� ������� ����� ��������������
    ������� WR_STOP ����������� ��� ����������� ����� �������������� ����� ����� ������


    ��� ���������� �������� ������� � �������������, ���������� ������� � ������������� ������������ ������ PG2, PG3, PG5, PG7
    PG5 - ����� ������� (���������)
    PG7 - ����� �������� (���������)
    PG2 - �������� ��������
    PG3 - ������

    �������� ������� ������ - ������.
    ����� ������� ���������� ������� ��������� �������� ������ ���� ����� ����.
    ������ ���������� �� ����� ��������� ��������.
    ���������� AD5314.
                            __      __      __      __         __      __      __      __      __
                           /  \    /  \    /  \    /  \       /  \    /  \    /  \    /  \    /  \
    PG2       ____________/    \__/    \__/    \__/    \__/  /    \__/    \__/    \__/    \__/    \_____________
              _________                                                                                _________
    PG5 (PG7)          \___________________________________   ________________________________________/
                             ____    ____    ____    ____       ____    ____    ____    ____    ____                                                              
    PG3       ______________/    \__/    \__/    \__/    \_   _/    \__/    \__/    \__/    \__/    \___________
                            \____/  \____/  \____/  \____/     \____/  \____/  \____/  \____/  \____/                                                    
                             DB15    DB14    DB13    DB12       DB4     DB3     DB2     DB1     DB0

    @endverbatim 
*/


/*
* ��� �������������� ������ � �8-53/1. � �8-53� �������������� �� ������ *******************************
*/
#define WR_CAL_A        ((uint8 *)12)            //    ������������� ����������� ������ 1.
#define WR_CAL_B        ((uint8 *)13)            //    ������������� ����������� ������ 2.
/*
* ******************************************************************************************************
*/


#define WR_START                (HAL_FMC::ADDR_FPGA + 0x00)    // ������ ����� ���������. ��� ������� ����� �������� 1.
#define WR_RAZV                 (HAL_FMC::ADDR_FPGA + 0x01)    // ��������� ������� �������������. ������������ � TBase::Load().
#define WR_PRED                 (HAL_FMC::ADDR_FPGA + 0x02)
#define WR_POST                 (HAL_FMC::ADDR_FPGA + 0x03)
#define WR_TRIG                 (HAL_FMC::ADDR_FPGA + 0x04)
#define WR_UPR                  (HAL_FMC::ADDR_FPGA + 0x05)
    #define UPR_BIT_RAND                    0   // ���������� �� �������������
    #define UPR_BIT_PEAKDET                 1   // ������� �������� - 0/1 ����/���
    #define UPR_BIT_CALIBRATOR_AC_DC        2   // ����������/����������
    #define UPR_BIT_CALIBRATOR_VOLTAGE      3   // 0/4�
    #define UPR_BIT_RECORDER                4   // 0 - ������� �����, 1 - �����������
#define WR_ADDR_READ            (HAL_FMC::ADDR_FPGA + 0x06) // ��� ������� ���������� ����� ������ � ������� �����������
#define WR_FREQ_METER_PARAMS    (HAL_FMC::ADDR_FPGA + 0x0a)
#define WR_STOP                 (HAL_FMC::ADDR_FPGA + 0x1f)

#define RD_ADC_A                (HAL_FMC::ADDR_FPGA + 0x00)
#define RD_ADC_B                (HAL_FMC::ADDR_FPGA + 0x02)
#define RD_ADDR_LAST_RECORD     (HAL_FMC::ADDR_FPGA + 0x08) // ������ ������ ��������� ������
#define RD_FREQ_LOW             (HAL_FMC::ADDR_FPGA + 0x10)
#define RD_FREQ_HI              (HAL_FMC::ADDR_FPGA + 0x18)
#define RD_PERIOD_LOW           (HAL_FMC::ADDR_FPGA + 0x20)
#define RD_PERIOD_HI            (HAL_FMC::ADDR_FPGA + 0x28)
#define RD_FL                   (HAL_FMC::ADDR_FPGA + 0x30)
    #define FL_DATA             0   // 0 - ������ ������
    #define FL_TRIG             1   // 1 - ������� ��������������
    #define FL_PRED             2   // 2 - ��������� ����� �����������
    #define FL_POINT            3   // 3 - ������� ����, ��� ����� ������ (� ���������� ������)
    #define FL_FREQ             4   // 4 - ����� ��������� �������
    #define FL_PERIOD           5   // 5 - ����� ��������� ������
    #define FL_LAST_RECOR       6   // 6 - ������� ��������� ������ - ����������, ����� ��� ������� ������
    #define FL_OVERFLOW_FREQ    8   // 8 - ������� ������������ �������� �������
    #define FL_OVERFLOW_PERIOD  9   // 9 - ������� ������������ �������� �������

struct TypeWriteAnalog
{
    enum E
    {
        All,
        RangeA,
        RangeB,
        TrigParam,
        ChanParam0,
        ChanParam1
    };
};


struct TypeWriteDAC
{
    enum E
    {
        RShiftA,
        RShiftB,
        TrigLev
    };
};


// ��� �������� �������� �������, ��������� � ���, ������������� ������ ������� ����� (-5 ������ �� ������). ����
// �������� == 0, ������, ��� ���. ��� ����� ��� ������� ������������� � ����������� ������ p2p, � ����� ��� tshift
// ����� ���������� �������.
struct ValueFPGA
{
    static const uint8 NONE = 0;

    static const uint8 MIN = 2;
    static const uint8 AVE = 127;   // ��� �������� �������� �������, ��������� � ���, ������������� ������ �����.
                                    // ���� �������� == 0, ������, ��� ���. ��� ����� ��� ������� ������������� �
                                    // ����������� ������ p2p, � ����� ��� tshift ����� ���������� �������.
    static const uint8 MAX = 252;   // ��� �������� �������� �������, ��������� � ���, ������������� ������� �������
                                    // ����� (+5 ������ �� ������). ���� �������� == 0, ������, ��� ���. ��� ����� ���
                                    // ������� ������������� � ����������� ������ p2p, � ����� ��� tshift �����
                                    // ���������� �������.

    static float ToVoltage(uint8 value, Range::E, RShift);
    static uint8 FromVoltage(float voltage, Range::E, RShift);
    static void ToVoltageArray(const uint8 *points, int numPoints, Range::E, RShift, float *voltage);
    static void FromVoltageArray(const float *in, int numPoints, Range::E, RShift, uint8 *out);
};
