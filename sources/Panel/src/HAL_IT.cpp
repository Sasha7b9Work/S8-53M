// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "HAL.h"
#include <stm32l0xx_hal.h>


#ifdef __cplusplus
extern "C" {
#endif


void NMI_Handler()
{
}


void HardFault_Handler()
{
    while (1)
    {
    }
}


void SVC_Handler()
{

}


void PendSV_Handler()
{

}


void SysTick_Handler()
{
    HAL_IncTick();
}


#ifdef __cplusplus
}
#endif
