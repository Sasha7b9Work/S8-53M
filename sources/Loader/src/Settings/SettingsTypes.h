// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Display/Colors.h"
#include "Utils/Containers/String.h"
#include "Utils/Containers/Values.h"


struct Chan { enum E {
        A,
        B,
        A_B,
        Math,
        Count = 2
    } value;

    Chan(E v = A) : value(v) {}
    void Enable() const;
    void Disable() const;
    bool IsEnabled() const;
    bool IsA() const        { return (value == A);    }
    bool IsB() const        { return (value == B);    }
    bool IsAB() const       { return (value == A_B);  }
    bool IsMath() const     { return (value == Math); }
    operator int() const    { return (int)value;      }
    Color GetColor() const;
    bool IsInversed() const;
    int ToNumber() const;

    // Данные этого канала нужно отрисовывать первыми
    static Chan &FirstDrawing();

    // Данные этого канала нужно отрисовывать последними
    static Chan &SecondDrawing();

    // Возвращает "противоположный канал"
    Chan Opposed() const;
};

extern Chan ChA;
extern Chan ChB;


// Полярность синхронизацц
struct TrigPolarity { enum E {
        Front,  // Синхронизация по фронту.
        Back    // Синхронизация по срезу.
    };

    static void Set(E polarity);

    // Переключение полярности - принудетиельный запуск сбора информации
    static void Switch();

    static void Load();

    static uint timeSwitch;     // Время принудительного запуска сбора информации. Установленное в 0 значение означает,
                                // что принудительный запуск не производился
};


// Минимальное значение на вертикальной координате графика спектра.
struct MaxDbFFT { enum E {
        _40,        // Вертикальная координата до -40Дб
        _60,        // Вертикальная координата до -60Дб
        _80         // Вертикальная координата до -80Дб
    };

    static float GetReal(E);
};



// Масштаб по времени
struct TBase { enum E {
        _1ns,
        _2ns,
        _5ns,
        _10ns,
        _20ns,
        _50ns,
        _100ns,
        _200ns,
        _500ns,
        _1us,
        _2us,
        _5us,
        _10us,
        _20us,
        _50us,
        _100us,
        _200us,
        _500us,
        _1ms,
        _2ms,
        _5ms,
        _10ms,
        _20ms,
        _50ms,
        _100ms,
        _200ms,
        _500ms,
        _1s,
        _2s,
        _5s,
        _10s,
        Count
    };

    static void Set(TBase::E tbase);

    static void Load();

    static void Decrease();

    static void Increase();

    static pchar Name(TBase::E tbase);

    static pchar ToString(TBase::E tbase);
    static pchar ToStringEN(TBase::E tbase);

    static E MIN_P2P;       // С этой развёртки начинается поточечный вывод
    static E MIN_PEAK_DET;  // С этой развёртки можно включать пиковый детектор
    static E MAX_RANDOMIZE;  // Максимальная развёртка, на которой работает рандомизатор

    // Возвращает шаг между точками для рандомизатора при установленной развёртке
    static int StepRand();
};


struct TShift
{
    static void Set(int tshift);

    static void Draw();

    // Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть
    // разное.
    static void SetDelta(int16 shift);

    // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран
    static String ToString(int tshift_rel);

    // Минимальное смещение по времени, которое может быть записано в аппаратную часть.
    static int16 Min();

    // Смещение по времени, соответствующее позиции TPos.
    static int16 Zero()
    {
        return -TShift::Min();
    }

    // Смещение по времени, используемое для засылки в пред/после- запуски
    static int ForLaunchFPGA();

    // tshift может принимать нецелые значения в результате расчёта
    static Time ToAbs(float shift, TBase::E base);
    static Time ToAbs(int shift, TBase::E base);

    static float ToRel(float tshift_abs, TBase::E tbase);

    static const int NULL_VALUE = 1000000;

    static const int16 MAX = 16000;

