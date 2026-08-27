#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef *hltdc)
{
    UNUSED(hltdc);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{
    UNUSED(hltdc);
    UNUSED(pLayerCfg);
    UNUSED(LayerIdx);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, uint32_t *pCLUT, uint32_t CLUTSize, uint32_t LayerIdx)
{
    UNUSED(hltdc);
    UNUSED(pCLUT);
    UNUSED(CLUTSize);
    UNUSED(LayerIdx);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
    UNUSED(hltdc);
    UNUSED(LayerIdx);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef * /*hltdc*/, const uint32_t * /*pCLUT*/, uint32_t /*CLUTSize*/, uint32_t /*LayerIdx*/)
{
    return HAL_OK;
}
