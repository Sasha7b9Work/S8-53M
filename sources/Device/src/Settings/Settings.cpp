// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Settings.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"
#include "Menu/Menu.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>
#include <cstring>


bool Settings::need_save = false;
uint Settings::time_save = 0;
bool Settings::isLoaded = false;
bool Settings::needReset = false;


static const Settings defaultSettings =
{
    0,                              // size
    // Display
    {
        5,                          // timeShowLevels
        512 - 140,                  // shiftInMemory
        5,                          // timeMessages
        100,                        // brightness
        20,                         // brightnessGrid
        {
            Color::Make(0x00, 0x00, 0x00),   // Color::BLACK = 0x00,
            Color::Make(0xFF, 0xFF, 0xFF),   // Color::WHITE = 0x01,
            Color::Make(0x80, 0x80, 0x80),   // Color::GRID = 0x02,
            Color::Make(0x01, 0xCA, 0xFF),   // Color::DATA_A = 0x03,
            Color::Make(0x00, 0xFF, 0x00),   // COLOR_DATA_B = 0x04,
            Color::Make(0xD5, 0xDA, 0xD5),   // Color::MENU_FIELD = 0x05,
            Color::Make(0xFF, 0xB2, 0x00),   // COLOR_MENU_TITLE = 0x06,
            Color::Make(0x7B, 0x59, 0x00),   // Color::MENU_TITLE_DARK = 0x07,
            Color::Make(0xFF, 0xFF, 0x00),   // Color::MENU_TITLE_BRIGHT = 0x08,
            Color::Make(0x69, 0x89, 0x00),   // COLOR_MENU_ITEM = 0x09,
            Color::Make(0x6B, 0x45, 0x00),   // Color::MENU_ITEM_DARK = 0x0a,
            Color::Make(0xFF, 0xCE, 0x00),   // Color::MENU_ITEM_BRIGHT = 0x0b,
            Color::Make(0x00, 0x00, 0x00),   // Color::MENU_SHADOW = 0x0c,
            Color::Make(0x00, 0x00, 0x00),   // COLOR_EMPTY = 0x0d,
            Color::Make(0x08, 0xA2, 0xCE),   // COLOR_EMPTY_A = 0x0e,
            Color::Make(0x00, 0xCE, 0x00)    // COLOR_EMPTY_B = 0x0f,
        },
        ModeDrawSignal_Lines,       // modeDrawSignal
        TypeGrid_1,                 // typeGrid
        NumAccumulation_1,          // numAccumulation
        ENumAveraging::_1,          // numAveraging
        ModeAveraging::Accurately,  // modeAveraging
        NumMinMax_1,                // enumMinMax
        Smoothing::Disable,         // smoothing
        ENumSignalsInSec::_25,      // num signals in one second
        Chan::A,                    // lastAffectedChannel
        ModeAccumulation_NoReset,   // modeAccumulation
        AM_Show,                    // altMarkers
        MenuAutoHide_None,          // menuAutoHide
        true,                       // showFullMemoryWindow
        false,                      // showStringNavigation
        LinkingRShift_Position      // linkingRShift
    },
    // channels
    {
        {
            1.0f,                   // cal_stretch
            {                       // cal_rshift
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0}
            },

            RShift::ZERO,
            ModeCouple::AC,         // ModCouple
            Divider::_1,            // Divider
            Range::_500mV,          // range
            true,                   // enable
            false,                  // inverse
            false                   // filtr
        },
        {
            1.0f,                   // cal_stretch
            {                       // cal_rshift
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0}
            },

            RShift::ZERO,
            ModeCouple::AC,         // ModCouple
            Divider::_1,            // Divider
            Range::_500mV,          // range
            true,                   // enable
            false,                  // inverse
            false                   // filtr
        }

    },
    // trig
    {
        StartMode::Auto,
        TrigSource::ChannelA,
        TrigPolarity::Front,
        TrigInput::Full,
        {TrigLev::ZERO, TrigLev::ZERO, TrigLev::ZERO},
        ModeLongPressTrig::Auto,
        TrigModeFind::Hand
    },
    // time
    {
        TBase::_200us,
        0,                          // set.time.tShiftRel
        FunctionTime::Time,
        TPos::Center,
        SampleType::Equal,
        PeackDetMode::Disable,
        false
    },
    // cursors
    {
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl U
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl T
        Chan::A,                                    // source
        { 60.0f,  140.0f, 60.0f, 140.0f },          // posCur U
        { 80.0f,  200.0f, 80.0f, 200.0f },          // posCur T
        { 80.0f,  80.0f },                          // ���������� ����� ��������� ���������� ��� 100%
        { 120.0f, 120.0f },                         // ���������� ����� ��������� ������� ��� 100%
        CursMovement_Points,                        // CursMovement
        CursActive_None,                            // CursActive
        { CursLookMode_None, CursLookMode_None },   // ����� �������� ��������.
        false,                                      // showFreq
        false                                       // showCursors
    },
    // memory
    {
        ENUM_POINTS_FPGA::_1024,
        ModeWork_Direct,
        FileNamingMode_Mask,
        "Signal_\x07\x03",      // \x07 - ��������, ��� ����� ������ ���� ��������� �����, \x03 - ������� �� ��� ����������
        "Signal01",
        0,
        {
            false
        },
        ModeShowIntMem_Both,    // modeShowIntMem
        false,                  // flashAutoConnect
        ModeBtnMemory_Menu,
        ModeSaveSignal_BMP
    },
    // measures
    {
        MN_1,                       // measures number
        Chan::A_B,                  // source
        ModeViewSignals_AsIs,       // mode view signal
        {
            Measure::VoltageMax,     Measure::VoltageMin,  Measure::VoltagePic,      Measure::VoltageMaxSteady,  Measure::VoltageMinSteady,
            Measure::VoltageAverage, Measure::VoltageAmpl, Measure::VoltageRMS,      Measure::VoltageVybrosPlus, Measure::VoltageVybrosMinus,
            Measure::Period,         Measure::Freq,        Measure::TimeNarastaniya, Measure::TimeSpada,         Measure::DurationPlus
        },
        false,                      // show
        MeasuresField_Screen,       // ����� ��� ��������� ��������, ������� �� ������
        //{0, 200},                 // ��������� �������� �������� ���������� ��� ���� ���������
        //{372, 652},               // ��������� �������� �������� ������� ��� ���� ���������
        {50, 150},                  // ��������� �������� �������� ���������� ��� ���� ���������
        {422, 602},                 // ��������� �������� �������� ������� ��� ���� ���������
        CursCntrl_1,                // ��������� ������ �������� ����������
        CursCntrl_1,                // ��������� ������ �������� �������
        CursActive_T,               // ������� ������� �������.
        Measure::None                // marked Measure
    },
    // math
    {
        ScaleFFT_Log,
        SourceFFT_A,
        WindowFFT_Rectangle,
        FFTmaxDB::_60,
        Function_Sum,
        0,
        {100, 256 - 100},
        1,
        1,
        1,
        1,
        false,
        ModeDrawMath_Disable,       // modeDrawMath
        ModeRegSet_Range,           // modeRegSet
        Range::_50mV,
        Divider::_1,
        RShift::ZERO                  // rShift
    },
    // service
    {
        true,                       // screenWelcomeEnable
        true,                       // soundEnable
        CalibratorMode::Freq,       // calibratorEnable
        0,                          // IP-����� (��������)  WARN
        ColorScheme_WhiteLetters    // colorScheme
    },
    {
        0x8b, 0x2e, 0xaf, 0x8f, 0x13, 0x00, // mac
        192, 168, 1, 200,                   // ip
        7,
        255, 255, 255, 0,                   // netmask
        192, 168, 1, 1                      // gateway
    },
    // common
    {
        0
    },
    // menu
    {
        {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        0
    },
    // debug
    {
        15,         // numStrings
        0,          // ������ ������ - 5
        1000,       // numMeasuresForGates
        false,      // showStats
        6,          // numAveForRand
        {0, 0},     // balanceADC
        false,      // fpga_compact
        0,          // fpga_gates_min
        0           // fpga_gates_max
    }
};

