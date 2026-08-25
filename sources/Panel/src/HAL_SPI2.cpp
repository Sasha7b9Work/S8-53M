// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "HAL.h"
#include <stm32l0xx_hal.h>


extern Pin pinSPI2_NSS;


namespace HAL_SPI2
{
    static SPI_HandleTypeDef handleSPI2 =
    {
        SPI2,
        {
            SPI_MODE_MASTER,
            SPI_DIRECTION_2LINES,
            SPI_DATASIZE_8BIT,
            SPI_POLARITY_HIGH,
            SPI_PHASE_2EDGE,
            SPI_NSS_SOFT,                   // Для мастера этот параметр ни на что не влияет
            SPI_BAUDRATEPRESCALER_128,       // Период CLK - 1 мкс, длительность импульса - 0.5 мкс
            SPI_FIRSTBIT_MSB,
            SPI_TIMODE_DISABLED,
            SPI_CRCCALCULATION_DISABLED,
            7
        },
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, HAL_UNLOCKED, HAL_SPI_STATE_RESET, 0
    };

    static uint time_last_transmit = 0;
}


void HAL_SPI2::Init()
{
    HAL_PINS::InitSPI2();

    HAL_SPI_Init(&handleSPI2);
}


bool HAL_SPI2::TransmitReceivce(uint8 *buffer_out, uint8 *buffer_in, uint size)
{
    pinSPI2_NSS.Off();

    bool result = (HAL_SPI_TransmitReceive(&handleSPI2, buffer_out, buffer_in, (uint16)size, 100) == HAL_OK);

    time_last_transmit = TIME_MS;

    pinSPI2_NSS.On();

    return (result == HAL_OK);
}


uint HAL_SPI2::TimeAfterTransmit()
{
    return (TIME_MS - time_last_transmit);
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) //-V2009
{
    if (hspi == &HAL_SPI2::handleSPI2)
    {
        volatile int i = 0;
        i++;
    }
}


void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *) //-V2009
{

}
