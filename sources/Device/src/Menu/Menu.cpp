// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Menu/Menu.h" 
#include "Settings/Settings.h"
#include "Pages/Definition.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Display/Display.h"
#include "Data/Measures.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include "Utils/Text/Warnings.h"
#include <cstring>



namespace Menu
{
    bool showDebugMenu = false;
    bool needClosePageSB = true;


    // Включить/выключить светодиод ручки УСТАНОВКА, если необходимо.
    void SwitchSetLED();
    // Обработка короткого нажатия на элемент NamePage с адресом page.
    void ShortPress_Page(Item *page);
    // Обработка короткого нажатия на элемент Choice с адресом choice.
    void ShortPress_Choice(Item *choice);

    void ShortPress_Time(Item *time);
    // Обработка короткого нажатия на элемент Button с адресом button.
    void ShortPress_Button(Item *button);
    // Обработка короткого нажатия на элемент Governor с адресом governor.
    void ShortPress_Governor(Item *governor);
    // Обработка короткого нажатия на элемент GovernorColor с адресом governorColor.
    void ShortPress_GovernorColor(Item *governorColor);

    void ShortPress_IP(Item *item);

    void ShortPress_MAC(Item *item);

    void ShortPress_ChoiceReg(Item *choice);

    void ShortPress_SmallButton(Item *smallButton);
    // Обработка длинного нажатия на элемент меню item.
    void FuncOnLongPressItem(Item *item);

    void FuncOnLongPressItemTime(Item *item);
    // Обработка длинного нажатия на элемент Button с адресом button.
    void FuncOnLongPressItemButton(Item *button);

    // Возвращает true, если лампочка УСТАНОВКА должна гореть
    bool NeedForFireSetLED();

    // Обработка события таймера автоматического сокрытия меню.
    void OnTimerAutoHide();

    namespace AdditionPage
    {
        const Page *current = nullptr;

        void Set(const Page *page)
        {
            current = page;

            if (current->funcOnPress)
            {
                current->funcOnPress();
            }
        }

        void Remove()
        {
            current = nullptr;
        }
    }
}


void Menu::UpdateInput()
{
    SwitchSetLED();

    if (FM::needOpen)
    {
        FM::needOpen = false;
        PageMemory::FileManager::self->OpenAndSetCurrent();
        PageMemory::OnPressExtFileManager();
    }
};


