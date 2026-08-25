// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <usbh_def.h>
#include <stm32f4xx_hal.h>
#include "redefine_hal.h"


namespace HAL
{
    static void EnablePeripherals();
    static void ConfigSystemClock();
}


static HCD_HandleTypeDef handleHCD;
void *HAL_HCD::handle = &handleHCD;

static PCD_HandleTypeDef handlePCD;
void *HAL_PCD::handle = &handlePCD;


static const uint ADDR_BANK = 0x60000000;
static const uint ADDR_RAM = ADDR_BANK + 0x04000000;

#ifndef GUI

// Адрес записи в аппаратные регистры. 0x100000 - это смещение нужно для установки A19 на FPGA в 1. Хотя по расчёту
// должно быть 0x80000 (потому что a18 работает на 0x40000)
uint16 *const HAL_FMC::ADDR_FPGA = (uint16 *)(ADDR_BANK + 0x00000000 + 0x100000); //-V566

uint8 *const HAL_FMC::ADDR_RAM_BEGIN = (uint8 *)(ADDR_RAM); //-V566

uint8 *const HAL_FMC::ADDR_RAM_END = (uint8 *)(ADDR_RAM + 1024 * 1024 - 320 * 240); //-V566

uint16 *const HAL_FMC::ADDR_RAM_DISPLAY_FRONT = (uint16 *)(ADDR_RAM + 1024 * 1024 - 320 * 240); //-V566

#endif


void HAL::Init()
{
    HAL_Init();

    ConfigSystemClock();

    EnablePeripherals();
    
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    HAL_PINS::Init();

    HAL_DAC1::Init();

    HAL_TIM2::Init();
    
    HAL_TIM6::Init();
    
#ifdef DEVICE

    HAL_SPI1::Init();

    HAL_SPI5::Init();

    HAL_ADC1::Init();

    HAL_TIM7::Init();

#endif

    HAL_FMC::Init();

    HAL_ROM::Init();
   

#ifdef DEVICE

    HAL_RTC::Init();
    
#endif
}


void HAL_MspInit()
{
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* SVCall_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void HAL::EnablePeripherals()
{
    __SYSCFG_CLK_ENABLE();

    __PWR_CLK_ENABLE();

    __HAL_RCC_RTC_ENABLE();

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();
    __GPIOI_CLK_ENABLE();

    __ADC1_CLK_ENABLE();

    __CRC_CLK_ENABLE();

    __DAC_CLK_ENABLE();         // Для бикалки 
    __DMA1_CLK_ENABLE();        // Для DAC1 (бикалка)
    __DMA2D_CLK_ENABLE();

    __ETH_CLK_ENABLE();

    __FMC_CLK_ENABLE();

    __LTDC_CLK_ENABLE();

    __PWR_CLK_ENABLE();

    __SPI1_CLK_ENABLE();
    __SPI5_CLK_ENABLE();        // Свзяь с панелью
    __SYSCFG_CLK_ENABLE();

    __TIM2_CLK_ENABLE();        // Для тиков
    __TIM6_CLK_ENABLE();        // Для отсчёта миллисекунд
    __TIM7_CLK_ENABLE();        // Для DAC1 (бикалка)

    __USB_OTG_FS_CLK_ENABLE();
    __USB_OTG_HS_CLK_ENABLE();
}


void HAL::ConfigSystemClock()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 15;
    RCC_OscInitStruct.PLL.PLLN = 144;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


void HAL_ETH::Init()
{
    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}


void HAL::SystemReset()
{
    HAL_NVIC_SystemReset();
}


void HAL::DeInit()
{
    HAL_DeInit();
}


void HAL::JumpToApplication()
{
#define MAIN_PROGRAM_START_ADDRESS  (uint)0x08020000

    typedef void(*pFunction)();

    pFunction JumpToApplication;

    __disable_irq();
    // Теперь переходим на основную программу
    JumpToApplication = (pFunction)(*(__IO uint *)(MAIN_PROGRAM_START_ADDRESS + 4)); //-V566
    __set_MSP(*(__IO uint *)MAIN_PROGRAM_START_ADDRESS); //-V566
    __enable_irq();
    JumpToApplication();
}


void HAL::ErrorHandler(pchar file, int line)
{
    UNUSED(file);
    UNUSED(line);

    while (true)
    {
    }
}
