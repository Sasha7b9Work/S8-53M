// 2022/02/11 17:44:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/font/Font.h"
#include "Log.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/VCP/VCP.h"
#include "Utils/Math.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/InterCom.h"
#include "Display/Screen/Grid.h"
#include "Utils/Text/Warnings.h"


bool     Color::inverse = false;
Color::E Color::current = Color::Count;


namespace Display
{
    static const int SIZE_BUFFER = WIDTH * HEIGHT;
    uint8 back[240][320];                                           // Здесь будем рисовать изображение

    uint8 *const back_buffer = &back[0][0];
    uint8 *const back_buffer_end = back_buffer + SIZE_BUFFER;      // Конец буфера отрисовки
}


namespace Painter
{
    bool noFonts = false;

    void BoundingX(int &x) { if (x < 0) x = 0; if (x >= Display::WIDTH) x = Display::WIDTH - 1; }
    void BoundingY(int &y) { if (y < 0) y = 0; if (y >= Display::HEIGHT) y = Display::HEIGHT - 1; }

    Color::E GetColor(int x, int y);
}


void Color::CalculateColor()
{
    if (current == Color::FLASH_10)
    {
        current = inverse ? COLOR_BACK : COLOR_FILL;
    }
    else if (current == Color::FLASH_01)
    {
        current = inverse ? COLOR_FILL : COLOR_BACK;
    }
}


void Color::OnTimerFlashDisplay()
{
    inverse = !inverse;
}


void Rectangle::Draw(int x, int y, Color::E color)
{
    Color::SetCurrent(color);

    Painter::DrawHLine(true, y, x, x + width);
    Painter::DrawVLine(true, x, y, y + height);
    Painter::DrawHLine(true, y + height, x, x + width);

    if (x + width < SCREEN_WIDTH)
    {
        Painter::DrawVLine(true, x + width, y, y + height);
    }
}


void Painter::DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // Если линию нужно рисовать не с начала штриха
    {
        y += (deltaFill + deltaEmtpy - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            DrawVLine(false, x, y0, y - 1);
        }
    }

    while (y < y1)
    {
        DrawVLine(false, x, y, y + deltaFill - 1);
        y += (deltaFill + deltaEmtpy);
    }
}


void Painter::DrawDashedHLine(int y, int x0, int x1, int deltaFill, int deltaEmpty, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmpty))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // Если линию нужно рисовать не с начала штриха
    {
        x += (deltaFill + deltaEmpty - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            DrawHLine(false, y, x0, x - 1);
        }
    }

    while (x < x1)
    {
        DrawHLine(false, y, x, x + deltaFill - 1);
        x += (deltaFill + deltaEmpty);
    }
}


void Color::SetCurrent(Color::E color)
{
    if (color != current && color != Color::Count)
    {
        current = color;

        if (current > Color::Count)
        {
            CalculateColor();
        }

        if (InterCom::TransmitGUIinProcess())
        {
            CommandBuffer<2> command(SET_COLOR);
            command.PushByte(color);
            command.Transmit();
        }
    }
}


Color::E Color::GetCurrent()
{
    return current;
}


void Painter::DrawVLine(bool send, int x, int y0, int y1, Color::E color)
{
    Color::SetCurrent(color);

    if (x < 0 || x >= Display::WIDTH)
    {
        return;
    }

    BoundingY(y0);
    BoundingY(y1);

    Math::Sort(&y0, &y1);

    uint8 *address = Display::back_buffer + Display::WIDTH * y0 + x;

    int counter = y1 - y0 + 1;

    uint8 value = (uint8)Color::GetCurrent();

    do
    {
        *address = value;
        address += Display::WIDTH;

    } while (--counter > 0);

    if (send && InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<5> command(DRAW_VLINE);
        command.PushHalfWord(x);
        command.PushByte(y0);
        command.PushByte(y1);
        command.Transmit();
    }
}


void Painter::DrawVPointLine(int x, int y0, int y1, float delta, Color::E color)
{
    Color::SetCurrent(color);

    for (int y = y0; y <= y1; y += (int)delta)
    {
        Point().Set(false, x, y);
    }
}


