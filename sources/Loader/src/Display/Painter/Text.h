// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Display/Colors.h"
#include "Display/Font/Font.h"
#include "Utils/Containers/String.h"


class Char
{
public:

    Char(char s) : symbol(s) {}

    int Draw(int x, int y, const Color &color = Color::Count) const;

    int Draw(int x, int y, TypeFont::E typeFont, const Color &color = Color::Count) const;

    void Draw2SymbolsInPosition(int x, int y, char symbol2, const Color &color1, const Color &color2);

    void Draw4SymbolsInRect(int x, int y, const Color &color = Color::Count);

    void Draw4SymbolsInRect(int x, int y, TypeFont::E typeFont, const Color &color = Color::Count);

    void Draw10SymbolsInRect(int x, int y);

    void Draw10SymbolsInRect(int x, int y, TypeFont::E typeFont);

private:
    char symbol;
};


class Text : public String
{
public:

    explicit Text() : String() {}

    explicit Text(const char symbol);

    explicit Text(pchar format, ...);

    virtual ~Text();

    int Draw(int x, int y, const Color &color) const;
    int Draw(int x, int y, TypeFont::E typeFont, const Color &color) const;
    int Draw(int x, int y) const;
    int Draw(int x, int y, TypeFont::E typeFont) const;

    void DrawInRect(int x, int y, uint width, uint height);

    void DrawBig(int x, int y, uint size);

    int DrawInCenterRect(int x, int y, int width, int height, const Color &color = Color::Count);

    void DrawRelativelyRight(int xRight, int y, const Color &color = Color::Count);

    // Выводит текст на прямоугольнике цвета colorBackgound
    int DrawOnBackground(int x, int y, const Color &colorBackground);

    int DrawWithLimitation(int x, int y, const Color &color, int limitX, int limitY, int limitWidth, int limitHeight);

    // Возвращает нижнюю координату прямоугольника.
    int DrawInBoundedRectWithTransfers(int x, int y, int width, const Color &colorBackground, const Color &colorFill);

    int DrawInRectWithTransfers(int x, int y, int width, int height, const Color &color = Color::Count);

    // Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра
    // widthBorder цвета colorBackground.
    void DrawInCenterRectOnBackground(int x, int y, int width, int height, const Color &colorText, int widthBorder,
        const Color &colorBackground);

    int DrawInCenterRectAndBoundIt(int x, int y, int width, int height, const Color &colorBackground,
        const Color &colorFill);

    static void DrawBigTextInBuffer(int eX, int eY, int size, pchar text, uint8 *buffer /* [320][240] */);

private:

    int DrawChar(int x, int y, uint8 symbol) const;
};


class EmptyText : public Text
{
public:
    EmptyText() : Text("--.--") {}
};
