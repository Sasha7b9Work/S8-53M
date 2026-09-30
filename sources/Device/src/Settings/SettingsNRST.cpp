#include "defines.h"
#include "Settings/SettingsNRST.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Utils/Text/Warnings.h"


static SettingsNRST defaultNRST =
{
    0,              // crc32
    0,              // numStrings
    0,              // размер шрифта - 5
    1000,           // numMeasuresForGates
    false,          // showStats
    6,              // numAveForRand
    false,          // fpga_compact
    0,              // fpga_gates_min
    0,              // fpga_gates_max
    0,              // first_byte
    {0,    0},      // balanceADC
    {               // hand_rshift
        {0, 0, 0},
        {0, 0, 0}
    },
    0                   // not_used
};

int16 SettingsNRST::stored_rshift[Chan::Count][3];


SettingsNRST nrst = defaultNRST;


bool SettingsNRST::Save()
{
    return HAL_ROM::Settings::SaveNRST(this);
}


void SettingsNRST::Load()
{
    HAL_ROM::Settings::LoadNRST(this);

    if (*this == defaultNRST)
    {
        Warning::ShowBad(Warning::InstrumentNotCalibrated);
    }
}


bool SettingsNRST::operator==(const SettingsNRST &rhs)
{
    return std::memcmp(((uint8 *)&rhs) + 4, ((uint8 *)this) + 4, sizeof(rhs)) == 0;
}


int CAL::RShift(Chan ch, Range::E range)
{
    int result = gset.chan[ch].cal_rshift[range][SET_COUPLE(ch)];

    if (range <= Range::_10mV && SET_COUPLE(ch) == ModeCouple::DC)
    {
        result += RSHIFT_HAND(ch, range);
    }

    return result;
}


void SettingsNRST::ClearHandRShift(Chan ch)
{
    std::memset(&hand_rshift[ch][0], 0, sizeof(hand_rshift[0][0]) * 3);
}


void SettingsNRST::StoreAndClearHandRShift(Chan ch, Range::E range)
{
    if ((ch.IsA() || ch.IsB()) && range <= Range::_10mV)
    {
        stored_rshift[ch][range] = hand_rshift[ch][range];
        hand_rshift[ch][range] = 0;
    }
    else
    {
        LOG_ERROR_TRACE("ошибка диапазона");
    }
}


void SettingsNRST::RestoreHandRShift(Chan ch, Range::E range)
{
    if ((ch.IsA() || ch.IsB()) && range <= Range::_10mV)
    {
        hand_rshift[ch][range] = stored_rshift[ch][range];
    }
    else
    {
        LOG_ERROR_TRACE("ошибка диапазона");
    }
}
