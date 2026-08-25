// 2021/02/25 8:24:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "LEDS.h"


LED_ChannelA led_ChannelA;
LED_ChannelB led_ChannelB;
LED_Trig     led_Trig;
LED_Set      led_Set;


void LED::Enable()
{
    (typeEnable == TypeEnable::Direct) ? pin.On() : pin.Off();
}


void LED::Disable()
{
    (typeEnable == TypeEnable::Direct) ? pin.Off() : pin.On();
}


void LED::DisableAll()
{
    led_ChannelA.Disable();
    led_ChannelB.Disable();
    led_Trig.Disable();
    led_Set.Disable();
}