    static const float abs_step[TBase::Count];

    static int add_rand;                // Дополнительное смещение в режиме рандомизатора
};


// Источник синхронизации.
struct TrigSource { enum E {
        A,      // Канал 1.
        B,      // Канал 2.
        Ext     // Внешняя.
    };

    static void Set(E);
    static void Set(const Chan &);
};


// Делитель.
struct Divider { enum E {
        _1,
        _10
    };

    static int ToAbs(E multiplier)
    {
        static const int results[2] = { 1, 10 };

        return results[multiplier];
    }
    static int ToAbs(const Chan &ch);
};


// Масштаб по напряжению.
struct Range { enum E {
        _2mV,
        _5mV,
        _10mV,
        _20mV,
        _50mV,
        _100mV,
        _200mV,
        _500mV,
        _1V,
        _2V,
        _5V,
        _10V,
        _20V,
        Count
    };

    static void Set(const Chan &, E);

    static void Load(const Chan &);

    static bool Increase(const Chan &);

    static bool Decrease(const Chan &);

    static pchar Name(Range::E);

    static pchar ToString(Range::E, Divider::E);
};

Range::E &operator++(Range::E &);
Range::E &operator--(Range::E &);


struct RShift
{
    static void Set(const Chan &ch, int16 rShift);

    static void Load(const Chan &ch);

    static int ToRel(float rshift_abs, Range::E);

    static Voltage ToAbs(int16 rshift, Range::E);

    static Voltage ToAbs(int16 rshift, Range::E, Divider::E multiplier);

    static void Draw();

    static pchar ToString(int16 rshift, Range::E, Divider::E multiplier);

    static bool show_line[Chan::Count];    // Нужно ли рисовать горизонтальную линию уровня смещения каналов

    static const int16 MIN = 20;    // Минимальное значение смещения канала по напряжению, засылаемое в аналоговую часть
                                    // Соответствует смещению 10 клеток вниз от центральной линии
    static const int16 ZERO = 500;  // Соответствует расположению марера по центру экрана.
    static const int16 MAX = 980;   // Соответствует смещению 10 клеток вверх от центральной лиини.

    static int16 STEP();            // На столько единиц нужно изменить значение смещения, чтобы маркер смещения по
                                    // напряжению передвинулся на одну точку

private:

    static void Draw(const Chan &ch);

    static void OnChanged(const Chan &ch);

    static void FuncOnTimerDisableShowA();
    static void FuncOnTimerDisableShowB();

    // Отключить вспомогательную линию маркера смещения по напряжению
    static void DisableShowLine(const Chan &ch);

    static const float abs_step[Range::Count];
};


struct TrigLev
{
    static void Load();

    static void Set(TrigSource::E ch, int16 trigLev);
    static void Set(const Chan &ch, int16 trigLev);

    // Найти и установить уровень синхронизации по последнему считанному сигналу
    static void FindAndSet();

    static void Draw();

    // Отобразить значение на экране
    static void WriteValue();

    static bool need_auto_find;   // Если true, то нужно произвести поиск синхронизации

    static const int16 MIN = RShift::MIN;
    static const int16 MAX = RShift::MAX;
    static const int16 ZERO = RShift::ZERO;     // Нулевое значение уровня синхронизации, засылаемое в прибор.
                          // Маркер синхронизации при этом находитися на одном уровне с маркером смещения по напряжению.

    static bool show_level;                     // Нужно ли отображать значение на сетке
};


// Число точек сигнала, с которым идёт работа.
struct EnumPointsFPGA { enum E {
        _281,
        _512,
        _1k,
        Count
    };

    // Переводит перечисление в количество точек
    static int ToPoints(E v)
    {
        static const int points[Count] =
        {
            281,
            512,
            1024
        };

        return points[v];
    }

    static E FromPoints(uint num_points);

    // Столько точек в одном канале при текущих настройках
    static int PointsInChannel();

