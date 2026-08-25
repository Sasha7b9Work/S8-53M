// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"
#include "FPGA/FPGA.h"
#include "Data/Storage.h"
#include "Display/Screen/Grid.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include "Utils/Text/Tables.h"
#include "Menu/Menu.h"


static bool FuncActiveMemoryNumPoinst()
{
    return SET_PEAKDET_IS_DISABLED;
}


void PageMemory::OnChanged_NumPoints(bool)
{
    bool running = FPGA::IsRunning();
    FPGA::Stop();

    Storage::Clear();

    if (ENUM_POINTS_FPGA::ToNumPoints() == 281)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else
    {
        if (SET_TPOS_IS_LEFT)
        {
            SHIFT_IN_MEMORY = 0;
        }
        else if (SET_TPOS_IS_CENTER)
        {
            SHIFT_IN_MEMORY = ENUM_POINTS_FPGA::ToNumPoints() / 2 - Grid::Width() / 2;
        }
        else if (SET_TPOS_IS_RIGHT)
        {
            SHIFT_IN_MEMORY = ENUM_POINTS_FPGA::ToNumPoints() - Grid::Width() - 2;
        }
    }

    TShift::Set(SET_TSHIFT);

    if (running)
    {
        FPGA::Start();
    }
}


const Choice mcMemoryNumPoints =
{
    TypeItem::Choice, PageMemory::self, FuncActiveMemoryNumPoinst,
    {
        "Точки", "Points"
        ,
        "Выбор количества отсчётов для сохраняемых сигналов. "
        "При увеличении количества отсчётов уменьшается количество сохранённых в памяти сигналов."
        ,
        "Choice of number of counting for the saved signals. "
        "At increase in number of counting the quantity of the signals kept in memory decreases."
    },
    {   
        {"281",     "281"},
        {"512",     "512"},
        {"1024",    "1024"}
    },
    (int8*)&SET_ENUM_POINTS, PageMemory::OnChanged_NumPoints
};


void PageMemory::SaveSignalToFlashDrive()
{
    if (FDrive::isConnected)
    {
        if (FILE_NAMING_MODE_IS_HAND)
        {
            Menu::AdditionPage::Set(PageMemory::SetName::self);
        }
        else
        {
            FDrive::needSave = true;
        }
    }
    else
    {
        PageMemory::SetName::exitTo = RETURN_TO_MAIN_MENU;
    }
}


void PageMemory::OnPressExtFileManager()
{
    if(FDrive::isConnected)
    {
        PageMemory::FileManager::self->OpenAndSetCurrent();
        Display::SetDrawMode(DrawMode::Hand, FM::Draw);
        FM::needRedraw = 1;
    }
}


static const arrayItems itemsMemory =
{
    (void *)&mcMemoryNumPoints,
    (void *)PageMemory::Latest::self,
    (void *)PageMemory::Internal::self,
    (void *)PageMemory::External::self
};


static const Page pMemory
(
    PageMain::self, 0,
    "ПАМЯТЬ", "MEMORY",
    "Работа с внешней и внутренней памятью.",
    "Working with external and internal memory.",
    NamePage::Memory, &itemsMemory
);


const Page *PageMemory::self = &pMemory;
