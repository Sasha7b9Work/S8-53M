// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Panel/Panel.h"


#define LED_CHANA_ENABLE    129
#define LED_CHANA_DISABLE   1
#define LED_CHANB_ENABLE    130
#define LED_CHANB_DISABLE   2
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4


static Key::E pressedButton = Key::None;


Key::E Panel::PressedButton()
{
    return pressedButton;
}


uint16 Panel::NextData()
{
    return 0;
}


void Panel::CallbackOnReceiveSPI5(uint8 * /*data*/, uint /*size*/)
{

}
