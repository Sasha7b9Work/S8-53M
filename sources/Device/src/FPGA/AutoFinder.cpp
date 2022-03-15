// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Data/Storage.h"
#include "Hardware/Timer.h"
#include <stm32f4xx.h>
#include <cstring>
#include <climits>


namespace FPGA
{
    namespace AutoFinder
    {
        class DataFinder : public Buffer<uint8>
        {
        public:
            DataFinder() : Buffer<uint8>(1024) { }

            // ������ ������ � ��������� �������� �������������
            bool ReadDataWithSynchronization(Chan, uint timeWait);

            // ���������� ������ ������� - �������� ����� ����������� � ������������ ����������
            BitSet64 GetBound();
        };

        static Waiter waiter;

        static bool FindWave(Chan);

        static Range::E FindRange(Chan);

        static bool FindParams(TBase::E *);

        static void FunctionDraw();

        static TBase::E CalculateTBase(float freq);
    }

    namespace Reader
    {
        // ������ ���� ���� ������ 1 (� �������������� ��������������, ���� �����)
        BitSet16 ReadA();

        // ������ ���� ���� ������ 2 (� �������������� ��������������, ���� �����)
        BitSet16 ReadB();
    }
}


void FPGA::AutoFinder::FindSignal()
{
    /*
        �������� ������.
        1. ������������� �������� ���� �� ������, ������������� - ��.
        2. ������������� �������� 20��/���, �������� - 2��/���.
        3. ��������� ���.
        4. ��� �������������. ���� ������������� ��� - ��������� �� ������ �����. ���� ����:
        5. �������� ������� ������� � ��������� ������. ���� ��������� ������ �������� �� ������ ������ 2 ������ -
            ��������� � ������ �� ������� ������. ���� �������� ������ 2 ������:
        6. ������������� ������������ ������� 5��, 10��, 20�� � �����, ���� ��������� ������ ��������� �� �������� �
            �����.
        7. ��������� � ������ TBase.
    */

    /** \todo �������������� �������� � ����� ��������.\n
        1. ������� ������� ����� ����� ������� �������������.\n
        2. ����� ������������� ����� ��������, ����� �� ����� ������ ����������� �� ����� ���� ��������� �������������.\n
        3. ��� �������� ������� ��������� � ������������ ������.
    */

    bool sound_enabled = SOUND_ENABLED;
    SOUND_ENABLED = false;

    waiter.Reset();

    Display::SetDrawMode(DrawMode::Hand, FunctionDraw);

    Settings old = set;

    FPGA::Stop(false);

    if (!FindWave(ChA))
    {
        if (!FindWave(ChB))
        {
            Display::ShowWarningBad(Warning::SignalNotFound);
            set = old;
            set.RunAfterLoad();
            FPGA::Init();
        }
    }

    Display::SetDrawMode(DrawMode::Auto);

    SOUND_ENABLED = sound_enabled;

    FPGA::Start();
}


