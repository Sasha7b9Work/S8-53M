// 2022/02/11 15:58:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"


struct TypeFont
{
    enum E
    {
        _5,
        _8,
        UGO,
        UGO2,
        Count,
        None
    };
};


struct Symbol
{
    uchar width;
    uchar bytes[8];
};


struct Font
{
    int height;
    Symbol symbol[256];

    static const Font *font;
    static const Font *fonts[TypeFont::Count];

    static TypeFont::E current;

    static int GetSize();

    static int GetLengthText(pchar text);

    static int GetHeightSymbol(char symbol);

    static int GetLengthSymbol(uchar symbol);

    static void Set(TypeFont::E);

    static int GetSpacing();

    // Возвращает размер шрифта, которым нужно выводить сообщения в консоли.
    static int GetSizeFontForConsole();
};
