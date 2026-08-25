// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Utils/StringUtils.h"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <errno.h>


char SU::ToUpper(char symbol)
{
    uint8 s = (uint8)(symbol);

    if (s == 0x40)        // ¸
    {
        return '\xc5';
    }
    else if (s >= 0x60 && s <= 0x7a)
    {
        return (char)(s - 0x20);
    }
    else if (s >= 0xf0)
    {
        return (char)(s - 0x20);
    }

    return symbol;
}


char SU::ToLower(char symbol)
{
    uint8 s = (uint8)(symbol);

    if (s >= 0x41 && s <= 0x5a)
    {
        return (char)(s + 0x20);
    }
    else if (s >= 0xc0 && s < 0xE0)
    {
        return (char)(s + 0x20);
    }

    return symbol;
}


char *SU::DoubleToString(double value)
{
    static char buffer[30];

    std::sprintf(buffer, "%f", value);

    return buffer;
}


bool SU::StringToDouble(double *value, pchar in)
{
    *value = std::atof(in);

    return !(*value == 0 && errno == ERANGE);
}


bool SU::IsDigit(char symbol)
{
    return (symbol >= '0') && (symbol <= '9');
}


bool SU::EqualsStrings(pchar str1, pchar str2, uint size)
{
    for (uint i = 0; i < size; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }

    return true;
}


bool Word::GetWord(pchar string, const int numWord)
{
    string = ChooseSpaces(string);

    int currentWord = 0;

    while (true)
    {
        if (currentWord == numWord)
        {
            address = string;
            string = ChooseSymbols(string);
            numSymbols = (int8)(string - address);

            char *pointer = (char *)address;

            for (int i = 0; i < numSymbols; i++)
            {
                *pointer = (char)std::toupper((int8)*pointer);
                pointer++;
            }

            return true;
        }

        string = ChooseSymbols(string);

        if (string != nullptr)
        {
            currentWord++;
        }
        else
        {
            return false;
        }

        string = ChooseSpaces(string);
    }
}


bool Word::WordEqualZeroString(char *string)
{
    char *ch = string;
    char *w = (char *)(address);

    while (*ch != 0)
    {
        if (*ch++ != *w++)
        {
            return false;
        }
    }

    return (ch - string) == numSymbols;
}


#define  SYMBOL(x) (*(x))


pchar Word::ChooseSymbols(pchar string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return nullptr;
    }

    while (SYMBOL(string) != ' ' && SYMBOL(string) != 0x0d && SYMBOL(string + 1) != 0x0a)
    {
        string++;
    }

    return string;
}


pchar Word::ChooseSpaces(pchar string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return nullptr;
    }

    while (SYMBOL(string) == ' ')
    {
        string++;
    }

    return string;
}


#undef SYMBOL
