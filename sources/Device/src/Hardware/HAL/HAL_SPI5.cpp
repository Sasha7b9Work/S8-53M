// 2022/02/16 10:14:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Panel/Panel.h"
#include <stm32f4xx_hal.h>


namespace HAL_SPI5
{
    static SPI_HandleTypeDef handleSPI5 =       // Для связи с панелью
    {
        SPI5,
        {
            SPI_MODE_SLAVE,                 // Init.Mode
            SPI_DIRECTION_2LINES,           // Init.Direction
            SPI_DATASIZE_8BIT,              // Init.DataSize
            SPI_POLARITY_HIGH,              // Init.CLKPolarity
            SPI_PHASE_2EDGE,                // Init.CLKPhase
            SPI_NSS_SOFT,                   // Init.NSS
            SPI_BAUDRATEPRESCALER_128,      // Init.BaudRatePrescaler
            SPI_FIRSTBIT_MSB,               // Init.FirstBit
            SPI_TIMODE_DISABLED,            // Init.TIMode
            SPI_CRCCALCULATION_DISABLED,    // Init.CRCCalculation
            7                               // InitCRCPolynomial
        },
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0, 0, HAL_UNLOCKED, HAL_SPI_STATE_RESET, 0
    };


    void *handle = &handleSPI5;


    static const int  SIZE_PACKET = 3;
    static uint8 buffer_in[SIZE_PACKET];                // Сюда принимаем данные
}


void HAL_SPI5::Init()
{
    __SPI5_CLK_ENABLE();

    HAL_PINS::SPI5_::Init();

    HAL_SPI_Init(&handleSPI5);

    HAL_NVIC_SetPriority(SPI5_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(SPI5_IRQn);

    HAL_SPI_Receive_IT(&handleSPI5, buffer_in, SIZE_PACKET);
}


// Для обработки ошибок
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) //-V2009
{
    using namespace HAL_SPI5;

    if (hspi == &handleSPI5)
    {
        HAL_SPI_Receive_IT(&handleSPI5, buffer_in, SIZE_PACKET);
    }
}


// Вызывается на приёме
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) //-V2009
{
    using namespace HAL_SPI5;

    if (hspi == &handleSPI5)
    {
        Panel::Callback::OnReceiveSPI5(buffer_in, SIZE_PACKET);

        HAL_SPI_Receive_IT(&handleSPI5, buffer_in, SIZE_PACKET);

        SPI5->DR = Panel::Data::Next();
    }
}