    // Столько байт занимает один канал при текущих настройках
    static int BytesInChannel();

    // Столько байт нужно для сохранения данных обоих каналов при текущих настройках
    static int BytesForData()
    {
        int result = 0;

        int bytes_in_channel = BytesInChannel();

        if (ChA.IsEnabled()) { result += bytes_in_channel; }

        if (ChB.IsEnabled()) { result += bytes_in_channel; }

        return result;
    }
};


// Привязка синхронизации к памяти.
struct TPos { enum E {
        Left,      // Синхронизация привязана к началу памяти.
        Center,    // Синхронизация привязана к центру памяти.
        Right,     // Синхронизация привязана к концу памяти.
        Count
    };

    // Узнать привязку отсительно уровня синхронизации в точках
    static int InPoints(EnumPointsFPGA::E enum_points, E t_pos);

    static void Draw();

    static void Draw(int x, int y);
};


// Через какое время после последнего нажатия кнопки скрывать меню.
struct MenuAutoHide { enum E {
        None,   // Никогда.
        _5,     // Через 5 секунд.
        _10,    // Через 10 секунд.
        _15,    // Через 15 секунд.
        _30,    // Через 30 секунд.
        _60     // Через 60 секунд.
    };

    // Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
    static int Time(E);
};


// Количество измерений для расчёта минимального и максимального значений.
struct ENumMinMax { enum E {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128
    };

    static int ToAbs(E value) { return (1 << value); }
    // Возвращает 0, если value==_1
    static int ToDisplayValue(E);
};


// Количество точек для расчёта скользящего фильтра.
struct Smoothing { enum E {
        Disable,
        _2points,
        _3points,
        _4points,
        _5points,
        _6points,
        _7points,
        _8points,
        _9points,
        _10points
    };

    // Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
    static uint NumPoints(E);
};


// Ограничение FPS.
struct ENumSignalsInSec { enum E {
        _25,
        _10,
        _5,
        _2,
        _1,
        Count
    };

    // Возвращает ограничение частоты кадров.
    static int NumSignalsInS(E);

    // Возвращает промежуток времени между отрисовками
    static uint DeltaTime(E);
};


// Тип усреднений по измерениям.
struct ModeAveraging { enum E {
        Accurately,   // Усреднять точно.
        Around        // Усреднять приблизительно.
    };

    // Возвращает режим усреднения
    static ModeAveraging::E Current();
};


// Количество усреднений по измерениям.
struct ENumAveraging { enum E {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        _256,
        _512
    };

    static int NumAverages(E);
    static int ToAbs(E);
    // Возвращает 0, если E==_1
    static int ToDisplayValue(E);
};


// Перечисление накоплений.
struct ENumAccumulation { enum E {
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

    // Преобразование в значение для вывода на экран. 1 - 0, Infinity - -1
    static int ToDisplayValue(E);
};


// Режим канала по входу.
struct ModeCouple { enum E {
        DC,      // Открытый вход.
        AC,      // Закрытый вход.
        GND      // Вход заземлён.
    };

