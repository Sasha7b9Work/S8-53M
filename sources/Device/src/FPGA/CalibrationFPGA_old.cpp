// 2022/02/11 17:48:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "FPGA/FPGA.h" 
#include "FPGA/TypesFPGA_old.h"
#include "Display/Display.h"
#include "Display/DisplayPrimitives.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include <stm32f4xx_hal.h>
#include <stdio.h>
#include <limits.h>


namespace FPGA
{
    int16 CalculateAdditionRShift(Chan::E ch, Range::E range);   // �������� ���������� �������� ������ �� ����������.
    float CalculateKoeffCalibration(Chan::E ch);              // �������� ����������� ���������� ������ �� ����������.
    void  AlignmentADC();
    void  FuncAttScreen();                                    // ������� ���������� ������ � ������ ����������.
    float CalculateDeltaADC(Chan::E ch, float *avgADC1, float *avgADC2, float *delta);
    void  DrawParametersChannel(Chan::E ch, int x, int y, bool inProgress);
    void  FuncAttScreen();
    void  DrawParametersChannel(Chan::E ch, int eX, int eY, bool inProgress);
    float CalculateDeltaADC(Chan::E ch, float *avgADC1, float *avgADC2, float *delta);
    void  AlignmentADC();
    void  OnTimerDraw();

    float deltaADC[2] = {0.0f, 0.0f};
    float deltaADCPercents[2] = {0.0f, 0.0f};
    float avrADC1[2] = {0.0f, 0.0f};
    float avrADC2[2] = {0.0f, 0.0f};

    float deltaADCold[2] = {0.0f, 0.0f};
    float deltaADCPercentsOld[2] = {0.0f, 0.0f};
    float avrADC1old[2] = {0.0f, 0.0f};
    float avrADC2old[2] = {0.0f, 0.0f};

    int8 shiftADC0 = 0;
    int8 shiftADC1 = 0;

    float koeffCal0 = -1.0f;
    float koeffCal1 = -1.0f;

    ProgressBar bar0;                            // ��������-��� ��� ���������� ������� ������.
    ProgressBar bar1;                            // ��������-��� ��� ���������� ������� ������.

    uint startTimeChan0 = 0;                     // ����� ������ ���������� ������� ������.
    uint startTimeChan1 = 0;                     // ����� ������ ���������� ������� ������.

    float koeffCalibrationOld[2];
}


void FPGA::OnTimerDraw()
{
    Display::Update();
}


