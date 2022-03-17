// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "SCPI.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/VCP/VCP.h"
#include "FPGA/FPGA.h"


/*
    TRIG:
        MODE {AUTO | WAIT | SINGLE | ?}
        SOURCE {CHAN1 | CHAN2 | EXT | ?}
        POLARITY {FRONT | BACK | ?}
        INPUT {FULL | AC | LPF | HPF | ?}
        OFFSET {-200...200 | ?}
*/



ENTER_PARSE_FUNC(SCPI::ProcessTRIG)
    {"MODE",        SCPI::TRIGGER::MODE},
    {"SOURCE",      SCPI::TRIGGER::SOURCE},
    {"POLARITY",    SCPI::TRIGGER::POLARITY},
    {"POLAR",       SCPI::TRIGGER::POLARITY},
    {"INPUT",       SCPI::TRIGGER::INPUT},
    {"FIND",        SCPI::TRIGGER::FIND},
    {"OFFSET",      SCPI::TRIGGER::OFFSET},
LEAVE_PARSE_FUNC



void SCPI::TRIGGER::MODE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"AUTO",   0},
        {"WAIT",   1},
        {"SINGLE", 2},
        {"?",      3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { StartMode::Set(StartMode::Auto); }
        else if (1 == value)    { StartMode::Set(StartMode::Wait); }
        else if (2 == value)    { StartMode::Set(StartMode::Single); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:MODE %s", map[START_MODE].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TRIGGER::SOURCE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"CHAN1", 0},
        {"CHAN2", 1},
        {"EXT",   2},
        {"?",     3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { TrigSource::Set(TrigSource::ChannelA); }
        else if (1 == value)    { TrigSource::Set(TrigSource::ChannelB); }
        else if (2 == value)    { TrigSource::Set(TrigSource::Ext); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:SOUCRE %s", map[START_MODE].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TRIGGER::POLARITY(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"FRONT", 0},
        {"BACK",  1},
        {"?",     2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { TrigPolarity::Set(TrigPolarity::Front); }
        else if (1 == value)    { TrigPolarity::Set(TrigPolarity::Back); }
        else if (2 == value)
        {
            SCPI_SEND(":TRIGGER:POLARITY %s", map[TRIG_POLARITY].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TRIGGER::INPUT(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"FULL", 0},
        {"AC",   1},
        {"LPF",  2},
        {"HPF",  3},
        {"?",    4},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { TrigInput::Set(TrigInput::Full); }
        else if (1 == value)    { TrigInput::Set(TrigInput::AC); }
        else if (2 == value)    { TrigInput::Set(TrigInput::LPF); }
        else if (3 == value)    { TrigInput::Set(TrigInput::HPF); }
        else if (4 == value)
        {
            SCPI_SEND(":TRIGGER:INPUT %s", map[TRIG_INPUT].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TRIGGER::FIND(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"HAND", 0},
        {"AUTO", 1},
        {"FIND", 2},
        {"?",    3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { TRIG_MODE_FIND = TrigModeFind::Hand; }
        else if (1 == value)    { TRIG_MODE_FIND = TrigModeFind::Auto; }
        else if (2 == value)    { FPGA::FindAndSetTrigLevel(); }
        else if (3 == value)
        {
            SCPI_SEND(":TRIGGER:FIND %s", map[TRIG_MODE_FIND].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TRIGGER::OFFSET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"?", 0},
        {0}
    };

    int intVal = 0;
    if (SCPI::FirstIsInt(buffer, &intVal, -240, 240))
    {
        int trigLev = RShift::ZERO + 2 * intVal;
        TrigLev::Set(TRIG_SOURCE, (int16)trigLev);
        return;
    }

    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = (int)(0.5f * (float)(TRIG_LEVEL_SOURCE - RShift::ZERO));
            SCPI_SEND(":TRIGGER:OFFSET %d", retValue);
        }
    LEAVE_ANALYSIS
}