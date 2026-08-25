// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Settings/SettingsTypes.h"


#define SOUND_ENABLED       (set.service.soundEnabled)          // SettingsService.soundEnabled


// Настройки изображения
struct SettingsDisplay
{
    int16               timeShowLevels;             // Время, в течение которого нужно показывать уровни смещения
    int16               shiftInMemory;              // Показывает смещение левого края сетки относительно нулевого байта памяти. Нужно для правильного отображения сигнала в окне.
    int16               timeMessages;               // Время в секундах, на которое сообщение остаётся на экране
    int16               brightness;                 // Яркость дисплея (только для цветного)
    int16               brightnessGrid;             // Яркость сетки от 0 до 100
    uint                colors[16];
    ModeDrawSignal::E   modeDrawSignal;             // Режим отрисовки сигнала
    TypeGrid::E         typeGrid;                   // Тип сетки
    ENumAccumulation::E enumAccumulation;           // Число накоплений сигнала на экране
    ENumAveraging::E    enumAve;                    // Число усреднений сигнала
    ModeAveraging::E    modeAve;                    // Тип усреднений по измерениям
    ENumMinMax::E       enumMinMax;                 // Число измерений для определения минимумов и максимумов
    Smoothing::E        smoothing;                  // Число точек для скользящего фильтра
    ENumSignalsInSec::E enumSignalsInSec;           // Число считываний сигнала в секунду
    Chan::E             lastAffectedChannel;        // Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation::E modeAccumulation;           // Задаёт режим накопления сигналов
    AltMarkers::E       altMarkers;                 // Режим отображения дополнительных боковых маркеров смещений
    MenuAutoHide::E     menuAutoHide;               // Через сколько времени после последнего нажатия клавиши прятать меню 
    bool                showFullMemoryWindow;       // Показывать ли окно памяти вверху экрана.
    bool                showStringNavigation;       // Показывать ли строку текущего состояния меню.
};


// Настройки синхронизации
struct SettingsTrig
{
    StartMode::E         startMode;          // Режим запуска.
    TrigSource::E        source;             // Источник.
    TrigPolarity::E      polarity;           // Тип синхронизации.
    TrigInput::E         input;              // Вход синхронизации.
    ModeLongPressTrig::E modeLongPressTrig;  // Режим работы длительного нажатия кнопки СИНХР.
    TrigModeFind::E      modeFind;           // Поиск синхронизации - вручную или автоматически.
    int16                levelRel[3];        // Уровень синхронизации для трёх источников.
};


// ВременнЫе настройки
struct SettingsTime
{
    bool                selfRecorder;   // Включен ли режим самописца.
    int16               tShiftRel;      // Смещение по времени
    TBase::E            tBase;          // Масштаб по времени.
    FunctionTime::E     timeDivXPos;
    TPos::E             tPos;
    SampleType::E       sampleType;
    PeakDetMode::E     peakDet;
    EnumPointsFPGA::E oldNumPoints;     // Когда переключаемся в режим пикового детектора, устанавливаем количество
                                        // точек в 1024, а сюда записываем то, что было, чтобы потом восстановить
};


// Настройки курсоров
struct SettingsCursors
{
    CursCntrl::E        cntrlU[Chan::Count];     // Активные курсоры напряжения.
    CursCntrl::E        cntrlT[Chan::Count];     // Активные курсоры напряжения.
    Chan::E          source;                     // Источник - к какому каналу относятся курсоры.
    float               posCurU[Chan::Count][2]; // Текущие позиции курсоров напряжения обоих каналов.
    float               posCurT[Chan::Count][2]; // Текущие позиции курсоров времени обоих каналов.
    float               deltaU100percents[2];       // Расстояние между курсорами напряжения для 100%, для обоих каналов.
    float               deltaT100percents[2];       // Расстояние между курсорами времени для 100%, для обоих каналов.
    CursMovement::E     movement;                   // Как перемещаться курсорам - по точкам или по процентам.
    CursActive::E       active;                     // Какие курсоры сейчас активны.
    CursLookMode::E     lookMode[2];                // Режимы слежения за курсорами для двух пар курсоров.
    bool                showFreq;                   // Установленное в true значение, что нужно показывать на экране
                                                    // значение 1/dT между курсорами.
    bool                showCursors;                // Показывать ли курсоры
};

