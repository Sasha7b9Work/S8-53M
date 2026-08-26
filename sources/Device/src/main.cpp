// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "Hardware/LAN/LAN.h"
#include "SCPI/SCPI.h"
#include <stm32f4xx_hal.h>


int main()
{
    HAL::Init();

    Sound::Init();

    nrst.Load();

    gset.Load();

    FPGA::Init();

    Display::Init();

    Panel::Init();

    VCP::Init();

    LAN::Init();

    FDrive::Init();

    FPGA::Start();
    
    Debug::MemoryTest();

    while(1)
    {
        DEBUG_POINT_0

        Timer::StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.

        DEBUG_POINT_0

        FPGA::meterStart.Reset();

        DEBUG_POINT_0

        FDrive::Update();

        DEBUG_POINT_0

        FPGA::Update();                      // Обновляем аппаратную часть.

        DEBUG_POINT_0

        Panel::Update();

        DEBUG_POINT_0

        Menu::UpdateInput();                 // Обновляем состояние меню

        DEBUG_POINT_0

        Display::Update();                   // Рисуем экран.

        DEBUG_POINT_0

        LAN::Update();

        DEBUG_POINT_0

        SCPI::Update();

        DEBUG_POINT_0
    }
}