static bool FPGA::AutoFinder::FindWave(Chan ch)
{
    TBase::Set((TBase::E)(TBase::MIN_P2P - 1));
    TrigSource::Set(ch);
    TrigLev::Set(ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    TrigInput::Set(TrigInput::Full);

    Range::E range = FindRange(ch);

    if (range == Range::Count)
    {
        LOG_ERROR("%d : range �� ������", ch.ToNumber());
        return false;
    }
    else
    {
        LOG_WRITE("%d : range = %d", ch.ToNumber(), range);
    }

    Range::Set(ch, range);

    TBase::E tbase = TBase::Count;

    FindParams(&tbase);

    return (tbase != TBase::Count);
}


static Range::E FPGA::AutoFinder::FindRange(Chan ch)
{
    PeackDetMode::E peackDet = SET_PEAKDET;
    Range::E oldRange = SET_RANGE(ch);

    FPGA::Stop(false);

    PeackDetMode::Set(PeackDetMode::Enable);

    Range::E result = Range::Count;

    for (Range::E range = (Range::E)(Range::Count - 1); (int)range >= 0; --range)
    {
        Range::Set(ch, range);

        DataFinder data;

        if (data.ReadDataWithSynchronization(ch, 1000))
        {
            BitSet64 limits = data.GetBound();

            if (limits.iword[0] < ValueFPGA::MIN || limits.iword[1] > ValueFPGA::MAX)
            {
                result = range;

                if (result != Range::_20V)
                {
                    ++result;
                }
                
                break;
            }
        }
    }

    Range::Set(ch, oldRange);
    PeackDetMode::Set(peackDet);

    return result;
}


bool FPGA::AutoFinder::DataFinder::ReadDataWithSynchronization(Chan ch, uint time_wait)
{
    Fill(ValueFPGA::NONE);

    HAL_FMC::Write(WR_PRED, (uint16)(~(2)));
    HAL_FMC::Write(WR_POST, (uint16)(~(Size() + 20)));
    HAL_FMC::Write(WR_START, 1);

    while (_GET_BIT(flag.Read(), FL_PRED) == 0) { }

    Waiter localWaiter;

    while(_GET_BIT(flag.Read(), FL_TRIG) == 0)
    {
        if (waiter.ElapsedTime() > time_wait)
        {
            FPGA::Stop(false);

            LOG_ERROR("%d : range %d, ��� �������", ch.ToNumber(), SET_RANGE(ch));

            return false;
        }
    }

    while(_GET_BIT(flag.Read(), FL_DATA) == 0) { }

    uint16 address = (uint16)(HAL_FMC::Read(RD_ADDR_LAST_RECORD) - Size() - 2);
    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    typedef BitSet16 (*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    uint8 *elem = Data();
    uint8 *last = Last();

    funcRead();

    if (SET_TBASE <= TBase::MAX_RAND)
    {
        while (elem < last)
        {
            *elem++ = funcRead().byte0;
        }
    }
    else
    {
        while (elem < last)
        {
            BitSet16 bytes = funcRead();

            *elem++ = bytes.byte0;
            *elem++ = bytes.byte1;
        }
    }

    LOG_WRITE("%d : range %d ������ ������", ch.ToNumber(), SET_RANGE(ch));

    return true;
}


BitSet64 FPGA::AutoFinder::DataFinder::GetBound()
{
    int min = 255;
    int max = 0;

    uint8 *dat = Data();

    for (int i = 0; i < Size(); i++)
    {
        SET_MIN_IF_LESS(dat[i], min);
        SET_MAX_IF_LARGER(dat[i], max);
    }

    return BitSet64(min, max);
}


bool FPGA::AutoFinder::FindParams(TBase::E *tbase)
{
    TrigInput::Set(TrigInput::Full);

    Start();

    while (_GET_BIT(FPGA::flag.Read(), FL_FREQ) == 0) { };

    Stop(false);
    float freq = FreqMeter::GetFreq();

    TrigInput::Set(freq < 1e6f ? TrigInput::LPF : TrigInput::Full);

    Start();

    while (_GET_BIT(FPGA::flag.Read(), FL_FREQ) == 0) { };

    Stop(false);
    freq = FreqMeter::GetFreq();

    if (freq >= 50.0f)
    {
        *tbase = CalculateTBase(freq);

        if (*tbase >= TBase::MIN_P2P)
        {
            *tbase = TBase::MIN_P2P;
        }

        TBase::Set(*tbase);

        Start();

        TrigInput::Set(freq < 500e3f ? TrigInput::LPF : TrigInput::HPF);

        return true;
    }
    else
    {
        TrigInput::Set(TrigInput::LPF);
        freq = FreqMeter::GetFreq();

        if (freq > 0.0f)
        {
            *tbase = CalculateTBase(freq);

            if (*tbase >= TBase::MIN_P2P)
            {
                *tbase = TBase::MIN_P2P;
            }

            TBase::Set(*tbase);
            Timer::PauseOnTime(10);
            Start();
            return true;
        }
    }

    return false;
}


static void FPGA::AutoFinder::FunctionDraw()
{
    Painter::BeginScene(COLOR_BACK);
    Color::SetCurrent(COLOR_FILL);

    int width = 220;
    int height = 60;
    int x = 160 - width / 2;
    int y = 120 - height / 2;
    Painter::FillRegion(x, y, width, height, COLOR_BACK);
    Painter::DrawRectangle(x, y, width, height, COLOR_FILL);
    PText::DrawStringInCenterRect(x, y, width, height - 20, "��� ����� �������. ���������");

    char buffer[101] = "";
    uint progress = (waiter.ElapsedTime() / 50) % 80;

    for (uint i = 0; i < progress; i++)
    {
        std::strcat(buffer, ".");
    }

    PText::DrawStringInCenterRect(x, y + (height - 30), width, 20, buffer);

    Display::DrawConsole();

    Painter::EndScene();
}


static TBase::E FPGA::AutoFinder::CalculateTBase(float freq)
{
    struct STR
    {
        float    freq;
        TBase::E tbase;
    };

    static const STR structs[TBase::Count] =
    {
        {100e6f,    TBase::_2ns  },
        {40e6f,     TBase::_5ns  },
        {20e6f,     TBase::_10ns },
        {10e6f,     TBase::_20ns },
        {3e6f,      TBase::_50ns },
        {2e6f,      TBase::_100ns},
        {900e3f,    TBase::_200ns},
        {200e3f,    TBase::_1us  },
        {400e3f,    TBase::_500ns},
        {90e3f,     TBase::_2us  },
        {30e3f,     TBase::_5us  },
        {20e3f,     TBase::_10us },
        {10e3f,     TBase::_20us },
        {4e3f,      TBase::_50us },
        {2e3f,      TBase::_100us},
        {1e3f,      TBase::_200us},
        {350.0f,    TBase::_500us},
        {200.0f,    TBase::_1ms  },
        {100.0f,    TBase::_2ms  },
        {40.0f,     TBase::_5ms  },
        {20.0f,     TBase::_10ms },
        {10.0f,     TBase::_20ms },
        {4.0f,      TBase::_50ms },
        {2.0f,      TBase::_100ms},
        {0.0f,      TBase::Count }
    };


    const STR *str = &structs[0];

    while (str->freq != 0.0f)
    {
        if (freq >= str->freq)
        {
            return str->tbase;
        }
        ++str;
    }

    return TBase::_200ms;
}
