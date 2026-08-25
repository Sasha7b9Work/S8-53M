// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"
#include "SettingsTypes.h"
#include "Display/Colors.h"


#define TIME_SHOW_LEVELS            (gset.display.timeShowLevels)

#define SHIFT_IN_MEMORY             (gset.display.shiftInMemory)

#define TIME_MESSAGES               (gset.display.timeMessages)

#define BRIGHTNESS                  (gset.display.brightness)

#define BRIGHTNESS_GRID             (gset.display.brightnessGrid)

#define MODE_DRAW_SIGNAL            (gset.display.modeDrawSignal)
#define MODE_DRAW_IS_SIGNAL_LINES   (MODE_DRAW_SIGNAL == ModeDrawSignal::Lines)

#define TYPE_GRID                   (gset.display.typeGrid)
#define TYPE_GRID_IS_1              (TYPE_GRID == TypeGrid::_1)
#define TYPE_GRID_IS_2              (TYPE_GRID == TypeGrid::_2)
#define TYPE_GRID_IS_3              (TYPE_GRID == TypeGrid::_3)

#define ENUM_ACCUM                  (gset.display.enumAccumulation)
#define ENUM_ACCUM_IS_NONE          (ENUM_ACCUM == ENumAccumulation::_1)
#define ENUM_ACCUM_IS_INFINITY      (ENUM_ACCUM == ENumAccumulation::Infinity)
#define NUM_ACCUM                   (1 << (int)gset.display.enumAccumulation)

#define ENUM_MIN_MAX                (gset.display.enumMinMax)
#define NUM_MIN_MAX                 (1 << ENUM_MIN_MAX)

#define SMOOTHING                   (gset.display.smoothing)
#define SMOOTHING_IS_DISABLE        (SMOOTHING == Smoothing::Disable)

#define ENUM_SIGNALS_IN_SEC         (gset.display.enumSignalsInSec)

#define LAST_AFFECTED_CHANNEL       (gset.display.lastAffectedChannel)
#define LAST_AFFECTED_CHANNEL_IS_A  (LAST_AFFECTED_CHANNEL == Chan::A)
#define LAST_AFFECTED_CHANNEL_IS_B  (LAST_AFFECTED_CHANNEL == Chan::B)

#define MODE_ACCUM                  (gset.display.modeAccumulation)
#define MODE_ACCUM_IS_RESET         (MODE_ACCUM == ModeAccumulation::Reset)
#define MODE_ACCUM_IS_NORESET       (MODE_ACCUM == ModeAccumulation::NoReset)

#define ALT_MARKERS                 (gset.display.altMarkers)
#define ALT_MARKERS_HIDE            (ALT_MARKERS == AltMarkers::Hide)

#define MENU_AUTO_HIDE              (gset.display.menuAutoHide)

#define SHOW_STRING_NAVIGATION      (gset.display.showStringNavigation)

#define LINKING_RSHIFT              (gset.display.linkingRShift)
#define LINKING_RSHIFT_IS_VOLTAGE   (LINKING_RSHIFT == LinkingRShift::Voltage)



 // Режим отрисовки сигнала.
struct ModeDrawSignal
{
    enum E
    {
        Lines = 0,   // Сигнал рисуется линиями.
        Points = 1   // Сигнал рисуется точками.
    };
};

// Тип сетки на экране.
struct TypeGrid
{
    enum E
    {
        _1,
        _2,
        _3,
        _4,
        Count
    };
};

// Перечисление накоплений.
struct ENumAccumulation
{
    enum E
    {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        Infinity
    };

    static int number_drawing;      // Число уже нарисованных сигналов
};

// Количество усреднений по измерениям.
struct ENumAveraging
{
    enum E
    {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        _256,
        _512,
        Count
    };

    static int ToNumber();
};

// Тип усреднений по измерениям.
struct ModeAveraging
{
    enum E
    {
        Accurately,   // Усреднять точно.
        Around,       // Усреднять приблизительно.
        Count
    };

    static bool IsAccurately() { return Current() == Accurately; }
    static bool IsAround()     { return Current() == Around;     }
    static E Current();
};

