// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


class String
{
public:
    explicit String();
    String(const String &);
    explicit String(char t);
    explicit String(pchar, ...);
    virtual ~String();

    void SetFormat(pchar format, ...);
    void SetString(const String &);
    void SetString(pchar);

    char *c_str() const { return buffer; }

    static pchar const _ERROR;

    void Free();

    void Append(pchar str);
    void Append(pchar str, int numSymbols);
    void Append(char);
    void Append(const String &);

    int Size() const;

    bool ToInt(int *out);

    char &operator[](int i) const;

    String &operator=(const String &);

    int Draw(int x, int y, Color::E = Color::Count);
    int DrawInCenterRect(int x, int y, int width, int height, Color::E = Color::Count);
    void DrawInRect(int x, int y, int width, int height, int dy = 0);
    int DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
    int DrawInRectWithTransfers(int x, int y, int width, int height, Color::E = Color::Count);
    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, Color::E colorBackground, Color::E colorFill);
    void DrawRelativelyRight(int xRight, int y, Color::E = Color::Count);
    // ������� ����� �� �������������� ����� colorBackgound
    int DrawOnBackground(int x, int y, Color::E colorBackground);
    // ���������� ������ ���������� ��������������.
    int DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill);
    // ����� ������ ������ � ������ �������(x, y, width, height)������ ColorText �� �������������� � ������� �������
    // widthBorder ����� colorBackground.
    void DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText, int widthBorder, Color::E colorBackground);

private:

    bool Allocate(int size);

    int NeedMemory(int size);

    char *buffer;

    int capacity;

    static const int _SIZE_SEGMENT = 32;

    int GetLenghtSubString(char *text);

    int DrawSubString(int x, int y, char *text);

    int DrawSpaces(int x, int y, char *text, int *numSymbols);
};


class Char
{
public:
    Char(char s) : symbol(s) {}
    int Draw(int x, int y, Color::E = Color::Count);
private:
    char symbol;
};


namespace PText
{
    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBig(int x, int y, int size, pchar text);
}
