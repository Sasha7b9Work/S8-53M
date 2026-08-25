// 2022/2/11 19:33:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "Panel/Panel.h"
#include "Menu/Pages/Definition.h"
#include "Menu/Menu.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Warnings.h"
#include "Hardware/LAN/LAN.h"
#include <stm32f4xx.h>


int16 PageMemory::Latest::current = 0;
int8  PageMemory::Internal::currentSignal = 0;
bool  PageMemory::Internal::showAlways = false;
uint8 PageMemory::SetName::exitTo = RETURN_TO_MAIN_MENU;


void CalculateConditions(int16 pos0, int16 pos1, CursCntrl::E cursCntrl, bool *cond0, bool *cond1)
{
    bool zeroLessFirst = pos0 < pos1;
    *cond0 = (cursCntrl == CursCntrl::_1_2) || (cursCntrl == CursCntrl::_1 && zeroLessFirst) || (cursCntrl == CursCntrl::_2 && !zeroLessFirst); //-V537
    *cond1 = (cursCntrl == CursCntrl::_1_2) || (cursCntrl == CursCntrl::_1 && !zeroLessFirst) || (cursCntrl == CursCntrl::_2 && zeroLessFirst);
}


int CalculateYforCurs(int y, bool top)
{
    return top ? y + Item::HEIGHT / 2 + 4 : y + Item::HEIGHT - 2;
}


int CalculateXforCurs(int x, bool left)
{
    return left ? x + MI_WIDTH - 20 : x + MI_WIDTH - 5;
}


void CalculateXY(int *x0, int *x1, int *y0, int *y1)
{
    *x0 = CalculateXforCurs(*x0, true);
    *x1 = CalculateXforCurs(*x1, false);
    *y0 = CalculateYforCurs(*y0, true);
    *y1 = CalculateYforCurs(*y1, false);
}


void DrawMenuCursTime(int x, int y, bool left, bool right)
{
    x -= 58;
    y -= 16;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);

    for(int i = 0; i < (left ? 3 : 1); i++)
    {
        Painter::DrawVLine(false, x0 + i, y0, y1);
    }

    for(int i = 0; i < (right ? 3 : 1); i++)
    {
        Painter::DrawVLine(false, x1 - i, y0, y1);
    }
}


void PageCursors::DrawMenuCursVoltage(int x, int y, bool top, bool bottom)
{
    x -= 65;
    y -= 21;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (top ? 3 : 1); i++)
    {
        Painter::DrawHLine(true, y0 + i, x0, x1);
    }
    for(int i = 0; i < (bottom ? 3 : 1); i++)
    {
        Painter::DrawHLine(true, y1 - i, x0, x1);
    }
}


void DrawSB_Exit(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, '\x2e');
    Font::Set(TypeFont::_8);
}


#include "PageHelp.cpp"


static arrayItems itemsMainPage =
{
    (void *)PageDisplay::self,
    (void *)PageChannelA::self,
    (void *)PageChannelB::self,
    (void *)PageTrig::self,
    (void *)PageTime::self,
    (void *)PageCursors::self,
    (void *)PageMemory::self,
    (void *)PageMeasures::self,
    (void *)PageService::self,
    (void *)PageHelp::self,
    (void *)PageDebug::self
};

static const Page mainPage
(
    0, 0,
    "лемч", "MENU",
    "",
    "",
    NamePage::MainPage, &itemsMainPage
);


const Page *PageMain::self = &mainPage;


void PageMain::EnablePageDebug()
{
    Warning::ShowGood(Warning::MenuDebugEnabled);

    Menu::showDebugMenu = true;
}


bool IsMainPage(void *item)
{
    return item == &mainPage;
}


bool PageService::Math::Enabled()
{
    return !DISABLED_DRAW_MATH || ENABLED_FFT;
}


const Page *Page::FromName(NamePage::E name)
{
    if (name == NamePage::SB_MemExtSetName)
    {
        return PageMemory::SetName::self;
    }
    else if (name == NamePage::SB_MemInt)
    {
        return PageMemory::Internal::self;
    }

    return nullptr;
}

