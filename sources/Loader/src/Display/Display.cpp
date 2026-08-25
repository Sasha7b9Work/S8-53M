// 2021/06/30 15:45:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "main.h"
#include "Display/Painter/Animated.h"
#include "Display/Painter/Primitives.h"
#include "Display/Painter/Text.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Math.h"
#include "Utils/Containers/Buffer.h"
#include "Display/Display.h"
#include <cmath>
#include <cstring>


namespace Display
{
    static void DrawMessage(pchar message1, pchar message2 = nullptr);

    static void DrawProgressBar();

    static void DrawBigMNIPI();

    struct Vector
    {
        uint16 x;
        uint8 y;
    };


    static int numPoints = 0;
    static const int SIZE_ARRAY = 7000;
    static Vector array[SIZE_ARRAY];

    static bool running = false;

    static ABorder border1(Display::WIDTH - 4, Display::HEIGHT - 4, DirectionRotate::Right);

    static void InitPoints();
}


void Display::Update()
{
    if (running)
    {
        return;
    }

    running = true;

    static bool first = true;

    if (first)
    {
        first = false;

        InitPoints();
    }

    BeginFrame(Color::BLACK);

    Rectangle(Display::WIDTH, Display::HEIGHT).Draw(0, 0, Color::WHITE);
    border1.Draw(2, 2);
    Rectangle(Display::WIDTH - 8, Display::HEIGHT - 8).Draw(4, 4);

    if (MainStruct::state < State::EraseSectors)
    {
        Text("МНИПИ").DrawBig(30, 80, 9);
    }
    else if (MainStruct::state == State::EraseSectors)
    {
        DrawMessage("Стираю сектора...");

        DrawProgressBar();
    }
    else if (MainStruct::state == State::UpdateInProgress)
    {
        DrawMessage("Устанавливаю программное обеспечение...");

        DrawProgressBar();

        Text("%d/%d кБайт, %d кБайт/сек, осталось %d сек", MainStruct::sizeUpdated / 1024,
            MainStruct::sizeFirmware / 1024, MainStruct::speed / 1024, MainStruct::timeLeft).Draw(60, 200);
    }
    else if (MainStruct::state == State::UpdateIsFinished)
    {
        DrawMessage("Обновление завершено");
    }

    EndFrame();

    running = false;
}


void Display::DrawProgressBar()
{
    int height = 30;
    int fullWidth = 280;
    int width = (int)((float)(fullWidth)*MainStruct::percentUpdate);

    Region(width, height).Fill(20, 130);
    Rectangle(fullWidth, height).Draw(20, 130);
}


bool Display::IsRunning()
{
    return running;
}


void Display::DrawMessage(pchar message1, pchar message2)
{
    Text(message1).DrawInCenterRect(0, 0, Display::WIDTH, 100);

    if (message2)
    {
        Text(message2).DrawInCenterRect(0, 0, Display::WIDTH, 120);
    }
}


void Display::DrawBigMNIPI()
{
    static uint startTime = 0;
    static bool first = true;

    if (first)
    {
        first = false;
        startTime = TIMER_MS;
    }

    uint time = TIMER_MS - startTime;

    int numColor = (int)(time / (float)TIME_WAIT * 13.0f);
    Math::Limitation<int>(&numColor, 0, 13);

    Color((uint8)(numColor + 2)).SetAsCurrent();

    float amplitude = 3.0f - (time / (TIME_WAIT / 2.0f)) * 3;
    Math::LimitBelow(&amplitude, 0.0f);
    float frequency = 0.05f;

    float radius = 5000.0f * (TIME_WAIT) / 3000.0f / time;
    Math::LimitBelow(&radius, 0.0f);

    float shift[240];

    for (int i = 0; i < 240; i++)
    {
        shift[i] = amplitude * std::sin(frequency * time + i / 5.0f);
    }

    for (int i = 0; i < numPoints; i++)
    {
        int x = (int)(array[i].x + shift[array[i].y]); //-V537
        int y = array[i].y;
        if (x > 0 && x < 319 && y > 0 && y < 239)
        {
            Point().Draw(x, y);
        }
    }
}


static void Display::InitPoints()
{
    Buffer<uint8> buffer(320 * 240);

    Text::DrawBigTextInBuffer(31, 70, 9, "МНИПИ", buffer.Data());

    for (int x = 0; x < 320; x++)
    {
        for (int y = 0; y < 240; y++)
        {
            if (*(buffer.Data() + y * 320 + x))
            {
                array[numPoints].x = (uint16)x;
                array[numPoints].y = (uint8)y;
                numPoints++;
            }
        }
    }
}
