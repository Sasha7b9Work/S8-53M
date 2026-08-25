// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "main.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


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

    void MemManage_Handler()
    {
        while (1)
        {
        }
    }

    void BusFault_Handler()
    {
        while (1)
        {
        }
    }

    void UsageFault_Handler()
    {
        while (1)
        {
        }
    }

    void SVC_Handler()
    {
    }

    void DebugMon_Handler()
    {
    }

    void PendSV_Handler()
    {
    }

    void SysTick_Handler()
    {
        HAL_IncTick();
    }
    
    void TIM6_DAC_IRQHandler()
    {
       HAL_TIM6::CallbackOnIRQ();
    }
    
    void OTG_HS_IRQHandler(void)
    {
        if (MainStruct::state == State::NoDrive)
        {
            MainStruct::state = State::DriveDetected;
        }

        HAL_HCD_IRQHandler((HCD_HandleTypeDef *)HAL_HCD::handle);
    }

#ifdef __cplusplus
}
#endif