void FPGA::ProcedureCalibration()
{
    bool chanAenable = SET_ENABLED_A;
    bool chanBenable = SET_ENABLED_B;

    SET_ENABLED_A = SET_ENABLED_B = true;

    Display::SetDrawMode(DrawMode_Hand, FuncAttScreen);
    Timer::Enable(TypeTimer::TimerDrawHandFunction, 100, OnTimerDraw);

    koeffCalibrationOld[Chan::A] = STRETCH_ADC_A;
    koeffCalibrationOld[Chan::B] = STRETCH_ADC_B;

    bar0.fullTime = bar0.passedTime = bar1.fullTime = bar1.passedTime = 0;

    FPGA::SaveState();                               // ��������� ������� ���������.
    Panel::Disable();                                // ��������� ������ ����������.

    while(1)
    {
        gStateFPGA.stateCalibration = StateCalibration_ADCinProgress;                  // ��������� ��������� ������������ ���.

        TBase::Set(TBase::_500us);
        TShift::Set(0);
        STRETCH_ADC_A = 1.0f;
        STRETCH_ADC_B = 1.0f;
        FPGA::LoadKoeffCalibration(Chan::A);
        FPGA::LoadKoeffCalibration(Chan::B);
        Range::Set(Chan::A, Range::_500mV);
        Range::Set(Chan::B, Range::_500mV);
        RShift::Set(Chan::A, RShift::ZERO);
        RShift::Set(Chan::B, RShift::ZERO);
        ModeCouple::Set(Chan::A, ModeCouple::GND);
        ModeCouple::Set(Chan::B, ModeCouple::GND);

        deltaADCPercentsOld[0] = CalculateDeltaADC(Chan::A, &avrADC1old[Chan::A], &avrADC2old[Chan::A], &deltaADCold[Chan::A]);
        deltaADCPercentsOld[1] = CalculateDeltaADC(Chan::B, &avrADC1old[Chan::B], &avrADC2old[Chan::B], &deltaADCold[Chan::B]);

        AlignmentADC();

        deltaADCPercents[Chan::A] = CalculateDeltaADC(Chan::A, &avrADC1[Chan::A], &avrADC2[Chan::A], &deltaADC[Chan::A]);
        deltaADCPercents[Chan::B] = CalculateDeltaADC(Chan::B, &avrADC1[Chan::B], &avrADC2[Chan::B], &deltaADC[Chan::B]);

        gStateFPGA.stateCalibration = StateCalibration_RShift0start;                 

        koeffCal0 = koeffCal1 = ERROR_VALUE_FLOAT;

        if(Panel::WaitPressingButton() == B_Start)             // ������� ������������� ��� ������ ��������� ���������� ������� ������.
        {
            gStateFPGA.stateCalibration = StateCalibration_RShift0inProgress;

            koeffCal0 = CalculateKoeffCalibration(Chan::A);
            if(koeffCal0 == ERROR_VALUE_FLOAT)
            {
                gStateFPGA.stateCalibration = StateCalibration_ErrorCalibration0;
                Panel::WaitPressingButton();
                DEBUG_STRETCH_ADC_TYPE = StretchADC_Hand;
                PageDebug::LoadStretchADC(Chan::A);
            }
            else
            {
                STRETCH_ADC_A = koeffCal0;
                FPGA::LoadKoeffCalibration(Chan::A);
            }

            for (int range = 0; range < Range::Count; range++)
            {
                for (int mode = 0; mode < 2; mode++)
                {
                    if (!(mode == 0 && (range == Range::_2mV || range == Range::_5mV || range == Range::_10mV)))
                    {
                        ModeCouple::Set(Chan::A, (ModeCouple::E)mode);
                        RSHIFT_ADD(Chan::A, range, mode) = 0;
                        RSHIFT_ADD(Chan::A, range, mode) = CalculateAdditionRShift(Chan::A, (Range::E)range);
                    }
                }
            }
        }

        gStateFPGA.stateCalibration = StateCalibration_RShift1start;

        HAL_Delay(500);

        if(Panel::WaitPressingButton() == B_Start)                 // ������� ������������� ��� ������ ��������� ���������� ������� ������.
        {
            gStateFPGA.stateCalibration = StateCalibration_RShift1inProgress;

            koeffCal1 = CalculateKoeffCalibration(Chan::B);
            if(koeffCal1 == ERROR_VALUE_FLOAT)
            {
                gStateFPGA.stateCalibration = StateCalibration_ErrorCalibration1;
                Panel::WaitPressingButton();
                DEBUG_STRETCH_ADC_TYPE = StretchADC_Hand;
                PageDebug::LoadStretchADC(Chan::B);
            }
            else
            {
                STRETCH_ADC_B = koeffCal1;
                FPGA::LoadKoeffCalibration(Chan::B);
            }

            for (int range = 0; range < Range::Count; range++)
            {
                for (int mode = 0; mode < 2; mode++)
                {
                    if (!(mode == 0 && (range == Range::_2mV || range == Range::_5mV || range == Range::_10mV)))
                    {
                        ModeCouple::Set(Chan::B, (ModeCouple::E)mode);
                        RSHIFT_ADD(Chan::B, range, mode) = 0;
                        RSHIFT_ADD(Chan::B, range, mode) = CalculateAdditionRShift(Chan::B, (Range::E)range);
                    }
                }
            }
        }

        break;
    }

    FPGA::RestoreState();

    SET_BALANCE_ADC_A = shiftADC0;
    SET_BALANCE_ADC_B = shiftADC1;

    RShift::Set(Chan::A, SET_RSHIFT_A);
    RShift::Set(Chan::B, SET_RSHIFT_B);

    STRETCH_ADC_A = (koeffCal0 == ERROR_VALUE_FLOAT) ? koeffCalibrationOld[0] : koeffCal0;

    FPGA::LoadKoeffCalibration(Chan::A);

    STRETCH_ADC_B = (koeffCal1 == ERROR_VALUE_FLOAT) ? koeffCalibrationOld[1] : koeffCal1;
    FPGA::LoadKoeffCalibration(Chan::B);

    gStateFPGA.stateCalibration = StateCalibration_None;
    Panel::WaitPressingButton();
    Panel::Enable();
    Timer::Disable(TypeTimer::TimerDrawHandFunction);
    Display::SetDrawMode(DrawMode_Auto, 0);
    gStateFPGA.stateCalibration = StateCalibration_None;

    SET_ENABLED_A = chanAenable;
    SET_ENABLED_B = chanBenable;

    FPGA::OnPressStartStop();
}


