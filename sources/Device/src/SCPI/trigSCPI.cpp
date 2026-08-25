// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Definition.h"
#include "SCPI/SCPI.h"
#include "Utils/Containers/Values.h"
#include "Hardware/InterCom.h"


/*
    TRIG:
        MODE {AUTO | WAIT | SINGLE | ?}
        SOURCE {CHAN1 | CHAN2 | EXT | ?}
        POLARITY {FRONT | BACK | ?}
        INPUT {FULL | AC | LPF | HPF | ?}
        OFFSET {-200...200 | ?}
*/


pchar SCPI::ProcessTRIG(pchar buffer)
{
    static const StructCommand commands[] =
    {
        {":MODE",        SCPI::TRIGGER::MODE},
        {":SOURCE",      SCPI::TRIGGER::SOURCE},
        {":POLARITY",    SCPI::TRIGGER::POLARITY},
        {":POLAR",       SCPI::TRIGGER::POLARITY},
        {":INPUT",       SCPI::TRIGGER::INPUT},
        {":FIND",        SCPI::TRIGGER::FIND},
        {":OFFSET",      SCPI::TRIGGER::OFFSET},
        {0}
    };

    return SCPI::ProcessingCommand(commands, buffer);
}


pchar SCPI::TRIGGER::MODE(pchar buffer)
{
    static const MapElement map[] =
    {
        {" AUTO",   0},
        {" WAIT",   1},
        {" SINGLE", 2},
        {0}
    };

    SCPI_CYCLE(START_MODE = (StartMode::E)it->value; PageTrig::OnPress_Mode(true));

    IF_REQUEST(InterCom::SendFormat0D(":TRIGGER:MODE%s", map[START_MODE].key));

    return buffer;
}


pchar SCPI::TRIGGER::SOURCE(pchar buffer)
{
    static const MapElement map[] =
    {
        {" CHAN1", 0},
        {" CHAN2", 1},
        {" EXT",   2},
        {0}
    };

    SCPI_CYCLE(TrigSource::Set((TrigSource::E)it->value));

    IF_REQUEST(InterCom::SendFormat0D(":TRIGGER:SOUCRE%s", map[TRIG_SOURCE].key));

    return buffer;
}


pchar SCPI::TRIGGER::POLARITY(pchar buffer)
{
    if      FIRST_SYMBOLS(" FRONT") { TrigPolarity::Set(TrigPolarity::Front); }
    else if FIRST_SYMBOLS(" BACK")  { TrigPolarity::Set(TrigPolarity::Back);  }

    IF_REQUEST(InterCom::SendFormat0D(":TRIGGER:POLARITY %s", TRIG_POLARITY_IS_FRONT ? "FRONT" : "BACK"));

    return buffer;
}


pchar SCPI::TRIGGER::INPUT(pchar buffer)
{
    static const MapElement map[] =
    {
        {" FULL", 0},
        {" AC",   1},
        {" LPF",  2},
        {" HPF",  3},
        {0}
    };

    SCPI_CYCLE(TrigInput::Set((TrigInput::E)it->value));

    IF_REQUEST(InterCom::SendFormat0D(":TRIGGER:INPUT%s", map[TRIG_INPUT].key));

    return buffer;
}


pchar SCPI::TRIGGER::FIND(pchar buffer)
{
    if      FIRST_SYMBOLS(" HAND") { TRIG_MODE_FIND = TrigModeFind::Hand; }
    else if FIRST_SYMBOLS(" AUTO") { TRIG_MODE_FIND = TrigModeFind::Auto; }
    else if FIRST_SYMBOLS(" FIND") { TrigLev::FindAndSet();               }

    IF_REQUEST(InterCom::SendFormat0D(":TRIGGER:FIND %s", TRIG_MODE_FIND_IS_HAND ? "HAND" : "AUTO"));

    return buffer;
}


pchar SCPI::TRIGGER::OFFSET(pchar buffer)
{
    if FIRST_SYMBOLS("?")
    {
        int trig_lev = (int)(0.5F * (TRIG_LEVEL_SOURCE - RShift::ZERO));
        InterCom::SendFormat0D(":TRIGGER:OFFSET %d", trig_lev);
    }
    else
    {
        Int value(buffer);

        if (value.IsValid() && value >= -240 && value <= 240)
        {
            int trig_lev = RShift::ZERO + 2 * value;
            TrigLev::Set(TRIG_SOURCE, (int16)trig_lev);
        }
    }

    return buffer;
}
