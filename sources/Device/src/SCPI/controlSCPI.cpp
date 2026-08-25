// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Panel/Panel.h"
#include "SCPI/SCPI.h"


pchar SCPI::CONTROL::KEY(pchar buffer)
{
    static const MapElement keys[] =
    {
        {":1",         1},
        {":2",         2},
        {":3",         3},
        {":4",         4},
        {":5",         5},
        {":CHANNEL1",  6},
        {":CHANNEL2",  7},
        {":TIME",      8},
        {":TRIG",      9},
        {":CURSORS",  10},
        {":MEASURES", 11},
        {":DISPLAY",  12},
        {":HELP",     13},
        {":START",    14},
        {":MEMORY",   15},
        {":SERVICE",  16},
        {":MENU",     17},
        {":POWER",    18},
        {0}
    };

    Word button;        // Здесь хранится принятое название кнопки
    Word action;        // Здесь хранится принятое действие кнопки

    if (button.GetWord(buffer, 0) && action.GetWord(buffer, 1))
    {
        int numKey = 0;
        while (keys[numKey].key != 0)
        {
            if (button.WordEqualZeroString(keys[numKey].key))
            {
                uint8 data[3] = { 255, keys[numKey].value, 0 };

                if (action.WordEqualZeroString("PRESS"))
                {
                    data[2] = Action::Down;
                    Panel::Callback::OnReceiveSPI5(data, 3);

                    data[2] = Action::Up;
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return nullptr;
                }
                else if (action.WordEqualZeroString("DOWN"))
                {
                    data[2] = Action::Down;
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return nullptr;
                }
                else if (action.WordEqualZeroString("UP"))
                {
                    data[2] = Action::Up;
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return nullptr;
                }
                else if (action.WordEqualZeroString("LONG"))
                {
                    data[2] = Action::Long;
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return nullptr;
                }

                return buffer;
            }

            numKey++;
        }
    }

    return buffer;
}


pchar SCPI::CONTROL::GOVERNOR(pchar buffer)
{
    static const MapElement governors[] =
    {
        {":RANGE1",  19},
        {":RANGE2",  20},
        {":RSHIFT1", 21},
        {":RSHIFT2", 22},
        {":TBASE",   23},
        {":TSHIFT",  24},
        {":TRIGLEV", 25},
        {":SETTING", 26},
        {0}
    };

    Word governor;
    Word action;

    if (governor.GetWord(buffer, 0) && action.GetWord(buffer, 1))
    {
        int numGov = 0;
        while (governors[numGov].key != 0)
        {
            if (governor.WordEqualZeroString(governors[numGov].key))
            {
                uint8 data[3] = {255, governors[numGov].value, 0};

                if (action.WordEqualZeroString("LEFT"))
                {
                    data[2] = Action::Left;
                }
                else if (action.WordEqualZeroString("RIGHT"))
                {
                    data[2] = Action::Right;
                }

                if (data[2])
                {
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return nullptr;
                }

                return buffer;
            }

            numGov++;
        }
    }

    return buffer;
}
