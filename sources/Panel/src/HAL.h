// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "HAL_PINS.h"


#define TIME_MS HAL_TIM::TimeMS()       /* Количество миллисекунд с момента запуска */

extern PinSL0 pinSL0;
extern PinSL1 pinSL1;
extern PinSL2 pinSL2;
extern PinSL3 pinSL3;
extern PinSL4 pinSL4;
extern PinSL5 pinSL5;

extern PinRL0 pinRL0;
extern PinRL1 pinRL1;
extern PinRL2 pinRL2;
extern PinRL3 pinRL3;
extern PinRL4 pinRL4;
extern PinRL5 pinRL5;
extern PinRL6 pinRL6;
extern PinRL7 pinRL7;

extern PinRC0_ChannelA pinRC0_ChannelA;
extern PinRC1_ChannelB pinRC1_ChannelB;
extern PinRC3_Set      pinRC3_Set;
extern PinRC2_Trig     pinRC2_Trig;

extern PinPower pinPower;


namespace HAL
{
    void Init();
    void ErrorHandler();
};


namespace HAL_SPI2
{
    void Init();

    bool TransmitReceivce(uint8 *buffer_out, uint8 *buffer_in, uint size);

    // Время, прошедшее после последней передачи
    uint TimeAfterTransmit();
};


namespace HAL_TIM
{
    uint TimeMS();
};
