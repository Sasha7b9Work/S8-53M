// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


namespace HAL_PINS
{
    void Init();

    namespace ADC1_ { void Init(); }

    // ƒл€ управлени€ €ркостью диспле€
    namespace DAC1_ { void Init(); }

    // ƒл€ звука
    namespace DAC2_ { void Init(); }

    namespace FMC_  { void Init(); }

    namespace SPI1_ { void Init(); }

    namespace SPI4_ { void Init(); }

    namespace SPI5_ { void Init(); }
};


struct PinMode { enum E
{
    _Input,
    _Output,
    _ADC1_IT,
    _ADC1_IN,
    _DAC1,          // яркость диспле€
    _DAC2,          // ƒинамик
    _ETH,
    _OTG_HS_FS,
    _OTG_FS,
    _SPI1,
    _SPI5,
    _FMC,
    _LTDC
};};

struct PinPort { enum E
{
    _A,
    _B,
    _C,
    _D,
    _E,
    _F,
    _G,
    _H,
    _I,
    Count
};};

struct PinPin { enum E
{
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _10,
    _11,
    _12,
    _13,
    _14,
    _15
};};


struct Pin
{
    Pin(PinMode::E _mode, PinPort::E _port, PinPin::E _pin) : port(_port), pin(_pin), mode(_mode) { };
    void Init();
    void Write(int state);
    void Set();
    void Reset();
    uint Read();

    static Pin SPI4_CS1;
    static Pin SPI4_CS2;
    static Pin SPI4_CLK;
    static Pin SPI4_DAT;
    static Pin LED;
    static Pin DisplayReady;

private:
    PinPort::E port;
    PinPin::E  pin;
    PinMode::E mode;
};