struct StructMemoryLast
{
    bool isActiveModeSelect;
};

// Настройки МЕНЮ->ПАМЯТЬ
struct SettingsMemory
{
#define MAX_SYMBOLS_IN_FILE_NAME 35
    EnumPointsFPGA::E fpgaNumPoints;          // Число точек.
    ModeWork::E         modeWork;               // Режим работы.
    FileNamingMode::E   fileNamingMode;         // Режим именования файлов.
    char                fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
         // Правила именования.\n
         // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
         // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
         // Примеры\n
         // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
         // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char                fileName[MAX_SYMBOLS_IN_FILE_NAME];     // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask; // Индекс текущего символа в режиме задания маски или выбора имени.
    StructMemoryLast    strMemoryLast;
    ModeShowIntMem::E   modeShowIntMem;         // Какие сигналы показывать в режиме внутреннего ЗУ
    bool                flashAutoConnect;       // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory::E    modeBtnMemory;     
    ModeSaveSignal::E   modeSaveSignal;         // В каком виде сохранять сигнал.
};
                                    
struct SettingsMath
{
    ScaleFFT::E     scaleFFT;
    SourceFFT::E    sourceFFT;
    WindowFFT::E    windowFFT;
    MaxDbFFT::E     fftMaxDB;
    Function::E     func;
    uint8           currentCursor;          // Определяет, каким курсором спектра управляет ручка УСТАНОВКА
    uint8           posCur[2];              // Позиция курсора спектра. Изменятеся 0...256.
    int8            koeff1add;              // Коэффициент при первом слагаемом для сложения.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;
    ModeDrawMath::E modeDraw;               // Раздельный или общий дисплей в режиме математической функции
    ModeRegSet::E   modeRegSet;             // Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали
    Range::E        range;
    Divider::E      multiplier;
    int16           rShift;
};

// Эти настройки меняются через МЕНЮ -> СЕРВИС
struct SettingsService
{
    bool              screenWelcomeEnable;    // Будет ли показываться экран приглашения при включении прибора.
    bool              sound_enabled;           // Включены ли звуки.
    int8              IPaddress;              // IP-адрес (временно)
    CalibratorMode::E calibrator;             // Режим работы калибратора.
    ColorScheme::E    colorScheme;            //
};

struct SettingsEthernet
{
    uint8 mac0;
    uint8 mac1;
    uint8 mac2;
    uint8 mac3;
    uint8 mac4;
    uint8 mac5;

    uint8 ip0;
    uint8 ip1;
    uint8 ip2;
    uint8 ip3;

    uint16 port;

    uint8 mask0;
    uint8 mask1;
    uint8 mask2;
    uint8 mask3;

    uint8 gw0;
    uint8 gw1;
    uint8 gw2;
    uint8 gw3;

    bool enable;
};
                                    
struct SettingsCommon
{
    int         countEnables;               // Количество включений. Увеличивается при каждом включении
    int         countErasedFlashData;       // Сколько раз стирался первый сектор с ресурсами
    int         countErasedFlashSettings;   // Сколько раз стирался сектор с настройкаи
    int         workingTimeInSecs;          // Время работы в секундах
    Language::E lang;                       // Язык меню
};

struct OutputRegisters
{
    bool    all;                                // Показывать значения всех регистров.
    bool    flag;                               // Выводить ли флаг готовности.
    bool    rShift0;
    bool    rShift1;
    bool    trigLev;
    bool    range[2];
    bool    chanParam[2];
    bool    trigParam;
    bool    tShift;
    bool    tBase;
};


// Струкура хранит все настройки прибора.
struct Settings
{
    SettingsDisplay     display;                    // настройки изображения          (меню ДИСПЛЕЙ)
    SettingsTime        time;                       // временнЫе настройки            (меню РАЗВЁРТКА)
    SettingsCursors     cursors;                    // настройки курсорных измерений  (меню КУРСОРЫ)
    SettingsMemory      memory;                     // настройки режимов памяти       (меню ПАМЯТЬ)
    SettingsMath        math;                       // настройки режима математических измерений
    SettingsService     service;                    // дополнительные настройки       (меню СЕРВИС)
    SettingsEthernet    eth;
    SettingsCommon      common;                     // системные настройки
    int temp;

    static void Load();
};

extern Settings set;
