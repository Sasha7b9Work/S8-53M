// 2022/02/11 15:58:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "Colors.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Utils/Math.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cmath>


uint Color::Make(uint8 r, uint8 g, uint8 b)
{
    return ((uint)(b + (g << 8) + (r << 16)));
}


void Color::ResetFlash()
{
    Timer::Enable(TypeTimer::FlashDisplay, 400, OnTimerFlashDisplay);
    inverse = false;
}


Color::E ColorChannel(Chan ch)
{
    static const Color::E colors[4] = {Color::DATA_A, Color::DATA_B, Color::WHITE, Color::WHITE};
    return colors[ch];
}


Color::E ColorCursors(Chan ch)
{
    static const Color::E colors[4] = {Color::DATA_A, Color::DATA_B, Color::WHITE, Color::WHITE};
    return colors[ch];
}


Color::E ColorTrig()
{
    TrigSource::E trigChan = TRIG_SOURCE;

    if (trigChan == TrigSource::ChannelA || trigChan == TrigSource::ChannelB)
    {
        return ColorChannel((Chan::E)trigChan);
    }

    return COLOR_FILL;
}


Color::E ColorMenuTitle(bool inShade)
{
    return inShade ? Color::MENU_ITEM : Color::MENU_TITLE;
}


Color::E ColorMenuItem(bool inShade)
{
    return inShade ? Color::MENU_ITEM_DARK : Color::MENU_ITEM;
}


Color::E ColorBorderMenu(bool inShade)
{
    return ColorMenuTitle(inShade);
}


Color::E LightShadingTextColor()
{
    return ColorMenuTitle(false);
}


void Color::Log(Color::E color)
{
    uint colorValue = COLOR(color);

    LOG_WRITE("%s   r=%d, g=%d, b=%d", NameColor(color), R_FROM_COLOR(colorValue), G_FROM_COLOR(colorValue), B_FROM_COLOR(colorValue));
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
    stepRed = red / (brightness * 100.0f);
    stepGreen = green / (brightness * 100.0f);
    stepBlue = blue / (brightness * 100.0f);
}



void ColorType::SetBrightness()
{
    brightness = Math::Max<float>(red / 255.0f, green / 255.0f, blue / 255.0f);

    CalcSteps();
}



void ColorType::SetBrightness(float bright)
{
    int delta = (int)((bright + 0.0005f) * 100.0f) - (int)(brightness * 100.0f);

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
    if ((delta > 0 && brightness == 1.0f) || (delta < 0 && brightness == 0.0f))
    {
        return;
    }

    int sign = Math::Sign(delta);

    LIMITATION(brightness, brightness + (float)sign * 0.01f, 0.0f, 1.0f);

    red += (float)sign * stepRed;
    green += (float)sign * stepGreen;
    blue += (float)sign * stepBlue;

    COLOR(color) = Color::Make((uint8)red, (uint8)green, (uint8)blue);

    if (stepRed < 0.01f && stepGreen < 0.01f && stepBlue < 0.01f)
    {
        stepRed = 2.55f;
        stepGreen = 2.55f;
        stepBlue = 2.55f;
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

        if (red == 0.0f && green == 0.0f && blue == 0.0f)
        {
            stepRed = 2.55f;
            stepGreen = 2.55f;
            stepBlue = 2.55f;
        }
    }
}



void ColorType::ComponentChange(int delta)
{
    static const float maxs[4] = {0.0f, 255.0f, 255.0f, 255.0f};
    int8 index = currentField;

    if (index >= 1 && index <= 3)
    {
        float *pointers[4] = {0, &blue, &green, &red};
        Math::AddLimitation<float>(pointers[index], (float)Math::Sign(delta), 0.0f, maxs[index]);
    }

    COLOR(color) = Color::Make((uint8)red, (uint8)green, (uint8)blue);

    SetBrightness();
}



static pchar  colorNames[] =
{
    "Color::BLACK",
    "Color::WHITE",
    "Color::GRID",
    "Color::DATA_A",
    "Color::DATA_B",
    "Color::MENU_FIELD",
    "Color::MENU_TITLE",
    "COLOM_MENU_TITLE_DARK",
    "Color::MENU_TITLE_BRIGHT",
    "Color::MENU_ITEM",
    "Color::MENU_ITEM_DARK",
    "Color::MENU_ITEM_BRIGHT",
    "Color::MENU_SHADOW",
    "Color::Count",
    "Color::FLASH_10",
    "Color::FLASH_01",
    "SET_INVERSE"
};



pchar  NameColorFromValue(uint16 colorValue)
{
    for (int i = 0; i < Color::Count; i++)
    {
        if (COLOR(i) == colorValue)
        {
            return colorNames[i];
        }
    }
    return "Sorry, this color is not in the palette";
}



pchar  NameColor(Color::E color)
{
    return colorNames[color];
}
