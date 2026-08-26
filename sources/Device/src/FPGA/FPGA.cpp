// 2022/02/18 15:30:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/HAL/HAL.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Data/Processing.h"
#include <cstring>


#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                                        \
    if(SET_INVERSE(ch))                                                                                             \
    {                                                                                                               \
        data = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data, ValueFPGA::MIN, ValueFPGA::MAX));  \
    }                                                                                                               \
    *addr = data;


#define CLC_HI      Pin::SPI4_CLK.Set();
#define CLC_LOW     Pin::SPI4_CLK.Reset();
#define DATA_SET(x) Pin::SPI4_DAT.Write(x);

namespace FPGA
{
    StateFPGA state =
    {
        false,
        StateWorkFPGA::Stop,
        StateCalibration::None
    };

    uint timeStart = 0;

    TimeMeterUS meterStart;

    volatile static int numberMeasuresForGates = 1000;

    bool IN_PROCESS_READ = false;
    bool TRIG_AUTO_FIND = false;    // Установленное в 1 значение означает, что нужно производить автоматический поиск
                                    // синхронизации, если  выбрана соответствующая настройка.
    bool runningBeforeSmallButtons = false;
    bool CAN_READ_DATA = false;
    bool NEED_AUTO_TRIG = false;    // Если true, нужно делать автозапуск (автоматический режим запуска и отсутсвует
                                    // синхроимпульс

    Flag flag;

    // Перед каждым циклом измерений нужно произвести некоторые действия по подготовке
    void PrepareForCycle();

    void ProcessingData();

    // Смещение с АЦП рандомизатора
    int ShiftRandomizerADC();

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Эта функция вызывается по таймеру, когда можно считывать новыый сигнал
    void OnTimerCanReadData();

    void ProcessP2P();

    // Отложенная задача по выключению лампочки синхронизации
    void DefferedTaskTrigLED();

    namespace FreqMeter
    {
        void Update(uint16 flag);
    }

    namespace Reader
    {
        // Возвращает адрес, с которого нужно читать первую точку
        extern uint16 CalculateAddressRead();

        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();
    }
}


void FPGA::Init()
{
    Storage::Clear();
    FPGA::LoadSettings();
    FPGA::SetNumSignalsInSec(ENumSignalsInSec::ToNum(ENUM_SIGNALS_IN_SEC));
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FreqMeter::Init();
}


void FPGA::Update()
{
    DEBUG_POINT_0

    FPGA::Reader::P2P::SavePoints();

    DEBUG_POINT_0

    if (SET_SELFRECORDER)
    {
        return;
    }

    DEBUG_POINT_0

    flag.Read();

    DEBUG_POINT_0

    if (state.needCalibration)              // Если вошли в режим калибровки -
    {
        DEBUG_POINT_0

        FPGA::Calibrator::RunCalibrate();       // выполняем её.

        DEBUG_POINT_0

        state.needCalibration = false;

        DEBUG_POINT_0
    }

    DEBUG_POINT_0

    if(!CAN_READ_DATA)
    {
        DEBUG_POINT_0

        return;
    }

    DEBUG_POINT_0

    if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
    {
        DEBUG_POINT_0

        return;
    }

    DEBUG_POINT_0

    for (int i = 0; i < TBase::StretchRand(); i++)
    {
        DEBUG_POINT_0

        ProcessingData();

        DEBUG_POINT_0

        if (Flags::needFinishRead)
        {
            DEBUG_POINT_0

            Flags::needFinishRead = false;
            break;
        }

        DEBUG_POINT_0

        if (Panel::Data::Exist())
        {
            DEBUG_POINT_0

            break;
        }
    }

    DEBUG_POINT_0

    CAN_READ_DATA = false;

    DEBUG_POINT_0
}


void FPGA::DefferedTaskTrigLED()
{
    LED::Trig.Disable();
    Timer::Disable(TypeTimer::DisableTrigLED);
}


void FPGA::ProcessingData()
{
    DEBUG_POINT_0

    flag.Read();

    DEBUG_POINT_0

    if (flag.Trig() && !LED::Trig.dontFireTrig)
    {
        DEBUG_POINT_0

        LED::Trig.Enable();
        DEBUG_POINT_0

        Timer::Disable(TypeTimer::DisableTrigLED);
        DEBUG_POINT_0
    }

    DEBUG_POINT_0
    if (NEED_AUTO_TRIG)
    {
        DEBUG_POINT_0
        if (TIME_MS - timeStart > 500)
        {
            DEBUG_POINT_0
            SwitchingTrig();
            DEBUG_POINT_0
            LED::dontFireTrig = true;
            TRIG_AUTO_FIND = true;
            NEED_AUTO_TRIG = false;
            DEBUG_POINT_0
        }
        else if (flag.Trig())
        {
            DEBUG_POINT_0
            NEED_AUTO_TRIG = false;
            LED::dontFireTrig = false;
            DEBUG_POINT_0
        }
    }
    else if (flag.Data())
    {
        DEBUG_POINT_0
        Stop();
        DEBUG_POINT_0

        Reader::DataRead();

        DEBUG_POINT_0

        Timer::Enable(TypeTimer::DisableTrigLED, 500, DefferedTaskTrigLED);

        DEBUG_POINT_0

        if (!START_MODE_IS_SINGLE)
        {
            DEBUG_POINT_0
            Start();
            DEBUG_POINT_0
            StateWorkFPGA::SetCurrent(StateWorkFPGA::Work);
            DEBUG_POINT_0
        }
    }
    else
    {
        DEBUG_POINT_0
        if (flag.Pred())
        {
            DEBUG_POINT_0
            if (START_MODE_IS_AUTO)
            {
                DEBUG_POINT_0
                NEED_AUTO_TRIG = true;
                DEBUG_POINT_0
            }
            DEBUG_POINT_0
            timeStart = TIME_MS;
            DEBUG_POINT_0
        }
        DEBUG_POINT_0
    }
    DEBUG_POINT_0
}


