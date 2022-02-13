// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "Utils/GlobalFunctions.h"


typedef enum
{
    kPressKey,                  // ����� �������������, ����� �������� ������� �������.
    kShowLevelRShift0,          // ����� �������������, ����� ���������� ��������� ����� �������� ������ 1.
    kShowLevelRShift1,          // ����� �������������, ����� ���������� ��������� ����� �������� ������ 2.
    kShowLevelTrigLev,          // ����� �������������, ����� ���������� ��������� ����� ������ �������������
    kNumSignalsInSec,           // ��� ��������� ���������� ���������� ������� � �������.
    kFlashDisplay,              // ������ ��� ��������� �������� ������ �����-������ �������.
    kP2P,                       // ������ ��� ������ ������ � ������ ����������� ������.
    kShowMessages,              // ������ ��� ��������� ����� ������ �������������� � ��������������� ���������.
    kMenuAutoHide,              // ������ ��� ������� ������� ��������� ����.
    kRShiftMarkersAutoHide,     // ������ ��� ������� ������� ��������� �������������� ������� �����.
    kTemp,                      // ���������������, ��� ����� ����.
    kStopSound,                 // ��������� ����
    kTemporaryPauseFPGA,        // ��������� ����� ��� �������� ������� �� ������ ����� �������� �����
    kTimerDrawHandFunction,     // ���� ������ ����� ������������ �������, ������������ ���� ��������� ���������
    TypeTimerSize               // ����� ���������� ��������.
} TypeTimer;


// ���������������� ���������� ��������� �� timeMS �����������
void Timer_PauseOnTime(uint timeMS);
// ���������������� ���������� ��������� �� numTicks �����
void Timer_PauseOnTicks(uint numTicks);
// ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ��������� ����� �������� �� 1��.
void Timer_Update1ms();
// ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ���������� ����� �������� �� 10��.
void Timer_Update10ms();
// ������ �������
void Timer_Enable(TypeTimer type,   // ��� �������
                  int timeInMS,     // ����� ������������ �������
                  pFuncVV func      // �������, ������� ���������� ������ ���, ����� ����������� ������
                  );
// ���������� ������
void Timer_Disable(TypeTimer type);
// ������������� ������. ��������� ����� ����� �������� Timer_Continue()
void Timer_Pause(TypeTimer type);
// ���������� ������ �������, ����� ����������������� �������� Timer_Pause()
void Timer_Continue(TypeTimer type);
// � ������� ���� ������� ����� ������, �������� �� ������
bool Timer_IsRun(TypeTimer type);
// ��������� ������� ��� ��������� ����� �������� �������.
void Timer_StartMultiMeasurement();
// ������������� ��������� ����� ������������. ����� ������ Timer_LogPoint �������� ��������� ��������� �� ��� �����.
void Timer_StartLogging();

uint Timer_LogPointUS(char *name);

uint Timer_LogPointMS(char *name);
// �����, ��������� � ������� ������������� �������, � ��������
//extern volatile uint gTimerMS;                  

// ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement().
// � ����� ������� 120.000.000 �����. ������������ ������� �������, ������� ����� ��������� � � ������� - 35 ���.
// ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement(). �� ����� (1 << 32).
#define gTimerTics (TIM2->CNT)