// Количество измерений для расчёта минимального и максимального значений.
struct ENumMinMax
{
    enum E
    {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        Count
    };
};

// Количество точек для расчёта скользящего фильтра.
struct Smoothing
{
    enum E
    {
        Disable,
        _2points = 1,
        _3points = 2,
        _4points = 3,
        _5points = 4,
        _6points = 5,
        _7points = 6,
        _8points = 7,
        _9points = 8,
        _10points = 9
    };

    // Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
    static int ToPoints();
};

// Ограничение FPS.
struct ENumSignalsInSec
{
    enum E
    {
        _25,
        _10,
        _5,
        _2,
        _1
    };

    static ENumSignalsInSec::E FromNum(int num);
    static int ToNum(ENumSignalsInSec::E);
};

// Режим накопления.
struct ModeAccumulation
{
    enum E
    {
        NoReset,   // В этом режиме показываются строго N последних измерений.
        Reset      // В этом режиме набираются N последних измерений и потом сбрасываются.
    };
};

// Режим отображения дополнительных боковых маркеров смещения по напряжению.
struct AltMarkers
{
    enum E
    {
        Hide,        // Никода не выводить.
        Show,        // Всегда выводить.
        AutoHide     // Выводить и прятать через timeShowLevels.
    };
};

// Через какое время после последнего нажатия кнопки скрывать меню.
struct MenuAutoHide
{
    enum E
    {
        None = 0,  // Никогда.
        _5 = 5,     // Через 5 секунд.
        _10 = 10,   // Через 10 секунд.
        _15 = 15,   // Через 15 секунд.
        _30 = 30,   // Через 30 секунд.
        _60 = 60    // Через 60 секунд.
    };
};

// Тип привязки к смещению по вертикали
struct LinkingRShift
{
    enum E
    {
        Voltage,      // Смещение привязано к напряжению
        Position      // Смещение привязаоно к позиции на экране
    };
};



// Настройки изображения
struct SettingsDisplay
{
    int16               timeShowLevels;         // Время, в течение которого нужно показывать уровни смещения.
    int16               shiftInMemory;          // Показывает смещение левого края сетки относительно нулевого
                                                // байта памяти. Нужно для правильного отображения сигнала в окне.
    int16               timeMessages;           // Время в секундах, на которое сообщение остаётся на экране.
    int16               brightness;             // Яркость дисплея.
    int16               brightnessGrid;         // Яркость сетки от 0 до 100.
    uint                colors[16];             // Цвета.
    ModeDrawSignal::E   modeDrawSignal;         // Режим отрисовки сигнала.
    TypeGrid::E         typeGrid;               // Тип сетки.
    ENumAccumulation::E enumAccumulation;       // Перечисление накоплений сигнала на экране.
    ENumAveraging::E    enumAve;                // Перечисление усреднений сигнала.
    ModeAveraging::E    modeAve;                // Тип усреднений по измерениям.
    ENumMinMax::E       enumMinMax;             // Число измерений для определения минимумов и максимумов.
    Smoothing::E        smoothing;              // Число точек для скользящего фильтра.
    ENumSignalsInSec::E enumSignalsInSec;       // Перечисление считываний сигнала в секунду.
    Chan                lastAffectedChannel;    // Здесь хранится номер последнего канала, которым управляли ручками.
                                                // Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation::E modeAccumulation;       // Задаёт режим накопления сигналов.
    AltMarkers::E       altMarkers;             // Режим отображения дополнительных боковых маркеров смещений.
    MenuAutoHide::E     menuAutoHide;           // Через сколько времени после последнего нажатия клавиши прятать меню.
    bool                showFullMemoryWindow;   // Показывать ли окно памяти вверху экрана. \todo Не используется.
    bool                showStringNavigation;   // Показывать ли строку текущего состояния меню..
    LinkingRShift::E    linkingRShift;          // Тип привязки смещения по вертикали.

    // Возвращает index первой и последней точки на экране. Вернее, второе значение - точка после 
    static BitSet32 PointsOnDisplay();

    // Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
    static int TimeMenuAutoHide();

    // Если экран разделён на две части и основной сигнал выводится сверху - например, в режиме вывода спектра
    static bool IsSeparate();
};
