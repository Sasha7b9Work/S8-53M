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
    void AddData(DataSettings, uint8 *dataA, uint8 *dataB);

    // ������� ����� ��������� ��������� � ������.
    int NumElements();

    // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
    int NumElementsWithSameSettings();

    // ���������� ����� ����������� ���������, ������� � ����������, � �������� ����������� �������
    int NumElementsWithCurrentSettings();

    // �������� ��������� �� ������
    bool GetData(int fromEnd, DataStruct &);

    uint8* GetData(Chan, int fromEnd);

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    uint8* GetLimitation(Chan, int direction);                                      

    int NumberAvailableEntries();

    namespace P2P
    {
        // ������� ����� ����� ����������� ������.
        void CreateFrame(const DataSettings &);

        // �������� ����� �������� ������
        void Reset();

        // �������� ����� � ������� �����
        void AddPoints(BitSet16 dataA, BitSet16 dataB);
    }
};
