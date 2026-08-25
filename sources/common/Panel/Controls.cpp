// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "common/Panel/Controls.h"


bool KeyboardEvent::IsFunctional() const
{
    return (key.value >= Key::F1) && (key.value <= Key::F5);
}


bool KeyboardEvent::IsRelease() const
{
    return (action.value == Action::Up) || (action.value == Action::Long);
}


pchar Key::Name() const
{
    static pchar names[Count] =
    {
        "None",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "ÊÀÍÀË 1",
        "ÊÀÍÀË 2",
        "ÐÀÇÂ",
        "ÑÈÍÕÐ",
        "ÊÓÐÑÎÐÛ",
        "ÈÇÌÅÐ",
        "ÄÈÑÏËÅÉ",
        "ÏÎÌÎÙÜ",
        "ÏÓÑÊ/ÑÒÎÏ",
        "ÏÀÌßÒÜ",
        "ÑÅÐÂÈÑ",
        "ÌÅÍÞ",
        "ÏÈÒÀÍÈÅ",

        "ÂÎËÜÒ/ÄÅË 1",
        "ÂÎËÜÒ/ÄÅË 2",
        "RSHIFT 1",
        "RSHIFT 2",
        "ÂÐÅÌß/ÄÅË",
        "TSHIFT",
        "ÓÐÎÂÍÜ",
        "ÓÑÒÀÍÎÂÊÀ"
    };

    return names[value];
}


Key::E &operator++(Key::E &right)
{
    right = (Key::E)((int)right + 1);
    return right;
}


bool Key::IsGovernor() const
{
    return (value >= Key::RangeA) && (value <= Key::Setting);
}


bool Key::Is(Key::E key)
{
    return (key < Key::RangeA) && (key != Key::None);
}


bool Key::IsFunctional() const
{
    return (value >= Key::F1) && (value <= Key::F5);
}


bool Key::IsFunctional(Key::E val)
{
    return Key(val).IsFunctional();
}


pchar Action::Name() const
{
    static const pchar names[Count] =
    {
        "Down",
        "Up",
        "Long",
        "Rotate-Left",
        "Rotate-Right"
    };

    return names[value];
}


KeyboardEvent::KeyboardEvent(const uint8 *buffer)
{
    key = (Key::E) *(buffer + 1);
    action = (Action::E) *(buffer + 2);
}


void KeyboardEvent::Log() const
{
    if (key.value != Key::None)
    {
        LOG_WRITE("%s : %s", key.Name(), action.Name());
    }
}