void Painter::DrawHPointLineF(int y, int x0, int x1, float delta)
{
    for (int x = x0; x <= x1; x += (int)delta)
    {
        Point().Set(false, x, y);
    }
}


void Painter::DrawHPointLineI(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        Point().Set(false, x, y);
        x += delta;
    }
}


void Point::Set(bool send, int x, int y)
{
    if (x < 0 || y < 0 || (x > Display::WIDTH - 1) || (y > Display::HEIGHT - 1))
    {
        return;
    }

    uint8 *address = Display::back_buffer + Display::WIDTH * y + x;

    if (address < Display::back_buffer_end)
    {
        *address = (uint8)Color::GetCurrent();
    }

    if (send && InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<4> command(SET_POINT);
        command.PushHalfWord(x);
        command.PushByte(y);
        command.Transmit();
    }
}


Color::E Painter::GetColor(int x, int y)
{
    if (x < 0 || y < 0 || (x > Display::WIDTH - 1) || (y > Display::HEIGHT - 1))
    {
        return COLOR_BACK;
    }

    uint8 *address = Display::back_buffer + Display::WIDTH * y + x;

    if (address < Display::back_buffer_end)
    {
        return (Color::E)*address;
    }

    return COLOR_BACK;
}


void Painter::DrawMultiVPointLine(int numLines, const int y, const uint16 x[], int delta, int count, Color::E color)
{
    numLines = Math::Limitation<int>(numLines, 0, 20);

    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        DrawVPointLine(x[i], y, count, delta);
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<128> command(DRAW_MULTI_VPOINT_LINES);
        command.PushByte(numLines);
        command.PushByte(y);
        command.PushByte(count);
        command.PushByte(delta);
        command.PushByte(0);
        for (int i = 0; i < numLines; i++)
        {
            command.PushHalfWord(x[i]);
        }

        command.Transmit(1 + 1 + 1 + 1 + 1 + 1 + numLines * 2);
    }
}


void Painter::DrawVPointLine(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        Point().Set(false, x, y);
        y += delta;
    }
}


void Painter::DrawMultiHPointLine(int numLines, int x, const uint8 y[], int delta, int count, Color::E color)
{
    numLines = Math::Limitation<int>(numLines, 0, 20);

    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        DrawHPointLineI(x, y[i], count, delta);
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<128> command(DRAW_MULTI_HPOINT_LINES);
        command.PushByte(numLines);
        command.PushHalfWord(x);
        command.PushByte(count);
        command.PushByte(delta);
        for (int i = 0; i < numLines; i++)
        {
            command.PushByte(y[i]);
        }

        command.Transmit(1 + 1 + 2 + 1 + 1 + numLines);
    }
}


void Painter::DrawLine(int x0, int y0, int x1, int y1, Color::E color)
{
    Color::SetCurrent(color);

    if (x0 == x1)
    {
        DrawVLine(true, x0, y0, y1);
    }
    else if (y0 == y1)
    {
        DrawHLine(true, y0, x0, x1);
    }
}


void Painter::DrawHLine(bool send, int y, int x0, int x1, Color::E color)
{
    Color::SetCurrent(color);

    if (y < 0 || y >= Display::HEIGHT)
    {
        return;
    }

    Painter::BoundingX(x0);
    Painter::BoundingX(x1);

    if (x0 > x1)
    {
        Math::Swap(&x0, &x1);
    }

    uint8 *start = Display::back_buffer + y * Display::WIDTH + x0;

    std::memset(start, Color::GetCurrent(), (uint)(x1 - x0 + 1));

    if (send && InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<6> command(DRAW_HLINE);
        command.PushByte(y);
        command.PushHalfWord(x0);
        command.PushHalfWord(x1);
        command.Transmit();
    }
}


