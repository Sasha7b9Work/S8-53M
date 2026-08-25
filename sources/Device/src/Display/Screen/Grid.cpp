// 2022/02/11 17:44:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Grid.h"
#include "Settings/Settings.h"
#include "Menu/Menu.h"


namespace Grid
{
    int SignalWidth();

    // Нарисовать сетку.
    void DrawGrid(int left, int top, int width, int height);

    void DrawGridSpectrum();

    int CalculateCountV();

    int CalculateCountH();

    void DrawType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX,
        float deltaY, float stepX, float stepY);

    void DrawType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY);

    void DrawType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY,
        int stepX, int stepY);
}


int Grid::Left()
{
    return (Menu::IsMinimize() && Menu::IsShown() ? 9 : 20) + Measures::GetDeltaGridLeft();
}


int Grid::Right()
{
    return ((Menu::IsMinimize() && Menu::IsShown()) ? 9 : 20) + 280;
}


int Grid::ChannelBottom()
{
    return (SettingsDisplay::IsSeparate()) ? (GRID_TOP + GRID_HEIGHT / 2) : FullBottom();
}


int Grid::SignalWidth()
{
    return Width();
}


int Grid::ChannelHeight()
{
    return (SettingsDisplay::IsSeparate()) ? FullHeight() / 2 : FullHeight();
}


int Grid::ChannelCenterHeight()
{
    return (GRID_TOP + ChannelBottom()) / 2;
}


int Grid::FullBottom()
{
    return GRID_BOTTOM - Measures::GetDeltaGridBottom();
}


int Grid::FullHeight()
{
    return FullBottom() - GRID_TOP;
}


int Grid::Width()
{
    return Right() - Left();
}


int Grid::FullCenterHeight()
{
    return (FullBottom() + GRID_TOP) / 2;
}


int Grid::WidthInCells()
{
    return Menu::IsShown() ? 10 : 14;
}


float Grid::DeltaY()
{
    float delta = (FullBottom() - GRID_TOP) / 10.0f;

    return SettingsDisplay::IsSeparate() ? (delta / 2.0f) : delta;
}


float Grid::DeltaX()
{
    float delta = (Right() - Left()) / 14.0f;
    return delta;
}


int Grid::MathTop()
{
    return MathBottom() - MathHeight();
}


int Grid::MathHeight()
{
    if (ENABLED_FFT || MODE_DRAW_MATH_IS_SEPARATE)
    {
        return FullHeight() / 2;
    }
    return FullHeight();
}


int Grid::MathBottom()
{
    return FullBottom();
}


int Grid::BottomMessages()
{
    int retValue = FullBottom();

    if (MODE_WORK_IS_MEMINT)
    {
        retValue -= 12;
    }
    else if (SHOW_MEASURES)
    {
        retValue = Display::topMeasures;
        if (MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2 || MEAS_NUM_IS_1)
        {
            retValue = FullBottom();
        }
    }

    return retValue - 12;;
}


void Grid::Draw()
{
    if (SettingsDisplay::IsSeparate())
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight() / 2);

        if (ENABLED_FFT)
        {
            DrawGridSpectrum();
        }

        if (!DISABLED_DRAW_MATH)
        {
            DrawGrid(Grid::Left(), GRID_TOP + Grid::FullHeight() / 2, Grid::Width(), Grid::FullHeight() / 2);
        }

        Painter::DrawHLine(true, GRID_TOP + Grid::FullHeight() / 2, Grid::Left(), Grid::Left() + Grid::Width(), COLOR_FILL);
    }
    else
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight());
    }
}


void Grid::DrawGridSpectrum()
{
    if (SCALE_FFT_IS_LOG)
    {
        static const int nums[] = {4, 6, 8};
        static pchar strs[] = {"0", "-10", "-20", "-30", "-40", "-50", "-60", "-70"};
        int numParts = nums[FFT_MAX_DB];
        float scale = (float)Grid::MathHeight() / numParts;

        for (int i = 1; i < numParts; i++)
        {
            int y = (int)(Grid::MathTop() + i * scale);
            Painter::DrawHLine(true, y, Grid::Left(), Grid::Left() + 256, ColorGrid());

            if (!Menu::IsMinimize())
            {
                Color::SetCurrent(COLOR_FILL);
                String<>(strs[i]).Draw(3, y - 4);
            }
        }

        if (!Menu::IsMinimize())
        {
            Color::SetCurrent(COLOR_FILL);
            String<>(LANG_RU ? "дБ" : "dB").Draw(5, Grid::MathTop() + 1);
        }
    }
    else if (SCALE_FFT_IS_LINEAR)
    {
        static pchar strs[] = {"1.0", "0.8", "0.6", "0.4", "0.2"};
        float scale = (float)Grid::MathHeight() / 5;

        for (int i = 1; i < 5; i++)
        {
            int y = (int)(Grid::MathTop() + i * scale);
            Painter::DrawHLine(true, y, Grid::Left(), Grid::Left() + 256, ColorGrid());

            if (!Menu::IsMinimize())
            {
                String<>(strs[i]).Draw(5, y - 4, COLOR_FILL);
            }
        }
    }

    Painter::DrawVLine(true, Grid::Left() + 256, Grid::MathTop(), Grid::MathBottom(), COLOR_FILL);
}


