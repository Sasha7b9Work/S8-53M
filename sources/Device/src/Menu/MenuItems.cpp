// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Menu.h"
#include "MenuItems.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"


int Regulator::angle = 0;

int8 MACaddress::cur_digit = 0;
int8 IPaddress::cur_digit = 0;
int8 Governor::cur_digit = 0;

Item *Item::now_pressed = nullptr;
Item *Hint::item = nullptr;
pchar Hint::string = nullptr;
bool  Hint::show = false;


bool Governor::inMoveIncrease = false;
bool Governor::inMoveDecrease = false;
void *Governor::address = nullptr;
uint Governor::timeStart = 0;


// »спользуетс€ дл€ анимации изменени€ значени€ Choice
struct TimeStructChoice
{
    Choice* choice;                 // јдрес Choice, который находитс€ в данный момент в движении. ≈сли 0 - все статичны.
    uint    timeStartMS;            // ¬рем€ начала анимации choice.
    uint    inMoveIncrease : 1;
    uint    inMoveDecrease : 1;
};

static TimeStructChoice tsChoice;



Item::Item(const ItemStruct *str) : type(str->type), keeper(str->keeper), funcOfActive(str->funcOfActive)
{
    titleHint[0] = str->titleHint[0];
    titleHint[1] = str->titleHint[1];
    titleHint[2] = str->titleHint[2];
    titleHint[3] = str->titleHint[3];
}


Item::Item(TypeItem::E type_, const Page* keeper_, pFuncBV funcOfActive_, pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN) :
    type(type_), keeper(keeper_), funcOfActive(funcOfActive_)
{
    titleHint[0] = titleRU;
    titleHint[1] = titleEN;
    titleHint[2] = hintRU;
    titleHint[3] = hintEN;
}


Page::Page(const Page *keeper_, pFuncBV funcOfActive_, pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, 
           NamePage::E name_, const arrayItems *items_, pFuncVV funcOnPress_, pFuncVV funcOnDraw_, pFuncVI funcRegSetSB_) :
    Item(TypeItem::Page, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    name(name_), items(items_), funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), funcRegSetSB(funcRegSetSB_)
{
}


Button::Button(const Page *keeper_, pFuncBV funcOfActive_,
       pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, pFuncVV funcOnPress_) :
    Item(TypeItem::Button, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_)
{
};


SmallButton::SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
            pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
            pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_) :
    Item(TypeItem::SmallButton, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), hintUGO(hintUGO_)
{
}


Governor::Governor(const Page *keeper_, pFuncBV funcOfActive_,
         pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
         int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_, pFuncVV funcBeforeDraw_) :
    Item(TypeItem::Governor, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    cell(cell_), minValue(minValue_), maxValue(maxValue_), funcOfChanged(funcOfChanged_), funcBeforeDraw(funcBeforeDraw_)
{
}


IPaddress::IPaddress(const IPaddressStruct *str) :
    Item(&str->str), ip0(str->ip0), ip1(str->ip1), ip2(str->ip2), ip3(str->ip3), funcOfChanged(str->funcOfChanged), port(str->port)
{

}


String<> Choice::NameSubItem(int i)
{
    return String<>(names[i][LANG]);
}


String<> Choice::NameCurrentSubItem()
{
    return (cell == 0) ? String<>("") : String<>(names[*cell][LANG]);
}


pchar Choice::NameNextSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell + 1;
    if (index == NumSubItems())
    {
        index = 0;
    }
    return names[index][LANG];
}


pchar Choice::NamePrevSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell - 1;
    if (index < 0)
    {
        index = NumSubItems() - 1;
    }
    return names[index][LANG];
}


int Choice::NumSubItems()
{
    int i = 0;
    for (; i < MAX_NUM_SUBITEMS_IN_CHOICE; i++)
    {
        if (names[i][LANG] == 0)
        {
            return i;
        }
    }
    return i;
}


void Choice::StartChange(int delta)
{
    if (tsChoice.choice != 0)
    {
        return;
    }

    Sound::GovernorChangedValue();

    if (!((Item *)this)->IsActive())
    {
        FuncOnChanged(false);
    }
    else
    {
        tsChoice.choice = this;
        tsChoice.timeStartMS = TIME_MS;

        if (delta > 0)
        {
            tsChoice.inMoveIncrease = 1;
        }
        else if (delta < 0)
        {
            tsChoice.inMoveDecrease = 1;
        }
    }
}


void Choice::FuncOnChanged(bool active)
{
    if (funcOnChanged)
    {
        funcOnChanged(active);
    }
}


void Choice::FuncForDraw(int x, int y)
{
    if (funcForDraw)
    {
        funcForDraw(x, y);
    }
}


float Choice::Step()
{
    static const float speed = 0.1f;
    static const int numLines = 12;

    if (tsChoice.choice == this)
    {
        float delta = speed * (TIME_MS - tsChoice.timeStartMS);

        if (delta == 0.0f)
        {
            delta = 0.001f; // “аймер в несколько первых кадров может показать, что прошло 0 мс, но мы возвращаем большее число, потому что ноль будет говорить о том, что движени€ нет
        }

        if (tsChoice.inMoveIncrease == 1)
        {
            if (delta <= numLines)
            {
                return delta;
            }

            Math::CircleIncrease<int8>(cell, 0, NumSubItems() - 1);
            Regulator::RotateRight();
        }
        else if (tsChoice.inMoveDecrease == 1)
        {
            delta = -delta;

            if (delta >= -numLines)
            {
                return delta;
            }

            Math::CircleDecrease<int8>(cell, 0, NumSubItems() - 1);
            Regulator::RotateLeft();
        }

        tsChoice.choice = 0;
        FuncOnChanged(((Item *)this)->IsActive());
        Flags::needFinishDraw = true;
        tsChoice.inMoveDecrease = tsChoice.inMoveIncrease = 0;

        return 0.0f;
    }

    return 0.0f;
}


void Choice::ChangeValue(int delta)
{
    if (delta < 0)
    {
        Math::CircleIncrease<int8>(cell, 0, NumSubItems() - 1);
        Regulator::RotateLeft();
    }
    else
    {
        Math::CircleDecrease<int8>(cell, 0, NumSubItems() - 1);
        Regulator::RotateRight();
    }

    FuncOnChanged(((Item *)this)->IsActive());
    Sound::GovernorChangedValue();

    Flags::needFinishDraw = true;
}


void Hint::SetItem(Item *_item)
{
    string = nullptr;
    item = _item;
}
