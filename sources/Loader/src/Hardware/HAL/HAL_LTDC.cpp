// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include <stm32f4xx_hal.h>


namespace HAL_LTDC
{
    static void SetBuffers(uint8 *frontBuffer, uint8 *backBuffer);

    static LTDC_HandleTypeDef handleLTDC;
    static uint frontBuffer = 0;
    static uint backBuffer = 0;
}


void HAL_LTDC::Init(uint8 *front, uint8 *back)
{
    handleLTDC.Instance = LTDC;
    handleLTDC.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    handleLTDC.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    handleLTDC.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    handleLTDC.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    handleLTDC.Init.HorizontalSync = 0;
    handleLTDC.Init.VerticalSync = 0;
    handleLTDC.Init.AccumulatedHBP = 70;
    handleLTDC.Init.AccumulatedVBP = 13;
    handleLTDC.Init.AccumulatedActiveW = 390;
    handleLTDC.Init.AccumulatedActiveH = 253;
    handleLTDC.Init.TotalWidth = 408;
    handleLTDC.Init.TotalHeigh = 263;
    handleLTDC.Init.Backcolor.Blue = 0;
    handleLTDC.Init.Backcolor.Green = 0;
    handleLTDC.Init.Backcolor.Red = 0;

    if (HAL_LTDC_Init(&handleLTDC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    SetBuffers(front, back);

    LoadPalette();
}


void HAL_LTDC::LoadPalette()
{
    if (HAL_LTDC_ConfigCLUT(&handleLTDC, &COLOR(0), Color::Count, 0) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_LTDC_EnableCLUT(&handleLTDC, 0) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


void HAL_LTDC::SetBuffers(uint8 *front, uint8 *back)
{
#ifndef __linux
    frontBuffer = reinterpret_cast<uint>(front);
    backBuffer = reinterpret_cast<uint>(back);
#endif

    LTDC_LayerCfgTypeDef pLayerCfg;

    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = 320;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = 240;
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
    pLayerCfg.Alpha = 0xFF;
    pLayerCfg.Alpha0 = 0xFF;
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    pLayerCfg.FBStartAdress = frontBuffer;
    pLayerCfg.ImageWidth = 320;
    pLayerCfg.ImageHeight = 240;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;

    if (HAL_LTDC_ConfigLayer(&handleLTDC, &pLayerCfg, 0) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


void HAL_LTDC::ToggleBuffers()
{
    DMA2D_HandleTypeDef hDMA2D;

    hDMA2D.Init.Mode = DMA2D_M2M;
    hDMA2D.Init.ColorMode = LTDC_PIXEL_FORMAT_L8;
    hDMA2D.Init.OutputOffset = 0;

    hDMA2D.XferCpltCallback = nullptr;

    hDMA2D.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hDMA2D.LayerCfg[1].InputAlpha = 0xFF;
    hDMA2D.LayerCfg[1].InputColorMode = LTDC_PIXEL_FORMAT_L8;
    hDMA2D.LayerCfg[1].InputOffset = 0;

    hDMA2D.Instance = DMA2D;

    HAL_DMA2D_Init(&hDMA2D);

    HAL_DMA2D_ConfigLayer(&hDMA2D, 1);

    HAL_DMA2D_Start(&hDMA2D, backBuffer, frontBuffer, 320, 240);

    HAL_DMA2D_PollForTransfer(&hDMA2D, 100);
}
