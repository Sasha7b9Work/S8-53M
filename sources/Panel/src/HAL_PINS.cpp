// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "HAL.h"
#include <stm32l0xx_hal.h>


// Этими пинами можно управлять во время исполнения программы
PinSL0 pinSL0;
PinSL1 pinSL1;
PinSL2 pinSL2;
PinSL3 pinSL3;
PinSL4 pinSL4;
PinSL5 pinSL5;

PinRL0 pinRL0;
PinRL1 pinRL1;
PinRL2 pinRL2;
PinRL3 pinRL3;
PinRL4 pinRL4;
PinRL5 pinRL5;
PinRL6 pinRL6;
PinRL7 pinRL7;

PinRC0_ChannelA pinRC0_ChannelA;
PinRC1_ChannelB pinRC1_ChannelB;
PinRC2_Trig     pinRC2_Trig;
PinRC3_Set      pinRC3_Set;

PinPower pinPower;


PinSL0::PinSL0() : Pin(PinPort::A, PinPin::_0) { }
PinSL1::PinSL1() : Pin(PinPort::A, PinPin::_1) { }
PinSL2::PinSL2() : Pin(PinPort::A, PinPin::_2) { }
PinSL3::PinSL3() : Pin(PinPort::A, PinPin::_5) { }
PinSL4::PinSL4() : Pin(PinPort::A, PinPin::_3) { }
PinSL5::PinSL5() : Pin(PinPort::A, PinPin::_4) { }

PinRL0::PinRL0() : Pin(PinPort::B, PinPin::_0) { }
PinRL1::PinRL1() : Pin(PinPort::B, PinPin::_1) { }
PinRL2::PinRL2() : Pin(PinPort::B, PinPin::_2) { }
PinRL3::PinRL3() : Pin(PinPort::B, PinPin::_3) { }
PinRL4::PinRL4() : Pin(PinPort::B, PinPin::_4) { }
PinRL5::PinRL5() : Pin(PinPort::B, PinPin::_5) { }
PinRL6::PinRL6() : Pin(PinPort::B, PinPin::_6) { }
PinRL7::PinRL7() : Pin(PinPort::B, PinPin::_7) { }

PinRC0_ChannelA::PinRC0_ChannelA() : Pin(PinPort::A, PinPin::_10) { }
PinRC1_ChannelB::PinRC1_ChannelB() : Pin(PinPort::A, PinPin::_11) { }
PinRC2_Trig::PinRC2_Trig() : Pin(PinPort::A, PinPin::_9) { }
PinRC3_Set::PinRC3_Set() : Pin(PinPort::A, PinPin::_12) { }

PinPower::PinPower() : Pin(PinPort::A, PinPin::_8) { }


Pin pinSPI2_NSS(PinPort::B, PinPin::_12);    // 25


void HAL_PINS::InitSPI2()
{
    Pin pinSCK(PinPort::B, PinPin::_10);    // 21
    Pin pinMISO(PinPort::B, PinPin::_14);   // 27
    Pin pinMOSI(PinPort::B, PinPin::_15);   // 28

    pinSCK.Init(PinMode::_SPI2);
    pinMISO.Init(PinMode::_SPI2);
    pinMOSI.Init(PinMode::_SPI2);

    pinSPI2_NSS.Init(PinMode::_Output);
    pinSPI2_NSS.On();
}


void HAL_PINS::Init()
{
    pinRL0.Init(PinMode::_Input);
    pinRL1.Init(PinMode::_Input);
    pinRL2.Init(PinMode::_Input);
    pinRL3.Init(PinMode::_Input);
    pinRL4.Init(PinMode::_Input);
    pinRL5.Init(PinMode::_Input);
    pinRL6.Init(PinMode::_Input);
    pinRL7.Init(PinMode::_Input);
 
    pinSL0.Init(PinMode::_Output);
    pinSL1.Init(PinMode::_Output);
    pinSL2.Init(PinMode::_Output);
    pinSL3.Init(PinMode::_Output);
    pinSL4.Init(PinMode::_Output);
    pinSL5.Init(PinMode::_Output);

    pinRC0_ChannelA.Init(PinMode::_Output);
    pinRC0_ChannelA.Off();

    pinRC1_ChannelB.Init(PinMode::_Output);
    pinRC1_ChannelB.Off();

    pinRC2_Trig.Init(PinMode::_Output);
    pinRC2_Trig.Off();

    pinRC3_Set.Init(PinMode::_Output);
    pinRC3_Set.Off();

    pinPower.Init(PinMode::_Output);
    pinPower.Off();
}


Pin::Pin(PinPort::E _port, PinPin::E _pin)
{
    static const GPIO_TypeDef *ports[2] = { GPIOA, GPIOB };

    static const uint16 pins[16] =
    {
        GPIO_PIN_0,
        GPIO_PIN_1,
        GPIO_PIN_2,
        GPIO_PIN_3,
        GPIO_PIN_4,
        GPIO_PIN_5,
        GPIO_PIN_6,
        GPIO_PIN_7,
        GPIO_PIN_8,
        GPIO_PIN_9,
        GPIO_PIN_10,
        GPIO_PIN_11,
        GPIO_PIN_12,
        GPIO_PIN_13,
        GPIO_PIN_14,
        GPIO_PIN_15
    };

    port = const_cast<GPIO_TypeDef *>(ports[_port]);
    pin = pins[_pin];
}

void Pin::Init(PinMode::E mode)
{
    GPIO_InitTypeDef isGPIO =
    {
        GPIO_PIN_0,
        GPIO_MODE_INPUT,
        GPIO_PULLUP,
        GPIO_SPEED_FREQ_VERY_HIGH
    };

    isGPIO.Pin = pin;

    if (mode == PinMode::_Input)
    {
        isGPIO.Mode = GPIO_MODE_INPUT; //-V1048
    }
    else if (mode == PinMode::_Output)
    {
        isGPIO.Mode = GPIO_MODE_OUTPUT_PP;
    }
    else if (mode == PinMode::_SPI2)
    {
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Alternate = (pin == GPIO_PIN_10) ? GPIO_AF5_SPI2 : GPIO_AF0_SPI2;
    }

    HAL_GPIO_Init(reinterpret_cast<GPIO_TypeDef *>(port), &isGPIO);
}


void Pin::Write(uint state)
{
    HAL_GPIO_WritePin(reinterpret_cast<GPIO_TypeDef *>(port), pin, (GPIO_PinState)(state));
}


void Pin::On()
{
    HAL_GPIO_WritePin(reinterpret_cast<GPIO_TypeDef *>(port), pin, GPIO_PIN_SET);
}


void Pin::Off()
{
    HAL_GPIO_WritePin(reinterpret_cast<GPIO_TypeDef *>(port), pin, GPIO_PIN_RESET);
}


uint Pin::Read()
{
    return HAL_GPIO_ReadPin(reinterpret_cast<GPIO_TypeDef *>(port), pin);
}