void Hint::ProcessButton(Key::E button)
{
    if (button == Key::Menu)
    {
        Hint::string = LANG_RU ?
            "Кнопка МЕНЮ выполняет следующие функции:\n"
            "1. При закрытом меню нажатие либо нажатие с удержанием в течение 0.5с открывает меню.\n"
            "2. При открытом меню удержание кнопки в течение 0.5с закрывает меню.\n"
            "3. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Закрывать\" текущей становится страница предыдущего уровня меню. Если текущей является корневая страница, меню закрывается.\n"
            "4. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Переключать\" текущей становится страница текущего уровня меню. Если текущая страница последняя в текущем уровне, происходит переход на предыдущий уровень меню.\n"
            "5. Если меню находится в режиме малых кнопок, то нажатие закрывает страницу."
            :
        "MENU button performs the following functions:\n"
            "1. At the closed menu pressing or pressing with deduction during 0.5s opens the menu.\n"
            "2. At the open menu deduction of the button during 0.5s closes the menu.\n"
#ifndef WIN32
#pragma clang diagnostic push
#pragma diag_suppress 192
#endif
            "3. At control \"SERVICE\x99Mode btn MENU\x99\x43lose\" current becomes the page of the previous level of the menu. If the root page is current, the menu is closed.\n"
#ifndef WIN32
#pragma clang diagnostic pop
#endif
            "4. At control \"SERVICE\x99Mode btn MENU\x99Toggle\" current becomes the page of the current level of the menu. If the current page the last in the current level, happens transition to the previous level of the menu.\n"
            "5. If the menu is in the mode of small buttons, pressing closes the page.";

    } else if (button == Key::Cursors)
    {
        Hint::string = LANG_RU ?
            "Кнопка КУРСОРЫ открывает меню курсорных измерений."
            :
            "КУРСОРЫ button to open the menu cursor measurements.";
    }
    else if (button == Key::Display)
    {
        Hint::string = LANG_RU ?
            "Кнопка ДИСПЛЕЙ открывает меню настроек дисплея."
            :
            "DISPLAY button opens the display settings menu.";
    }
    else if (button == Key::Memory)
    {
        Hint::string = LANG_RU ?
            "1. При настройке \"ПАМЯТЬ\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Меню\" открывает меню работы с памятью.\n"
            "2. При настройке \"ПАМЯТь\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Сохранение\" сохраняет сигнал на флеш-диск."
            :
            "1. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Menu\" opens a menu of memory\n"
            "2. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Save\" saves the signal to the flash drive";
    }
    else if (button == Key::Measures)
    {
        Hint::string = LANG_RU ?
            "Кнопка ИЗМЕР открывает меню автоматических измерений."
            :
            "ИЗМЕР button opens a menu of automatic measurements.";
    }
    else if (button == Key::Help)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка ПОМОЩЬ открывает меню помощи.\n"
            "2. Нажатие и удержание кнопки ПОМОЩЬ в течение 0.5с включает и отключает режим вывода подсказок."
            :
            "1. ПОМОЩЬ button opens the help menu.\n"
            "2. Pressing and holding the ПОМОЩЬ for 0.5s enables and disables output mode hints.";
    }
    else if (button == Key::Service)
    {
        Hint::string = LANG_RU ?
            "Кнопка СЕРВИС открывает меню сервисных возможностей."
            :
            "СЕРВИС button opens a menu of service options.";
    }
    else if (button == Key::Start)
    {
        Hint::string = LANG_RU ?
            "Кнопка ПУСК/СTOП запускает и останавливает процесс сбора информации."
            :
            "ПУСК/СTOП button starts and stops the process of gathering information.";
    }
    else if (button == Key::ChannelA)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка КАНАЛ1 открывает меню настроек канала 1.\n"
            "2. Нажатие и удержание кнопки КАНАЛ1 в течение 0.5с устанавливает смещение канала 1 по вертикали 0В."
            :
            "1. КАНАЛ1 button opens the settings menu of the channel 1.\n"
            "2. Pressing and holding the button КАНАЛ1 for 0.5c for the offset of the vertical channel 1 0V.";
    }
    else if (button == Key::ChannelB)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка КАНАЛ2 открывает меню настроек канала 2.\n"
            "2. Нажатие и удержание кнопки КАНАЛ2 в течение 0.5с устанавливает смещение канала 2 по вертикали 0В."
            :
            "1. КАНАЛ2 button opens the settings menu of the channel 2.\n"
            "2. Pressing and holding the button КАНАЛ2 for 0.5c for the offset of the vertical channel 2 0V.";
    }
    else if (button == Key::Time)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка РАЗВ открывает меню настроек развертки.\n"
            "2. Нажатие и удержание кнопки РАЗВ в течение 0.5с устанавливает смещение по горизонтали 0с."
            :
            "1. РАЗВ button open the settings menu sweep.\n"
            "2. Pressing and holding the button РАЗВ for 0.5s Is the offset horizontal 0s.";
    }
    else if (button == Key::Trig)
    {
    Hint::string = LANG_RU ?
            "1. Кнопка СИНХР открывает меню настроек синхронизации.\n"
            "2. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\" производит автоматическую настройку уровня синхронизации.\n"
            "3. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Сброс уровня\" устанавливает уровень синхронизации 0В."
            :
            "1. СИНХР button opens a menu settings synchronization.\n"
#ifndef WIN32
#pragma clang diagnostic push
#pragma diag_suppress 192
#endif
            "2. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99\x41utolevel\" automatically adjust the trigger level.\n"
#ifndef WIN32
#pragma clang diagnostic pop
#endif
            "3. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99SReset trig level\" sets the trigger level 0V.";
    }
}


void Menu::SetAutoHide(bool)
{
    if(!IsShown())
    {
        return;
    }
    if(SettingsDisplay::TimeMenuAutoHide() == 0)
    {
        Timer::Disable(TypeTimer::MenuAutoHide);
    }
    else
    {
        Timer::Enable(TypeTimer::MenuAutoHide, SettingsDisplay::TimeMenuAutoHide(), OnTimerAutoHide);
    }
}


