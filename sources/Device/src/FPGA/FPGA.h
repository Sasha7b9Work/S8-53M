// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"
#include "FPGA/TypesFPGA.h"
#include "FPGA/StructuresFPGA.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;
    static const int MAX_BYTES = MAX_POINTS * 2;

    extern StateFPGA state;

    extern TimeMeterUS meterStart;

    namespace FreqMeter
    {
        void Init();

        // Получить значение частоты.
        float GetFreq();

        void Update(uint16 flag);

        void Reset();
    }

    namespace AutoFinder
    {
        // Ищет сигнал. Если сигнал найден, то по завершении сигнал будет на экране
        void FindSignal();
    }

    namespace Compactor
    {
        // Если  true - будем уплотнять сигнал посредством установки предыдущей на 2 растяжки и
        // выкидыванием точки
        inline bool Enabled() { return SET_FPGA_COMPACT; };

        // Коэффициент уплотнения - можеть быть 1, 4, или 5.
        // Возвращается дле текущей SET_TBASE
        int Koeff();

        // Возвращает TBase, который нужно устанавливать при включённом уплотнении для текущей SET_TBASE
        TBase::E CompactTBase();
    }

    namespace Calibrator
    {
        // Запуск функции калибровки.
        void RunCalibrate();
    }

    struct Flag
    {
        uint16 value;

        uint16 Read()
        {
            value = HAL_FMC::Read(RD_FL);
            FreqMeter::Update(value);
            return value;
        }

        bool FirstByte() { return _GET_BIT(value, FL_LAST_RECOR) == 1; }
        bool Trig()      { return _GET_BIT(value, FL_TRIG) == 1; }
        bool Data()      { return _GET_BIT(value, FL_DATA) == 1; }
        bool Pred()      { return _GET_BIT(value, FL_PRED) == 1; }
    };

    extern Flag flag;

    void Init();

    // Установить количество считываемых сигналов в секунду.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

    // Запускает цикл сбора информации.
    void OnPressStartStop();

    // Запуск процесса сбора информации.
    void Start();

    // Прерывает процесс сбора информации.
    void Stop();

    // Возвращает true, если прибор находится не в процессе сбора информации.
    bool IsRunning();

    // Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора.
    void SetNumberMeasuresForGates(int number);

    // Принудительно запустить синхронизацию.
    void SwitchingTrig();

    // Загрузить регистр WR_UPR (пиковый детектор и калибратор и самописец).
    void LoadRegUPR();
}


namespace BUS_FPGA
{
    // Запись в регистр ПЛИС нового значения. restart - true означает, что после записи нужно запусить режим измерений,
    // если до этого прибор не находился в режиме паузы.
    void Write(uint16 *, uint16, bool restart);

    void WriteAnalog(TypeWriteAnalog::E, uint);

    void WriteDAC(TypeWriteDAC::E, uint16);
}