Settings set;


void Settings::Load()
{
    if (!HAL_ROM::LoadSettings())
    {
        Reset();
    }

    RunAfterLoad();
}


void Settings::Reset()
{
    /*
    * ����������� ���������:
    *   set.ch.cal_stretch
    *   set.ch.cal_rshift
    *   set.debug
    */

    Settings old = set;

    set = defaultSettings;

    CopyCalibrationSettings(ChA, set, old);
    CopyCalibrationSettings(ChB, set, old);

    RunAfterLoad();
}


void Settings::CopyCalibrationSettings(Chan ch, Settings &dest, Settings &src)
{
    if (src.chan[ch].cal_stretch < 0.75f || src.chan[ch].cal_stretch > 1.25f)
    {
        return;
    }

    for (int range = 0; range < Range::Count; range++)
    {
        for (int couple = 0; couple < ModeCouple::Count; couple++)
        {
            if (src.chan[ch].cal_rshift[range][couple] > 40 || src.chan[ch].cal_rshift[range][couple] < -40)
            {
                return;
            }
        }
    }

    if (src.debug.numStrings < 0 || src.debug.numStrings > 40)
    {
        return;
    }

    if (src.debug.sizeFont < 0 || src.debug.sizeFont > 1)
    {
        return;
    }

    if (src.debug.balanceADC[0] < 0 || src.debug.balanceADC[0] > 20)
    {
        return;
    }

    if (src.debug.balanceADC[1] < 0 || src.debug.balanceADC[1] > 20)
    {
        return;
    }

    dest.chan[ch].cal_stretch = src.chan[ch].cal_stretch;

    for (int range = 0; range < Range::Count; range++)
    {
        for (int couple = 0; couple < ModeCouple::Count; couple++)
        {
            dest.chan[ch].cal_rshift[range][couple] = src.chan[ch].cal_rshift[range][couple];
        }
    }

    dest.debug = src.debug;
}


