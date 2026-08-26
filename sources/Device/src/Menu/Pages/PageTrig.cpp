// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"


void PageTrig::OnPress_Mode(bool)
{
    StartMode::Set(START_MODE);
}


static const Choice mcMode =
{
    TypeItem::Choice, PageTrig::self, 0,
    {
        "Режим", "Mode"
        ,
        "Задаёт режим запуска:\n"
        "1. \"Авто\" - запуск происходит автоматически.\n"
        "2. \"Ждущий\" - запуск происходит по уровню синхронизации.\n"
        "3. \"Однократный\" - запуск происходит по достижении уровня синхронизации один раз. Для следующего измерения нужно нажать кнопку ПУСК/СТОП."
        ,
        "Sets the trigger mode:\n"
        "1. \"Auto\" - start automatically.\n"
        "2. \"Standby\" - the launch takes place at the level of synchronization.\n"
        "3. \"Single\" - the launch takes place on reaching the trigger levelonce. For the next measurement is necessary to press the START/STOP."
    },
    {
        {"Авто ",       "Auto"},
        {"Ждущий",      "Wait"},
        {"Однократный", "Single"}
    },
    (int8*)&START_MODE, PageTrig::OnPress_Mode
};


static void OnChanged_Source(bool)
{
    TrigSource::Set(TRIG_SOURCE);
}


static const Choice mcSource =
{
    TypeItem::Choice, PageTrig::self, 0,
    {
        "Источник", "Source",
        "Выбор источника сигнала синхронизации.",
        "Synchronization signal source choice."
    },
    {
        {"Канал 1", "Channel 1"},
        {"Канал 2", "Channel 2"},
        {"Внешний", "External"}
    },
    (int8*)&TRIG_SOURCE, OnChanged_Source
};


static void OnChanged_Polarity(bool)
{
    TrigPolarity::Set(TRIG_POLARITY);
}


static const Choice mcPolarity =
{
    TypeItem::Choice, PageTrig::self, 0,
    {
        "Полярность", "Polarity"
        ,
        "1. \"Фронт\" - запуск происходит по фронту синхроимпульса.\n"
        "2. \"Срез\" - запуск происходит по срезу синхроимпульса."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    {
        {"Фронт",   "Front"},
        {"Срез",    "Back"}
    },
    (int8*)&TRIG_POLARITY, OnChanged_Polarity
};


static void OnChanged_Input(bool)
{
    TrigInput::Set(TRIG_INPUT);
}


static const Choice mcInput =
{
    TypeItem::Choice, PageTrig::self, 0,
    {
        "Вход", "Input"
        ,
        "Выбор связи с источником синхронизации:\n"
        "1. \"ПС\" - полный сигнал.\n"
        "2. \"АС\" - закрытый вход.\n"
        "3. \"ФНЧ\" - фильтр низких частот.\n"
        "4. \"ФВЧ\" - фильтр высоких частот."
        ,
        "The choice of communication with the source of synchronization:\n"
        "1. \"SS\" - a full signal.\n"
        "2. \"AS\" - a gated entrance.\n"
        "3. \"LPF\" - low-pass filter.\n"
        "4. \"HPF\" - high-pass filter frequency."
    },
    {
        {"ПС",  "Full"},
        {"АС",  "AC"},
        {"ФНЧ", "LPF"},
        {"ФВЧ", "HPF"}
    },
    (int8*)&TRIG_INPUT, OnChanged_Input
};


static const Choice mcAutoFind_Mode =
{
    TypeItem::Choice, PageTrig::AutoFind::self, 0,
    {
        "Режим", "Mode"
        ,
        "Выбор режима автоматического поиска синхронизации:\n"
        "1. \"Ручной\" - поиск производится по нажатию кнопки \"Найти\" или по удержанию в течение 0.5с кнопки СИНХР, если установлено \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\".\n"
        "2. \"Автоматический\" - поиск производится автоматически."
        ,
        "Selecting the automatic search of synchronization:\n"
    "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the СИНХР button if it is established \"SERVICE\x99Mode long СИНХР\x99\x41utolevel\".\n"
    "2. \"Auto\" - the search is automatically."
    },
    {
        {"Ручной",          "Hand"},
        {"Автоматический",  "Auto"}
    },
    (int8 *)&TRIG_MODE_FIND
};


static bool IsActive_AutoFind_Search()
{
    return TRIG_MODE_FIND_IS_HAND;
}


static void OnPress_AutoFind_Search()
{
    TrigLev::FindAndSet();
}


static const Button mbAutoFind_Search
(
    PageTrig::AutoFind::self, IsActive_AutoFind_Search,
    "Найти", "Search",
    "Производит поиск уровня синхронизации.",
    "Runs for search synchronization level.",
    OnPress_AutoFind_Search
);


static const arrayItems itemsAutoFind =
{
    (void*)&mcAutoFind_Mode,     // СИНХР - ПОИСК - Режим
    (void*)&mbAutoFind_Search     // СИНХР - ПОИСК - Найти    
};


static const Page mpAutoFind
(
    PageTrig::self, 0,
    "ПОИСК", "SEARCH",
    "Управление автоматическим поиском уровня синхронизации.",
    "Office of the automatic search the trigger level.",
    NamePage::TrigAuto, &itemsAutoFind
);


static const arrayItems itemsTrig =
{
    (void *)&mcMode,
    (void *)&mcSource,
    (void *)&mcPolarity,
    (void *)&mcInput,
    (void *)&mpAutoFind
};


static const Page pTrig
(
    PageMain::self, 0,
    "СИНХР", "TRIG",
    "Содержит настройки синхронизации.",
    "Contains synchronization settings.",
    NamePage::Trig, &itemsTrig
);

const Page *PageTrig::self = &pTrig;
const Page *PageTrig::AutoFind::self = &mpAutoFind;
