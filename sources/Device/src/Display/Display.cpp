// 2022/02/11 17:43:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Menu.h"
#include "Display/Screen/Grid.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Display/DataPainter.h"
#include "Display/Screen/HiPart.h"
#include "Display/Screen/Console.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/VCP/VCP.h"
#include "Data/Storage.h"
#include "Display/Screen/LowPart.h"
#include "Utils/Text/Warnings.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Symbols.h"
#include <cstdio>


namespace Display
{
    pFuncVV funcOnHand       = nullptr;

    pFuncVV funcAfterDraw    = nullptr;

    bool framesElapsed = false;

    int topMeasures = GRID_BOTTOM;

    void DrawCursorsWindow();

    // Ќарисовать курсоры курсорных измерений.
    void DrawCursors();

    // ¬ывести значени€ автоматических измерений.
    void DrawMeasures();

    void FuncOnTimerDisableShowLevelRShiftA();

    void FuncOnTimerDisableShowLevelRShiftB();

    void DrawStringNavigation();

    bool NeedForClearScreen();

    void DrawTimeForFrame();

    // Ќарисовать горизонтальный курсор курсорных измерений.
    // y - числовое значение курсора.
    // xTearing - координата места, в котором необходимо сделать разрыв дл€ квадрата пересечени€.
    void DrawHorizontalCursor(int y, int xTearing);

    // Ќарисовать вертикальный курсор курсорных измерений.
    // x - числовое значение курсора.
    // yTearing - координата места, в котором необходимо сделать разрыв дл€ квадрата пересечени€.
    void DrawVerticalCursor(int x, int yTearing);

    // ¬ывести значение уровн€ синхронизации. 
    void WriteValueTrigLevel();
}


void Display::Init() 
{
    HAL_DAC1::Init();

    Color::ResetFlash();

    HAL_LTDC::Init(HAL_FMC::ADDR_RAM_DISPLAY_FRONT, Display::back_buffer);

    Painter::BeginScene(Color::BLACK);

    Painter::EndScene();

#ifdef DEVICE

    HAL_DAC1::SetBrightness(BRIGHTNESS);

#else

    HAL_DAC1::SetBrightness(100);

#endif
}



void Display::DrawStringNavigation() 
{
    String<> string = Menu::StringNavigation();

    if(string.Size()) 
    {
        int length = Font::GetLengthText(string.c_str());
        int height = 10;
        Rectangle(length + 2, height).Draw(Grid::Left(), GRID_TOP, COLOR_FILL);
        Region(length, height - 2).Fill(Grid::Left() + 1, GRID_TOP + 1, COLOR_BACK);
        string.Draw(Grid::Left() + 2, GRID_TOP + 1, COLOR_FILL);
    }
}


void Display::RotateRShift(Chan ch)
{
    LAST_AFFECTED_CHANNEL = ch;

    if(TIME_SHOW_LEVELS)
    {
        RShift::showLevel[ch] = true;

        Timer::Enable((ch == Chan::A) ?
            TypeTimer::ShowLevelRShiftA :
            TypeTimer::ShowLevelRShiftB, TIME_SHOW_LEVELS  * 1000,
            (ch == Chan::A) ?
            FuncOnTimerDisableShowLevelRShiftA :
            FuncOnTimerDisableShowLevelRShiftB);
    };

    Flags::needFinishDraw = true;
};


void Display::FuncOnTimerDisableShowLevelRShiftA()
{
    RShift::DisableShowLevel(ChA);
}


void Display::FuncOnTimerDisableShowLevelRShiftB()
{
    RShift::DisableShowLevel(ChB);
}


void Display::RotateTrigLev()
{
    if (TIME_SHOW_LEVELS)
    {
        TrigLev::showLevel = true;
        Timer::Enable(TypeTimer::ShowLevelTrigLev, TIME_SHOW_LEVELS * 1000, TrigLev::DisableShowLevel);
    }

    Flags::needFinishDraw = true;
}


