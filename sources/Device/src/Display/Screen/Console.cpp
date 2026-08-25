// 2022/03/23 12:04:28 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Screen/Console.h"
#include "Display/font/Font.h"
#include "Settings/Settings.h"
#include "Display/Screen/Grid.h"
#include <cstring>
#include <cstdio>


namespace Console
{
    const int MAX_NUM_STRINGS = 35;
    const int SIZE_BUFFER_FOR_STRINGS = 2000;
    char *strings[MAX_NUM_STRINGS] = {0};
    char  bufferForStrings[SIZE_BUFFER_FOR_STRINGS] = {0};

    int FirstEmptyString();

    int CalculateFreeSize();

    void DeleteFirstString();

    void AddString(pchar);
}


void Console::Draw()
{
    Font::Set(Font::GetSizeFontForConsole() == 5 ? TypeFont::_5 : TypeFont::_8);

    int height = Font::GetSize();
    int lastString = FirstEmptyString() - 1;
    int numStr = NUM_STRINGS;

    if (height == 8 && numStr > 22)
    {
        numStr = 22;
    }

    if (SHOW_STRING_NAVIGATION)
    {
        numStr -= ((height == 8) ? 1 : 2);
    }

    int firstString = lastString - numStr + 1;

    if (firstString < 0)
    {
        firstString = 0;
    }

    int y = GRID_TOP + 1;

    for (int numString = firstString; numString <= lastString; numString++)
    {
        int width = Font::GetLengthText(strings[numString]);

        Region(width, 5).Fill(Grid::Left() + 1, y, COLOR_BACK);

        String<1024>(strings[numString]).Draw(Grid::Left() + 2, y - 10, COLOR_FILL);

        y += height + 1;
    }

    Font::Set(TypeFont::_8);
}


int Console::FirstEmptyString()
{
    for (int i = 0; i < MAX_NUM_STRINGS; i++)
    {
        if (strings[i] == 0)
        {
            return i;
        }
    }

    return MAX_NUM_STRINGS;
}


int Console::CalculateFreeSize()
{
    int firstEmptyString = FirstEmptyString();

    if (firstEmptyString == 0)
    {
        return SIZE_BUFFER_FOR_STRINGS;
    }

    return (int)(SIZE_BUFFER_FOR_STRINGS - (strings[firstEmptyString - 1] - bufferForStrings)
        - std::strlen(strings[firstEmptyString - 1]) - 1);
}


void Console::DeleteFirstString()
{
    if (FirstEmptyString() < 2)
    {
        return;
    }

    int delta = (int)std::strlen(strings[0]) + 1;
    int numStrings = FirstEmptyString();

    for (int i = 1; i < numStrings; i++)
    {
        strings[i - 1] = strings[i] - delta;
    }

    for (int i = numStrings - 1; i < MAX_NUM_STRINGS; i++)
    {
        strings[i] = 0; //-V557
    }

    for (int i = 0; i < SIZE_BUFFER_FOR_STRINGS - delta; i++)
    {
        bufferForStrings[i] = bufferForStrings[i + delta];
    }
}


void Console::AddString(pchar string)
{
    static int num = 0;
    char buffer[100];
    std::sprintf(buffer, "%d\x11", num++);
    std::strcat(buffer, string);
    int size = (int)std::strlen(buffer) + 1;

    while (CalculateFreeSize() < size)
    {
        DeleteFirstString();
    }

    if (!strings[0])
    {
        strings[0] = bufferForStrings;
        std::strcpy(strings[0], buffer);
    }
    else
    {
        char *addressLastString = strings[FirstEmptyString() - 1];
        char *address = addressLastString + std::strlen(addressLastString) + 1;
        strings[FirstEmptyString()] = address; //-V557
        std::strcpy(address, buffer);
    }
}


void Console::Append(pchar string)
{
    if (FirstEmptyString() == MAX_NUM_STRINGS)
    {
        DeleteFirstString();
    }

    AddString(string);
}
