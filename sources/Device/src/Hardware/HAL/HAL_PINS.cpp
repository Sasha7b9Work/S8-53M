// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


enum Port
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I
};


static const GPIO_TypeDef *ports[PinPort::Count] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI };

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



struct PinFMC : public Pin
{
    PinFMC(int port, int pin) : Pin(PinMode::_FMC, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


struct PinETH : public Pin
{
    PinETH(int port, int pin) : Pin(PinMode::_ETH, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


struct PinSPI1 : public Pin
{
    PinSPI1(int port, int pin) : Pin(PinMode::_SPI1, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


struct PinOTG_HS : public Pin
{
    PinOTG_HS(int port, int pin) : Pin(PinMode::_OTG_HS_FS, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


struct PinOTG_FS : public Pin
{
    PinOTG_FS(int port, int pin) : Pin(PinMode::_OTG_FS, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


struct PinLTDC : public Pin
{
    PinLTDC(int port, int pin) : Pin(PinMode::_LTDC, (PinPort::E)(port), (PinPin::E)(pin)) { }
};


Pin Pin::SPI4_CS1(PinMode::_Output, PinPort::_E, PinPin::_5);
Pin Pin::SPI4_CS2(PinMode::_Output, PinPort::_I, PinPin::_8);
Pin Pin::SPI4_CLK(PinMode::_Output, PinPort::_E, PinPin::_2);
Pin Pin::SPI4_DAT(PinMode::_Output, PinPort::_E, PinPin::_6);
Pin Pin::LED(PinMode::_Output, PinPort::_G, PinPin::_12);
Pin Pin::DisplayReady(PinMode::_Input, PinPort::_G, PinPin::_11);


void HAL_PINS::ADC1_::Init()
{
    Pin(PinMode::_ADC1_IN, PinPort::_A, PinPin::_0).Init();         // ADC3 IN  PA0 40
    Pin(PinMode::_ADC1_IT, PinPort::_H, PinPin::_4).Init();         // ADC3 IT  PH4 45
}


void HAL_PINS::DAC1_::Init()
{
    Pin(PinMode::_DAC1, PinPort::_A, PinPin::_4).Init();    // 50 - яркость дисплея
}


void HAL_PINS::DAC2_::Init()
{
    Pin(PinMode::_DAC2, PinPort::_A, PinPin::_5).Init();    // 51 - динамик
}


void HAL_PINS::FMC_::Init()
{
    PinFMC(D, 4).Init();        // 146 NOE  Здесь привязывать нужно
    PinFMC(D, 5).Init();        // 147 NWE  к единице

    PinFMC(D, 7).Init();        // 151 NE1  Здесь привязывать нужно
    PinFMC(G, 9).Init();        // 152 NE2  к нулю

    PinFMC(D, 14).Init();       // 104 D0
    PinFMC(D, 15).Init();       // 105 D1
    PinFMC(D, 0).Init();        // 142 D2
    PinFMC(D, 1).Init();        // 143 D3
    PinFMC(E, 7).Init();        //  68 D4
    PinFMC(E, 8).Init();        //  69 D5
    PinFMC(E, 9).Init();        //  70 D6
    PinFMC(E, 10).Init();       //  73 D7
    PinFMC(E, 11).Init();       //  74 D8
    PinFMC(E, 12).Init();       //  75 D9
    PinFMC(E, 13).Init();       //  76 D10
    PinFMC(E, 14).Init();       //  77 D11
    PinFMC(E, 15).Init();       //  78 D12
    PinFMC(D, 8).Init();        //  96 D13
    PinFMC(D, 9).Init();        //  97 D14
    PinFMC(D, 10).Init();       //  98 D15
    PinFMC(F, 0).Init();        //  16 A0
    PinFMC(F, 1).Init();        //  17 A1
    PinFMC(F, 2).Init();        //  18 A2
    PinFMC(F, 3).Init();        //  19 A3
    PinFMC(F, 4).Init();        //  20 A4
    PinFMC(F, 5).Init();        //  21 A5
    PinFMC(F, 12).Init();       //  60 A6
    PinFMC(F, 13).Init();       //  63 A7
    PinFMC(F, 14).Init();       //  64 A8
    PinFMC(F, 15).Init();       //  65 A9
    PinFMC(G, 0).Init();        //  66 A10
    PinFMC(G, 1).Init();        //  67 A11
    PinFMC(G, 2).Init();        // 106 A12
    PinFMC(G, 3).Init();        // 107 A13
    PinFMC(G, 4).Init();        // 108 A14
    PinFMC(G, 5).Init();        // 109 A15
    PinFMC(D, 11).Init();       //  99 A16
    PinFMC(D, 12).Init();       // 100 A17
    PinFMC(D, 13).Init();       // 101 A18
    PinFMC(E, 3).Init();        //   2 A19
}


void HAL_PINS::SPI1_::Init()
{
    PinSPI1(A, 5).Init();   // SCK //-V525
    PinSPI1(A, 6).Init();   // MISO
    PinSPI1(B, 5).Init();   // MOSI
    PinSPI1(G, 0).Init();   // NSS
}


void HAL_PINS::SPI5_::Init()
{
    Pin pinNSS (PinMode::_Input, PinPort::_F, PinPin::_6);  // 24 NSS
    Pin pinSCK (PinMode::_SPI5, PinPort::_F, PinPin::_7);   // 25 SCK
    Pin pinMISO(PinMode::_SPI5, PinPort::_F, PinPin::_8);   // 26 MISO
    Pin pinMOSI(PinMode::_SPI5, PinPort::_F, PinPin::_9);   // 27 MOSI

    pinNSS.Init();
    pinSCK.Init();
    pinMISO.Init();
    pinMOSI.Init();
}


void HAL_PINS::SPI4_::Init()
{
    Pin::SPI4_CS1.Init();
    Pin::SPI4_CS1.Set();

    Pin::SPI4_CS2.Init();
    Pin::SPI4_CS2.Set();

    Pin::SPI4_CLK.Init();

    Pin::SPI4_DAT.Init();
}


void HAL_PINS::Init()
{
    SPI4_::Init();

    Pin::LED.Init();
    Pin::DisplayReady.Init();

    // Флешка
    PinOTG_HS(B, 15).Init();    // DP
    PinOTG_HS(B, 14).Init();    // DM

    // Для определения подключения VCP без передёргивания шнура
    Pin pin(PinMode::_Output, PinPort::_A, PinPin::_12);
    pin.Init();
    pin.Reset();

    // VCP
    PinOTG_FS(A, 12).Init();    // DP
    PinOTG_FS(A, 11).Init();    // DM
    PinOTG_FS(A, 9).Init();     // VBUS

    PinETH(A, 2).Init();        // MDIO
    PinETH(C, 1).Init();        // MDC
    PinETH(H, 6).Init();        // RXD2
    PinETH(B, 1).Init();        // RXD3
    PinETH(C, 3).Init();        // TX_CLK       35
    PinETH(C, 2).Init();        // TXD2
    PinETH(B, 8).Init();        // TXD3
    PinETH(A, 1).Init();        // RX_CLK       41
    PinETH(A, 7).Init();        // RX_DV
    PinETH(C, 4).Init();        // RXD0
    PinETH(C, 5).Init();        // RXD1
    PinETH(B, 11).Init();       // TX_EN
    PinETH(B, 12).Init();       // TXD0
    PinETH(G, 14).Init();       // TXD1
    PinETH(I, 10).Init();       // RX_ER
    PinETH(H, 2).Init();        // CRS
    PinETH(H, 3).Init();        // COL

    Pin(PinMode::_DAC1, PinPort::_A, PinPin::_4).Init();        // PWM для дисплея

    Pin pinPWM(PinMode::_Output, PinPort::_A, PinPin::_4);
    pinPWM.Init();
    pinPWM.Set();

    Pin pinRL(PinMode::_Output, PinPort::_G, PinPin::_13);      // Выбор горизонтальной ориентации дисплея R/L
    pinRL.Init();
    pinRL.Set();

    Pin pinUD(PinMode::_Output, PinPort::_G, PinPin::_10);      // Выбор вертикальной ориентации дисплея U/D
    pinUD.Init();
    pinUD.Reset();

    PinLTDC(I, 9).Init();       // VSYNC    11
    PinLTDC(C, 6).Init();       // HSYNC    115
    PinLTDC(G, 7).Init();       // CLK      111
    PinLTDC(F, 10).Init();      // DE       28

    PinLTDC(H, 8).Init();       // R2   85  +
    PinLTDC(B, 0).Init();       // R3   56  +
    PinLTDC(H, 10).Init();      // R4   87  +
    PinLTDC(H, 11).Init();      // R5   88  +
    PinLTDC(H, 12).Init();      // R6   89  +
    PinLTDC(G, 6).Init();       // R7   110 +
    
    PinLTDC(A, 6).Init();       // G2   52  +
    PinLTDC(H, 14).Init();      // G3   129 +
    PinLTDC(B, 10).Init();      // G4   79  +
    PinLTDC(I, 0).Init();       // G5   131 +
    PinLTDC(C, 7).Init();       // G6   116 +
    PinLTDC(D, 3).Init();       // G7   145 +

    PinLTDC(D, 6).Init();       // B2   150 +
    PinLTDC(G, 11).Init();      // B3   154 +
    PinLTDC(G, 12).Init();      // B4   155 +
    PinLTDC(A, 3).Init();       // B5   47  +
    PinLTDC(I, 6).Init();       // B6   175 +
    PinLTDC(B, 9).Init();       // B7   168 +
}


void Pin::Init()
{
    GPIO_InitTypeDef isGPIO = { pins[pin] };

    isGPIO.Pull = GPIO_PULLUP;

    if (mode == PinMode::_Input)
    {
        isGPIO.Mode = GPIO_MODE_INPUT;
    }
    else if (mode == PinMode::_Output)
    {
        isGPIO.Mode = GPIO_MODE_OUTPUT_PP;
        isGPIO.Speed = GPIO_SPEED_HIGH;
    }
    else if (mode == PinMode::_ADC1_IN)
    {
        isGPIO.Mode = GPIO_MODE_ANALOG;
    }
    else if (mode == PinMode::_ADC1_IT)
    {
        isGPIO.Mode = GPIO_MODE_IT_RISING;
    }
    else if (mode == PinMode::_DAC1 || mode == PinMode::_DAC2)
    {
        isGPIO.Mode = GPIO_MODE_ANALOG;
    }
    else if (mode == PinMode::_ETH)
    {
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Speed = GPIO_SPEED_HIGH;
        isGPIO.Alternate = GPIO_AF11_ETH;
        isGPIO.Pull = GPIO_NOPULL;
    }
    else if (mode == PinMode::_OTG_HS_FS)
    {
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Pull = GPIO_NOPULL;
        isGPIO.Speed = GPIO_SPEED_HIGH;
        isGPIO.Alternate = GPIO_AF12_OTG_HS_FS;
    }
    else if (mode == PinMode::_OTG_FS)
    {
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Speed = GPIO_SPEED_HIGH;
        isGPIO.Alternate = GPIO_AF10_OTG_FS;
    }
    else if (mode == PinMode::_SPI1)
    {
        if (pin == PinPin::_0)
        {
            isGPIO.Mode = GPIO_MODE_IT_RISING;
        }
        else
        {
            isGPIO.Mode = GPIO_MODE_AF_PP;
            isGPIO.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            isGPIO.Alternate = GPIO_AF5_SPI1;
        }
    }
    else if (mode == PinMode::_SPI5)
    {
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        isGPIO.Alternate = GPIO_AF5_SPI5;
    }
    else if (mode == PinMode::_FMC)
    {
        if ((port == PinPort::_D && pin == 7) ||
            (port == PinPort::_G && pin == 9))
        {
            isGPIO.Pull = GPIO_PULLDOWN;
        }

        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Speed = GPIO_SPEED_FREQ_HIGH;
        isGPIO.Alternate = GPIO_AF12_FMC;
    }
    else if (mode == PinMode::_LTDC)
    {
        isGPIO.Pull = GPIO_PULLDOWN;
        isGPIO.Mode = GPIO_MODE_AF_PP;
        isGPIO.Speed = GPIO_SPEED_FREQ_LOW;
        isGPIO.Alternate = GPIO_AF14_LTDC;
    }

    HAL_GPIO_Init((GPIO_TypeDef *)(const_cast<GPIO_TypeDef *>(ports[port])), &isGPIO);
}


void Pin::Write(int state)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)ports[port], pins[pin], (GPIO_PinState)(state));
}


void Pin::Set()
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)ports[port], pins[pin], GPIO_PIN_SET);
}


void Pin::Reset()
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)ports[port], pins[pin], GPIO_PIN_RESET);
}


uint Pin::Read()
{
    return HAL_GPIO_ReadPin((GPIO_TypeDef *)ports[port], pins[pin]);
}