void FPGA::FuncAttScreen()
{
    Painter::BeginScene(Color::BLACK);

    static bool first = true;
    static uint startTime = 0;
    if(first)
    {
        first = false;
        startTime = gTimerMS;
    }
    int16 y = 10;
    Display::Clear();
    
    Color::SetCurrent(COLOR_FILL);
    
#define dX 20
#define dY -15
    
    switch(gStateFPGA.stateCalibration)
    {
        case StateCalibration_None:
        {
                PText::DrawTextInRect(40 + dX, y + 25 + dY, SCREEN_WIDTH - 100, 200, "���������� ���������. ������� ����� ������, ����� ����� �� ������ ����������.");

                PText::DrawText(10 + dX, 55 + dY, "�������� ���� 1� :");
                PText::DrawText(10 + dX, 80 + dY, "�������� ���� 2� :");
                for (int i = 0; i < Range::Count; i++)
                {
                    PText::DrawFormatText(95 + i * 16 + dX, 55 + dY, COLOR_FILL, "%d", RSHIFT_ADD(Chan::A, i, 0));
                    PText::DrawFormatText(95 + i * 16 + dX, 65 + dY, COLOR_FILL, "%d", RSHIFT_ADD(Chan::A, i, 1));
                    PText::DrawFormatText(95 + i * 16 + dX, 80 + dY, COLOR_FILL, "%d", RSHIFT_ADD(Chan::B, i, 0));
                    PText::DrawFormatText(95 + i * 16 + dX, 90 + dY, COLOR_FILL, "%d", RSHIFT_ADD(Chan::B, i, 1));
                }
                
                PText::DrawFormatText(10 + dX, 110 + dY, COLOR_FILL, "����������� ���������� 1� : %f, %d", STRETCH_ADC_A, (int)(STRETCH_ADC_A * 0x80));
                PText::DrawFormatText(10 + dX, 130 + dY, COLOR_FILL, "������������ ���������� 2� : %f, %d", STRETCH_ADC_B, (int)(STRETCH_ADC_B * 0x80));

                DrawParametersChannel(Chan::A, 10 + dX, 150 + dY, false);
                DrawParametersChannel(Chan::B, 10 + dX, 200 + dY, false);
        }
            break;

        case StateCalibration_ADCinProgress:
            DrawParametersChannel(Chan::A, 5, 25, true);
            DrawParametersChannel(Chan::B, 5, 75, true);
            break;

        case StateCalibration_RShift0start:
            PText::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "���������� �� ����� ������ 1 ����� ����������� � ������� ������ ����/����. \
���� �� �� ������ ����������� ������ ������, ������� ����� ������ ������.");
            break;

        case StateCalibration_RShift0inProgress:
            break;

        case StateCalibration_RShift1start:
            PText::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "���������� �� ����� ������ 2 ����� ����������� � ������� ������ ����/����. \
���� �� �� ������ ����������� ������ �����, ������� ����� ������ ������.");
            break;

        case StateCalibration_RShift1inProgress:
            break;

        case StateCalibration_ErrorCalibration0:
            PText::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "�������� !!! ����� 1 �� �����������.");
            break;

        case StateCalibration_ErrorCalibration1:
            PText::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "�������� !!! ����� 2 �� �����������.");
            break;
    }

    /*
    if(stateFPGA.stateCalibration == kNone || stateFPGA.stateCalibration == kRShift0start || stateFPGA.stateCalibration == kRShift1start) {
        x = 230;
        y = 187;
        int delta = 32;
        width = 80;
        height = 25;
        DrawRectangle(x, y, width, height, Color::BLACK);
        DrawStringInCenterRect(x, y, width, height, "����������", Color::BLACK, false);
        DrawRectangle(x, y - delta, width, height, Color::BLACK);
        DrawStringInCenterRect(x, y - delta, width, height, "��������", Color::BLACK, false);
    }
    */
    char buffer[100];
    sprintf(buffer, "%.1f", (gTimerMS - startTime) / 1000.0f);
    PText::DrawText(0, 0, buffer, Color::BLACK);

    Painter::EndScene();
}


