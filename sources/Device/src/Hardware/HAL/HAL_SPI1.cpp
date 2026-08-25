// 2022/02/16 10:14:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_SPI1
{
    SPI_HandleTypeDef handleSPI1 =
    {
        SPI1,
        {
            SPI_MODE_SLAVE,                 // Init.Mode
            SPI_DIRECTION_2LINES,           // Init.Direction
            SPI_DATASIZE_8BIT,              // Init.DataSize
            SPI_POLARITY_HIGH,              // Init.CLKPolarity
            SPI_PHASE_1EDGE,                // Init.CLKPhase
            SPI_NSS_SOFT,                   // Init.NSS
            SPI_BAUDRATEPRESCALER_2,        // Init.BaudRatePrescaler
            SPI_FIRSTBIT_MSB,               // Init.FirstBit
            SPI_TIMODE_DISABLED,            // Init.TIMode
            SPI_CRCCALCULATION_DISABLED,    // Init.CRCCalculation
            7                               // InitCRCPolynomial
        }
    };

    void *handle = &handleSPI1;
}
