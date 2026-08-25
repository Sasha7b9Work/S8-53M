// 2022/02/22 16:16:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Data/Storage.h"
#include "Utils/Containers/Queue.h"
#include "Panel/Panel.h"


namespace FPGA
{
    extern bool IN_PROCESS_READ;
    extern bool TRIG_AUTO_FIND;

    extern int ShiftRandomizerADC();

    namespace Reader
    {
        Mutex mutex_read;

        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();

        namespace P2P
        {
            static const int SIZE_BUFFER = 10;
            BitSet16 bufferA[SIZE_BUFFER];
            BitSet16 bufferB[SIZE_BUFFER];
            int pointer = 0;

            Mutex mutex;        // Мьютекс лочится в основном потоке. И если по прерыванию приходит запрос на чтение
                                // точек, то он просто пропускается
        }
    }
}


void FPGA::Reader::ReadPoints(Chan ch, uint8 *first, const uint8 *last)
{
    uint16 address = Reader::CalculateAddressRead();

    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    typedef BitSet16(*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    funcRead();                 // Делаем лишнее чтение, потому что адрес первой точки на 1 меньше требуемого (что-то глючит в альтере)

    uint8 *data = first;

    while (data < last)
    {
        BitSet16 bytes = funcRead();

        *data++ = bytes.byte0;
        *data++ = bytes.byte1;
    }
}


BitSet16 FPGA::Reader::ReadA()
{
    BitSet16 data(*RD_ADC_A);

    int delta0 = data.byte0 - ValueFPGA::AVE;
    delta0 *= CAL_STRETCH_A;
    
    int byte0 = ValueFPGA::AVE + delta0;

    if (byte0 < 0) { byte0 = 0; }
    else if (byte0 > 255) { byte0 = 255; }

    data.byte0 = (uint8)byte0;

    int byte1 = (int)data.byte1 + SET_BALANCE_ADC_A;

    int delta1 = byte1 - ValueFPGA::AVE;
    delta1 *= CAL_STRETCH_A;
    byte1 = ValueFPGA::AVE + delta1;

    if (byte1 < 0)        { byte1 = 0; }
    else if (byte1 > 255) { byte1 = 255; }

    data.byte1 = (uint8)byte1;

    return data;
}


BitSet16 FPGA::Reader::ReadB()
{
    BitSet16 data(*RD_ADC_B);

    int delta0 = data.byte0 - ValueFPGA::AVE;
    delta0 *= CAL_STRETCH_B;

    int byte0 = ValueFPGA::AVE + delta0;

    if (byte0 < 0) { byte0 = 0; }
    else if (byte0 > 255) { byte0 = 255; }

    data.byte0 = (uint8)byte0;

    int byte1 = (int)data.byte1 + SET_BALANCE_ADC_B;

    int delta1 = byte1 - ValueFPGA::AVE;
    delta1 *= CAL_STRETCH_B;
    byte1 = ValueFPGA::AVE + delta1;

    if (byte1 < 0)        { byte1 = 0; }
    else if (byte1 > 255) { byte1 = 255; }

    data.byte1 = (uint8)byte1;

    return data;
}


void FPGA::Reader::Read1024Points(uint8 buffer[1024], Chan ch)
{
    Timer::PauseOnTime((SET_RANGE(ch) < 2) ? 500U : 100U);

    FPGA::Start();

    std::memset(buffer, 255, 1024);

    Timer::PauseOnTime(8);

    uint16 fl = HAL_FMC::Read(RD_FL);

    while (_GET_BIT(fl, FL_PRED) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::SwitchingTrig();

    while (_GET_BIT(fl, FL_TRIG) == 0) { fl = HAL_FMC::Read(RD_FL); }

    Timer::PauseOnTime(8);

    while (_GET_BIT(fl, FL_DATA) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::Stop();

    FPGA::Reader::ReadPoints(ch, buffer, &buffer[0] + 1024);
}


void FPGA::Reader::P2P::ReadPoints()
{
    if (!mutex.Locked())
    {
        flag.Read();

        if (_GET_BIT(flag.value, FL_POINT))
        {
            if (pointer < SIZE_BUFFER)
            {
                bufferA[pointer] = Reader::ReadA();
                bufferB[pointer] = Reader::ReadB();
                pointer++;
            }
            else
            {
                Reader::ReadA();
                Reader::ReadB();
            }

            if (SET_SELFRECORDER)
            {
                HAL_FMC::Write(WR_START, 1);
            }
        }
    }
}


void FPGA::Reader::P2P::SavePoints()
{
    mutex.Lock();

    if (pointer > 0)
    {
        int index = 0;

        while (index < pointer)
        {
            Storage::current.AppendPoints(bufferA[index], bufferB[index]);
            index++;
        }

        pointer = 0;
    }

    mutex.Unlock();
}


void FPGA::Reader::DataRead()
{
    IN_PROCESS_READ = true;

    ReadPoints(ChA, Storage::current.data);
    ReadPoints(ChB, Storage::current.data);

    Storage::Append(Storage::current.data);

    if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
    {
        TrigLev::FindAndSet();

        TRIG_AUTO_FIND = false;
    }

    IN_PROCESS_READ = false;
}


void FPGA::Reader::ReadPoints(Chan ch, DataStruct &data_out)
{
    static uint16 address = 0;

    if (ch.IsA())
    {
        address = Reader::CalculateAddressRead();
    }

    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    uint8 *const begin = data_out.Data(ch).Data();
    const uint8 *const end = data_out.Data(ch).Last();

    uint8 *dat = begin;

    typedef BitSet16(*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    funcRead();         // Это лишнее чтение сделано потому, что первая считанная точка бракованная.
                        // Для компенсации этого эффекта лишняя точка получается уменьшением адреса на один в
                        // FPGA::Reader::CalculateAddressRead()

    if (SET_PEAKDET_IS_ENABLED)
    {
        uint8 *p = dat;
        p++;                                        // Пробуем сдвигать для пикового детектора

        while (p < end && IN_PROCESS_READ)
        {
            BitSet16 bytes = funcRead();            // Данные в режиме пикового детектора хранятся по очереди:
            *p++ = bytes.byte0;                     // максимальное значение
            if (p < end)
            {
                *p++ = bytes.byte1;                 // минимальное значение
            }
        }

        *dat = *(dat + 1);                          // Тупо первый байт мухлюем
    }
    else
    {
        const int stretch = TBase::StretchRand();

        const int shift_rand = ShiftRandomizerADC();

        if (shift_rand == TShift::ERROR)
        {
            return;
        }

        dat += shift_rand;

        if (Compactor::Koeff() == 1)             // Без уплотнения
        {
            if (TBase::InModeRandomizer())
            {
                dat += TShift::ShiftForRandomizer();

                while (dat < begin)
                {
                    dat += stretch;
                    funcRead();
                }

                while (dat < end && IN_PROCESS_READ)
                {
                    BitSet16 bytes = funcRead();

                    *dat = bytes.byte0;
                    dat += stretch;
                }
            }
            else
            {
                if (!flag.FirstByte())
                {
                    BitSet16 bytes = funcRead();
                    *dat = bytes.byte1;
                    dat += stretch;
                }

                while (dat < end && IN_PROCESS_READ)
                {
                    BitSet16 bytes = funcRead();

                    *dat = bytes.byte0;
                    dat += stretch;
                    *dat = bytes.byte1;
                    dat += stretch;
                }
            }

        }
        else if (Compactor::Koeff() == 4)       // Выкидываются 3 байта из 4 (уплотнение в 4 раза)
        {
            while (dat < end && IN_PROCESS_READ)
            {
                BitSet16 data1 = funcRead();

                *dat = data1.byte0;
                dat += stretch;

                funcRead();
            }
        }
        else if (Compactor::Koeff() == 5)       // Выкидываются 4 байта зи 5 (уплотнение в 5 раз)
        {
            while (dat < end && IN_PROCESS_READ)
            {
                BitSet16 data1 = funcRead();
                BitSet16 data2 = funcRead();
                BitSet16 data3 = funcRead(); //-V821
                BitSet16 data4 = funcRead();
                BitSet16 data5 = funcRead();

                *dat = data1.byte0;
                dat += stretch;

                if (dat < end)
                {
                    *dat = data3.byte1;
                    dat += stretch;
                }
            }
        }
    }
}
