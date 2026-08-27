#include "defines.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_rcc.h>


__weak HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *, uint32_t)
{
    return HAL_OK;
}


void HAL_RCC_MCOConfig(uint32_t, uint32_t, uint32_t)
{

}


HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RCC_OscConfig(const RCC_OscInitTypeDef  * /*RCC_OscInitStruct*/)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef * /*RCC_ClkInitStruct*/, uint32_t /*FLatency*/)
{
    return HAL_OK;
}