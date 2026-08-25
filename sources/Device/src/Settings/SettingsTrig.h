// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define START_MODE                          (gset.trig.startMode)
#define START_MODE_IS_SINGLE                (START_MODE == StartMode::Single)
#define START_MODE_IS_WAIT                  (START_MODE == StartMode::Wait)
#define START_MODE_IS_AUTO                  (START_MODE == StartMode::Auto)

#define TRIG_SOURCE                         (gset.trig.source)
#define TRIG_SOURCE_IS_EXT                  (TRIG_SOURCE == TrigSource::Ext)

#define TRIG_POLARITY                       (gset.trig.polarity)
#define TRIG_POLARITY_IS_FRONT              (TRIG_POLARITY == TrigPolarity::Front)

#define TRIG_INPUT                          (gset.trig.input)
#define TRIG_INPUT_IS_AC                    (TRIG_INPUT == TrigInput::AC)

#define TRIG_LEVEL(source)                  (gset.trig.levelRel[source])
#define TRIG_LEVEL_SOURCE                   (TRIG_LEVEL(TRIG_SOURCE))
#define TRIG_LEVEL_A                        (TRIG_LEVEL(Chan::A))
#define TRIG_LEVEL_B                        (TRIG_LEVEL(Chan::B))

#define MODE_LONG_PRESS_TRIG                (gset.trig.modeLongPressTrig)
#define MODE_LONG_PRESS_TRIG_IS_LEVEL_ZERO  (MODE_LONG_PRESS_TRIG == ModeLongPressTrig::LevelZero)

#define TRIG_MODE_FIND                      (gset.trig.modeFind)
#define TRIG_MODE_FIND_IS_AUTO              (TRIG_MODE_FIND == TrigModeFind::Auto)
#define TRIG_MODE_FIND_IS_HAND              (TRIG_MODE_FIND == TrigModeFind::Hand)




// Режим запуска.
struct StartMode
{
    enum E
    {
        Auto,       // Автоматический.
        Wait,       // Ждущий.
        Single      // Однократный.
    };

    static void Set(E);
};


// Режим длительного нажатия кнопки СИНХР.
struct ModeLongPressTrig
{
    enum E
    {
        LevelZero,  // Сброс уровня синхронизации в 0.
        Auto        // Автоматический поиск синхронизации - уровень устанавливается посередине между максимумом и минимумом.
    };
};


// Режим установки синхронизации.
struct TrigModeFind
{
    enum E
    {
        Hand,       // Уровень синхронизации устанавливается вручную.
        Auto        // Подстройки уровня синхронизации производится автоматически после каждого нового считанного сигнала.
    };
};




// Настройки синхронизации
struct SettingsTrig
{
    StartMode::E         startMode;          // Режим запуска.
    TrigSource::E        source;             // Источник.
    TrigPolarity::E      polarity;           // Тип синхронизации.
    TrigInput::E         input;              // Вход синхронизации.
    TrigLev              levelRel[3];        // Уровень синхронизации для трёх источников.
    ModeLongPressTrig::E modeLongPressTrig;  // Режим работы длительного нажатия кнопки СИНХР.
    TrigModeFind::E      modeFind;           // Поиск синхронизации - вручную или автоматически.
};
