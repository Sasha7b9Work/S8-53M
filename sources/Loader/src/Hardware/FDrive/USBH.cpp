// 2021/06/24 14:23:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/FDrive/USBH.h"
#include <usbh_def.h>


USBH_HandleTypeDef handleUSBH;

void *USBH::handle = &handleUSBH;

