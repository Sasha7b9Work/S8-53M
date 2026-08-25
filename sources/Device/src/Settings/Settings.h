// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "SettingsTypes.h"
#include "Data/Measures.h"
#include "Menu/MenuItems.h"
#include "SettingsChannel.h"
#include "SettingsDisplay.h"
#include "SettingsMemory.h"
#include "SettingsTime.h"
#include "SettingsTrig.h"
#include "SettingsService.h"
#include "SettingsCursors.h"
#include "SettingsNRST.h"
#include "SettingsMemory.h"
#include "SettingsMeasures.h"
#include "SettingsMath.h"


#define LANG            (gset.menu.lang)
#define LANG_RU         (LANG == Lang::Ru)
#define LANG_EN         (LANG == Lang::En)

#define POS_ACT_ITEM(name_page) gset.menu.posActItem[name_page]


struct SettingsMenu
{
    int8        posActItem[NamePage::Count];        // Позиция ативного пункта. bit7 == 1 - item is opened, 0x7f - нет активного пункта
    int8        currentSubPage[NamePage::Count];    // Номер текущей подстраницы.
    Lang::E     lang;                               // Язык меню.
    unsigned    isShown : 1;                        // Меню показано.
};


// Струкура хранит все настройки прибора.
struct Settings
{
    static const int SIZE_FIELD_RECORD = 1024;

    uint                crc32;
    SettingsDisplay     display;            // настройки изображения          (меню ДИСПЛЕЙ).
    SettingsChannel     chan[Chan::Count];  // настройки каналов              (меню КАНАЛ 1 и КАНАЛ 2).
    SettingsTrig        trig;               // настройки синхронизации        (меню СИНХР).
    SettingsTime        time;               // временнЫе настройки            (меню РАЗВЁРТКА).
    SettingsCursors     cursors;            // настройки курсорных измерений  (меню КУРСОРЫ).
    SettingsMemory      memory;             // настройки режимов памяти       (меню ПАМЯТЬ).
    SettingsMeasures    measures;           // настройки измерений            (меню ИЗМЕРЕНИЯ).
    SettingsMath        math;               // настройки режима математических измерений.
    SettingsService     service;            // дополнительные настройки       (меню СЕРВИС).
    SettingsEthernet    eth;                // настройки для соединения по локальной сети.
    SettingsMenu        menu;               // состояние меню.
    uint                notUsed;

    void Load();                    // Загрузить настройки. Если _default == true, загружаются настройки по
                                    // умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае неудачи -
                                    // тоже настройки по умолчанию.
    bool Save();                    // Сохранить настройки во флеш-память перед выключением

    void Reset();                   // Сбросить настройки, сохранив калибровочные настройки
    void RunAfterLoad();            // После загрузки настроек нужно выполнить эти команды

    uint CalculateCRC32();
};

extern Settings gset;

