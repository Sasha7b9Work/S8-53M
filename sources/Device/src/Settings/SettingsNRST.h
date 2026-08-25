// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define NUM_STRINGS             (nrst.numStrings)
#define SIZE_FONT_CONSOLE       (nrst.sizeFont)
#define NUM_MEAS_FOR_GATES      (nrst.numMeasuresForGates)
#define SHOW_STATS              (nrst.showStats)

#define SET_BALANCE_ADC(ch)     (nrst.balanceADC[ch])
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(ChA))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(ChB))

#define SET_FPGA_COMPACT        nrst.fpga_compact
#define FPGA_GATES_MIN          nrst.fpga_gates_min
#define FPGA_GATES_MAX          nrst.fpga_gates_max

#define RSHIFT_HAND(ch, range)  nrst.hand_rshift[ch][range]

namespace CAL
{
    // Значение калибровочного смещения. В диапазонах 2мВ, 5мВ, 10мВ здесь учитывается ручное смещение, а на остальных-
    // только автоматическая калибровка
    int RShift(Chan, Range::E);
}


// Несбрасываемые настройки настройки.
struct SettingsNRST
{
    static const int SIZE_FIELD_RECORD = 64;        // Размер поля для записи настроек. Этот размер должен быть больше размера структуры.
                                                    // Адрес сохранения всегда выровнен по этому значению

    uint    crc32;                  // Контрольная сумма. Подсчитывается от первого байта, следующего за последним байтом crc32
    int16   numStrings;             // Число строк в консоли.
    int8    sizeFont;               // Размер шрифта консоли - 0 - 5, 1 - 8,
    int16   numMeasuresForGates;    // Число измерений для ворот.
    bool    showStats;              // Показывать статистику на экране (fps, например).

    int16   numAveForRand;          // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool    fpga_compact;           // Если true - уплотнять сигнал (FPGA::Compactor)
    int16   fpga_gates_min;         // \ Минимальные и максимальные значения, за пределами которых нужно отбрасывать
    int16   fpga_gates_max;         // /

    int16   first_byte;

    int16   balanceADC[Chan::Count];        // Баланс АЦП (смещение первой точки относительно второй) для обоих каналов
    int16   hand_rshift[Chan::Count][3];    // Вручную устанавливаемое дополнительное смещение. Для закрытого входа и 2мВ, 5мВ, 10мВ
    uint    not_used;                       // Это поле всегда равно нулю. Для правильного подсчёта контрольной суммы

    void Load();

    bool Save();

    uint CalculateCRC32();

    void ClearHandRShift(Chan);

    // Сохранить и очистить ручную добавку смещения
    void StoreAndClearHandRShift(Chan, Range::E);
    void RestoreHandRShift(Chan, Range::E);
    static int16 stored_rshift[Chan::Count][3];

    bool operator==(const SettingsNRST &);
};


extern SettingsNRST nrst;