void FPGA::DrawParametersChannel(Chan::E ch, int eX, int eY, bool inProgress)
{
    Color::SetCurrent(COLOR_FILL);
    if(inProgress)
    {
        PText::DrawText(eX, eY + 4, (ch == Chan::A) ? "����� 1" : "����� 2");
        ProgressBar *bar = (ch == Chan::A) ? &bar0 : &bar1;
        bar->width = 240;
        bar->height = 15;
        bar->y = eY;
        bar->x = 60;
        ProgressBar_Draw(bar);
    }

    if(Settings::DebugModeEnable())
    {
        int x = inProgress ? 5 : eX;
        int y = eY + (inProgress ? 110 : 0);
        PText::DrawText(x, y, "���������� �� ����:");
        char buffer[100] = {0};
        sprintf(buffer, "���1 = %.2f/%.2f, ���2 = %.2f/%.2f, d = %.2f/%.2f", avrADC1old[ch] - ValueFPGA::AVE, avrADC1[ch] - ValueFPGA::AVE, 
                                                                             avrADC2old[ch] - ValueFPGA::AVE, avrADC2[ch] - ValueFPGA::AVE,
                                                                             deltaADCold[ch], deltaADC[ch]);
        y += 10;
        PText::DrawText(x, y, buffer);
        buffer[0] = 0;
        sprintf(buffer, "����������� A�� = %.2f/%.2f %%", deltaADCPercentsOld[ch], deltaADCPercents[ch]);
        PText::DrawText(x, y + 11, buffer);
        buffer[0] = 0;
        sprintf(buffer, "�������� %d", SET_BALANCE_ADC(ch));
        PText::DrawText(x, y + 19, buffer);
    }
}


float FPGA::CalculateDeltaADC(Chan::E ch, float *avgADC1, float *avgADC2, float *delta)
{
    uint *startTime = (ch == Chan::A) ? &startTimeChan0 : &startTimeChan1;
    *startTime = gTimerMS;
    
    ProgressBar *bar = (ch == Chan::A) ? &bar0 : &bar1;
    bar->passedTime = 0;
    bar->fullTime = 0;

    TrigSource::Set((TrigSource::E)ch);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO);

    uint8 *address1 = ch == Chan::A ? RD_ADC_A1 : RD_ADC_B1; //-V566
    uint8 *address2 = ch == Chan::A ? RD_ADC_A2 : RD_ADC_B2; //-V566

    static const int numCicles = 10;
    for(int cicle = 0; cicle < numCicles; cicle++)
    {
        HAL_FMC::Write(WR_START, 1);
        while(_GET_BIT(HAL_FMC::Read(RD_FL), 2) == 0) {};
        FPGA::SwitchingTrig();
        while(_GET_BIT(HAL_FMC::Read(RD_FL), 0) == 0) {};
        HAL_FMC::Write(WR_STOP, 1);

        for(int i = 0; i < FPGA::MAX_POINTS; i++)
        {
            if(ch == Chan::A)
            {
                *avgADC1 += o_HAL_FMC_::Read(address1);
                *avgADC2 += o_HAL_FMC_::Read(address2);
                o_HAL_FMC_::Read(RD_ADC_B1);
                o_HAL_FMC_::Read(RD_ADC_B2);
            }
            else
            {
                o_HAL_FMC_::Read(RD_ADC_A1);
                o_HAL_FMC_::Read(RD_ADC_A2);
                *avgADC1 += o_HAL_FMC_::Read(address1);
                *avgADC2 += o_HAL_FMC_::Read(address2);
            }
        }
        
        bar->passedTime = (float)(gTimerMS - *startTime);
        bar->fullTime = bar->passedTime * (float)numCicles / (cicle + 1);
    }

    *avgADC1 /= (FPGA::MAX_POINTS * numCicles);
    *avgADC2 /= (FPGA::MAX_POINTS * numCicles);

    *delta = *avgADC1 - *avgADC2;

    return ((*avgADC1) - (*avgADC2)) / 255.0f * 100;
}


void FPGA::AlignmentADC()
{
    shiftADC0 = (int8)((deltaADCold[0] > 0) ? (deltaADCold[0] + 0.5f) : (deltaADCold[0] - 0.5f));
    SET_BALANCE_ADC_A = shiftADC0;
    shiftADC1 = (int8)((deltaADCold[1] > 0) ? (deltaADCold[1] + 0.5f) : (deltaADCold[1] - 0.5f));
    SET_BALANCE_ADC_B = shiftADC1;
}