bool Display::NeedForClearScreen()
{
    if (TBase::InModeRandomizer() || (NUM_ACCUM == 1) || SET_SELFRECORDER)
    {
        return true;
    }

    if (Flags::needFinishDraw)
    {
        Flags::needFinishDraw = false;
        ENumAccumulation::number_drawing = 0;
        return true;
    }

    if (ENUM_ACCUM_IS_INFINITY)
    {
        return false;
    }

    if (MODE_ACCUM_IS_NORESET || ENumAccumulation::number_drawing > NUM_ACCUM)
    {
        ENumAccumulation::number_drawing = 0;
        return true;
    }

    return false;
}


void Display::Update()
{
    if (funcOnHand != 0)
    {
        funcOnHand();
        return;
    }

    bool needClear = NeedForClearScreen();

    if (Processing::_out.Data(ChA).IsEmpty())
    {
        int i = 0;
    }

    if (needClear)
    {
        Painter::BeginScene(COLOR_BACK);

        DataPainter::MemoryWindow::Draw();

        Grid::Draw();
    }

    DataPainter::DrawData();

    if (needClear)
    {
        DataPainter::DrawMath();
        DataPainter::Spectrum::Draw();
        DrawCursors();
        HiPart::Draw();
        LowPart::Draw();
        DrawCursorsWindow();
        TShift::Draw();
        TrigLev::Draw();
        RShift::Draw();
        DrawMeasures();
        DrawStringNavigation();
    }

    Menu::Draw();

    if (needClear)
    {
        Warning::DrawWarnings();
    }

    Console::Draw();

    if (needClear)    
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame();

    Color::SetCurrent(COLOR_FILL);

    Painter::EndScene();

    FDrive::SaveSignal();

    if (funcAfterDraw)
    {
        funcAfterDraw();
        funcAfterDraw = 0;
    }
}


void Display::WriteValueTrigLevel()
{
    if (TrigLev::showLevel && MODE_WORK_IS_DIRECT)
    {
        float trig_lev = 0.0f;

        if (TRIG_SOURCE == TrigSource::Ext)
        {
            trig_lev = -5.0f + (float)(TRIG_LEVEL_SOURCE - TrigLev::MIN) / (float)(TrigLev::MAX - TrigLev::MIN) * 10.0f;
        }
        else
        {
            trig_lev = TRIG_LEVEL_SOURCE.ToAbs(SET_RANGE(TRIG_SOURCE));
        }

        TrigSource::E trigSource = TRIG_SOURCE;

        if (TRIG_INPUT_IS_AC && trigSource <= TrigSource::ChannelB)
        {
            RShift rshift = SET_RSHIFT((Chan::E)trigSource);
            float rShiftAbs = rshift.ToAbs(SET_RANGE(trigSource));
            trig_lev += rShiftAbs;
        }

        int width = 96;
        int x = (Grid::Width() - width) / 2 + Grid::Left();
        int y = Grid::BottomMessages() - 20;
        Rectangle(width, 10).Draw(x, y, COLOR_FILL);
        Region(width - 2, 8).Fill(x + 1, y + 1, COLOR_BACK);

        String<> string(LANG_RU ? "”р синхр = " : "Trig lvl = ");
        string.Append(SU::Voltage2String(trig_lev, true));
        string.Draw(x + 2, y + 1, COLOR_FILL);
    }
}


void Display::DrawCursorsWindow()
{
    if((!Menu::IsMinimize() || !Menu::IsShown()) && RShift::drawMarkers)
    {
        Painter::DrawScaleLine(2, false);
    }
}


void Display::DrawHorizontalCursor(int y, int xTearing)
{
    y += GRID_TOP;
    if(xTearing == -1)
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, Grid::Right() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, xTearing - 2, 1, 1, 0);
        Painter::DrawDashedHLine(y, xTearing + 2, Grid::Right() - 1, 1, 1, 0);
    }
    Rectangle(2, 2).Draw(Grid::Left() - 1, y - 1);
    Rectangle(2, 2).Draw(Grid::Right() - 1, y - 1);
}


