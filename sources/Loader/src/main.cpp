// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/Memory/Sector.h"
#include "Utils/Containers/String.h"
#include "main.h"
#include "Hardware/FDrive/FDrive.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include <cstdlib>
#include <usbh_def.h>

USBH_HandleTypeDef hUSBHost;

static void Update();
static void EraseSectors();


float MainStruct::percentUpdate = 0.0f;
State::E MainStruct::state = State::NoDrive;
int MainStruct::sizeFirmware = 0;
int MainStruct::sizeUpdated = 0;
int MainStruct::speed = 0;
int MainStruct::timeLeft = 0;


int main()
{
#ifndef ENABLE_UPDATE
    HAL::JumpToApplication();
#endif

    Settings::Load();

    HAL::Init();

    FDrive::Init();

    Display::Init();

    Timer::Enable(TypeTimer::DisplayUpdate, 10, Display::Update);

    uint timeStart = TIMER_MS;

    while (TIMER_MS - timeStart < TIME_WAIT)
    {
        FDrive::Update();

        if (MainStruct::state == State::DriveIsMounted)
        {
            if (FDrive::FileExist(FILE_NAME))
            {
                EraseSectors();

                Update();

                MainStruct::state = State::UpdateIsFinished;

                break;
            }
        }
    }

    Timer::Disable(TypeTimer::DisplayUpdate);

    while (Display::IsRunning())
    {
    }

    Display::Update();

    HAL::DeInit();

    HAL::JumpToApplication();

    return 0;
}


static void EraseSectors()
{
    MainStruct::state = State::EraseSectors;

    const int startSector = Sector::_05_FIRM_1;

    MainStruct::percentUpdate = 0.0f;

    int size = FDrive::OpenFileForRead(FILE_NAME);

    FDrive::CloseOpenedFile();

    int numSectors = size / (128 * 1024);

    if (size % (128 * 1024) != 0)
    {
        numSectors++;
    }

    for (int sector = startSector; sector <= startSector + numSectors; sector++)
    {
        Display::Update();

        Sector::Get((Sector::E)sector).Erase();

        MainStruct::percentUpdate += 1.0f / (float)numSectors;

        Display::Update();
    }

}


static void Update()
{
    MainStruct::state = State::UpdateInProgress;

    const int sizeSector = 1 * 1024;

    uint8 buffer[sizeSector];

    int size = FDrive::OpenFileForRead(FILE_NAME);
    const int fullSize = size;
    uint address = 0x08020000U;

    MainStruct::percentUpdate = 0.0f;

    MainStruct::sizeFirmware = size;
    MainStruct::sizeUpdated = 0;
    MainStruct::speed = 0;
    MainStruct::timeLeft = 0;

    uint timeStart = TIMER_MS;

    while (size)
    {
        uint readedBytes = FDrive::ReadFromFile(sizeSector, buffer);

        HAL_ROM::WriteBufferBytes(address, buffer, readedBytes);

        size -= readedBytes;
        address += readedBytes;

        MainStruct::percentUpdate = 1.0F - (float)(size) / (float)(fullSize);

        MainStruct::sizeUpdated += readedBytes;
        MainStruct::speed = (int)(MainStruct::sizeUpdated * 1000.0f / (TIMER_MS - timeStart));

        int timePassed = (int)(TIMER_MS - timeStart);       // Прошло времени
        int timeNeed = fullSize / MainStruct::speed * 1000;

        MainStruct::timeLeft = (timeNeed - timePassed) / 1000 + 1;
    }

    FDrive::CloseOpenedFile();
}


/*! \page page1 Алгоритм работы загрузчика
    \verbatim
    По адресу 0x08000000 находится загрузчик.
    После сброса он проверяет наличие флешки.
        Если флешка есть:
            проверяет наличие файла S8-53.bin. Если таковой имеется, выводит сообщение "На USB-диске обнаружено новоое программное обеспечение. Установить?"
            Если нажато "Да":
                1. Стираются сектора:
                    5 - 0x08020000
                    6 - 0x08040000
                    7 - 0x08060000
                2. На их место записывается содержимое файла S8-53M.bin
        Если флешку примонтировать не удалось:
            Вывести сообщение "Не удалось примонтировать флешку. Убедитесь, что на ней файловая система fat32"
    Далее выполняется переход по адресу, указанному в 0x0802004
    \endverbatim
*/