void Grid::DrawGrid(int left, int top, int width, int height)
{
    int right = left + width;
    int bottom = top + height;

    Color::SetCurrent(COLOR_FILL);

    if (top == GRID_TOP)
    {
        Painter::DrawHLine(true, top, 1, left - 2);
        Painter::DrawHLine(true, top, right + 2, SCREEN_WIDTH - 2);

        if (!Menu::IsMinimize() || !Menu::IsShown())
        {
            Painter::DrawVLine(true, 1, top + 2, bottom - 2);
            Painter::DrawVLine(true, 318, top + 2, bottom - 2);
        }
    }

    float deltaX = Grid::DeltaX() * (float)width / width;
    float deltaY = Grid::DeltaY() * (float)height / height;
    float stepX = deltaX / 5;
    float stepY = deltaY / 5;

    int centerX = left + width / 2;
    int centerY = top + height / 2;

    Color::SetCurrent(ColorGrid());

    if (TYPE_GRID_IS_1)
    {
        DrawType1(left, top, right, bottom, (float)centerX, (float)centerY, deltaX, deltaY, stepX, stepY);
    }
    else if (TYPE_GRID_IS_2)
    {
        DrawType2(left, top, right, bottom, (int)deltaX, (int)deltaY, (int)stepX, (int)stepY);
    }
    else if (TYPE_GRID_IS_3)
    {
        DrawType3(left, top, right, bottom, centerX, centerY, (int)deltaX, (int)deltaY, (int)stepX, (int)stepY);
    }
}


void Grid::DrawType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX,
    float deltaY, float stepX, float stepY)
{
    uint16 masX[17];
    masX[0] = (uint16)(left + 1);

    for (int i = 1; i < 7; i++)
    {
        masX[i] = (uint16)(left + deltaX * i);
    }
    for (int i = 7; i < 10; i++)
    {
        masX[i] = (uint16)(centerX - 8 + i);
    }
    for (int i = 10; i < 16; i++)
    {
        masX[i] = (uint16)(centerX + deltaX * (i - 9));
    }

    masX[16] = (uint16)(right - 1);

    Painter::DrawMultiVPointLine(17, (int)(top + stepY), masX, (int)stepY, CalculateCountV(), ColorGrid());

    uint8 mas[13];
    mas[0] = (uint8)(top + 1);

    for (int i = 1; i < 5; i++)
    {
        mas[i] = (uint8)(top + deltaY * i);
    }
    for (int i = 5; i < 8; i++)
    {
        mas[i] = (uint8)(centerY - 6 + i);
    }
    for (int i = 8; i < 12; i++)
    {
        mas[i] = (uint8)(centerY + deltaY * (i - 7));
    }

    mas[12] = (uint8)(bottom - 1);

    Painter::DrawMultiHPointLine(13, (int)(left + stepX), mas, (int)stepX, CalculateCountH(), ColorGrid());
}


void Grid::DrawType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY)
{
    uint16 masX[15];
    masX[0] = (uint16)(left + 1);

    for (int i = 1; i < 14; i++)
    {
        masX[i] = (uint16)(left + deltaX * i);
    }

    masX[14] = (uint16)(right - 1);
    Painter::DrawMultiVPointLine(15, top + stepY, masX, stepY, CalculateCountV(), ColorGrid());

    uint8 mas[11];
    mas[0] = (uint8)(top + 1);

    for (int i = 1; i < 10; i++)
    {
        mas[i] = (uint8)(top + deltaY * i);
    }

    mas[10] = (uint8)(bottom - 1);
    Painter::DrawMultiHPointLine(11, left + stepX, mas, stepX, CalculateCountH(), ColorGrid());
}


void Grid::DrawType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY,
    int stepX, int stepY)
{
    Painter::DrawHPointLineF(centerY, left + stepX, right, (float)stepX);
    Painter::DrawVPointLine(centerX, top + stepY, bottom, (float)stepY, ColorGrid());

    uint8 masY[6] = {(uint8)(top + 1), (uint8)(top + 2), (uint8)(centerY - 1), (uint8)(centerY + 1),
        (uint8)(bottom - 2), (uint8)(bottom - 1)};

    Painter::DrawMultiHPointLine(6, left + deltaX, masY, deltaX, (right - top) / deltaX, ColorGrid());

    uint16 masX[6] = {(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1),
        (uint16)(right - 2), (uint16)(right - 1)};

    Painter::DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, ColorGrid());
}


int Grid::CalculateCountV()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_1_5)
        {
            return MEAS_SOURCE_IS_A_B ? 42 : 44;
        }

        if (MEAS_NUM_IS_2_5)
        {
            return MEAS_SOURCE_IS_A_B ? 69 : 39;
        }

        if (MEAS_NUM_IS_3_5)
        {
            return MEAS_SOURCE_IS_A_B ? 54 : 68;
        }
    }

    return 49;
}


int Grid::CalculateCountH()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_6_1)
        {
            return 73;
        }

        if (MEAS_NUM_IS_6_2)
        {
            return 83;
        }
    }
    return 69;
}
