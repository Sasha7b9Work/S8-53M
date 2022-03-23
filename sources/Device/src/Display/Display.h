// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


namespace Display
{
    const int WIDTH = 320;
    const int HEIGHT = 240;

    extern uint8 *const back_buffer;

    extern bool framesElapsed;
    extern int  numDrawingSignals;  // ����� ������������ �������� ��� ������ ����������

    extern int topMeasures;         // ���� ������� ������ ���������. ��� �������� ����� ��� ����������� ������ ��������� �� ����� - ����� 
                                    // ��� ������ �� �����������

    void Init();

    void Update(bool endScene = true);

    void RotateRShift(Chan);

    void RotateTrigLev();

    void EnableTrigLabel(bool enable);

    void ShowWarningBad(Warning::E warning);

    void ShowWarningGood(Warning::E warning);

    void ClearFromWarnings();

    void SetDrawMode(DrawMode::E, pFuncVV = nullptr);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    // ����� ��������� ���������� ������ ��� ������� ����� ������� ���� ���.
    void RunAfterDraw(pFuncVV func);
};
