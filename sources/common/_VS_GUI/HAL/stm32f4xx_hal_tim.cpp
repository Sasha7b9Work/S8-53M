#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *, TIM_MasterConfigTypeDef *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef * /*htim*/, const TIM_MasterConfigTypeDef * /*sMasterConfig*/)
{
    return HAL_OK;
}