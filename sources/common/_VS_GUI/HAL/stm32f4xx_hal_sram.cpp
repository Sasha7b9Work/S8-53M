#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram, FMC_NORSRAM_TimingTypeDef *Timing,
    FMC_NORSRAM_TimingTypeDef *ExtTiming)
{
    UNUSED(hsram);
    UNUSED(Timing);
    UNUSED(ExtTiming);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_SRAM_Read_16b(SRAM_HandleTypeDef * /*hsram*/, uint32_t * /*pAddress*/, uint16_t * /*pDstBuffer*/, uint32_t /*BufferSize*/)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_SRAM_Write_16b(SRAM_HandleTypeDef * /*hsram*/, uint32_t * /*pAddress*/, uint16_t * /*pSrcBuffer*/, uint32_t /*BufferSize*/)
{
    return HAL_OK;
}