int16 FPGA::CalculateAdditionRShift(Chan::E ch, Range::E range)
{
    Range::Set(ch, range);
    RShift::Set(ch, RShift::ZERO);
    TBase::Set(TBase::_200us);
    TrigSource::Set(ch == Chan::A ? TrigSource::ChannelA : TrigSource::ChannelB);
    TrigPolarity::Set(TrigPolarity::Front);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO);

    FPGA::WriteToHardware(WR_UPR, BINARY_U8(00000000), false);   // ������������� ����� ����������� � ����

    int numMeasures = 8;
    int sum = 0;
    int numPoints = 0;

    uint time = gTimerMS;

    while(gTimerMS - time < 50) {};

    for(int i = 0; i < numMeasures; i++)
    {
        uint startTime = gTimerMS;
        const uint timeWait = 100;

        HAL_FMC::Write(WR_START, 1);
        while(_GET_BIT(HAL_FMC::Read(RD_FL), 2) == 0 && (gTimerMS - startTime < timeWait)) {}; 
        if(gTimerMS - startTime > timeWait)                 // ���� ������ ������� ����� ������� - 
        {
            return ERROR_VALUE_INT16;                       // ����� � �������.
        }

        FPGA::SwitchingTrig();

        startTime = gTimerMS;

        while(_GET_BIT(HAL_FMC::Read(RD_FL), 0) == 0 && (gTimerMS - startTime < timeWait)) {};
        if(gTimerMS - startTime > timeWait)                 // ���� ������ ������� ����� ������� - 
        {
            return ERROR_VALUE_INT16;                       // ����� � �������.
        }

        HAL_FMC::Write(WR_STOP, 1);

        uint8 *addressRead1 = ch == Chan::A ? RD_ADC_A1 : RD_ADC_B1; //-V566
        uint8 *addressRead2 = ch == Chan::A ? RD_ADC_A2 : RD_ADC_B2; //-V566

        for(int j = 0; j < FPGA::MAX_POINTS; j += 2)
        {
            sum += o_HAL_FMC_::Read(addressRead1);
            sum += o_HAL_FMC_::Read(addressRead2);
            numPoints += 2;
        }
    }

    float aveValue = (float)sum / numPoints;
    int16 retValue = (int16)(-(aveValue - ValueFPGA::AVE) * 2);

    if(retValue < - 100 || retValue > 100)
    {
        return ERROR_VALUE_INT16;
    }
    return retValue;
}


float FPGA::CalculateKoeffCalibration(Chan::E ch)
{
    FPGA::WriteToHardware(WR_UPR, BINARY_U8(00000100), false);

    RShift::Set(ch, RShift::ZERO - 40 * 4);
    ModeCouple::Set(ch, ModeCouple::DC);
    TrigSource::Set((TrigSource::E)ch);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO + 40 * 4);
    
    int numMeasures = 16;
    int sumMIN = 0;
    int numMIN = 0;
    int sumMAX = 0;
    int numMAX = 0;

    for(int i = 0; i < numMeasures; i++)
    {
        uint startTime = gTimerMS;
        const uint timeWait = 1000;

        while(gTimerMS - startTime < 20) {}
        startTime = gTimerMS;

        HAL_FMC::Write(WR_START, 1);
        while(_GET_BIT(HAL_FMC::Read(RD_FL), 2) == 0 && (gTimerMS - startTime > timeWait)) {};
        if(gTimerMS - startTime > timeWait)
        {
            return ERROR_VALUE_FLOAT;
        }

        FPGA::SwitchingTrig();
        startTime = gTimerMS;

        while(_GET_BIT(HAL_FMC::Read(RD_FL), 0) == 0 && (gTimerMS - startTime > timeWait)) {};
        if(gTimerMS - startTime > timeWait)
        {
            return ERROR_VALUE_FLOAT;
        }

        HAL_FMC::Write(WR_STOP, 1);

        uint8 *addressRead1 = ch == Chan::A ? RD_ADC_A1 : RD_ADC_B1; //-V566
        uint8 *addressRead2 = ch == Chan::A ? RD_ADC_A2 : RD_ADC_B2; //-V566

        for(int j = 0; j < FPGA::MAX_POINTS; j += 2)
        {
            uint8 val0 = o_HAL_FMC_::Read(addressRead1);
            if(val0 > ValueFPGA::AVE + 60)
            {
                numMAX++;
                sumMAX += val0;
            }
            else if(val0 < ValueFPGA::AVE - 60)
            {
                numMIN++;
                sumMIN += val0;
            }

            uint8 val1 = o_HAL_FMC_::Read(addressRead2);
            if(val1 > ValueFPGA::AVE + 60)
            {
                numMAX++;
                sumMAX += val1;
            }
            else if(val1 < ValueFPGA::AVE - 60)
            {
                numMIN++;
                sumMIN += val1;
            }
        }
    }

    float aveMin = (float)sumMIN / (float)numMIN;
    float aveMax = (float)sumMAX / (float)numMAX;

    float retValue = 160.0f / (aveMax - aveMin);

    if(retValue < 0.5f || retValue > 1.5f)
    {
        return ERROR_VALUE_FLOAT;
    }
    return retValue * 1.004f;
}