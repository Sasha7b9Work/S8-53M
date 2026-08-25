// 2022/02/11 17:45:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Display/Colors.h"
#include "Display/DisplayTypes.h"
#include "Display/font/Font.h"


namespace Painter 
{
    void BeginScene(Color::E color);

    void EndScene();

    void DrawHLine(bool send, int y, int x0, int x1, Color::E = Color::Count);

    // Если send, то передавать в интерфейсы
    void DrawVLine(bool send, int x, int y0, int y1, Color::E = Color::Count);

    void DrawVPointLine(int x, int y0, int y1, float delta, Color::E);

    void DrawHPointLineF(int y, int x0, int x1, float delta);

    void DrawHPointLineI(int x, int y, int count, int delta);

    void DrawMultiVPointLine(int numLines, const int y, const uint16 x[], int delta, int count, Color::E);

    void DrawVPointLine(int x, int y, int count, int delta);

    void DrawMultiHPointLine(int numLines, int x, const uint8 y[], int delta, int count, Color::E);

    void DrawLine(int x0, int y0, int x1, int y1, Color::E = Color::Count);

    // \brief Рисует прерывистую горизонтальную линию. dFill - длина штриха, dEmpty - расст. между штрихами.
    // Линия всегда начинается со штриха. dStart указывает смещение первой рисуемой точки относительно начала штриха.
    void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);

    //  Рисует прерывистую вертикальную линию.
    void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright,
        Color::E dark, bool isPressed, bool inShade);

    uint ReduceBrightness(uint colorValue, float newBrightness);

    // Нарисовать массив вертикальных линий. Линии рисуются одна за другой. y0y1 - массив вертикальных координат.
    // last_valid - последняя линия, которую нужно рисовать
    void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color::E, int last_valid);

    void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

    void DrawScaleLine(int x, bool forTrigLev);
};


struct Rectangle
{
    int width;
    int height;

    Rectangle(int w, int h) : width(w), height(h) {}

    void Draw(int x, int y, Color::E = Color::Count);
};


struct Region
{
    int width;
    int height;

    Region(int w, int h) : width(w), height(h) {}

    void Fill(int x, int y, Color::E = Color::Count);
};


struct Point
{
    void Set(bool send, int x, int y);
};
