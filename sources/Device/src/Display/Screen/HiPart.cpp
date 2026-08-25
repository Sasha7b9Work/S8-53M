// 2022/03/23 13:03:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Screen/HiPart.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Math.h"
#include "Display/Screen/Grid.h"
#include "FPGA/StructuresFPGA.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Symbols.h"
#include <cmath>


namespace HiPart
{
    // Вывести значения курсоров курсорных измерений.
    static void WriteCursors();

    static void DrawHiRightPart();
}


void HiPart::Draw()
{
    WriteCursors();
    DrawHiRightPart();
}


void HiPart::WriteCursors()
{
    int startX = 43;

    if (MODE_WORK_IS_DIRECT)
    {
        startX += 29;
    }

    int x = startX;
    int y1 = 0;
    int y2 = 9;

    if (PageCursors::NecessaryDrawCursors())
    {
        Painter::DrawVLine(true, x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;
        Chan::E source = CURS_SOURCE;
        Color::E colorText = ColorChannel(source);

        if (!CURS_CNTRL_U_IS_DISABLE(source))
        {
            String<>("1:").Draw(x, y1, colorText);
            String<>("2:").Draw(x, y2);
            x += 7;
            PageCursors::GetCursVoltage(source, 0).Draw(x, y1);
            PageCursors::GetCursVoltage(source, 1).Draw(x, y2);
            x = startX + 49;
            float pos0 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = std::fabsf(pos1 - pos0);
            String<>(":dU=").Draw(x, y1);
            SU::Voltage2String(delta, false).Draw(x + 17, y1);
            String<>(":").Draw(x, y2);
            PageCursors::GetCursorPercentsU(source).Draw(x + 10, y2);
        }

        x = startX + 101;
        Painter::DrawVLine(true, x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;

        if (!CURS_CNTRL_T_IS_DISABLE(source))
        {
            Color::SetCurrent(colorText);
            String<>("1:").Draw(x, y1);
            String<>("2:").Draw(x, y2);
            x += 7;
            PageCursors::GetCursorTime(source, 0).Draw(x, y1);
            PageCursors::GetCursorTime(source, 1).Draw(x, y2);
            x = startX + 153;
            float pos0 = Math::TimeCursor(CURS_POS_T0(source), SET_TBASE);
            float pos1 = Math::TimeCursor(CURS_POS_T1(source), SET_TBASE);
            float delta = std::fabsf(pos1 - pos0);
            String<>(":dT=").Draw(x, y1);
            SU::Time2String(delta, false).Draw(x + 17, y1);
            String<>(":").Draw(x, y2);
            PageCursors::GetCursorPercentsT(source).Draw(x + 8, y2);

            if (CURSORS_SHOW_FREQ)
            {
                int width = 65;
                int x0 = Grid::Right() - width;
                Rectangle(width, 12).Draw(x0, GRID_TOP, COLOR_FILL);
                Region(width - 2, 10).Fill(x0 + 1, GRID_TOP + 1, COLOR_BACK);
                String<>("1/dT=").Draw(x0 + 1, GRID_TOP + 2, colorText);
                SU::Freq2String(1.0f / delta, false).Draw(x0 + 25, GRID_TOP + 2);
            }
        }
    }
}


void HiPart::DrawHiRightPart()
{
    // Синхроимпульс
    int y = 2;

    static const int xses[3] = {280, 271, 251};
    int x = xses[MODE_WORK];

    if (!MODE_WORK_IS_LATEST)
    {
        Painter::DrawVLine(true, x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;

        if (TrigLev::fireLED)
        {
            Region(GRID_TOP - 3, GRID_TOP - 7).Fill(x, 1 + y);
            String<>(LANG_RU ? "СИ" : "Tr").Draw(x + 3, 3 + y, COLOR_BACK);
        }
    }

    // Режим работы
    static pchar strings_[][2] =
    {
        {"ИЗМ",     "MEAS"},
        {"ПОСЛ",    "LAST"},
        {"ВНТР",    "INT"}
    };

    if (!MODE_WORK_IS_DIRECT)
    {
        x += 18;
        Painter::DrawVLine(true, x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 2;
        String<>(LANG_RU ? "режим" : "mode").Draw(LANG_RU ? x : x + 3, -1);
        String<>(strings_[MODE_WORK][LANG]).DrawInCenterRect(x + 1, 9, 25, 8);
    }
    else
    {
        x -= 9;
    }

    if (!MODE_WORK_IS_LATEST)
    {

        x += 27;
        Painter::DrawVLine(true, x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;
        y = 1;
        if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Work)
        {
            Text::Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
        {
            Region(10, 10).Fill(x + 3, y + 3);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Wait)
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Region(w, h).Fill(x, y + 1);
            Region(w, h).Fill(x + w + delta, y + 1);
        }
    }
}
