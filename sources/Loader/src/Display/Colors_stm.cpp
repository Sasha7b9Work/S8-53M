// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"


void Color::SetAsCurrent() const
{
    if (index != Count)
    {
        current = *this;
    }
}
