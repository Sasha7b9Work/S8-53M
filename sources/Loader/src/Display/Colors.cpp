// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter/Text.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"


const Color Color::BLACK(0);
Color Color::WHITE(1);
Color Color::GRID(2);
Color Color::DATA_A(3);
Color Color::DATA_B(4);
Color Color::MENU_FIELD(5);
Color Color::MENU_TITLE(6);
Color Color::MENU_TITLE_DARK(7);
Color Color::MENU_TITLE_BRIGHT(8);
Color Color::MENU_ITEM(9);
Color Color::MENU_ITEM_DARK(10);
Color Color::MENU_ITEM_BRIGHT(11);
Color Color::MENU_SHADOW(12);
Color Color::EMPTY(13);
Color Color::EMPTY_A(14);
Color Color::EMPTY_B(15);

Color Color::BLACK_WHITE_0(Color::BLACK, Color::WHITE);
Color Color::BLACK_WHITE_1 = Color::BLACK_WHITE_0.GetInverse();

Color Color::current = Color::WHITE;
uint Color::timeResetFlash = 0;

Color Color::FILL(Color::WHITE.index);
Color Color::BACK(Color::BLACK.index);


void ColorType::Set()
{
    COLOR(color) = Color::Make((uint8)red, (uint8)green, (uint8)blue);

    HAL_LTDC::LoadPalette();
}


Color Chan::GetColor() const
{
    static const Color colors[4] = { Color::DATA_A, Color::DATA_B, Color::WHITE, Color::WHITE };
    return colors[value];
}


Color Color::Cursors(const Chan &ch)
{
    static const Color colors[4] = { Color::DATA_A, Color::DATA_B, Color::WHITE, Color::WHITE };
    return colors[ch];
}


Color Color::Contrast(const Color &color)
{
    uint colorValue = COLOR(color.index);
    if (R_FROM_COLOR(colorValue) > 128 || G_FROM_COLOR(colorValue) > 128 || B_FROM_COLOR(colorValue) > 128)
    {
        return Color::BLACK;
    }
    return Color::WHITE;
}


/*
    јлгоритм изменени€ €ркости.
    1. »нициализаци€.
    а. –ассчитать €ркость по принципу - €ркость равна относительной интенсивности самого €ркого цветового канала
    б. –ассчитать шаг изменени€ цветовой составл€ющей каждого канала на 1% €ркости по формуле:
    Ўаг = »нтенсивность канала * яркость, где яркость - относительна€ величина общей €ркости
    в. ≈сли интенсивность всех каналов == 0, то дать полный шаг каждому каналу
    2. ѕри изменнении €ркости на 1% мен€ть интенсивность каждого канала на Ўаг, расчитанный в предыдущем пункте.
    3. ѕри изменени€ интенсивности цветового канала пересчитывать €ркость и шаг изменени€ каждого канала.
*/
void ColorType::CalcSteps()
{
    stepRed = red / (brightness * 100.0F);
    stepGreen = green / (brightness * 100.0F);
    stepBlue = blue / (brightness * 100.0F);
}


void ColorType::SetBrightness()
{
    brightness = Math::Max(red / 255.0F, green / 255.0F, blue / 255.0F);

    CalcSteps();
}


void ColorType::SetBrightness(float br)
{
    int delta = (int)((br + 0.0005F) * 100.0F) - (int)(brightness * 100.0F);

    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
        {
            BrightnessChange(1);
        }
    }
    else
    {
        for (int i = 0; i < -delta; i++)
        {
            BrightnessChange(-1);
        }
    }
}


void ColorType::BrightnessChange(int delta)
{
    if ((delta > 0 && brightness == 1.0F) || (delta < 0 && brightness == 0.0F)) //-V550
    {
        return;
    }

    int sign = Math::Sign(delta);

    brightness = Math::Limitation(brightness + sign * 0.01F, 0.0F, 1.0F);

    red += sign * stepRed;
    green += sign * stepGreen;
    blue += sign * stepBlue;

    Set();

    if (stepRed < 0.01F && stepGreen < 0.01F && stepBlue < 0.01F)
    {
        stepRed = 2.55F;
        stepGreen = 2.55F;
        stepBlue = 2.55F;
    }
}


void ColorType::Init()
{
    if (!alreadyUsed)
    {
        alreadyUsed = true;                  // ѕризнак того, что начальные установки уже произведены

        uint colorValue = COLOR(color);

        red = (float)R_FROM_COLOR(colorValue);
        green = (float)G_FROM_COLOR(colorValue);
        blue = (float)B_FROM_COLOR(colorValue);

        SetBrightness();

        if (red == 0.0F && green == 0.0F && blue == 0.0F) //-V550
        {
            stepRed = 2.55F;
            stepGreen = 2.55F;
            stepBlue = 2.55F;
        }
    }
}


void ColorType::ComponentChange(int delta)
{
    static const float maxs[4] = {0.0F, 255.0F, 255.0F, 255.0F};
    int8 index = currentField;

    if (index >= 1 && index <= 3)
    {
        float *pointers[4] = { 0, &blue, &green, &red };

        *pointers[index] = Math::AddWithLimitation(*pointers[index], (float)Math::Sign(delta), 0.0F, maxs[index]);
    }

    Set();

    SetBrightness();
}


void Color::ResetFlash()
{
    timeResetFlash = HAL_TIM::TimeMS();
}


uint8 Color::ValueForDraw() const
{
    if (index < Count)
    {
        return index;
    }

    return ((HAL_TIM::TimeMS() - timeResetFlash) % 1000) < 500 ? index1 : index2;
}


Color Color::GetInverse() const
{
    if (index < Count)
    {
        return Color((uint8)(Count - index));
    }

    return Color(Color(index2), Color(index1));
}


uint Color::Make(uint8 r, uint8 g, uint8 b)
{
    return ((uint)(b + (g << 8) + (r << 16)));
}
