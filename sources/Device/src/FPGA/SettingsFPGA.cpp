// 2022/02/11 17:48:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include "GlobalEvents.h"
#include "Display/Screen/Grid.h"
#include "Menu/Menu.h"
#include "Utils/Text/Warnings.h"
#include "Utils/Text/Symbols.h"
#include "Utils/Text/Text.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include <cstring>


StateWorkFPGA::E StateWorkFPGA::current = StateWorkFPGA::Stop;


bool TrigLev::showLevel = false;
bool TrigLev::fireLED = false;


const float Range::scale[Range::Count] = {2e-3f, 5e-3f, 10e-3f, 20e-3f, 50e-3f, 100e-3f, 200e-3f, 500e-3f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f};

const float TShift::absStep[TBase::Count] =
{
    2e-9f / 20, 5e-9f / 20, 10e-9f / 20, 20e-9f / 20, 50e-9f / 20, 100e-9f / 20, 200e-9f / 20, 500e-9f / 20,
    1e-6f / 20, 2e-6f / 20, 5e-6f / 20, 10e-6f / 20, 20e-6f / 20, 50e-6f / 20, 100e-6f / 20, 200e-6f / 20, 500e-6f / 20,
    1e-3f / 20, 2e-3f / 20, 5e-3f / 20, 10e-3f / 20, 20e-3f / 20, 50e-3f / 20, 100e-3f / 20, 200e-3f / 20, 500e-3f / 20,
    1.0f / 20, 2.0f / 20, 5.0f / 20, 10.0f / 20
};


const float Range::voltsInPoint[Range::Count] =
{
    2e-3f   * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 2mV
    5e-3f   * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 5mV
    10e-3f  * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 10mV
    20e-3f  * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 20mV
    50e-3f  * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 50mV
    100e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 100mV
    200e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 200mV
    500e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 500mV
    1.0f    * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 1V
    2.0f    * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 2V
    5.0f    * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 5V
    10.0f   * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 10V
    20.0f   * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN)   // 20V
};


bool RShift::showLevel[2] = {false, false};
bool RShift::drawMarkers = false;

const float RShift::absStep[Range::Count] =
{
    2e-3f   / 20 / RShift::STEP,
    5e-3f   / 20 / RShift::STEP,
    10e-3f  / 20 / RShift::STEP,
    20e-3f  / 20 / RShift::STEP,
    50e-3f  / 20 / RShift::STEP,
    100e-3f / 20 / RShift::STEP,
    200e-3f / 20 / RShift::STEP,
    500e-3f / 20 / RShift::STEP,
    1.0f    / 20 / RShift::STEP,
    2.0f    / 20 / RShift::STEP,
    5.0f    / 20 / RShift::STEP,
    10.0f   / 20 / RShift::STEP,
    20.0f   / 20 / RShift::STEP
};


namespace FPGA
{
    // Добавочные смещения по времени для разверёток режима рандомизатора.
    int deltaTShift[TBase::Count] = {505, 489, 464, 412, 258};

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Загрузить все параметры напряжения каналов и синхронизации в аппаратную часть.
    void SetAttribChannelsAndTrig(TypeWriteAnalog::E type);
}


void FPGA::LoadSettings()
{
    TShift::Set(SET_TSHIFT);
    TBase::Load();
    Range::Load(ChA);
    RShift::Load(ChA);
    Range::Load(ChB);
    RShift::Load(ChB);
    TrigLev::Load();
    TrigPolarity::Load();
    LoadRegUPR();
    TrigLev::Load();
    TrigSource::Set(TRIG_SOURCE);

//    PrepareAndWriteDataToAnalogSPI(CS1);
//    PrepareAndWriteDataToAnalogSPI(CS2);
//    PrepareAndWriteDataToAnalogSPI(CS3);
//    PrepareAndWriteDataToAnalogSPI(CS4);
}


void FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::E type) 
{
    /*
    b0...b7 - Channel1
    b8...b15 - Channel2
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

    static const uint8 masksRange[Range::Count] =
    {
        BINARY_U8(0000000),
        BINARY_U8(1000000),
        BINARY_U8(1100000),
        BINARY_U8(1010000),
        BINARY_U8(1000100),
        BINARY_U8(1100100),
        BINARY_U8(1010100),
        BINARY_U8(1000010),
        BINARY_U8(1100010),
        BINARY_U8(1010010),
        BINARY_U8(1000110),
        BINARY_U8(1100110),
        BINARY_U8(1010110)
    };

    uint data = 0;

    // Range0, Range1
    data |= masksRange[SET_RANGE_A];
    data |= (masksRange[SET_RANGE_B] << 8);

    // Параметры каналов
    static const uint maskCouple[2][3] = 
    {
        {0x0008, 0x0000, 0x0030},
        {0x0800, 0x0000, 0x3000}
    };

    data |= maskCouple[Chan::A][SET_COUPLE_A];
    data |= maskCouple[Chan::B][SET_COUPLE_B];

    static const uint maskFiltr[2][2] = 
    {
        {0x0000, 0x0080},
        {0x0000, 0x0100}
    };

    data |= maskFiltr[Chan::A][SET_FILTR_A];
    data |= maskFiltr[Chan::B][SET_FILTR_B];


    // Параметры синхронизации
    static const uint maskSource[3] = {0x000000, 0x800000, 0x400000};
    data |= maskSource[TRIG_SOURCE];

    static const uint maskInput[4] = {0x000000, 0x030000, 0x020000, 0x010000};
    data |= maskInput[TRIG_INPUT];

    BUS_FPGA::WriteAnalog(type, data);
}


void Range::Set(Chan ch, Range::E range)
{
    if (!ch.Enabled())
    {
        return;
    }
    if (range < Range::Count && (int)range >= 0)
    {
        float rShiftAbs = SET_RSHIFT(ch).ToAbs(SET_RANGE(ch));
        float trigLevAbs = TRIG_LEVEL(ch).ToAbs(SET_RANGE(ch));

        SET_RANGE(ch) = range;

        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            SET_RSHIFT(ch) = (int16)RShift::ToRel(rShiftAbs, range);
            TRIG_LEVEL(ch) = (int16)RShift::ToRel(trigLevAbs, range);
        }

        Load(ch);
    }
    else
    {
        Warning::ShowBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
    }
};


void Range::Load(Chan ch)
{
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::RangeA);
    RShift::Load(ch);

    if (ch == (Chan::E)TRIG_SOURCE)
    {
        TrigLev::Load();
    }
}


void TBase::Set(TBase::E tbase)
{
    if (!SET_ENABLED_A && !SET_ENABLED_B)
    {
        return;
    }

    if (tbase < TBase::Count && (int)tbase >= 0)
    {
        float tShiftAbsOld = TShift::ToAbs(SET_TSHIFT, SET_TBASE);
        SET_TBASE = tbase;
        Load();
        TShift::Set(TShift::ToRel(tShiftAbsOld, SET_TBASE));
        Flags::needFinishDraw = true;
    }
    else
    {
        Warning::ShowBad(Warning::LimitSweep_Time);
    }
};


void TBase::Load()
{
    struct TBaseMaskStruct
    {
        uint8 maskNorm;         // Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
        uint8 maskPeackDet;     // Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
    };
    
    static const TBaseMaskStruct masksTBase[TBase::Count] =
    {
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 2ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 5ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 10ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 20ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 50ns
        {BINARY_U8(00100001), BINARY_U8(00100001)},     // 100ns
        {BINARY_U8(00100010), BINARY_U8(00100010)},     // 200ns
        {BINARY_U8(00100011), BINARY_U8(00100010)},     // 500ns
        {BINARY_U8(01000101), BINARY_U8(00100011)},     // 1us
        {BINARY_U8(01000110), BINARY_U8(01000101)},     // 2us
        {BINARY_U8(01000111), BINARY_U8(01000110)},     // 5us
        {BINARY_U8(01001001), BINARY_U8(01000111)},     // 10us
        {BINARY_U8(01001010), BINARY_U8(01001001)},     // 20us
        {BINARY_U8(01001011), BINARY_U8(01001010)},     // 50us
        {BINARY_U8(01001101), BINARY_U8(01001011)},     // 100us
        {BINARY_U8(01001110), BINARY_U8(01001101)},     // 200us
        {BINARY_U8(01001111), BINARY_U8(01001110)},     // 500us
        {BINARY_U8(01010001), BINARY_U8(01001111)},     // 1ms
        {BINARY_U8(01010010), BINARY_U8(01010001)},     // 2ms
        {BINARY_U8(01010011), BINARY_U8(01010010)},     // 5ms
        {BINARY_U8(01010101), BINARY_U8(01010011)},     // 10ms
        {BINARY_U8(01010110), BINARY_U8(01010101)},     // 20ms
        {BINARY_U8(01010111), BINARY_U8(01010110)},     // 50ms
        {BINARY_U8(01011001), BINARY_U8(01010111)},     // 100ms
        {BINARY_U8(01011010), BINARY_U8(01011001)},     // 200ms
        {BINARY_U8(01011011), BINARY_U8(01011010)},     // 500ms
        {BINARY_U8(01011101), BINARY_U8(01011011)},     // 1s
        {BINARY_U8(01011110), BINARY_U8(01011101)},     // 2s
        {BINARY_U8(01011111), BINARY_U8(01011110)},     // 5s
        {BINARY_U8(01111111), BINARY_U8(01011111)}      // 10s
    };

    int tbase = SET_TBASE;

    if (FPGA::Compactor::Enabled())
    {
        tbase = Math::Limitation<int>(tbase - 2, 0, tbase);
    }

    uint8 mask = SET_PEAKDET_IS_ENABLED ? masksTBase[tbase].maskPeackDet : masksTBase[tbase].maskNorm;

    BUS_FPGA::Write(WR_RAZV, mask, true);
}


void TBase::Decrease()
{
    if (SET_PEAKDET_IS_ENABLED && (SET_TBASE <= TBase::MIN_PEC_DEAT))
    {
        Warning::ShowBad(Warning::LimitSweep_Time);
        Warning::ShowBad(Warning::EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (SET_SELFRECORDER && SET_TBASE == TBase::MIN_P2P)
        {
            Warning::ShowBad(Warning::TooFastScanForSelfRecorder);
        }
        else
        {
            TBase::E base = (TBase::E)((int)SET_TBASE - 1);
            TBase::Set(base);
        }
    }
    else
    {
        Warning::ShowBad(Warning::LimitSweep_Time);
    }
}


void TBase::Increase()
{
    if (SET_TBASE < (TBase::Count - 1))
    {
        TBase::E base = (TBase::E)(SET_TBASE + 1);
        TBase::Set(base);
    }
    else
    {
        Warning::ShowBad(Warning::LimitSweep_Time);
    }
}


void RShift::Set(Chan ch, int16 rshift)
{
    if (!ch.Enabled())
    {
        return;
    }

    RShift::ChangedMarkers();

    if (rshift > RShift::MAX || rshift < RShift::MIN)
    {
        Warning::ShowBad(ch == Chan::A ? Warning::LimitChan1_RShift : Warning::LimitChan2_RShift);
    }

    LIMITATION(rshift, rshift, RShift::MIN, RShift::MAX);

    if (rshift > RShift::ZERO)
    {
        rshift &= 0xfffe;                                            // Делаем кратным двум, т.к. у нас 800 значений на 400 точек
    }
    else if (rshift < RShift::ZERO)
    {
        rshift = (rshift + 1) & 0xfffe;
    }

    SET_RSHIFT(ch) = rshift;
    Load(ch.value);

    Display::RotateRShift(ch.value);
};


void RShift::Load(Chan ch)
{
    static const uint16 mask[2] = {0x2000, 0x6000};

    Range::E range = SET_RANGE(ch);

    uint16 rshift = (uint16)(SET_RSHIFT(ch) + (SET_INVERSE(ch) ? -1 : 1) * CAL::RShift(ch, range));

    int16 delta = -(rshift - RShift::ZERO);

    if (SET_INVERSE(ch))
    {
        delta = -delta;
    }

    rshift = (uint16)(delta + RShift::ZERO);

    rshift = (uint16)(RShift::MAX + RShift::MIN - rshift);
    BUS_FPGA::WriteDAC(ch == Chan::A ? TypeWriteDAC::RShiftA : TypeWriteDAC::RShiftB, (uint16)(mask[ch] | (rshift << 2)));
}


void TrigLev::Set(Chan ch, int16 lev)
{
    if (ch < 2)
    {
        Set((TrigSource::E)ch.value, lev);
    }
}


void TrigLev::Set(TrigSource::E ch, int16 trigLev)
{
    RShift::ChangedMarkers();

    if (trigLev < TrigLev::MIN || trigLev > TrigLev::MAX)
    {
        Warning::ShowBad(Warning::LimitSweep_Level);
    }

    LIMITATION(trigLev, trigLev, TrigLev::MIN, TrigLev::MAX);

    if (trigLev > TrigLev::ZERO)
    {
        trigLev &= 0xfffe;
    }
    else if (trigLev < TrigLev::ZERO)
    {
        trigLev = (trigLev + 1) & 0xfffe;
    }

    if (TRIG_LEVEL(ch) != trigLev)
    {
        TRIG_LEVEL(ch) = trigLev;
        Load();
        Display::RotateTrigLev();
    }
};


void TrigLev::Load()
{
    uint16 data = 0xa000;
    uint16 trigLev = (uint16)TRIG_LEVEL_SOURCE;
    trigLev = (uint16)(TrigLev::MAX + TrigLev::MIN - trigLev);
    data |= trigLev << 2;
    BUS_FPGA::WriteDAC(TypeWriteDAC::TrigLev, data);
}


void TShift::Set(int tshift)
{
    if (!SET_ENABLED_A && !SET_ENABLED_B)
    {
        return;
    }

    if (tshift < TShift::Min() || tshift > TShift::MAX)
    {
        Math::Limitation<int>(&tshift, TShift::Min(), TShift::MAX);

        Warning::ShowBad(Warning::LimitSweep_TShift);
    }

    SET_TSHIFT = (int16)tshift;

    FPGA::Launch::Load();

    Flags::needFinishDraw = true;
};


void PeackDetMode::Set(PeackDetMode::E peackDetMode)
{
    SET_PEAKDET = peackDetMode;
    FPGA::LoadRegUPR();
}


void CalibratorMode::Set(CalibratorMode::E calibratorMode)
{
    CALIBRATOR = calibratorMode;
    FPGA::LoadRegUPR();
}


void FPGA::LoadRegUPR()
{
    uint8 data = 0;

    if (SET_TBASE < TBase::MAX_RAND)
    {
        _SET_BIT(data, UPR_BIT_RAND);
    }

    if (!SET_PEAKDET_IS_DISABLED)
    {
        _SET_BIT(data, UPR_BIT_PEAKDET);
    }

    if (CALIBRATOR_IS_FREQ)
    {
        _SET_BIT(data, UPR_BIT_CALIBRATOR_AC_DC);
    }
    else if (CALIBRATOR_IS_DC)
    {
        _SET_BIT(data, UPR_BIT_CALIBRATOR_VOLTAGE);
    }

    if (SET_SELFRECORDER)
    {
        _SET_BIT(data, UPR_BIT_RECORDER);
    }


    BUS_FPGA::Write(WR_UPR, data, true);
}


String<> TShift::ToString(int tShiftRel)
{
    float tShiftVal = TShift::ToAbs(tShiftRel, SET_TBASE);

    return SU::Time2String(tShiftVal, true);
}


bool Range::Increase(Chan ch)
{
    bool retValue = false;

    if (SET_RANGE(ch) < Range::Count - 1)
    {
        Range::Set(ch, (Range::E)(SET_RANGE(ch) + 1));
        retValue = true;
    }
    else
    {
       Warning::ShowBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
    }

    Flags::needFinishDraw = true;

    return retValue;
};


bool Range::Decrease(Chan ch)
{
    bool retValue = false;

    if (SET_RANGE(ch) > 0)
    {
        Range::Set(ch, (Range::E)(SET_RANGE(ch) - 1));
        retValue = true;
    }
    else
    {
        Warning::ShowBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
    }

    Flags::needFinishDraw = true;

    return retValue;
};


void TrigSource::Set(Chan ch)
{
    if (ch < 2)
    {
        Set((E)ch.value);
    }
}


void TrigSource::Set(TrigSource::E trigSource)
{
    TRIG_SOURCE = trigSource;
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::TrigParam);

    if (!TRIG_SOURCE_IS_EXT)
    {
        TrigLev::Set(TRIG_SOURCE, TRIG_LEVEL_SOURCE);
    }
}


void TrigPolarity::Set(TrigPolarity::E polarity)
{
    TRIG_POLARITY = polarity;
    Load();
}


void TrigPolarity::Load()
{
    BUS_FPGA::Write(WR_TRIG, TRIG_POLARITY_IS_FRONT ? 0x00U : 0x01U, true);
}


void TrigInput::Set(TrigInput::E trigInput)
{
    TRIG_INPUT = trigInput;
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::TrigParam);
}


void ModeCouple::Set(Chan ch, ModeCouple::E modeCoupe)
{
    SET_COUPLE(ch) = modeCoupe;
    FPGA::SetAttribChannelsAndTrig(ch == Chan::A ? TypeWriteAnalog::ChanParam0 : TypeWriteAnalog::ChanParam1);
    RShift::Set(ch, SET_RSHIFT(ch));
}


void Filtr::Enable(Chan ch, bool enable)
{
    SET_FILTR(ch) = enable;
    FPGA::SetAttribChannelsAndTrig(ch == Chan::A ? TypeWriteAnalog::ChanParam0 : TypeWriteAnalog::ChanParam1);
}


bool TBase::InModeRandomizer()
{
    return SET_TBASE <= MAX_RAND;
}


bool TBase::InModeP2P()
{
    return SET_TBASE >= MIN_P2P;
}


int TShift::Zero()
{
    return -Min();
}


int TShift::Min()
{
    static const int16 m[3][3] =
    {
        {-511, -441, -371},
        {-511, -383, -255},
        {-511, -255, 0}
    };

    if ((int)SET_ENUM_POINTS < 3 && (int)SET_ENUM_POINTS >= 0)
    {
        return m[SET_ENUM_POINTS][SET_TPOS];
    }

    LOG_ERROR_TRACE("");
    return 0;
}


int TPos::InPoints(ENUM_POINTS_FPGA::E enum_points, TPos::E tPos)
{
    static const int m[3][3] =
    {
        {0, 140, 280},
        {0, 255, 511},
        {0, 512, 1022}
    };

    return m[enum_points][tPos];
}


void TPos::Set(E pos)
{
    SET_TPOS = pos;
    PageMemory::OnChanged_NumPoints(true);
    TShift::Set(SET_TSHIFT);
}


int RShift::ToRel(float rShiftAbs, Range::E range)
{
    int retValue = ZERO + rShiftAbs / RShift::absStep[range];

    if (retValue < MIN)
    {
        retValue = MIN;
    }
    else if (retValue > MAX)
    {
        retValue = MAX;
    }

    return retValue;
};


void DataSettings::FillFromCurrentSettings()
{
    range[0] = SET_RANGE_A;
    range[1] = SET_RANGE_B;
    rshiftA = (uint)SET_RSHIFT_A;
    rshiftB = (uint)SET_RSHIFT_B;
    tbase = SET_TBASE;
    tshift = SET_TSHIFT;
    coupleA = SET_COUPLE_A;
    coupleB = SET_COUPLE_B;
    e_points_in_channel = SET_ENUM_POINTS;
    trigLevA = (uint)TRIG_LEVEL_A;
    trigLevB = (uint)TRIG_LEVEL_B;
    peak_det = (uint)SET_PEAKDET;
    div_a = SET_DIVIDER_A;
    div_b = SET_DIVIDER_B;
    valid = 0;
}


int TBase::StretchRand()
{
    static const int N = 100;

    //                                    2ns  5ns  10ns  20ns  50ns
    static const int Kr[TBase::Count] = { N/2, N/5, N/10, N/20, N/50,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    return Kr[SET_TBASE];
}


int DataSettings::BytesInChanReal() const
{
    int result = PointsInChannel();

    if (peak_det)
    {
        result *= 2;
    }

    return result;
}


int DataSettings::BytesInChanStored() const
{
    return (e_points_in_channel == ENUM_POINTS_FPGA::_281) ? 284 : BytesInChanReal();
}


int DataSettings::PointsInChannel() const
{
    static const int num_points[ENUM_POINTS_FPGA::Count] =
    {
        281, 512, 1024
    };

    return (int)num_points[e_points_in_channel];
}


int16 DataSettings::GetRShift(Chan ch) const
{
    return (int16)((ch == ChA) ? rshiftA : rshiftB);
}


float TShift::ToAbs(int shift, TBase::E base)
{
    return absStep[base] * shift * 2.0f;
}


int TShift::ShiftForRandomizer()
{
    int stretch_1 = TBase::StretchRand() - 1;

    if (stretch_1 < 2)
    {
        return 0;
    }

    int tshift = SET_TSHIFT;

    switch (SET_TBASE)
    {
    case TBase::_10ns:  tshift -= 6;    break;
    case TBase::_5ns:   tshift -= 11;   break;
    case TBase::_2ns:   tshift -= 16;   break;
    }

    if (tshift >= 0)
    {
        return -(tshift % stretch_1);
    }

    int shift = -((-tshift) % stretch_1);

    int result = -(stretch_1 + shift);

    if (result == -stretch_1)
    {
        result = 0;
    }

    return result;
}


int TShift::ToRel(float shift, TBase::E base)
{
    return shift / absStep[base] / 2.0f;
}


float Range::MaxOnScreen(Range::E range)
{
    return scale[range] * 5.0f;
}


bool DataSettings::Equal(const DataSettings &ds) const
{
    return  (range[0] == ds.range[0]) &&
            (range[1] == ds.range[1]) &&
            (rshiftA  == ds.rshiftA) &&
            (rshiftB  == ds.rshiftB) &&
            (tbase    == ds.tbase) &&
            (tshift   == ds.tshift) &&
            (coupleA  == ds.coupleA) &&
            (coupleB  == ds.coupleB) &&
            (trigLevA == ds.trigLevA) &&
            (trigLevB == ds.trigLevB) &&
            (div_a    == ds.div_a) &&
            (div_b    == ds.div_b) &&
            (peak_det == ds.peak_det) &&
            (e_points_in_channel == ds.e_points_in_channel);
}


void StartMode::Set(StartMode::E mode)
{
    START_MODE = mode;

    FPGA::Stop();

    if (!START_MODE_IS_SINGLE)
    {
        FPGA::Start();
    }
}


void RShift::Draw()
{
    if (!DISABLED_DRAW_MATH)
    {
        Draw(Chan::Math);
    }
    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        Draw(Chan::A);
        Draw(Chan::B);
    }
    else
    {
        Draw(Chan::B);
        Draw(Chan::A);
    }
}


void RShift::Draw(Chan ch)
{
    float x = (float)(Grid::Right() - Grid::Width() - Measures::GetDeltaGridLeft());

    if (ch == Chan::Math)
    {
        int rshift = SET_RSHIFT_MATH;
        float scale = (float)Grid::MathHeight() / 960;
        float y = (Grid::MathTop() + Grid::MathBottom()) / 2.0f - scale * (rshift - RShift::ZERO);
        Char(SYMBOL_RSHIFT_NORMAL).Draw(true, (int)(x - 9), (int)(y - 4), COLOR_FILL);
        Char('m').Draw(true, (int)(x - 8), (int)(y - 5), COLOR_BACK);
        return;
    }

    if (!ch.Enabled())
    {
        return;
    }

    int rshift = SET_RSHIFT(ch);

    float scale = (float)Grid::ChannelHeight() / (RShift::STEP * 200);
    float y = Grid::ChannelCenterHeight() - scale * (rshift - RShift::ZERO);

    float scaleFull = (float)Grid::ChannelHeight() / (RShift::MAX - RShift::MIN) *
        (PageService::Math::Enabled() ? 0.9f : 0.91f);

    float yFull = Grid::ChannelCenterHeight() - scaleFull * (rshift - RShift::ZERO);

    if (y > Grid::ChannelBottom())
    {
        Char(SYMBOL_RSHIFT_LOWER).Draw(true, (int)(x - 7), Grid::ChannelBottom() - 11, ColorChannel(ch));
        Point().Set(true, (int)(x - 5), Grid::ChannelBottom() - 2);
        y = (float)(Grid::ChannelBottom() - 7);
        x++;
    }
    else if (y < GRID_TOP)
    {
        Char(SYMBOL_RSHIFT_ABOVE).Draw(true, (int)(x - 7), GRID_TOP + 2, ColorChannel(ch));
        Point().Set(true, (int)(x - 5), GRID_TOP + 2);
        y = GRID_TOP + 7;
        x++;
    }
    else
    {
        Char(SYMBOL_RSHIFT_NORMAL).Draw(true, (int)(x - 8), (int)(y - 4), ColorChannel(ch));

        if (RShift::showLevel[ch] && MODE_WORK_IS_DIRECT)
        {
            Painter::DrawDashedHLine((int)y, Grid::Left(), Grid::Right(), 7, 3, 0);
        }
    }

    Font::Set(TypeFont::_5);
    int dY = 0;

    if ((!Menu::IsMinimize() || !Menu::IsShown()) && RShift::drawMarkers)
    {
        Region(4, 6).Fill(4, (int)(yFull - 3), ColorChannel(ch));
        Char(ch.IsA() ? '1' : '2').Draw(true, 5, (int)(yFull - 9 + dY), COLOR_BACK);
    }
    Char(ch.IsA() ? '1' : '2').Draw(true, (int)(x - 7), (int)(y - 9 + dY), COLOR_BACK);
    Font::Set(TypeFont::_8);
}


void TrigLev::Draw()
{
    TrigSource::E ch = TRIG_SOURCE;

    if (ch == TrigSource::Ext)
    {
        return;
    }

    int trigLev = TRIG_LEVEL(ch) + (SET_RSHIFT(ch) - RShift::ZERO);

    float scale = 1.0f / ((TrigLev::MAX - TrigLev::MIN) / 2.39f / Grid::ChannelHeight());
    int y0 = (int)((GRID_TOP + Grid::ChannelBottom()) / 2 + scale * (TrigLev::ZERO - TrigLev::MIN));
    int y = (int)(y0 - scale * (trigLev - TrigLev::MIN));

    y = (y - Grid::ChannelCenterHeight()) + Grid::ChannelCenterHeight();

    int x = Grid::Right();
    Color::SetCurrent(ColorTrig());
    if (y > Grid::ChannelBottom())
    {
        Char(SYMBOL_TRIG_LEV_LOWER).Draw(true, x + 3, Grid::ChannelBottom() - 11);
        Point().Set(true, x + 5, Grid::ChannelBottom() - 2);
        y = Grid::ChannelBottom() - 7;
        x--;
    }
    else if (y < GRID_TOP)
    {
        Char(SYMBOL_TRIG_LEV_ABOVE).Draw(true, x + 3, GRID_TOP + 2);
        Point().Set(true, x + 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x--;
    }
    else
    {
        Char(SYMBOL_TRIG_LEV_NORMAL).Draw(true, x + 1, y - 4);
    }
    Font::Set(TypeFont::_5);

    const char simbols[3] = {'1', '2', 'В'};
    int dY = 0;

    Char(simbols[TRIG_SOURCE]).Draw(true, x + 5, y - 9 + dY, COLOR_BACK);
    Font::Set(TypeFont::_8);

    if (RShift::drawMarkers && !Menu::IsMinimize())
    {
        Painter::DrawScaleLine(SCREEN_WIDTH - 11, true);
        int left = Grid::Right() + 9;
        int height = Grid::ChannelHeight() - 2 * Display::DELTA;
        int shiftFullMin = RShift::MIN + TrigLev::MIN;
        int shiftFullMax = RShift::MAX + TrigLev::MAX;
        scale = (float)height / (shiftFullMax - shiftFullMin);
        int shiftFull = TRIG_LEVEL_SOURCE.value + (TRIG_SOURCE_IS_EXT ? 0 : (int16)SET_RSHIFT(ch));
        int yFull = (int)(GRID_TOP + Display::DELTA + height - scale * (shiftFull - RShift::MIN - TrigLev::MIN) - 4);
        Region(4, 6).Fill(left + 2, yFull + 1, ColorTrig());
        Font::Set(TypeFont::_5);
        Char(simbols[TRIG_SOURCE]).Draw(true, left + 3, yFull - 5 + dY, COLOR_BACK);
        Font::Set(TypeFont::_8);
    }
}


void TShift::Draw()
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();
    int firstPoint = points.half_iword[0];
    int lastPoint = points.half_iword[1];

    // Рисуем TPos
    int shiftTPos = TPos::InPoints(SET_ENUM_POINTS, SET_TPOS) - SHIFT_IN_MEMORY;
    float scale = (float)(lastPoint - firstPoint) / Grid::Width();
    int gridLeft = Grid::Left();
    int x = (int)(gridLeft + shiftTPos * scale - 3);

    if (Math::InRange(x + 3, gridLeft, Grid::Right() + 1))
    {
        Text::Draw2Symbols(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_3, COLOR_BACK, COLOR_FILL);
    };

    // Рисуем tshift
    int shiftTShift = TPos::InPoints(SET_ENUM_POINTS, SET_TPOS) - SET_TSHIFT * 2;

    if (Math::InRange(shiftTShift, firstPoint, lastPoint))
    {
        x = gridLeft + shiftTShift - firstPoint - 3;
        Text::Draw2Symbols(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, COLOR_BACK, COLOR_FILL);
    }
    else if (shiftTShift < firstPoint)
    {
        Text::Draw2Symbols(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLine(Grid::Left() + 9, GRID_TOP + 1, Grid::Left() + 9, GRID_TOP + 7, COLOR_BACK);
    }
    else if (shiftTShift > lastPoint)
    {
        Text::Draw2Symbols(Grid::Right() - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLine(Grid::Right() - 9, GRID_TOP + 1, Grid::Right() - 9, GRID_TOP + 7, COLOR_BACK);
    }
}


void RShift::ChangedMarkers()
{
    drawMarkers = !ALT_MARKERS_HIDE;
    Timer::Enable(TypeTimer::RShiftMarkersAutoHide, 5000, RShift::OnMarkersAutoHide);
}


void RShift::OnMarkersAutoHide()
{
    drawMarkers = false;
    Timer::Disable(TypeTimer::RShiftMarkersAutoHide);
}


void TrigLev::FindAndSet()
{
    if (Storage::NumFrames() == 0 || TRIG_SOURCE_IS_EXT)
    {
        return;
    }
    else
    {
        Chan::E ch = (Chan::E)TRIG_SOURCE;

        const DataStruct data = Storage::GetData(0);

        const BufferFPGA &buffer = data.DataConst(ch);

        uint8 max = Math::GetMinFromArray(buffer.DataConst(), 0, buffer.Size());
        uint8 min = Math::GetMaxFromArray(buffer.DataConst(), 0, buffer.Size());

        int ave = ((int)min + (int)max) / 2;

        float scale = (float)(TrigLev::MAX - TrigLev::ZERO) / (float)(ValueFPGA::MAX - ValueFPGA::AVE) / 2.4f;

        int trig_lev = TrigLev::ZERO + scale * (ave - ValueFPGA::AVE) - (SET_RSHIFT(ch) - RShift::ZERO);

        Set(ch, (int16)trig_lev);
    }
}
