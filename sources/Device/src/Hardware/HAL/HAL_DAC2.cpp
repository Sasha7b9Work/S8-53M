// 2022/02/21 09:15:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_DAC2
{
    DAC_HandleTypeDef handleDAC = {DAC};
}


void HAL_DAC2::Init()
{
    __DAC_CLK_ENABLE();
    __DMA1_CLK_ENABLE();

    HAL_PINS::DAC2_::Init();

    static DMA_HandleTypeDef hdmaDAC2 =
    {
        DMA1_Stream6,
        {
            DMA_CHANNEL_7,
            DMA_MEMORY_TO_PERIPH,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_CIRCULAR,
            DMA_PRIORITY_HIGH,
            DMA_FIFOMODE_DISABLE,
            DMA_FIFO_THRESHOLD_HALFFULL,
            DMA_MBURST_SINGLE,
            DMA_PBURST_SINGLE
        }
    };

    if (HAL_DMA_Init(&hdmaDAC2) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    __HAL_LINKDMA(&handleDAC, DMA_Handle2, hdmaDAC2);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    DAC_ChannelConfTypeDef config =
    {
        DAC_TRIGGER_T7_TRGO,
        DAC_OUTPUTBUFFER_ENABLE
    };

    HAL_DAC_DeInit(&handleDAC);

    if (HAL_DAC_Init(&handleDAC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_DAC_ConfigChannel(&handleDAC, &config, DAC_CHANNEL_2) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


void HAL_DAC2::StartDMA(uint8 *points, int numPoints)
{
    if (HAL_DAC_Start_DMA(&handleDAC, DAC_CHANNEL_2, (uint32_t *)points, (uint)numPoints, DAC_ALIGN_8B_R) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


void HAL_DAC2::StopDMA()
{
    if (HAL_DAC_Stop_DMA(&handleDAC, DAC_CHANNEL_2) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}
