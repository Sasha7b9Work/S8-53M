// 2022/02/23 08:43:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include <climits>


namespace FPGA
{
    namespace Launch
    {
        static int pred = 0;    // Пред- и после- запуски хранятся в точках.
        static int post = 0;    // Перед засылкой их нужно уменьшать в два раза, потому что две точки считываются за раз

        void Calculate();

        // Возвращают значение, готовое для записи в ПЛИС
        uint16 PostForWrite();
    }
}


void FPGA::Launch::Load()
{
    if (!FPGA::Reader::mutex_read.Locked())
    {
        Calculate();

        BUS_FPGA::Write(WR_POST, PostForWrite(), true);
        BUS_FPGA::Write(WR_PRED, PredForWrite(), true);
    }
}


uint16 FPGA::Launch::PostForWrite()
{
    return (uint16)(~post);
}


uint16 FPGA::Launch::PredForWrite()
{
    return (uint16)(~pred);
}


uint16 FPGA::Reader::CalculateAddressRead()
{
    static const int shift[ENUM_POINTS_FPGA::Count][TPos::Count][TBase::Count] =
    {
        {
        //    2ns  5ns 10ns 20ns 50ns 100ns 200ns                                                                                                   // 512 points
            {  -9,  0,   1,   8, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Left
            { -11, -4,  -5,  -1, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Center
            { -11, -7, -11, -11, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }      // TPos::Right
        },
        {
        //    2ns  5ns  10ns 20ns 50ns 100ns 200ns                                                                                                   // 512 points
            {  -9,  0,    1,   8, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Left
            { -11, -7,  -11, -11, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Center
            { -11, -12, -22, -29, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }      // TPos::Right
        },
        {
        //    2ns  5ns  10ns 20ns 50ns 100ns 200ns                                                                                                   // 1024 points
            {  -9,   0,   1,   8, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Left
            { -14, -13, -22, -30, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     // TPos::Center
            { -19, -24, -44, -67, 9,   4,    2,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }      // TPos::Right
        }
    };

    if (TBase::InModeRandomizer())
    {
        return (uint16)(HAL_FMC::FPGA::Read(RD_ADDR_LAST_RECORD) -
            ENUM_POINTS_FPGA::ToNumBytes() * Compactor::Koeff() / (TBase::StretchRand() - 1) + shift[SET_ENUM_POINTS][SET_TPOS][SET_TBASE] - 1);
    }
    else
    {
        return (uint16)(HAL_FMC::FPGA::Read(RD_ADDR_LAST_RECORD) -
            ENUM_POINTS_FPGA::ToNumBytes() / 2 * FPGA::Compactor::Koeff() + shift[SET_ENUM_POINTS][SET_TPOS][SET_TBASE] - 5);
    }
}


void FPGA::Launch::Calculate()
{
    static const int d_post[TBase::Count] =   // Дополнительное смещение для послезапуска
    {// 2   5   10  20 50 100 200 500                ns
        20, 11, 10, 2, 0, 0,  0,  0,
     // 1                                                   us
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int num_bytes = ENUM_POINTS_FPGA::ToNumBytes() * Compactor::Koeff();

    int values[TPos::Count] = { 0, num_bytes / 4, num_bytes / 2 };

    pred = post = values[SET_TPOS];

    int tshift = SET_TSHIFT * (SET_PEAKDET_IS_ENABLED ? 2 : 1);

    pred = Math::Limitation(pred - tshift, 0, 65535);

    post = Math::Limitation(post + tshift, 0, 65535);

    if (TBase::InModeRandomizer())
    {
        if (pred + post < num_bytes)
        {
            post = num_bytes - pred;
        }

        int stretch = TBase::StretchRand();

        pred = pred / (stretch - 1) + 50;
        post = post / (stretch - 1) + d_post[SET_TBASE];
    }
    else
    {
        if (pred + post < num_bytes / 2)
        {
            post = num_bytes / 2 - pred;
        }

        post += 4;
    }
}


TBase::E FPGA::Compactor::CompactTBase()
{
    int base = SET_TBASE;

    return (TBase::E)Math::Limitation<int>(base - 2, 0, base);
}


int FPGA::Compactor::Koeff()
{
    if (!Enabled())
    {
        return 1;
    }

    // При включённом уплотнении сигнала будем засылать бОльшую растяжку, чем установлено (на 2 ступени)
    // После стрелки указана TBase, которую нужно засылать
    static const int koeff[TBase::Count] =
    {
        1,  // 2ns    -> 
        1,  // 5ns    ->
        1,  // 10ns   ->
        1,  // 20ns   ->
        1,  // 50ns   ->
        1,  // 100ns  ->
        1,  // 200ns  ->

        5,  // 500ns  -> 100 ns
        5,  // 1us    -> 200 ns
        4,  // 2us    -> 500 ns
        5,  // 5us    ->   1 us
        5,  // 10us   ->   2 us
        4,  // 20us   ->   5 us
        5,  // 50us   ->  10 us
        5,  // 100us  ->  20 us
        4,  // 200us  ->  50 us
        5,  // 500us  -> 100 us
        5,  // 1ms    -> 200 us
        4,  // 2ms    -> 500 us
        5,  // 5ms    ->   1 ms
        5,  // 10ms   ->   2 ms
        4,  // 20ms   ->   5 ms
        5,  // 50ms   ->  10 ms
        5,  // 100ms  ->  20 ms
        4,  // 200ms  ->  50 ms
        5,  // 500ms  -> 100 ms
        5,  // 1s     -> 200 ms
        4,  // 2s     -> 500 ms
        5,  // 5s     ->   1 s
        5,  // 10s    ->   2 s
    };

    return koeff[SET_TBASE];
}
