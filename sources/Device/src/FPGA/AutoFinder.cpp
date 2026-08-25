// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Data/Storage.h"
#include "Hardware/Timer.h"
#include "Display/Screen/Console.h"
#include "Utils/Text/Warnings.h"
#include <stm32f4xx.h>
#include <cstring>
#include <climits>


namespace FPGA
{
    namespace AutoFinder
    {
        class DataFinder : public Buffer512<uint8>
        {
        public:
            DataFinder() : Buffer512<uint8>() { }

            // Читать данные с ожиданием импульса синхронизации
            bool ReadDataWithoutSynchronization(Chan);

            // Возвращает размах сигнала - разность между минимальным и максимальным значениями
            BitSet64 GetBound();
        };

        static TimeMeterMS waiter;

        static bool FindWave(Chan);

        static bool FindRange(Chan);

        static bool FindTBase();

        static void FunctionDraw();

        static TBase::E CalculateTBase(float freq);
    }

    namespace Reader
    {
        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();
    }

    // Принудительно запустить синхронизацию.
    void SwitchingTrig();

}


void FPGA::AutoFinder::FindSignal()
{
    /*
        Алгоритм поиска.
        1. Устанавливаем закрытый вход по каналу, синхронизация - ПС.
        2. Устанавливаем развёртку 20мс/дел, растяжку - 2мВ/дел.
        3. Запускаем АЦП.
        4. Ждём синхронизацию. Если синхронизации нет - переходим на второй канал. Если есть:
        5. Включаем пиковый детектр и считываем данные. Если считанные данные занимают по высоте меньше 2 клеток -
            переходим к поиску по второму каналу. Если занимают больше 2 клеток:
        6. Устанавливаем вертикальный масштаб 5мВ, 10мВ, 20мВ и более, пока считанный сигнал полностью не впишется в
            экран.
        7. Переходим к поиску TBase.
    */

    /** \todo Оптимизировать алгоритм в плане скорости.\n
        1. Сначала находим время между флагами снихронизации.\n
        2. Затем устанавливаем такую развёртку, чтобы на длину памяти приходилось не более двух импульсов синхронизации.\n
        3. Это позволит быстрее считывать и обрабатывать данные.
    */

    bool sound_enabled = SOUND_ENABLED;
    SOUND_ENABLED = false;

    waiter.Reset();

    Display::SetDrawMode(DrawMode::Timer, FunctionDraw);

    Settings old = gset;

    bool running = FPGA::IsRunning();
    FPGA::Stop();

    if (!FindWave(ChA))
    {
        if (!FindWave(ChB))
        {
            Warning::ShowBad(Warning::SignalNotFound);
            gset = old;
            gset.RunAfterLoad();
            FPGA::Init();
        }
    }

    Display::SetDrawMode(DrawMode::Normal);

    SOUND_ENABLED = sound_enabled;

    if (running)
    {
        FPGA::Start();
    }
}


static bool FPGA::AutoFinder::FindWave(Chan ch)
{
    TBase::Set((TBase::E)(TBase::MIN_P2P));
    TrigSource::Set(ch);
    TrigLev::Set(ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    TrigInput::Set(TrigInput::Full);

    if (FindRange(ch))
    {
//        LOG_WRITE("range(%d) = %s", ch.value, Range::ToName(SET_RANGE(ch)));

        if (FindTBase())
        {
//            LOG_WRITE("tbase = %s", TBase::ToString(SET_TBASE));

            return true;
        }
    }

    return false;
}


static bool FPGA::AutoFinder::FindRange(Chan ch)
{
    struct StructFindRange
    {
        PeackDetMode::E peackDet;
        Range::E range;

        StructFindRange(Chan ch) : peackDet(SET_PEAKDET), range(SET_RANGE(ch)) { }

        void Restore(Chan ch)
        {
            Range::Set(ch, range);
            PeackDetMode::Set(peackDet);
        }
    } structFind (ch);

    bool result = false;

    FPGA::Stop();

    PeackDetMode::Set(PeackDetMode::Enable);

    for(int r = Range::_20V; r >= 0; r--)
    {
        Range::Set(ch, (Range::E)r);

        Timer::PauseOnTime(100);

        DataFinder data;

        data.ReadDataWithoutSynchronization(ch);

        BitSet64 limits = data.GetBound();

        if (limits.iword[0] < ValueFPGA::MIN || limits.iword[1] > ValueFPGA::MAX)
        {
            structFind.range = (Range::E)Math::Limitation<int>(r + 1, 0, Range::_20V);

            result = true;

            break;
        }
    }

    structFind.Restore(ch);

    return result;
}


bool FPGA::AutoFinder::DataFinder::ReadDataWithoutSynchronization(Chan ch)
{
    Fill(ValueFPGA::NONE);

    HAL_FMC::Write(WR_PRED, (uint16)(~(Size() / 2 + 20)));
    HAL_FMC::Write(WR_POST, (uint16)(~(Size() / 2 + 20)));
    HAL_FMC::Write(WR_START, 1);

    while (_GET_BIT(flag.Read(), FL_PRED) == 0) { }

    SwitchingTrig();

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


bool FPGA::AutoFinder::FindTBase()
{
    TrigInput::Set(TrigInput::Full);

    FreqMeter::Reset();

    Start();

    flag.Read();

    TimeMeterMS meter;

    while (!flag.Trig())
    {
        if (meter.ElapsedTime() > 5000)
        {
//            LOG_WRITE("to long time");

            Stop();

            return false;
        }

        flag.Read();

        if (FreqMeter::GetFreq() != ERROR_VALUE_FLOAT)
        {
//            LOG_WRITE("find frequency %f", FreqMeter::GetFreq());

            break;
        }
    }

    meter.Reset();

    float frequency = FreqMeter::GetFreq();

    while (frequency == ERROR_VALUE_FLOAT)
    {
        frequency = FreqMeter::GetFreq();

        if (meter.ElapsedTime() > 5000)
        {
            Stop();
            return false;
        }
    }

    TBase::E tbase = CalculateTBase(frequency);

    TBase::Set(tbase);

    Stop();

    return true;
}


static void FPGA::AutoFinder::FunctionDraw()
{
    Painter::BeginScene(COLOR_BACK);
    Color::SetCurrent(COLOR_FILL);

    int width = 220;
    int height = 60;
    int x = 160 - width / 2;
    int y = 120 - height / 2;
    Region(width, height).Fill(x, y, COLOR_BACK);
    Rectangle(width, height).Draw(x, y, COLOR_FILL);
    String<>("Идёт поиск сигнала. Подождите").DrawInCenterRect(x, y, width, height - 20);

    char buffer[101] = "";
    uint progress = (waiter.ElapsedTime() / 50) % 80;

    for (uint i = 0; i < progress; i++)
    {
        std::strcat(buffer, ".");
    }

    String<>(buffer).DrawInCenterRect(x, y + (height - 30), width, 20);

    Console::Draw();

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
