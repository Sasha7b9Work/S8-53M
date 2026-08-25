// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Font/Font.h"
#include "Display/Painter/Primitives.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Math.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#ifdef DEVICE
    #include "Menu/Pages/Definition.h"
#endif
#include <cstdlib>
#include <cstring>


namespace Display
{
    static const int SIZE_BUFFER = Display::WIDTH * Display::HEIGHT;

    static uint8 *front = (uint8 *)HAL_FMC::ADDR_RAM_DISPLAY_FRONT;
    static uint8 back[240][320];

    uint8 *display_back_buffer = &back[0][0];
    uint8 *display_back_buffer_end = display_back_buffer + SIZE_BUFFER;

}


void Display::Init()
{
    Color::ResetFlash();

    HAL_LTDC::Init(front, &back[0][0]);

    BeginFrame(Color::BLACK);

    EndFrame();

#ifdef DEVICE

    PageDisplay::PageSettings::LoadBrightness();

#else

    HAL_DAC1::SetBrightness(100);

#endif
}


void Display::BeginFrame(const Color &color)
{
    std::memset(back, color.index, SIZE_BUFFER);
}


void Display::EndFrame()
{
#ifdef DEBUG
    Text(LANG_RU ? "Отладка" : "Debug").Draw(260, 21, Color::FILL);
#endif

    HAL_LTDC::ToggleBuffers();
}


void HLine::Draw(int y, int x0, int x1) const
{
    if (y < 0 || y >= Display::HEIGHT)
    {
        return;
    }

    BoundingX(x0);
    BoundingX(x1);

    if (x0 > x1)
    {
        Math::Swap(&x0, &x1);
    }

    uint8 *start = &Display::back[y][x0];

    std::memset(start, Color::Get().ValueForDraw(), (uint)(x1 - x0 + 1));
}


void HLine::Draw(int x, int y) const
{
    if (y < 0 || y >= Display::HEIGHT)
    {
        return;
    }

    int w = width;  

    if (x < 0)
    {
        w += x;
        x = 0;
    }
    else if (x >= Display::WIDTH)
    {
        w -= (x - Display::WIDTH);
        x = Display::WIDTH - 1;
    }

    uint8 *address = &Display::back[y][x];

    std::memset(address, Color::Get().index, (uint)w);
}


#ifdef DEVICE

void Display::ReadRow(int row, uint8 pixels[320])
{
    std::memcpy(pixels, &back[row][0], 320);
}


uint8 *Display::GetBufferEnd()
{
    return GetBuffer() + SIZE_BUFFER;
}


uint8 *Display::GetBuffer()
{
    return &back[0][0];
}

#endif
