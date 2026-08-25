// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"


struct Chan;


// Тип цвета
struct Color
{
    static const Color BLACK;
    static Color WHITE;
    static Color GRID;
    static Color DATA_A;
    static Color DATA_B;
    static Color MENU_FIELD;
    static Color MENU_TITLE;
    static Color MENU_TITLE_DARK;       // Чуть менее светлый цвет, чем цвет заголовка страницы. Используется для
                                        // создания эффекта объёма.
    static Color MENU_TITLE_BRIGHT;     // Чуть более светлый цвет, чем цвет заголовка страницы. Используется для
                                        // создания эффекта объёма.
    static Color MENU_ITEM;
    static Color MENU_ITEM_DARK;        // Чуть менее светлый цвет, чем цвет элемента меню. Используется для создания
                                        // эффекта объёма.
    static Color MENU_ITEM_BRIGHT;      // Чуть более светлый цвет, чем цвет элемента меню. Используется для создания
                                        // эффекта объёма
    static Color MENU_SHADOW;
    static Color EMPTY;
    static Color EMPTY_A;
    static Color EMPTY_B;

    static const uint8 Count = 16;

    static Color FILL;
    static Color BACK;

    static Color BLACK_WHITE_0;
    static Color BLACK_WHITE_1;

    uint8 index;        // Здесь хранится индекс цвета, если не нужно мигание

    uint8 index1;       // \ Здесь находятся первый и второй цвета в случае мигающего цвета.
    uint8 index2;       // / В "индекс" в этом случае хранится значение больше Count.

    Color(uint8 i) : index(i), index1(0), index2(0) {}

    Color(const Color &c1, const Color &c2) : index(Count + 1), index1(c1.index), index2(c2.index) {}

    static void ResetFlash();

    void SetAsCurrent() const;

    static Color Cursors(const Chan &ch);

    // Возвращает текущий цвет (им сейчас производится рисование)
	static Color Get() { return Color(current); };

    // Возвращает цвет, которым нужно производить рисование
    uint8 ValueForDraw() const;

    Color GetInverse() const;

    static Color Trig();

    // Цвет окантовки меню.
    static Color BorderMenu()       { return Color::MENU_TITLE; }

    // Светлый цвет в тени.
    static Color LightShadingText() { return BorderMenu(); }

    // Возвращает цвет, контрастный к color. Может быть белым или чёрным.
    static Color Contrast(const Color &color);

    static uint Make(uint8 r, uint8 g, uint8 b);

private:

    static Color current;

    static uint timeResetFlash; // Время, в которое "сборошено" мерцание. Нужно для того, чтобы избавиться от
                                // "проскоков" при переключении мецающих элементов
};


struct ColorType
{
    float   red;
    float   green;
    float   blue;
    float   stepRed;
    float   stepGreen;
    float   stepBlue;
    float   brightness;
    uint8   color;
    int8    currentField;
    bool    alreadyUsed;

    void Init();
    void BrightnessChange(int delta);
    void SetBrightness(float brightness);
    void ComponentChange(int delta);

private:
    void SetBrightness();
    void Set();
    void CalcSteps();
};


#define R_FROM_COLOR(color) (((uint)(color) >> 16) & (uint)0xFF)
#define G_FROM_COLOR(color) (((uint)(color) >> 8) & (uint)0xFF)
#define B_FROM_COLOR(color) ((uint)(color) & 0xFF)
#define COLOR(i) set.display.colors[i]
