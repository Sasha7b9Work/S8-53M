// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once 
#include "Panel/Controls.h"


namespace Panel
{
    uint16 NextData();
    Key::E PressedButton();

    void CallbackOnReceiveSPI5(uint8 *data, uint size);
};