    static void Set(const Chan &, E);
};


// Тип выборки для режима рандомизатора.
struct SampleType { enum E {
    Real,   // реальное время - в построении участвуют только реально считанные точки, ничего не рассчитывается.
    Equal   // эквивалентная - сигнал строится по последним точкам, полученным от рандомизатора.
};};


// Источинк сигнала для отображения спектра.
struct SourceFFT { enum E {
    A_,          // Расчёт и отображение спектра первого канала.
    B_,          // Расчёт и отображение спектра второго канала.
    Both_        // Расчёт и отображение спектров обоих каналов.
};};


// Язык меню
struct Language { enum E
{
    Russian,    // Русский
    English,    // Английский
    Count
};};


// Нужно ли учитывать калибровчные коэффициенты для смещения
struct RShiftADCtype { enum E
{
    Disabled,
    Enabled
};};


// Режим отрисовки сигнала.
struct ModeDrawSignal { enum E {
    Lines,      // Сигнал рисуется линиями.
    Points,     // Сигнал рисуется точками.
    Count
};};


// Тип сетки на экране.
struct TypeGrid { enum E {
    _1,
    _2,
    _3,
    _4,
    Count
};};


// Цветовая схема
struct ColorScheme { enum E {
    WhiteLetters,   // В этом случае заголовки элементов меню пишутся белым - не очень хорошо видно снизу.
    BlackLetters    // В этом случае заголовки элементов меню пишутся чёрным - не очень красиво выглядит.
};};


// Режим накопления.
struct ModeAccumulation { enum E {
    NoReset,   // В этом режиме показываются строго N последних измерений.
    Reset      // В этом режиме набираются N последних измерений и потом сбрасываются.
};};


// Режим отображения дополнительных боковых маркеров смещения по напряжению.
struct AltMarkers { enum E {
    Hide,        // Никода не выводить.
    Show,        // Всегда выводить.
    AutoHide     // Выводить и прятать через timeShowLevels.
};};


// Тип привязки к смещению по вертикали
struct LinkingRShift { enum E {
    Voltage,      // Смещение привязано к напряжению
    Position      // Смещение привязаоно к позиции на экране
};};


// Количество и расположение на экране результатов измерений.
struct MeasuresNumber { enum E {
    _1,             // 1 измерение слева внизу.
    _2,             // 2 измерения слева внизу.
    _1_5,           // 1 строка с 5 измерениями.
    _2_5,           // 2 строки по 5 измерений.
    _3_5,           // 3 строки по 5 измерений.
    _6_1,           // 6 строк по 1 измерению.
    _6_2            // 6 строк по 2 измерения.
};};


// Зона, по которой считаются измрения
struct MeasuresField { enum E {
    Screen,         // Измерения будут производиться по той части сингала, которая видна на экране.
    AllMemory,      // Измерения будут производиться по всему сигналу.
    Hand            // Измерения будут производиться по окну, задаваемому пользователем.
};};


// Режим работы.
struct ModeWork { enum E {
        Direct,        // Основной режим.
        Latest,        // В этом режиме можно просмотреть последние сохранённые измерения.
        MemInt,        // В этом режиме можно сохранять во flash-памяти измерения просматривать ранее сохранённые.
    };
    E v;
    ModeWork(E _v = Direct) : v(_v) { }
    bool IsDirect() const { return v == Direct; }
    bool IsLatest() const { return v == Latest; }
    bool IsMemInt() const { return v == MemInt; }
    operator int() { return (int)v; }
};


// Режим наименования файлов.
struct FileNamingMode { enum E {
    Mask,        // Именовать по маске.
    Manually     // Именовать вручную.
};};


// Что показывать в режиме Внутр ЗУ - считанный или записанный сигнал.
struct ModeShowIntMem { enum E {
    Direct,  // Показывать данные реального времени.
    Saved,   // Показывать сохранённые данные.
    Both     // Показывать данные реального времени и сохранённые данные.
};};


// Что делать при нажатии кнопки ПАМЯТЬ.
struct ModeBtnMemory { enum E {
    Menu,     // Будет открывааться соответствующая страница меню.
    Save      // Сохранение содержимого экрана на флешку.
};};


// Как сохранять данные на флешку.
struct ModeSaveSignal { enum E {
    BMP,     // Сохранять данные на флешку в формате .bmp.
    TXT      // Сохранять данные на флешку в текствовом виде.
};};


// Функция ручки ВРЕМЯ/ДЕЛ.
struct FunctionTime { enum E {
    Time,              // Ручка управляет смещением по времени.
    ShiftInMemory      // Ручка управляет отображаемым на экране участком памяти.
};};


// Режим работы пикового детектора.
struct PeakDetMode { enum E {
    Disable,
    Enable,
    Average,
    Count
};};


// Режим запуска.
struct StartMode { enum E {
    Auto,             // Автоматический.
    Wait,             // Ждущий.
    Single            // Однократный.
};};


// Вход синхронизации.
struct TrigInput { enum E {
        Full,   // Полный сиганл.
        AC,     // Переменный.
        LPF,    // ФНЧ.
        HPF     // ФВЧ.
    };
    
