// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Debug.h"
#include "Hardware/Timer.h"
#include "Data/Storage.h"


static uint timeStart;
static uint timePrev;
static uint timeCounter;
static uint timeStartCounter;

// Время старта профилировщика
volatile static uint timeStartMS = 0;
// Время предыдущей точки профилировщика
volatile static uint timePrevMS = 0;

pchar Debug::file[10] = { 0 };
int   Debug::line[10] = { 0 };

int Debug::count_data = 0;
int *Debug::address_count_data = nullptr;
uint *Debug::address_first_ds = nullptr;


void Debug::StartProfilingMS()
{
    timeStartMS = TIME_MS;
    timePrevMS = TIME_MS; //-V656
}


uint Debug::PointProfilingMS(pchar _file, int _line)
{
    uint d = TIME_MS - timePrevMS;
    LOG_WRITE("%s %d %d", _file, _line, d);
    timePrevMS = TIME_MS;

    return d;
}


void Debug::StartProfilingUS()
{
    timeStart = TIME_TICKS;
    timePrev = timeStart;
}


uint Debug::PointProfilingUS(pchar _file, int _line)
{
    uint d = TIME_TICKS - timePrev;
    LOG_WRITE("%s:%d - %d us", _file, _line, d);
    timePrev = TIME_TICKS;
    return d;
}


void Debug::ClearTimeCounter()
{
    timeCounter = 0;
}


void Debug::StartIncreaseCounter()
{
    timeStartCounter = TIME_TICKS;
}


void Debug::StopIncreaseCounter()
{
    timeCounter += (TIME_TICKS - timeStartCounter);
}


uint Debug::GetTimeCounterUS()
{
    return timeCounter;
}


void Debug::Function()
{
    address_first_ds = (uint *)Storage::first_ds;
    if((uint)address_first_ds < 0x64000000 &&
        (uint)address_first_ds != 0x0)
    {
        HardFault_Handler();
    }
}


bool Debug::MemoryTest()
{
    uint16 *end = (uint16 *)(HAL_FMC::ADDR_RAM_BEGIN + HAL_FMC::RAM_SIZE);

    for (uint16 *address = (uint16 *)HAL_FMC::ADDR_RAM_BEGIN; address < end; address++)
    {
        *address = (uint16)((uint)address);
    }

    for (uint16 *address = (uint16 *)HAL_FMC::ADDR_RAM_BEGIN; address < end; address++)
    {
        if (*address != (uint16)((uint)address))
        {
            return false;
        }
    }

    return true;
}
