// 2022/2/12 10:16:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_DAC1
{
    DAC_HandleTypeDef handle = {DAC};


    // imi - входной минимум, oma - выходной максимум
    uint CalculateValue(double imi, double omi, double ima, double oma, int value)
    {
        double b = (omi - oma * imi / ima) / (1.0 - imi / ima);

        double a = (oma - b) / ima;

        return (uint)(value * a + b);
    }
}

void HAL_DAC1::Init()
{
#ifndef GUI

    RCC->APB1ENR |= RCC_APB1ENR_DACEN;      // Включаем ЦАП

    HAL_PINS::DAC1_::Init();

    if (HAL_DAC_Init(&handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);

    if (HAL_DAC_Init(&handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    DAC_ChannelConfTypeDef configDAC;
    configDAC.DAC_Trigger = DAC_TRIGGER_NONE;
    configDAC.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

    if (HAL_DAC_ConfigChannel(&handle, &configDAC, DAC1_CHANNEL_1) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    HAL_DAC_Start(&handle, DAC1_CHANNEL_1);

    SetBrightness(0);

#endif
}


void HAL_DAC1::SetBrightness(int brightness)
{
    /*
    *  На вход подаётся значение от 0 до 100, на выходе значения от 255 до 4095
    *  OUT = a * IN + b
    *  OUT = 38.4 * IN + 255
    *
    *  38.4 = (4095 - 255) / 100
    */

    static const uint imi[5] = {51,         26,   13,  7,   0};
    static const uint ima[5] = {100,        50,   25,  12,  6};

    static const uint omi[5] = {1024,       256,  64,  16,  0};
    static const uint oma[5] = {4095 - 255, 1023, 255, 63,  15};

    int i = 0;

    if (brightness > 50) { i = 0; }
    else if (brightness > 25) { i = 1; }
    else if (brightness > 12) { i = 2; }
    else if (brightness > 6) { i = 3; }
    else { i = 4; }

    uint value = CalculateValue(imi[i], (double)omi[i] + 255, ima[i], (double)oma[i] + 255, brightness);

    HAL_DAC_SetValue(&handle, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, value);
}
