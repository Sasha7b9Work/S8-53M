// 2021/06/20 20:57:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once

/*
    Переопределения макросов для GUI-версии
*/

#ifdef WIN32

#undef __SYSCFG_CLK_ENABLE
#define __SYSCFG_CLK_ENABLE()
#undef __HAL_RCC_PWR_CLK_ENABLE
#define __HAL_RCC_PWR_CLK_ENABLE()
#undef __HAL_PWR_VOLTAGESCALING_CONFIG
#define __HAL_PWR_VOLTAGESCALING_CONFIG(x)
#undef __HAL_RCC_RTC_ENABLE
#define __HAL_RCC_RTC_ENABLE()
#undef __GPIOA_CLK_ENABLE
#define __GPIOA_CLK_ENABLE()
#undef __GPIOB_CLK_ENABLE
#define __GPIOB_CLK_ENABLE()
#undef __GPIOC_CLK_ENABLE
#define __GPIOC_CLK_ENABLE()
#undef __GPIOD_CLK_ENABLE
#undef __GPIOE_CLK_ENABLE
#undef __GPIOF_CLK_ENABLE
#undef __GPIOG_CLK_ENABLE
#undef __GPIOH_CLK_ENABLE
#undef __GPIOI_CLK_ENABLE
#define __GPIOD_CLK_ENABLE()
#define __GPIOE_CLK_ENABLE()
#define __GPIOF_CLK_ENABLE()
#define __GPIOG_CLK_ENABLE()
#define __GPIOH_CLK_ENABLE()
#define __GPIOI_CLK_ENABLE()
#undef __ADC1_CLK_ENABLE
#define __ADC1_CLK_ENABLE()
#undef __CRC_CLK_ENABLE
#define __CRC_CLK_ENABLE()
#undef __DAC_CLK_ENABLE
#define __DAC_CLK_ENABLE()
#undef __DMA1_CLK_ENABLE
#define __DMA1_CLK_ENABLE()
#undef __DMA2D_CLK_ENABLE
#define __DMA2D_CLK_ENABLE()
#undef __ETH_CLK_ENABLE
#define __ETH_CLK_ENABLE()
#undef __FMC_CLK_ENABLE
#undef __LTDC_CLK_ENABLE
#undef __PWR_CLK_ENABLE
#undef __SPI1_CLK_ENABLE
#undef __SPI5_CLK_ENABLE
#undef __SYSCFG_CLK_ENABLE
#undef __TIM2_CLK_ENABLE
#undef __TIM6_CLK_ENABLE
#undef __TIM7_CLK_ENABLE
#undef __USB_OTG_FS_CLK_ENABLE
#undef __USB_OTG_HS_CLK_ENABLE
#define __USB_OTG_HS_CLK_ENABLE()
#define __FMC_CLK_ENABLE()
#define __LTDC_CLK_ENABLE()
#define __PWR_CLK_ENABLE()
#define __SPI1_CLK_ENABLE()
#define __SPI5_CLK_ENABLE()
#define __SYSCFG_CLK_ENABLE()
#define __TIM2_CLK_ENABLE()
#define __TIM6_CLK_ENABLE()
#define __TIM7_CLK_ENABLE()
#define __USB_OTG_FS_CLK_ENABLE()
#define __USB_OTG_HS_CLK_ENABLE()
#undef __SPI5_CLK_ENABLE
#define __SPI5_CLK_ENABLE()
#undef __TIM7_CLK_ENABLE
#define __TIM7_CLK_ENABLE()
#undef __HAL_RCC_RTC_ENABLE
#define __HAL_RCC_RTC_ENABLE()
#undef __HAL_RCC_RTC_ENABLE
#define __HAL_RCC_RTC_ENABLE()
#undef __DAC_CLK_ENABLE
#define __DAC_CLK_ENABLE()

#endif
