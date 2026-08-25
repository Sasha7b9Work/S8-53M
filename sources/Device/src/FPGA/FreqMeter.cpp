// 2022/02/21 11:58:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


namespace FPGA
{
    namespace FreqMeter
    {
        float frequency = ERROR_VALUE_FLOAT;
        bool readPeriod = false;     // Установленный в true флаг означает, что частоту нужно считать по счётчику периода
        float prevFreq = 0.0f;

        BitSet32 ReadRegFreq();

        BitSet32 ReadRegPeriod();

        BitSet32 ReadRegFrequency();

        void Update(uint16 flag);

        void ReadFrequency();

        void ReadPeriod();

        float CalculateFrequencyFromCounterFrequency();

        float CalculateFrequencyFromCounterPeriod();

        float FromFrequencyCounter(const BitSet32 &fr)
        {
            return fr.word * 10.0F;
        }

        float FromPeriodCounter(const BitSet32 &period)
        {
            return (period.word == 0) ? 0.0f : 1e8f / period.word;
        }
    }
}


void FPGA::FreqMeter::Init()
{
    struct TimeCounting
    {
        enum E
        {
            _100ms,
            _1s,
            _10s
        };
    };
    
    struct FreqClc
    {
        enum E
        {
            _100kHz,
            _1MHz,
            _10MHz,
            _100MHz
        };
    };

    struct NumberPeriods
    {
        enum E
        {
            _1,
            _10,
            _100
        };
    };

    TimeCounting::E FREQ_METER_TIMECOUNTING = TimeCounting::_100ms;
    FreqClc::E FREQ_METER_FREQ_CLC = FreqClc::_100MHz;
    NumberPeriods::E FREQ_METER_NUM_PERIODS = NumberPeriods::_1;

    uint16 data = 0;

    const uint16 maskTime[3] = {0, 1, 2};
    const uint16 maskFreqClc[4] = {0, (1 << 2), (1 << 3), ((1 << 3) + (1 << 2))};
    const uint16 maskPeriods[3] = {0, (1 << 4), (1 << 5)};

    data |= maskTime[FREQ_METER_TIMECOUNTING];
    data |= maskFreqClc[FREQ_METER_FREQ_CLC];
    data |= maskPeriods[FREQ_METER_NUM_PERIODS];

    *WR_FREQ_METER_PARAMS = data;

    Reset();
}


void FPGA::FreqMeter::Reset()
{
    frequency = ERROR_VALUE_FLOAT;
}


float FPGA::FreqMeter::GetFreq()
{
    return frequency;
}


BitSet32 FPGA::FreqMeter::ReadRegFreq()
{
    BitSet32 fr;
    fr.half_word[0] = HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = HAL_FMC::Read(RD_FREQ_HI);
    return fr;
}


BitSet32 FPGA::FreqMeter::ReadRegPeriod()
{
    BitSet32 period;
    period.half_word[0] = HAL_FMC::Read(RD_PERIOD_LOW);
    period.half_word[1] = HAL_FMC::Read(RD_PERIOD_HI);

    return period;
}


BitSet32 FPGA::FreqMeter::ReadRegFrequency()
{
    BitSet32 fr;

    fr.half_word[0] = (uint8)HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = (uint8)HAL_FMC::Read(RD_FREQ_HI);

    return fr;
}


void FPGA::FreqMeter::Update(uint16 fl)
{
    bool freqReady = _GET_BIT(fl, FL_FREQ) == 1;
    bool periodReady = _GET_BIT(fl, FL_PERIOD) == 1;

    if (freqReady)
    {
        if (!readPeriod)
        {
            ReadFrequency();
        }
    }

    if (periodReady)
    {
        if (readPeriod)
        {
            ReadPeriod();
        }
    }
}


void FPGA::FreqMeter::ReadFrequency()            // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                           // После чтения автоматически запускается новый цикл счёта
    BitSet32 freqSet = ReadRegFreq();

    if (freqSet.word < 1000)
    {
        readPeriod = true;
    }
    else
    {
        float fr = FromFrequencyCounter(freqSet);

        if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
        {
            frequency = ERROR_VALUE_FLOAT;
        }
        else
        {
//            LOG_WRITE("вычисляем по частоте %d", freqSet);
            frequency = fr;
        }

        prevFreq = fr;
    }
}


void FPGA::FreqMeter::ReadPeriod()
{
    BitSet32 periodSet = ReadRegPeriod();
    float fr = FromPeriodCounter(periodSet);

    if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
    {
        frequency = ERROR_VALUE_FLOAT;
    }
    else
    {
//        LOG_WRITE("вычисляем по периоду %d", periodSet);
        frequency = fr;
    }

    prevFreq = fr;
    readPeriod = false;
}


float FPGA::FreqMeter::CalculateFrequencyFromCounterFrequency()
{
    frequency = 0.0f;

    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0)
    {
    };

    ReadRegFrequency();

    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0)
    {
    };

    BitSet32 fr = ReadRegFrequency();

    if (fr.word >= 5)
    {
        frequency = FromFrequencyCounter(fr);
    }

    return 0.0F;
}


float FPGA::FreqMeter::CalculateFrequencyFromCounterPeriod()
{
    frequency = 0.0f;

    uint time = TIME_MS;

    while (TIME_MS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), FL_PERIOD) == 0)
    {
    };

    ReadRegPeriod();

    time = TIME_MS;

    while (TIME_MS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), FL_PERIOD) == 0)
    {
    };

    BitSet32 period = ReadRegPeriod();

    if (period.word > 0 && (TIME_MS - time < 1000))
    {
        frequency = FromPeriodCounter(period);
    }

    return frequency;
}