void Settings::RunAfterLoad()
{
    HAL_LTDC::LoadPalette();

    Panel::EnableLEDChannelA(SET_ENABLED_A);
    Panel::EnableLEDChannelB(SET_ENABLED_B);
    Menu::SetAutoHide(true);
    Display::ChangedRShiftMarkers();

    isLoaded = true;
}

void Settings::SaveBeforePowerDown()
{
    if (need_save)
    {
        HAL_ROM::SaveSettings();
    }
}


void Settings::NeedSave()
{
    need_save = true;
    time_save = TIME_MS + 5000;
}


void Settings::SaveIfNeed()
{
    if (need_save && (TIME_MS >= time_save))
    {
        SaveBeforePowerDown();
        need_save = false;
    }
}


int Page::NumCurrentSubPage() const
{
    return set.menu.currentSubPage[name];
}

void Page::SetCurrentSubPage(int8 posSubPage) const
{
    set.menu.currentSubPage[name] = posSubPage;
}

bool Menu::IsShown()
{
    return set.menu.isShown;
}

void Menu::Show(bool show)
{
    set.menu.isShown = show ? 1U : 0U;
    Menu::SetAutoHide(true);
}

bool Menu::IsMinimize()
{
    return Item::TypeOpened() == TypeItem::Page && ((const Page *)Item::Opened())->GetName() >= NamePage::SB_Curs;
}


void Page::RotateRegSetSB(int angle)
{
    Page *page = (Page *)Item::Opened();

    if (page->funcRegSetSB)
    {
        page->funcRegSetSB(angle);
    }
}

Range::E &operator++(Range::E &range)
{
    range = (Range::E)((int)range + 1);
    return range;

}


Range::E &operator--(Range::E &range)
{
    range = (Range::E)((int)range - 1);
    return range;
}


float FFTmaxDB::Abs()
{
    static const float db[] = {-40.0f, -60.0f, -80.0f};
    return db[FFT_MAX_DB];
}


int ENUM_POINTS_FPGA::ToNumPoints()
{
    static const int num_points[Count] =
    {
        281, 512, 1024
    };

    return num_points[SET_ENUM_POINTS];
}


int ENUM_POINTS_FPGA::ToNumBytes()
{
    int mul = SET_PEAKDET_IS_ENABLE ? 2 : 1;

    return ToNumPoints() * mul;
}
