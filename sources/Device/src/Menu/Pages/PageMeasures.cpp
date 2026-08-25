// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "Settings/SettingsTypes.h"
#include "Data/Measures.h"
#include "Utils/Math.h"
#include "Definition.h"
#include "Menu/MenuItems.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Colors.h"
#include "defines.h"
#include "Display/Painter.h"
#include "Utils/Text/Text.h"


extern const Page pMeasures;
extern const Page mspMeasTune;
extern const Page pMeasures;


bool PageMeasures::choiceMeasuresIsActive = false;


void DrawSB_MeasTune_Settings(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, '\x62');
    Font::Set(TypeFont::_8);
}


void PressSB_MeasTune_Settings()
{
    Measures::ShorPressOnSmallButtonSettings();
}


void DrawSB_MeasTune_Markers(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 2, '\x60');
    Font::Set(TypeFont::_8);
}


void PressSB_MeasTune_Markers()
{
    Measures::ShortPressOnSmallButonMarker();
}


// Каким курсором из активной пары сейчас происходит управление.
CursCntrl::E GetMeasuresCursCntrlActive()
{
    if (MEAS_CURS_ACTIVE_IS_T)
    {
        return MEAS_CURS_CNTRL_T;
    }

    return MEAS_CURS_CNTRL_U;
}


void RotateRegMeasureSetField(int angle)
{
    CursCntrl::E cursCntrl = GetMeasuresCursCntrlActive();
    CursActive::E cursActive = MEAS_CURS_ACTIVE;

    int sign = Math::Sign(angle);

    if (cursCntrl == CursCntrl::_1 || cursCntrl == CursCntrl::_1_2)
    {
        if (cursActive == CursActive::T)
        {
            LIMITATION(MEAS_POS_CUR_T0, MEAS_POS_CUR_T0 - sign, 0, ENUM_POINTS_FPGA::ToNumPoints() - 1);
        }
        else
        {
            LIMITATION(MEAS_POS_CUR_U0, MEAS_POS_CUR_U0 + sign, 0, 200);
        }
    }
    if (cursCntrl == CursCntrl::_2 || cursCntrl == CursCntrl::_1_2)
    {
        if (cursActive == CursActive::T)
        {
            LIMITATION(MEAS_POS_CUR_T1, MEAS_POS_CUR_T1 - sign, 0, ENUM_POINTS_FPGA::ToNumPoints() - 1);
        }
        else
        {
            LIMITATION(MEAS_POS_CUR_U1, MEAS_POS_CUR_U1 + sign, 0, 200);
        }
    }
}


const SmallButton sbMeasTuneSettings        // Настройка измерений.
(
    &mspMeasTune, 0,
    "Настройка", "Setup",
    "Позволяет выбрать необходимые измерения",
    "Allows to choose necessary measurements",
    PressSB_MeasTune_Settings,
    DrawSB_MeasTune_Settings
);


const SmallButton sbMeasTuneMarkers        // Включение/отключение маркера для режима измерений.
(
    &mspMeasTune, 0,
    "Маркер", "Marker",
    "Позволяет установить маркеры для визуального контроля измерений",
    "Allows to establish markers for visual control of measurements",
    PressSB_MeasTune_Markers,
    DrawSB_MeasTune_Markers
);


bool IsActiveChoiceMeasuresNumber()
{
    return SHOW_MEASURES;
}


bool IsActiveChoiceMeasuresChannels()
{
    return SHOW_MEASURES;
}


bool IsActivePageMeasuresFields()
{
    return SHOW_MEASURES;
}


bool IsActiveChoiceMeasuresSignal()
{
    return SHOW_MEASURES;
}


bool IsActiveButtonMeasuresTune()
{
    return SHOW_MEASURES;
}


bool IsActiveButtonMeasuresFieldSet()
{
    return MEAS_FIELD_IS_HAND;
}


