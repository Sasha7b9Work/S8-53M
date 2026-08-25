// 2022/2/11 19:47:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"


extern const char chanInputRu[] =   "1. \"Вкл\" - выводить сигнал на экран.\n"
                                    "2. \"Откл\" - не выводить сигнал на экран.";
extern const char chanInputEn[] =   "1. \"Enable\" - signal output to the screen.\n"
                                    "2. \"Disable\" - no output to the screen.";

extern const char chanCoupleRu[] =  "Задаёт вид связи с источником сигнала.\n"
                                    "1. \"Пост\" - открытый вход.\n"
                                    "2. \"Перем\" - закрытый вход.\n"
                                    "3. \"Земля\" - вход соединён с землёй.";
extern const char chanCoupleEn[] =  "Sets a type of communication with a signal source.\n"
                                    "1. \"AC\" - open input.\n"
                                    "2. \"DC\" - closed input.\n"
                                    "3. \"Ground\" - input is connected to the ground.";

extern const char chanFiltrRu[] = "Включает/отключает фильтр для ограничения полосы пропускания.";
extern const char chanFiltrEn[] = "Includes/switches-off the filter for restriction of a pass-band.";

extern const char chanInverseRu[] = "При \"Вкл\" сигнал на экране будет симметрично отражён относительно U = 0В.";
extern const char chanInverseEn[] = "When \"Enable\" signal on the screen will be reflected symmetrically with respect to U = 0V.";

extern const char chanMultiplierRu[] = "Ослабление сигнала:\n\"x1\" - сигнал не ослабляется.\n\"x10\" - сигнал ослабляется в 10 раз";
extern const char chanMultiplierEn[] = "Attenuation: \n\"x1\" - the signal is not attenuated.\n\"x10\" - the signal is attenuated by 10 times";


void PageChannelB::OnChanged_Input(bool)
{
    LED::ChanB.Switch(SET_ENABLED_B);
}


static const Choice mcInputA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_ENABLED_A, PageChannelA::OnChanged_Input
};


void PageChannelA::OnChanged_Input(bool)
{
    LED::ChanA.Switch(SET_ENABLED_A);
}


void PageChannelA::OnChanged_Couple(bool)
{
    ModeCouple::Set(Chan::A, SET_COUPLE_A);
}


static const Choice mcCoupleA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "Связь",   "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост",    "AC"},
        {"Перем",   "DC"},
        {"Земля",   "Ground"}
    },
    (int8*)&SET_COUPLE_A, PageChannelA::OnChanged_Couple
};


void PageChannelA::OnChanged_Filtr(bool)
{
    Filtr::Enable(Chan::A, SET_FILTR_A);
}


static const Choice mcFiltrA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "Фильтр", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_A, PageChannelA::OnChanged_Filtr
};


static void OnChanged_InverseA(bool)
{
    RShift::Set(Chan::A, SET_RSHIFT_A);
}


static const Choice mcInverseA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "Инверсия",    "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_INVERSE_A, OnChanged_InverseA
};


static const Choice mcMultiplierA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "Множитель", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"х1",  "x1"},
        {"x10", "x10"}
    },
    (int8*)&SET_DIVIDER(Chan::A)
};


static const Choice mcInputB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_ENABLED_B, PageChannelB::OnChanged_Input
};


void PageChannelB::OnChanged_Couple(bool)
{
    ModeCouple::Set(Chan::B, SET_COUPLE_B);
}


static const Choice mcCoupleB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "Связь", "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост",    "AC"},
        {"Перем",   "DC"},
        {"Земля",   "Ground"}
    },
    (int8 *)&SET_COUPLE_B, PageChannelB::OnChanged_Couple
};


void PageChannelB::OnChanged_Filtr(bool)
{
    Filtr::Enable(Chan::B, SET_FILTR_B);
}


static const Choice mcFiltrB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "Фильтр", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_FILTR_B, PageChannelB::OnChanged_Filtr
};


static void OnChanged_InverseB(bool)
{
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Choice mcInverseB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "Инверсия", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_INVERSE_B, OnChanged_InverseB
};


static const Choice mcMultiplierB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "Множитель", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"х1",  "x1"},
        {"x10", "x10"}
    },
    (int8 *)&SET_DIVIDER(Chan::B)
};


static const arrayItems itemsChanB =
{
    (void *)&mcInputB,
    (void *)&mcCoupleB,
    (void *)&mcFiltrB,
    (void *)&mcInverseB,
    (void *)&mcMultiplierB
};


static const Page pChanB
(
    PageMain::self, 0,
    "КАНАЛ 2", "CHANNEL 2",
    "Содержит настройки канала 2.",
    "Contains settings of the channel 2.",
    NamePage::Channel1, &itemsChanB
);

const Page *PageChannelB::self = &pChanB;


static const arrayItems itemsChanA =
{
    (void *)&mcInputA,
    (void *)&mcCoupleA,
    (void *)&mcFiltrA,
    (void *)&mcInverseA,
    (void *)&mcMultiplierA
};


static const Page pChanA
(
    PageMain::self, 0,
    "КАНАЛ 1", "CHANNEL 1",
    "Содержит настройки канала 1.",
    "Contains settings of the channel 1.",
    NamePage::Channel0, &itemsChanA
);

const Page *PageChannelA::self = &pChanA;
