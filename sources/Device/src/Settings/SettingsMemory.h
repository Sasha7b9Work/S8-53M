// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"


#define SET_ENUM_POINTS             (gset.memory.fpgaENumPoints)
#define SET_ENUM_POINTS_IS_281      (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_281)
#define SET_ENUM_POINTS_IS_1024     (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_1024)

#define MODE_WORK                   (gset.memory.modeWork)
#define MODE_WORK_IS_DIRECT         (MODE_WORK == ModeWork::Direct)
#define MODE_WORK_IS_LATEST         (MODE_WORK == ModeWork::Latest)
#define MODE_WORK_IS_MEMINT         (MODE_WORK == ModeWork::Internal)

#define FILE_NAMING_MODE            (gset.memory.fileNamingMode)
#define FILE_NAMING_MODE_IS_MASK    (FILE_NAMING_MODE == FileNamingMode::Mask)
#define FILE_NAMING_MODE_IS_HAND    (FILE_NAMING_MODE == FileNamingMode::Manually)

#define FILE_NAME_MASK              (gset.memory.fileNameMask)

#define FILE_NAME                   (gset.memory.fileName)

#define INDEX_SYMBOL                (gset.memory.indexCurSymbolNameMask)

#define MODE_SHOW_MEMINT            (gset.memory.modeShowIntMem)
#define MODE_SHOW_MEMINT_IS_SAVED   (MODE_SHOW_MEMINT == ModeShowIntMem::Saved)
#define MODE_SHOW_MEMINT_IS_DIRECT  (MODE_SHOW_MEMINT == ModeShowIntMem::Direct)
#define MODE_SHOW_MEMINT_IS_BOTH    (MODE_SHOW_MEMINT == ModeShowIntMem::Both)

#define FLASH_AUTOCONNECT           (gset.memory.flashAutoConnect)

#define MODE_BTN_MEMORY             (gset.memory.modeBtnMemory)
#define MODE_BTN_MEMORY_IS_SAVE     (MODE_BTN_MEMORY == ModeBtnMemory::Save)

#define MODE_SAVE_SIGNAL            (gset.memory.modeSaveSignal)
#define MODE_SAVE_SIGNAL_IS_BMP     (MODE_SAVE_SIGNAL == ModeSaveSignal::BMP)


// Режим работы.
struct ModeWork
{
    enum E
    {
        Direct,     // Основной режим.
        Latest,     // В этом режиме можно просмотреть последние сохранённые измерения.
        Internal,   // В этом режиме можно сохранять во flash-памяти измерения просматривать ранее сохранённые.
        Count
    };
};


// Режим наименования файлов.
struct FileNamingMode
{
    enum E
    {
        Mask,        // Именовать по маске.
        Manually     // Именовать вручную.
    };
};

// Что показывать в режиме Внутр ЗУ - считанный или записанный сигнал.
struct ModeShowIntMem
{
    enum E
    {
        Direct,  // Показывать данные реального времени.
        Saved,   // Показывать сохранённые данные.
        Both     // Показывать данные реального времени и сохранённые данные.
    };
};

// Что делать при нажатии кнопки ПАМЯТЬ.
struct ModeBtnMemory
{
    enum E
    {
        Menu,     // Будет открывааться соответствующая страница меню.
        Save      // Сохранение содержимого экрана на флешку.
    };
};

// Как сохранять данные на флешку.
struct ModeSaveSignal
{
    enum E
    {
        BMP,     // Сохранять данные на флешку в формате .bmp.
        TXT      // Сохранять данные на флешку в текствовом виде.
    };
};

// Настройки МЕНЮ->ПАМЯТЬ
struct SettingsMemory
{
    static const int MAX_SYMBOLS_IN_FILE_NAME = 35;

    ENUM_POINTS_FPGA::E fpgaENumPoints;                         // Число точек.
    ModeWork::E         modeWork;                               // Режим работы.
    FileNamingMode::E   fileNamingMode;                         // Режим именования файлов.
    char                fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
        // Правила именования.\n
        // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
        // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
        // Примеры\n
        // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
        // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет
        //всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char                fileName[MAX_SYMBOLS_IN_FILE_NAME];     // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask;                 // Индекс текущего символа в режиме задания маски или выбора имени.
    bool                isActiveModeSelect;
    ModeShowIntMem::E   modeShowIntMem;                         // Какие сигналы показывать в режиме внутреннего ЗУ
    bool                flashAutoConnect;                       // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory::E    modeBtnMemory;
    ModeSaveSignal::E   modeSaveSignal;                         // В каком виде сохранять сигнал.
};