String<> Menu::StringNavigation()
{
    if(SHOW_STRING_NAVIGATION && IsShown())
    {
        String<> result;

        pchar titles[10] = {0};
        int numTitle = 0;
        Item *item = Item::Opened();

        if(IsMainPage(item))
        {
            return String<>("");
        }

        while(!IsMainPage(item))
        {
            titles[numTitle++] = item->Title().c_str();
            item = item->Keeper();
        }

        for(int i = 9; i >= 0; i--)
        {
            if(titles[i])
            {
                result.Append(titles[i]);

                if(i != 0)
                {
                    result.Append(" - ");
                }
            }
        }

        return result;
    }

    return String<>("");
}


void Menu::OnTimerAutoHide()
{
    Show(false);
    Timer::Disable(TypeTimer::MenuAutoHide);
}


void Menu::ShortPress_Page(Item *item)
{
    Page *page = (Page *)item;

    if (page->funcOnPress)
    {
        page->funcOnPress();
    }

    if (!page->IsActive())
    {
        return;
    }

    page->SetCurrent(true);

    ((Page*)page)->Open(!page->IsOpened());
}


void Menu::FuncOnLongPressItem(Item *item)
{
    if (Item::Current() != item)
    {
        item->SetCurrent(true);
    }

    if (item->IsOpened())
    {
        item->Open(false);
    }
    else
    {
        item->Open(true);

        if (item->IsPage())
        {
            Page *page = (Page *)item;

            if (page->funcOnPress)
            {
                page->funcOnPress();
            }
        }
    }
}


void Menu::ShortPress_Choice(Item *choice)
{
    if (!choice->IsActive())
    {
        ((Choice *)choice)->FuncOnChanged(false);
    }
    else if (!choice->IsOpened())
    {
        choice->SetCurrent(Item::Current() != choice);
        ((Choice *)choice)->StartChange(1);
    }
    else
    {
        ((Choice *)choice)->ChangeValue(-1);
    }
}


void Menu::ShortPress_ChoiceReg(Item *choice)
{
    if (!choice->IsActive())
    {
        ((Choice *)choice)->FuncOnChanged(false);
    }
    else if (!choice->IsOpened())
    {
        choice->SetCurrent(Item::Current() != choice);
    }
    else
    {
        ((Choice *)choice)->ChangeValue(-1);
    }
}


void Menu::FuncOnLongPressItemButton(Item *button)
{
    ShortPress_Button(button);
}


void Menu::ShortPress_Button(Item *button)
{
    if(!button->IsActive())
    {
        return;
    }
    
    button->SetCurrent(true);
    ((Button*)button)->funcOnPress();
}


void Menu::FuncOnLongPressItemTime(Item *time)
{
    if (Item::Current() != time)
    {
        time->SetCurrent(true);
    }

    if(time->IsOpened() && *((TimeControl *)time)->curField == iSET)
    {
        ((TimeControl *)time)->SetNewTime();
    }

    time->Open(!time->IsOpened());

    ((TimeControl *)time)->SetOpened();
}


void Menu::ShortPress_Time(Item *time)
{
    if(!time->IsOpened())
    {
        time->SetCurrent(true);
        ((TimeControl *)time)->SetOpened();
        time->Open(true);
    }
    else
    {
        ((TimeControl *)time)->SelectNextPosition();
    }
}


void Menu::ShortPress_Governor(Item *governor)
{
    Governor *gov = (Governor*)governor;

    if(!governor->IsActive())
    {
        return;
    }

    if(Item::Opened() == gov)
    {
        gov->NextPosition();
    }
    else
    {
        gov->SetCurrent(Item::Current() != gov);
    }
}


void Menu::ShortPress_IP(Item *item)
{
    if (Item::Opened() == item)
    {
        ((IPaddress*)item)->NextPosition();
    }
}


void Menu::ShortPress_MAC(Item *item)
{
    if (Item::Opened() == item)
    {
        Math::CircleIncrease<int8>(&MACaddress::cur_digit, 0, 5);
    }
}


void Menu::ShortPress_GovernorColor(Item *governorColor)
{
    if(!governorColor->IsActive())
    {
        return;
    }

    GovernorColor *governor = (GovernorColor*)governorColor;

    if(Item::Opened() == (Item *)governor)
    {
        Math::CircleIncrease<int8>(&(governor->colorType->currentField), 0, 3);
    }
    else
    {
        FuncOnLongPressItem((Item *)governorColor);
    }
}


void Menu::ShortPress_SmallButton(Item *smallButton)
{
    SmallButton *sb = (SmallButton *)smallButton;

    if (sb)
    {
        pFuncVV func = sb->funcOnPress;

        if (func)
        {
            func();
            Item::now_pressed = (Item *)smallButton;
        }
    }
}


void Menu::ExecuteFuncForShortPressOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV shortFunction[TypeItem::Count] =
    {
        nullptr,                            // TypeItem::None
        &Menu::ShortPress_Choice,           // TypeItem::Choice
        &Menu::ShortPress_Button,           // TypeItem::Button
        &Menu::ShortPress_Page,             // TypeItem::Page
        &Menu::ShortPress_Governor,         // TypeItem::Governor
        &Menu::ShortPress_Time,             // TypeItem::Time
        &Menu::ShortPress_IP,               // TypeItem::IP
        nullptr,                            // Item_SwitchButton
        &Menu::ShortPress_GovernorColor,    // TypeItem::GovernorColor
        nullptr,                            // Item_Formula
        &Menu::ShortPress_MAC,              // TypeItem::MAC
        &Menu::ShortPress_ChoiceReg,        // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };
 
    pFuncMenuVpV func = shortFunction[item->GetType()];

    if (func)
    {
        (func)(item);
    }
}


void Menu::ExecuteFuncForLongPressureOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV longFunction[TypeItem::Count] =
    {
        0,                                  // TypeItem::None
        &Menu::FuncOnLongPressItem,         // TypeItem::Choice
        &Menu::FuncOnLongPressItemButton,   // TypeItem::Button
        &Menu::FuncOnLongPressItem,         // TypeItem::Page
        &Menu::FuncOnLongPressItem,         // TypeItem::Governor
        &Menu::FuncOnLongPressItemTime,     // TypeItem::Time
        &Menu::FuncOnLongPressItem,         // TypeItem::IP
        0,                                  // Item_SwitchButton
        &Menu::FuncOnLongPressItem,         // TypeItem::GovernorColor
        0,                                  // Item_Formula
        &Menu::FuncOnLongPressItem,         // TypeItem::MAC
        &Menu::FuncOnLongPressItem,         // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };

    if (item->IsActive())
    {
        pFuncMenuVpV func = longFunction[item->GetType()];
        if (func)
        {
            (func)(item);
        }
    }
}


void TimeControl::Open()
{
    Warning::ShowGood(Warning::TimeNotSet);
    Panel::ProcessEvent(KeyboardEvent(Key::Service, Action::Up));
    Display::Update();

    for (int i = 0; i < 2; i++)
    {
        Panel::ProcessEvent(KeyboardEvent(Key::Setting, Action::Right));
        Display::Update();
    }

    Panel::ProcessEvent(KeyboardEvent(Key::F4, Action::Up));
    Display::Update();
}


bool Menu::NeedForFireSetLED()
{
    if (!IsShown())
    {
        return false;
    }
    NamePage::E name = Page::NameOpened();

    if (name == NamePage::SB_MeasTuneMeas && (MEAS_NUM == MeasuresNumber::_1) && !PageMeasures::choiceMeasuresIsActive)
    {
        return false;
    }

    if (
        name == NamePage::SB_MathCursorsFFT                             ||
        name == NamePage::SB_MeasTuneMeas                               ||
        name == NamePage::SB_MemLatest                                  || 
        name == NamePage::SB_MemInt                                     ||
        ((name == NamePage::SB_MathFunction) && !DISABLED_DRAW_MATH)    ||
        ((name == NamePage::SB_Curs) && PageCursors::NecessaryDrawCursors())
        )
    {
        return true;
    }
    
    TypeItem::E typeCurrentItem = Item::Current()->GetType();
    if (typeCurrentItem == TypeItem::Governor    ||
        typeCurrentItem == TypeItem::ChoiceReg)
    {
        return true;
    }

    TypeItem::E typeOpenedItem = Item::TypeOpened();

    if (typeOpenedItem == TypeItem::Choice       ||
        (typeOpenedItem == TypeItem::Page && ((Page *)Item::Opened())->NumSubPages() > 1)
        )
    {
        return true;
    }
    
    return false;
}


void Menu::SwitchSetLED()
{
    static bool first = true;
    static bool prevState = false;  // true - горит, false - не горит

    bool state = NeedForFireSetLED();

    if (first)
    {
        first = false;
        LED::RegSet.Switch(state);
        prevState = state;
    }
    else if (prevState != state)
    {
        LED::RegSet.Switch(state);
        prevState = state;
    }
}
