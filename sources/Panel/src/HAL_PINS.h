// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct HAL_PINS
{
    static void Init();

    static void InitSPI2();
};


struct PinMode { enum E
{
    _Input,
    _Output,
    _SPI2
};};

struct PinPort { enum E
{
    A,
    B
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
    Pin(PinPort::E port, PinPin::E pin);
    void Init(PinMode::E mode);
    void Write(uint state);
    void On();
    void Off();
    uint Read();

private:
    void *port; // GPIO_TypeDef *
    uint16 pin;
};

struct PinSL0 : public Pin { PinSL0(); };
struct PinSL1 : public Pin { PinSL1(); };
struct PinSL2 : public Pin { PinSL2(); };
struct PinSL3 : public Pin { PinSL3(); };
struct PinSL4 : public Pin { PinSL4(); };
struct PinSL5 : public Pin { PinSL5(); };

struct PinRL0 : public Pin { PinRL0(); };
struct PinRL1 : public Pin { PinRL1(); };
struct PinRL2 : public Pin { PinRL2(); };
struct PinRL3 : public Pin { PinRL3(); };
struct PinRL4 : public Pin { PinRL4(); };
struct PinRL5 : public Pin { PinRL5(); };
struct PinRL6 : public Pin { PinRL6(); };
struct PinRL7 : public Pin { PinRL7(); };

struct PinRC0_ChannelA : public Pin { PinRC0_ChannelA(); };
struct PinRC1_ChannelB : public Pin { PinRC1_ChannelB(); };
struct PinRC2_Trig : public Pin { PinRC2_Trig(); };
struct PinRC3_Set : public Pin { PinRC3_Set(); };
struct PinPower : public Pin { PinPower(); };
