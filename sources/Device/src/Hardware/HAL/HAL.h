// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "FPGA/SettingsFPGA.h"
#include "Data/Data.h"


/*
                         |      Было                  |                        Стало                                    |

    АЦП рандомизатора    |                            | ADC1 ; ADC_CHANNEL_0 ; EXTI4_IRQHandler ; in - PA0 ; it - PH4   |
    Яркость дисплея      |                            | DAC1 ; DAC1_CHANNEL_1 ; out - PA4                               |
    Звук                 | DAC1 ; DMA1                | DAC2 ; DMA_CHANNEL_7 ; DAC_CHANNEL_2 ; DMA1_Stream6 ; out - PA5 |
    Таймер               | TIM6 ; TIM6_DAC_IRQHandler |                                                                 |
    Панель               |                            |                                                                 |
    USB                  |                            |                                                                 |
    Flash                |                            |                                                                 |
    Микросекунды         | TIM2                       |                                                                 |

    OTP                  |                            |                                                                 |
    RTC                  |                            |                                                                 |
    ROM                  |                            |                                                                 |
    LTDC                 |                            |                                                                 |
    LAN                  |                            |                                                                 |
    ШИНА                 |                            |                                                                 |

                         |                            |                                                                 |
*/


void ERROR_HANDLER();


namespace HAL
{
    void Init();
    uint CalculateCRC32();
}


// АЦП рандомизатора
namespace HAL_ADC1
{
    static const uint16 INVALID_VALUE = 0xFFFF;

    void Init();

    // Если 0xFFFF - значение неправильное
    uint16 GetValue();

    // Вызывается после каждого запуска
    void ResetValue();

    bool CallbackOnIRQ();
};


// Используется для управления яркостью дисплея
namespace HAL_DAC1
{
    void Init();

    // Значение value от 0 до 100
    void SetBrightness(int value);
};


// Динамик
namespace HAL_DAC2
{
    void Init();
    void StartDMA(uint8 *points, int numPoints);
    void StopDMA();
};


// Используется для связи с FPGA и RAM
namespace HAL_FMC
{
    void Init();

    inline uint16 Read(volatile const uint16 *const address) { return *address; };

    void Write(uint16 *const address, uint16 value);

    static const uint RAM_SIZE = 1024 * 1024;

    extern uint16 *const ADDR_FPGA;                 // Адрес записи в альтеру

    extern uint8 *const ADDR_RAM_BEGIN;             // Первый свободный адрес в RAM.
    extern uint8 *const ADDR_RAM_DATA;              // Здесь хранятся данные. Размер (1МБ - 75кБ)
    extern uint8 *const ADDR_RAM_DATA_END;          // Конец области хранения данных
    extern uint8 *const ADDR_RAM_DISPLAY_FRONT;     // Видимое изображение дисплея. Находится в RAM и не может исполь-
                                                    // зоваться для других целей. Размер 75кБ
};


namespace HAL_LTDC
{
    void Init(uint8 *front_buffer, uint8 *back_buffer);

    void LoadPalette();

    void ToggleBuffers();
};


// Для связи с панелью
namespace HAL_SPI5
{
    void Init();

    extern void *handle;       // SPI_HandleTypeDef
};


// Таймер для тиков
namespace HAL_TIM2
{
    void Init();
}


// Таймер миллисекунд
namespace HAL_TIM6
{
    void Init();

    bool CallbackOnIRQ();
}


// Вспомогательный таймер для DAC2
namespace HAL_TIM7
{
    void Init();

    void Config(uint16 prescaler, uint16 period);
};


namespace HAL_RTC
{
    void Init();
    bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);
    PackedTime GetPackedTime();
}


// Используется для флешки
namespace HAL_HCD
{
    void Init();
    extern void *handle;
}


// Использвуется для VCP
namespace HAL_PCD
{
    extern void *handle;
};


namespace HAL_USBH
{
    extern void *handle;
}


namespace HAL_CRC32
{
    void Init();

    uint Calculate(uint *first_word, int num_words);
}


#ifdef __cplusplus
extern "C" {
#endif

void HardFault_Handler();

#ifdef __cplusplus
}
#endif