void FPGA::SwitchingTrig()
{
    static const uint16 value[2][2] =
    {
        {1, 0},
        {0, 1}
    };

    HAL_FMC::Write(WR_TRIG, value[TRIG_POLARITY][0]);
    HAL_FMC::Write(WR_TRIG, value[TRIG_POLARITY][1]);
}


void FPGA::PrepareForCycle()
{
    Storage::current.PrepareForNewCycle();

    if (TBase::InModeP2P() || SET_SELFRECORDER)
    {
        Timer::Enable(TypeTimer::P2P, 1, Reader::P2P::ReadPoints);
    }
    else
    {
        Timer::Disable(TypeTimer::P2P);
    }
}


void FPGA::Start()
{
    PrepareForCycle();

    while (meterStart.ElapsedUS() < 220) {}

    meterStart.Reset();

    HAL_FMC::Write(WR_PRED, FPGA::Launch::PredForWrite());
    HAL_FMC::Write(WR_START, 1);

    timeStart = TIME_MS;
    StateWorkFPGA::SetCurrent(StateWorkFPGA::Wait);

    NEED_AUTO_TRIG = false;

    LED::dontFireTrig = false;
}


bool FPGA::IsRunning()
{
    return (StateWorkFPGA::GetCurrent() != StateWorkFPGA::Stop);
}


void FPGA::Stop()
{
    if (FPGA::IsRunning())
    {
        Timer::Disable(TypeTimer::P2P);

        HAL_FMC::Write(WR_STOP, 1);

        StateWorkFPGA::SetCurrent(StateWorkFPGA::Stop);
    }
}


void FPGA::OnPressStartStop()
{
    if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
    {
        FPGA::Start();
    }
    else
    {
        FPGA::Stop();
    }
}


void FPGA::SetNumSignalsInSec(int numSigInSec)
{
    Timer::Enable(TypeTimer::NumSignalsInSec, 1000.f / numSigInSec, OnTimerCanReadData);
}


void FPGA::OnTimerCanReadData()
{
    CAN_READ_DATA = true;
}


void FPGA::SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}


void BUS_FPGA::WriteDAC(TypeWriteDAC::E, uint16 data)
{
    bool running = FPGA::IsRunning();

    FPGA::Stop();

    Pin::SPI4_CS1.Reset();

    for (int i = 15; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS1.Set();

    if (running)
    {
        FPGA::Start();
    }
}


void BUS_FPGA::WriteAnalog(TypeWriteAnalog::E, uint data)
{
    bool running = FPGA::IsRunning();

    FPGA::Stop();

    Pin::SPI4_CS2.Reset();

    for (int i = 23; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS2.Set();

    if (running)
    {
        FPGA::Start();
    }
}


int FPGA::ShiftRandomizerADC()
{
    if (TBase::InModeRandomizer())
    {
        uint16 rand = HAL_ADC1::GetValue();

        if (rand == 0xFFFF)
        {
            return TShift::ERROR;
        }

        uint16 min = 0;
        uint16 max = 0;

        if (SOUND_ENABLED && Panel::TimePassedAfterLastEvent() < 100)
        {
            return TShift::ERROR;
        }

        if (!CalculateGate(rand, &min, &max))
        {
            return TShift::ERROR;
        }

        if (rand > max - nrst.fpga_gates_max)
        {
            return TShift::ERROR;
        }

        if (rand < min + nrst.fpga_gates_min)
        {
            return TShift::ERROR;
        }

        float tin = (float)(rand - min) / (float)(max - min) * 10e-9f;
        int shift = (int)(tin / 10e-9f * (float)TBase::StretchRand());

        return shift;
    }

    return 0;
}


bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (rand < 500 || rand > 3500)
    {
        return false;
    }

    static const int numValues = 1000;

    static float minGate = 0.0f;
    static float maxGate = 0.0f;
    static int numElements = 0;
    static uint16 min = 0xffff;
    static uint16 max = 0;

    numElements++;
    if (rand < min)
    {
        min = rand;
    }
    if (rand > max)
    {
        max = rand;
    }

    if (minGate == 0)
    {
        *eMin = min;
        *eMax = max;
        if (numElements < numValues)
        {
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if (numElements == numValues)
    {
        minGate = 0.9f * minGate + 0.1f * min;
        maxGate = 0.9f * maxGate + 0.1f * max;
//        LOG_WRITE("вор %.0F ... %.0F, min = %u, max = %u", minGate, maxGate, min, max);
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    *eMin = (uint16)minGate; //-V519
    *eMax = (uint16)maxGate; //-V519

    return true;
}


void BUS_FPGA::Write(uint16 *address, uint16 data, bool restart)
{
    bool is_running = FPGA::IsRunning();

    if (restart)
    {
        FPGA::Stop();
    }

    HAL_FMC::Write(address, data);

    if (is_running && restart)
    {
        FPGA::Start();
    }
}
