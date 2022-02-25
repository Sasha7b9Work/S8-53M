// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"


namespace Storage
{
    // �������� ���� ����������� ���������
    void Clear();

    // �������� ��������� ������. ��� ���� ����������� ��������� ������� ��������� �������.
    void AddData(uint8 *data0, uint8 *data1, DataSettings dss);

    // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
    int NumElementsWithSameSettings();

    // ���������� ����� ����������� ���������, ������� � ����������, � �������� ����������� �������
    int NumElementsWithCurrentSettings();

    // ���������� ���������� ����������� ���������
    int NumElementsInStorage();

    // �������� ��������� �� ������
    bool GetDataFromEnd(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1);

    uint8* GetData(Chan::E, int fromEnd);

    // �������� ����������� ������ �� ���������� ����������.
    uint8* GetAverageData(Chan::E);

    // ������� ����� ��������� ��������� � ������.
    int AllDatas();

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    uint8* GetLimitation(Chan::E, int direction);                                      

    int NumberAvailableEntries();

    extern uint8        *dataA;     // ��������� �� ������ ������� ������, ������� ���� �������� �� ������
    extern uint8        *dataB;     // ��������� �� ������ ������� ������, ������� ���� �������� �� ������
    extern DataSettings *DS;        // ��������� �� ��������� �������� ��������

    extern uint8        *dataLastA;
    extern uint8        *dataLastB;
    extern DataSettings *dsLast;

    extern uint8        *dataIntA;    // | ����� ������ �� ����, ������� ������ ���������� �� �����
    extern uint8        *dataIntB;    //-/
    extern DataSettings *dsInt;       //--
};