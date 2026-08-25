// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com

// For cstdlib:23 : C2733: 'abs': you cannot overload a function with 'extern "C"' linkage
#define _STL_COMPILER_PREPROCESSOR 0
#include "defines.h"
#include "Hardware/VCP/USBD.h"
#include "Hardware/VCP/usbd_desc.h"
#include "Hardware/VCP/usbd_cdc_interface.h"
#include <usbd_def.h>
#include <stm32f4xx_hal.h>


static USBD_HandleTypeDef handleUSBD;
void *USBD::handle = &handleUSBD;


void USBD::Init()
{
    USBD_Init(&handleUSBD, &VCP_Desc, 0);
    USBD_RegisterClass(&handleUSBD, &USBD_CDC);
    USBD_CDC_RegisterInterface(&handleUSBD, &USBD_CDC_fops);
    USBD_Start(&handleUSBD);
}


bool USBD::PrevSendingComplete()
{
    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;
    return pCDC->TxState == 0;
}


bool USBD::Transmit(uint8 *buffer, int size)
{
    if (USBD_CDC_SetTxBuffer(&handleUSBD, buffer, (uint16)size) == USBD_OK)
    {
        if (USBD_CDC_TransmitPacket(&handleUSBD) == USBD_OK)
        {
            return true;
        }
    }

    return false;
}


void USBD::Flush(uint8 *buffer, uint sizeBuffer)
{
    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;
    while (pCDC->TxState == 1)
    {
    };
    USBD_CDC_SetTxBuffer(&handleUSBD, buffer, (uint16)sizeBuffer);
    USBD_CDC_TransmitPacket(&handleUSBD);
    while (pCDC->TxState == 1)
    {
    };
}


void USBD::Wait()
{
    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;

    while (pCDC->TxState == 1)
    {
    }
}
