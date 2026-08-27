#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef *, uint32_t, uint32_t, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_DeInit(DAC_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef *, uint32_t, uint32_t *, uint32_t, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_Stop_DMA(DAC_HandleTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef *, DAC_ChannelConfTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef * /*hdac*/, uint32_t /*Channel*/, const uint32_t * /*pData*/, uint32_t /*Length*/, uint32_t /*Alignment*/)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef * /*hdac*/, const DAC_ChannelConfTypeDef * /*sConfig*/, uint32_t /*Channel*/)
{
    return HAL_OK;
}