void Display::DrawVerticalCursor(int x, int yTearing)
{
    x += Grid::Left();
    if(yTearing == -1)
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, yTearing - 2, 1, 1, 0);
        Painter::DrawDashedVLine(x, yTearing + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }

    Rectangle(2, 2).Draw(x - 1, GRID_TOP - 1);
    Rectangle(2, 2).Draw(x - 1, Grid::ChannelBottom() - 1);
}


void Display::DrawCursors()
{
    Chan::E source = CURS_SOURCE;
    Color::SetCurrent(ColorCursors(source));

    if (PageCursors::NecessaryDrawCursors())
    {
        bool bothCursors = !CURS_CNTRL_T_IS_DISABLE(source) && !CURS_CNTRL_U_IS_DISABLE(source);  // ѕризнак того, что включены и вертикальные и 
                                                                            // горизонтальные курсоры - надо нарисовать квадраты в местах пересечени€
        int x0 = -1;
        int x1 = -1;
        int y0 = -1;
        int y1 = -1;

        if (bothCursors)
        {
            x0 = (int)(Grid::Left() + CURS_POS_T0(source));
            x1 = (int)(Grid::Left() + CURS_POS_T1(source));
            y0 = (int)(GRID_TOP + PageCursors::GetCursPosU(source, 0));
            y1 = (int)(GRID_TOP + PageCursors::GetCursPosU(source, 1));

            Rectangle(4, 4).Draw(x0 - 2, y0 - 2);
            Rectangle(4, 4).Draw(x1 - 2, y1 - 2);
        }

        CursCntrl::E cntrl = CURS_CNTRL_T(source);

        if (cntrl != CursCntrl::Disable)
        {
            DrawVerticalCursor((int)CURS_POS_T0(source), y0);
            DrawVerticalCursor((int)CURS_POS_T1(source), y1);
        }

        cntrl = CURsU_CNTRL;

        if (cntrl != CursCntrl::Disable)
        {
            DrawHorizontalCursor((int)PageCursors::GetCursPosU(source, 0), x0);
            DrawHorizontalCursor((int)PageCursors::GetCursPosU(source, 1), x1);
        }
    }
}


void Display::DrawMeasures()
{
    if(!SHOW_MEASURES)
    {
        topMeasures = GRID_BOTTOM;
        return;
    }

    if (!SET_ENABLED_A && !SET_ENABLED_B)
    {
        return;
    }

    Processing::CalculateMeasures();

    if(MEAS_FIELD_IS_HAND)
    {
        int x0 = MEAS_POS_CUR_T0 - SHIFT_IN_MEMORY + Grid::Left();
        int y0 = MEAS_POS_CUR_U0 + GRID_TOP;
        int x1 = MEAS_POS_CUR_T1 - SHIFT_IN_MEMORY + Grid::Left();
        int y1 = MEAS_POS_CUR_U1 + GRID_TOP;
        Math::Sort(&x0, &x1);
        Math::Sort(&y0, &y1);
        Rectangle(x1 - x0, y1 - y0).Draw(x0, y0, COLOR_FILL);
    }

    int x0 = Grid::Left() - Measures::GetDeltaGridLeft();
    int dX = Measures::GetDX();
    int dY = Measures::GetDY();
    int y0 = Measures::GetTopTable();

    int numRows = Measures::NumRows();
    int numCols = Measures::NumCols();

    for(int str = 0; str < numRows; str++)
    {
        for(int elem = 0; elem < numCols; elem++)
        {
            int x = x0 + dX * elem;
            int y = y0 + str * dY;
            bool active = Measures::IsActive(str, elem) && (Page::NameOpened() == NamePage::SB_MeasTuneMeas);
            Color::E color = active ? COLOR_BACK : COLOR_FILL;
            Measure::E meas = Measures::Type(str, elem);

            if(meas != Measure::None)
            {
                Region(dX, dY).Fill(x, y, COLOR_BACK);
                Rectangle(dX, dY).Draw(x, y, COLOR_FILL);
                topMeasures = Math::MinFrom2Int(topMeasures, y);
            }

            if(active)
            {
                Region(dX - 4, dY - 4).Fill(x + 2, y + 2, COLOR_FILL);
            }

            if(meas != Measure::None)
            {
                String<>(Measures::Name(str, elem)).Draw(x + 4, y + 2, color);

                if(meas == MEAS_MARKED)
                {
                    Region(dX - 2, 9).Fill(x + 1, y + 1, active ? COLOR_BACK : COLOR_FILL);

                    String<>(Measures::Name(str, elem)).Draw(x + 4, y + 2, active ? COLOR_FILL : COLOR_BACK);
                }

                int meas_x = x + 2;
                int meas_y = y + 11;

                bool need_a = (MEAS_SOURCE_IS_A || MEAS_SOURCE_IS_A_B) && SET_ENABLED_A;
                bool need_b = (MEAS_SOURCE_IS_B || MEAS_SOURCE_IS_A_B) && SET_ENABLED_B;

                if (need_a)
                {
                    Processing::GetStringMeasure(meas, ChA).Draw(meas_x, meas_y, ColorChannel(ChA));
                }
                
                if (need_b)
                {
                    if (need_a)
                    {
                        meas_y += 9;
                    }

                    Processing::GetStringMeasure(meas, ChB).Draw(meas_x, meas_y, ColorChannel(ChB));
                }
            }
        }
    }

    if(Page::NameOpened() == NamePage::SB_MeasTuneMeas)
    {
        Measures::DrawPageChoice();
    }
}


