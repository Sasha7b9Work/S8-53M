// 2022/2/11 19:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"
#include "Settings/SettingsCursors.h"
#include "Utils/Text/String.h"


class Page;


struct PageMain
{
    static const Page *self;

    static void EnablePageDebug();
};


struct PageChannelA
{
    static void OnChanged_Input(bool);
    static void OnChanged_Filtr(bool);
    static void OnChanged_Couple(bool);

    static const Page *self;
};


struct PageChannelB
{
    static void OnChanged_Input(bool);
    static void OnChanged_Filtr(bool);
    static void OnChanged_Couple(bool);

    static const Page *self;
};


struct PageCursors
{
    // Получить позицию курсора напряжения.
    static float GetCursPosU(Chan, int numCur);

    // Возвращает true,если нужно рисовать курсоры.
    static bool NecessaryDrawCursors();

    // Получить строку курсора напряжения.
    static String<> GetCursVoltage(Chan, int numCur);

    // Получить строку курсора времени.
    static String<> GetCursorTime(Chan, int numCur);

    // Получить строку процентов курсоров напряжения.
    static String<> GetCursorPercentsU(Chan);

    // Получить строку процентов курсоров времени.
    static String<> GetCursorPercentsT(Chan);

    static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);

    static void Cursors_Update();    // Вызываем эту функцию для каждого измерения, чтобы обновить положие курсоров, если они должны обновляться автоматически.

    struct PageSet
    {
        static const Page *self;
    };

    static const Page *self;
};


struct PageDebug
{
    static const Page *self;

    struct Console
    {
        static const Page *self;
    };

    struct SerialNumber
    {
        static const Page *self;
    };

    struct _ADC
    {
        // Сбросить калибровочные коэффициенты смещения канала
        static void ResetCalRShift(Chan);

        static void ResetCalStretch(Chan);

        static const Page *self;

        struct Balance
        {
            static const Page *self;
        };

        struct Stretch
        {
            static const Page *self;
        };

        struct Shift
        {
            static const Page *self;
        };

        struct AltShift
        {
            static const Page *self;
        };
    };

    struct Randomizer
    {
        static const Page *self;
    };
};


struct PageDisplay
{
    static const Page *self;

    static void OnChanged_RefreshFPS(bool);

    struct Accumulation
    {
        static const Page *self;

        static void OnPress_Clear();
    };

    struct Grid
    {
        static const Page *self;

        static ColorType colorType;
    };
};


struct PageHelp
{
    static const Page *self;
};


struct PageMeasures
{
    static const Page *self;

    struct Tune
    {
        static void *pointer;
    };

    // Если true, то активен выбор типа измерений для выбора на странице ИЗМЕРЕНИЯ-НАСТРОИТЬ
    static bool choiceMeasuresIsActive;
};


struct PageMemory
{
    static void OnChanged_NumPoints(bool active = true);
    static void OnPressExtFileManager();
    static void SaveSignalToFlashDrive();

    struct Latest
    {
        static int16 current;               // Текущий номер последнего сигнала в режиме ПАМЯТЬ - Последние

        static const Page *self;
    };

    struct Internal
    {
        static int8 currentSignal;          // Текущий номер сигнала, сохранённого в ППЗУ
        static bool showAlways;             // Если true, то показывать всегда выбранный в режиме "Внутр. ЗУ" сигнал

        static const Page *self;
    };

    struct External
    {
        static const Page *self;
    };

    struct SetMask
    {
        static const Page *self;
    };

    struct SetName
    {
#define RETURN_TO_MAIN_MENU             0U
#define RETURN_TO_LAST_MEM              1U
#define RETURN_TO_INT_MEM               2U
#define RETURN_TO_DISABLE_MENU          3U

#define EXIT_FROM_SET_NAME_TO_MAIN_MENU (PageMemory::SetName::exitTo == RETURN_TO_MAIN_MENU)
#define EXIT_FROM_SET_NAME_TO_LAST      (PageMemory::SetName::exitTo == RETURN_TO_LAST_MEM)
#define EXIT_FROM_SET_NAME_TO_INT       (PageMemory::SetName::exitTo == RETURN_TO_INT_MEM)
#define EXIT_FROM_SET_NAME_TO_DIS_MENU  (PageMemory::SetName::exitTo == RETURN_TO_DISABLE_MENU)
        static uint8 exitTo;    // Куда возвращаться из окна установки имени при сохранении : 0 - в основное меню, 1 - в окно 
                                // последних, 2 - в окно Внутр ЗУ, 3 - в основно окно в выключенным меню
        static const Page *self;
    };

    struct FileManager
    {
        static const Page *self;
    };

    // Управляет состояниями при переключениях ПАМЯТЬ-ПОСЛЕДНИЕ, ПАМЯТЬ-ВНУТР ЗУ, ПАМЯЬ-НЕПОСР
    struct Resolver
    {
        static bool exitFromIntToLast;          // Если true, то выходить из страницы внутренней памяти нужно не
                                                // стандартно, а в меню последних

        static void OnPress_MemoryLatestEnter();
        static void OnPress_MemoryLatestExit();
        static void OnPress_MemoryLatestEnterToInternal();

        static void OnPress_MemoryInternalEnter();
        static void OnPress_MemoryInternalExit();
    };

    static const Page *self;
};


struct PageService
{
    static const Page *self;

    // Если wait_key, то нужно ждать нажатия клавиши ПУСК
    static void ResetSettings(bool wait_key);

    static void OnPress_AutoSearch();

    struct Math
    {
        static bool Enabled();

        struct Function
        {
            static const Page *self;
        };

        struct FFT
        {
            struct Cursors
            {
                static const Page *self;
            };

            static const Page *self;
        };
    };

    struct LAN
    {
        static const Page *self;
    };

    struct Information
    {
        static const Page *self;
    };

    struct Calibrator
    {
        static const Page *self;
    };
};


struct PageTime
{
    static const Page *self;

    static void OnChanged_TPos(bool);
};


struct PageTrig
{
    static const Page *self;

    static void OnPress_Mode(bool);

    struct AutoFind
    {
        static const Page *self;
    };
};


#define COMMON_BEGIN_SB_EXIT  0, "Выход", "Exit", "Кнопка для выхода в предыдущее меню", "Button for return to the previous menu"


bool IsMainPage(void *item);            // Возвращает true, если item - адрес главной страницы меню.


// Рассчитывает условия отрисовки УГО малых кнопок управления выбором курсорами.
void CalculateConditions(int16 pos0, int16 pos1, CursCntrl::E, bool *cond0, bool *cond1);
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