void Region::Fill(int _x, int _y, Color::E color)
{
    Color::SetCurrent(color);

    if (width == 0 || height == 0)
    {
        return;
    }

    for (int i = _y; i <= _y + height; i++)
    {
        if (i < 0 || i >= Display::HEIGHT)
        {
            continue;
        }

        int x0 = _x;
        int x1 = _x + width;

        Painter::BoundingX(x0);
        Painter::BoundingX(x1);

        if (x0 > x1)
        {
            Math::Swap(&x0, &x1);
        }

        uint8 *start = Display::back_buffer + i * Display::WIDTH + x0;

        std::memset(start, Color::GetCurrent(), (uint)(x1 - x0 + 1));
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<7> command(FILL_REGION);
        command.PushHalfWord(_x);
        command.PushByte(_y);
        command.PushHalfWord(width);
        command.PushByte(height);
        command.Transmit();
    }
}


void Painter::DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright,
    Color::E dark, bool isPressed, bool inShade)
{
    if (inShade)
    {
        thickness = 1;
    }

    Region(width - thickness * 2, height - thickness * 2).Fill(x + thickness, y + thickness, normal);

    if (isPressed || inShade)
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLine(true, y + i, x + i, x + width - i, dark);
            DrawVLine(true, x + i, y + 1 + i, y + height - i);
            DrawVLine(true, x + width - i, y + 1 + i, y + height - i, bright);
            DrawHLine(true, y + height - i, x + 1 + i, x + width - i);
        }
    }
    else
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLine(true, y + i, x + i, x + width - i, bright);
            DrawVLine(true, x + i, y + 1 + i, y + height - i);
            DrawVLine(true, x + width - i, y + 1 + i, y + height - i, dark);
            DrawHLine(true, y + height - i, x + 1 + i, x + width - i);
        }
    }
}


void Painter::DrawVLineArray(int x, int num_lines, uint8 *y0y1, Color::E color, int last_valid)
{
    Color::SetCurrent(color);

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<1024> command(DRAW_VLINES_ARRAY);
        command.PushHalfWord(x);
        command.PushHalfWord(num_lines);

        for (int i = 0; i < num_lines; i++)
        {
            command.PushByte(*(y0y1 + i * 2));
            command.PushByte(*(y0y1 + i * 2 + 1));
        }

        command.Transmit(1 + 2 + 2 + 2 * num_lines);
    }

    for (int i = 0; (i < num_lines) && (i < last_valid); i++)
    {
        int y0 = *y0y1++;
        int y1 = *y0y1++;

        DrawVLine(false, x++, y0, y1);
    }
}


void Painter::BeginScene(Color::E color)
{
    Region(319, 239).Fill(0, 0, color);
}


void Painter::EndScene()
{
    if (Display::framesElapsed != 1)
    {
        Display::framesElapsed = 1;
        return;
    }

#ifdef DEBUG

    String<>("Отладка").Draw(262, 207, COLOR_FILL);

#endif

    HAL_LTDC::ToggleBuffers();

    InterCom::EndScene();

    InterCom::BeginScene();
}


uint Painter::ReduceBrightness(uint colorValue, float newBrightness)
{
    int red = (int)(R_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(red, red, 0, 255);

    int green = (int)(G_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(green, green, 0, 255);

    int blue = (int)(B_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(blue, blue, 0, 255);

    return Color::Make((uint8)red, (uint8)green, (uint8)blue);
}


void Painter::DrawScaleLine(int x, bool forTrigLev)
{
    if (ALT_MARKERS_HIDE)
    {
        return;
    }
    int width = 6;
    int topY = GRID_TOP + Display::DELTA;
    int x2 = width + x + 2;
    int bottomY = Grid::ChannelBottom() - Display::DELTA;
    int centerY = (Grid::ChannelBottom() + GRID_TOP) / 2;
    int levels[] =
    {
        topY,
        bottomY,
        centerY,
        centerY - (bottomY - topY) / (forTrigLev ? 8 : 4),
        centerY + (bottomY - topY) / (forTrigLev ? 8 : 4)
    };
    for (int i = 0; i < 5; i++)
    {
        Painter::DrawLine(x + 1, levels[i], x2 - 1, levels[i], COLOR_FILL);
    }
}