void Display::DrawTimeForFrame()
{
    if(!SHOW_STATS)
    {
        return;
    }

    static int fps;
    static float duration;  // ƒлительность одной итарции главного цикла

    static TimeMeterMS meter;
    static int num_frames = 0;
    num_frames++;

    if (meter.ElapsedTime() > 1000)
    {
        fps = num_frames;
        duration = (float)meter.ElapsedTime() / fps;
        meter.Reset();
        num_frames = 0;
    }


    Rectangle(84, 10).Draw(Grid::Left(), Grid::FullBottom() - 10, COLOR_FILL);
    Region(82, 8).Fill(Grid::Left() + 1, Grid::FullBottom() - 9, COLOR_BACK);
    String<>("%.1fms/%d", duration, fps).Draw(Grid::Left() + 2, Grid::FullBottom() - 9, COLOR_FILL);

    String<>("%d/%d", Storage::SameSettings::GetCount(), Storage::NumberAvailableEntries()).
        Draw(Grid::Left() + 50, Grid::FullBottom() - 9);
}


void RShift::DisableShowLevel(Chan ch)
{
    showLevel[ch] = false;
    Timer::Disable(ch.IsA() ? TypeTimer::ShowLevelRShiftA : TypeTimer::ShowLevelRShiftB);
}


void TrigLev::DisableShowLevel()
{
    TrigLev::showLevel = false;
    Timer::Disable(TypeTimer::ShowLevelTrigLev);
}


void Display::SwitchTrigLabel(bool enable)
{
    TrigLev::fireLED = enable;
}


void Display::SetDrawMode(DrawMode::E mode, pFuncVV func)
{
    if (mode == DrawMode::Normal)
    {
        funcOnHand = nullptr;
        Timer::Disable(TypeTimer::UpdateDisplay);
    }
    else if (mode == DrawMode::Timer)
    {
        funcOnHand = EmptyFuncVV;
        Timer::Enable(TypeTimer::UpdateDisplay, 40, func);
    }
    else
    {
        funcOnHand = func;
        Timer::Disable(TypeTimer::UpdateDisplay);
    }
}


void Display::Clear()
{
    Region(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2).Fill(0, 0, COLOR_BACK);
}


void Display::ShiftScreen(int delta)
{
    LIMITATION(SHIFT_IN_MEMORY, (int16)(SHIFT_IN_MEMORY + delta), 0, (int16)(ENUM_POINTS_FPGA::ToNumPoints() - 282));
}


void Display::RunAfterDraw(pFuncVV func)
{
    funcAfterDraw = func;
}