    static void Set(E);
};


// Режим длительного нажатия кнопки СИНХР.
struct ModeLongPressTrig { enum E {
    Zero,   // Сброс уровня синхронизации в 0.
    Auto    // Автоматический поиск синхронизации - уровень устанавливается посередине между максимумом и минимумом
};};


// Режим установки синхронизации.
struct TrigModeFind { enum E {
    Hand,   // Уровень синхронизации устанавливается вручную.
    Auto    // Подстройки уровня синхронизации производится автоматически после каждого нового считанного сигнала
};};


// Каким курсором управлять.
struct CursCntrl { enum E {
    _1,            // первым.
    _2,            // вторым.
    _1_2,          // обоими.
    Disabled       // никаким.
};};


// Дискретность перемещения курсоров.
struct CursMovement { enum E {
    Points,    // по точкам.
    Percents   // по процентам.
};};


// Какие курсоры сейчас активны. Какие активны, те и будут перемещаться по вращению ручки УСТАНОВКА.
struct CursActive { enum E {
    U,
    T,
    None
};};


// Режим слежения курсоров.
struct CursLookMode { enum E {
    None,      // Курсоры не следят.
    Voltage,   // Курсоры следят за напряжением автоматически.
    Time,      // Курсоры следят за временем автоматически.
    Both       // Курсоры следят за временем и напряжением, в зависимости от того, какой курсоры вращали последним.
};};


// Сжимать ли сигналы при выводе измерений.
struct ModeViewSignals { enum E {
    AsIs,       // Показывать сигналы как есть
    Compress    // Сжимать сетку с сигналами
};};


struct ScaleMath { enum E
{
    Channel0,     // Масштаб берётся из канала 1
    Channel1,     // Масштаб берётся из канала 2
    Hand          // Масштаб задаётся вручную.
};};


// Масштаб шкалы для отображения спектра.
struct ScaleFFT { enum E {
    Log,        // Это значение означает логарифмическую шкалу вывода спектра.
    Linear      // Это значение означает линейную шкалу вывода спектра.
};};


// Вид окна для умножения сигнала перед расчётом спектра.
struct WindowFFT { enum E {
    Rectangle,  // Перед расчётом спектра сигнал не умножается на окно.
    Hamming,    // Перед расчётом спектра сигнал умножается на окно Хэмминга.
    Blackman,   // Перед расчётом спектра сигнал умножается на окно Блэкмана.
    Hann        // Перед расчётом спектра сигнал умножается на окно Ханна.
};};


// Какую функцию рассчитывать.
struct Function { enum E {
    Sum,        // Сумма двух каналов.
    Mul         // Произведение двух каналов.
};};


// Где отрисовывать математический сигнал.
struct ModeDrawMath { enum E    {
    Disable,    // Отображение математического сигнала выключено.
    Separate,   // Математический сигнал отображается на отдельном экране.
    Together    // Математический сигнал отображается поверх текущего сигнала.
};};


// Чем управляет ручка УСТАНОВКА, когда включена математическая функция.
struct ModeRegSet { enum E {
    Range,      // Ручка УСТАНОВКА управляет масштабом по напряжению.
    RShift      // Ручка УСТАНОВКА управляет смещением по напряжению.
};};


// Режим работы калибратора.
struct CalibratorMode { enum E {
    Freq,           // На выходе калибратора 4В, 1кГц.
    DC,             // На выходе калибратора 4В постоянного напряжения.
    GND             // На выходе калибратора 0В.
};};


struct CalibrationMode { enum E {
    x1,
    x10,
    Disable
};};
