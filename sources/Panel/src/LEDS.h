// 2021/02/25 8:24:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "HAL.h"


struct Pin;


struct LED
{
    struct TypeEnable { enum E {
        Direct,
        Inverse
    };};

    LED(Pin &_pin, TypeEnable::E _typeEnable) : pin(_pin), typeEnable(_typeEnable) { };

    void Enable();
    void Disable();

    static void DisableAll();

private:
    Pin          &pin;
    TypeEnable::E typeEnable;
};


struct LED_ChannelA : public LED { LED_ChannelA() : LED(pinRC0_ChannelA, LED::TypeEnable::Inverse) {};};

struct LED_ChannelB : public LED { LED_ChannelB() : LED(pinRC1_ChannelB, LED::TypeEnable::Inverse) {};};

struct LED_Trig     : public LED { LED_Trig()     : LED(pinRC2_Trig,     LED::TypeEnable::Inverse) {}  };

struct LED_Set      : public LED { LED_Set()      : LED(pinRC3_Set,      LED::TypeEnable::Direct) {}   };


extern LED_ChannelA led_ChannelA;
extern LED_ChannelB led_ChannelB;
extern LED_Trig     led_Trig;
extern LED_Set      led_Set;