const Choice mcMeasuresNumber =
{
    TypeItem::Choice, &pMeasures, IsActiveChoiceMeasuresNumber,
    {
        "Количество", "Number"
        ,
        "Устанавливает максимальное количество выводимых измерений:\n"
        "\"1\" - одно измерение\n"
        "\"2\" - два измерения\n"
        "\"1х5\" - 1 строка с пятью измерениями\n"
        "\"2х5\" - 2 строки с пятью измерениями в каждой\n"
        "\"3х5\" - 3 строки с пятью измерениями в каждой\n"
        "\"6x1\" - 6 строк по одному измерению в каждой\n"
        "\"6х2\" - 6 строк по два измерения в каждой"
        ,
        "Sets the maximum number of output measurements:\n"
        "\"1\" - one measurement\n"
        "\"2\" - two measurements\n"
        "\"1x5\" - 1 line with the five dimensions\n"
        "\"2x5\" - two rows with five measurements in each\n"
        "\"3x5\" - 3 lines with five measurements in each"
        "\"6x1\" - 6 lines, one in each dimension\n"
        "\"6x2\" - 6 lines of two dimensions in each\n"
    },
    {                          
        {"1",   "1"},
        {"2",   "2"},
        {"1x5", "1x5"},
        {"2x5", "2x5"},
        {"3x5", "3x5"},
        {"6x1", "6x1"},
        {"6x2", "6x2"}
    },
    (int8*)&MEAS_NUM
};


const Choice mcMeasuresChannels =
{
    TypeItem::Choice, &pMeasures, IsActiveChoiceMeasuresChannels,
    {
        "Каналы", "Channels",
        "По каким каналам выводить измерения",
        "Which channels to output measurement"
    },
    {   
        {"1",       "1"},
        {"2",       "2"},
        {"1 и 2",   "1 and 2"}
    },
    (int8*)&MEAS_SOURCE
};


const Choice mcMeasuresIsShow =
{
    TypeItem::Choice, &pMeasures, 0,
    {
        "Показывать", "Show",
        "Выводить или не выводить измерения на экран",
        "Output or output measurements on screen"
    },
    {   
        {"Нет", "No"},
        {"Да",  "Yes"}
    },
    (int8*)&SHOW_MEASURES
};


const Choice mcMeasuresSignal =
{
    TypeItem::Choice, &pMeasures, IsActiveChoiceMeasuresSignal,
    {
        "Вид", "View",
        "Уменьшать или нет зону вывода сигнала для исключения перекрытия его результами измерений",
        "Decrease or no zone output signal to avoid overlapping of its measurement results"
    },
    {   
        {"Как есть",    "As is"},
        {"Уменьшать",   "Reduce"}
    },
    (int8*)&MODE_VIEW_SIGNALS
};



extern const Page mspMeasuresField;

// ИЗМЕРЕНИЯ -> ЗОНА -> Область
/*
const Choice mcMeasuresFieldType =
{
    TypeItem::Choice, &mspMeasuresField, 0,
    {
        "Область", "Field",
        "Выбор области, из которой будут браться значения для автоматических измерений",
        "Select an area from which the values will be taken for automatic measurements"
    },
    {   
        {"Экран",   "Screen"},
        {"Память",  "Memorye"},
        {"Курсоры", "Cursors"}
    },
    (int8*)&MEAS_FIELD
};
*/


static void PressSB_MeasTune_Exit()
{
}


static const SmallButton sbExitMeasTune
(
    &mspMeasTune,
    COMMON_BEGIN_SB_EXIT,
    PressSB_MeasTune_Exit,
    DrawSB_Exit
);


static const arrayItems itemsMeasTune =
{
    (void*)&sbExitMeasTune,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)&sbMeasTuneMarkers,
    (void*)&sbMeasTuneSettings
};


static const Page mspMeasTune
(
    &pMeasures, IsActiveButtonMeasuresTune,
    "НАСТРОИТЬ", "CONFIGURE",
    "Переход в режми точной настройки количества и видов измерений",
    "Transition to rezhm of exact control of quantity and types of measurements",
    NamePage::SB_MeasTuneMeas, &itemsMeasTune, EmptyFuncVV, EmptyFuncVV, Measures::RotateRegSet
);


static const arrayItems itemsMeasures =
{
    (void*)&mcMeasuresIsShow,
    (void*)&mcMeasuresNumber,
    (void*)&mcMeasuresChannels,
    (void*)&mcMeasuresSignal,
    (void*)&mspMeasTune    
};

static const Page pMeasures
(
    PageMain::self, 0,
    "ИЗМЕРЕНИЯ", "MEASURES",
    "Автоматические измерения",
    "Automatic measurements",
    NamePage::Measures, &itemsMeasures
);


const Page *PageMeasures::self = &pMeasures;

void *PageMeasures::Tune::pointer = (void *)&mspMeasTune;
