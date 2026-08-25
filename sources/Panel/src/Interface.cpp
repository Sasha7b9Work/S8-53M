// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "LEDS.h"
#include "HAL.h"
#include "Interface.h"
#include "Keyboard.h"
#include "PowerSupply.h"


namespace Interface
{
    static const int SIZE_BUFFER = 3;

    uint8 output[SIZE_BUFFER] = {0xFF, 0, 0};
    uint8 input[SIZE_BUFFER];

    static void ProcessByte(uint8 byte);
}


void Interface::Update(const KeyboardEvent &event)
{
    if (!PowerSupply::IsEnabled())
    {
        return;
    }

    bool needTransmit = false;

    output[1] = (uint8)event.key.value;
    output[2] = (uint8)event.action.value;

    if (event.key.value != Key::None)
    {
        output[0] = 0xFF;
        needTransmit = true;
    }
    
    if (!needTransmit && HAL_SPI2::TimeAfterTransmit() > 10)      // Если данных нет (event.key == Key::None), но прошло
    {                                                    // более 10мс делаем пустую посылку для приёма сообщений от ЦПУ
        output[0] = 0x00;
        needTransmit = true;
    }

    if (needTransmit)
    {
        HAL_SPI2::TransmitReceivce(output, input, SIZE_BUFFER);

        for (uint i = 0; i < SIZE_BUFFER; i++)
        {
            ProcessByte(input[i]);
        }
    }
}


void Interface::ProcessByte(uint8 byte)
{
    uint8 control = (uint8)(byte & 0x0F);

    if (control == TypeLED::Power)
    {
        PowerSupply::TurnOff();
    }
    else if (control > 0 && control < 5)
    {
        static LED *leds[] = { nullptr, &led_Trig, &led_Set, &led_ChannelA, &led_ChannelB };

        LED *led = leds[control];

        (control == byte) ? led->Disable() : led->Enable();
    }
}
