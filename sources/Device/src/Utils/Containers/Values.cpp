// 2021/03/24 10:56:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/Settings.h"
#include "Utils/Containers/Values.h"
#include "Utils/Text/String.h"
#include "Utils/Text/Text.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>


String<> Float::ToText(bool always_sign, int num_digits)
{
    if (!IsValid())
    {
        return EmptyString();
    }

    String<> result;

    if (!always_sign)
    {
        if (value < 0.0f)
        {
            result.Append('-');
        }
    }
    else
    {
        result.Append(value < 0.0f ? '-' : '+');
    }


    char buffer[20];
    char format[] = "%4.2f\0\0";

    format[1] = (char)num_digits + 0x30;

    int num_digits_in_int = NumDigitsInIntPart(value);

    format[3] = (char)((num_digits - num_digits_in_int) + 0x30);
    if (num_digits == num_digits_in_int)
    {
        format[5] = '.';
    }

    std::sprintf(buffer, format, std::fabsf(value));

    result.Append(buffer);

    bool sign_exist = always_sign || value < 0.0f;

    while ((int)(result.Size()) < num_digits + (sign_exist ? 2 : 1))
    {
        result.Append("0");
    }

    RemoveDigits(result, num_digits);

    ReplacePoints(result);

    return result;
}


void Float::RemoveDigits(String<> &string, int max_digits)
{
    int size = string.Size();

    int num_digits = 0;

    for (int i = 0; i < size; i++)
    {
        if (num_digits == max_digits)
        {
            string[i] = '\0';
        }

        if (SU::IsDigit(string[i]))
        {
            num_digits++;
        }
    }
}


void Float::ReplacePoints(const String<> &string)
{
    char *txt = string.c_str();

    while (txt && *txt)
    {
        if (*txt == ',')
        {
            *txt = '.';
        }

        txt++;
    }
}


String<> Voltage::ToText(bool always_sign)
{
    if (!IsValid())
    {
        return EmptyString();
    }

    pchar suffix = nullptr;

    float voltage = *this;

    if (std::fabsf(voltage) + 0.5e-4F < 1e-3F)
    {
        suffix = LANG_RU ? "\x10ìêÂ" : "\x10uV";
        voltage *= 1e6F;
    }
    else if (std::fabsf(voltage) + 0.5e-4F < 1.0F)
    {
        suffix = LANG_RU ? "\x10ìÂ" : "\x10mV";
        voltage *= 1e3F;
    }
    else if (std::fabsf(voltage) + 0.5e-4F < 1000.0F)
    {
        suffix = LANG_RU ? "\x10Â" : "\x10V";
    }
    else
    {
        suffix = LANG_RU ? "\x10êÂ" : "\x10kV";
        voltage *= 1e-3F;
    }

    return String<>("%s%s", Float(voltage).ToText(always_sign, 4).c_str(), suffix);
}


String<> Time::ToText(bool always_sign)
{
    if (!IsValid())
    {
        return EmptyString();
    }

    pchar suffix = nullptr;

    float time = *this;

    if (std::fabsf(time) + 0.5e-10F < 1e-6F)
    {
        suffix = LANG_RU ? "íñ" : "ns";
        time *= 1e9F;
    }
    else if (std::fabsf(time) + 0.5e-7F < 1e-3F)
    {
        suffix = LANG_RU ? "ìêñ" : "us";
        time *= 1e6F;
    }
    else if (std::fabsf(time) + 0.5e-3F < 1.0F)
    {
        suffix = LANG_RU ? "ìñ" : "ms";
        time *= 1e3F;
    }
    else
    {
        suffix = LANG_RU ? "ñ" : "s";
    }

    return String<>("%s%s", Float(time).ToText(always_sign, 4).c_str(), suffix);
}


String<> Frequency::ToText()
{
    if (!IsValid())
    {
        return EmptyString();
    }

    pchar suffix = 0;

    float frequency = *this;

    if (frequency >= 1e6F)
    {
        suffix = LANG_RU ? "ÌÃö" : "MHz";
        frequency /= 1e6F;
    }
    else if (frequency >= 1e3F)
    {
        suffix = LANG_RU ? "êÃö" : "kHz";
        frequency /= 1e3F;
    }
    else
    {
        suffix = LANG_RU ? "Ãö" : "Hz";
    }

    return String<>("%s%s", Float(frequency).ToText(false, 4).c_str(), suffix);
}


String<> Phase::ToText()
{
    return String<>("%s\xa8", Float(*this).ToText(false, 4).c_str());
}


String<> Int::ToText(bool always_sign, int num_min_fields)
{
    String<> result;

    if (always_sign && value >= 0)
    {
        result.Append('+');
    }

    char buffer[20];

    char format[20] = "%";
    std::sprintf(&(format[1]), "0%d", num_min_fields);
    std::strcat(format, "d");

    std::sprintf(buffer, format, value);

    result.Append(buffer);

    return result;
}


Int::Int(pchar buffer) : ValueStruct(0)
{
    valid = false;

    Word param;

    if (param.GetWord(buffer, 0))
    {
        char *n = reinterpret_cast<char *>(std::malloc((uint)(param.numSymbols + 1)));
        if (n)
        {
            std::memcpy(n, param.address, (uint)(param.numSymbols));
            n[param.numSymbols] = '\0';
            valid = String<>(n).ToInt(&value);
            std::free(n);
        }
    }
}


String<> Uint8::ToStringBin()
{
    String<> result("00000000");

    char *buffer = result.c_str();

    for (int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(value, bit) ? '1' : '0';
    }

    return result;
}


String<> Uint8::ToStringHex()
{
    String<> result("00");

    std::sprintf(&result[value < 16 ? 1 : 0], "%x", value);

    return result;
}


String<> Uint16::ToStringHex()
{
    String<> result("0000");

    int index = 0;

    if (value < 0x10)        { index = 3; }
    else if (value < 0x100)  { index = 2; }
    else if (value < 0x1000) { index = 1; }

    std::sprintf(&result[index], "%x", value);

    return result;
}


String<> Uint32::ToStringHex()
{
    String<> result("00000000");

    std::sprintf(&result[0], "%08x", value);

    return result;
}


String<> Uint16::ToStringBin()
{
    String<> result("000000000000000000");

    std::strcpy(&result[0], Uint8((uint8)(value >> 8)).ToStringBin().c_str());

    result.c_str()[8] = ' ';

    std::strcpy(&result[9], Uint8((uint8)value).ToStringBin().c_str());

    return result;
}


String<> Decibel::ToText(int num_digits)
{
    return String<>("%säÁ", Float::ToText(false, num_digits).c_str());
}


int Float::NumDigitsInIntPart(float val)
{
    float fabsValue = std::fabsf(val);

    int numDigitsInInt = 0;

    if (fabsValue >= 10000)
    {
        numDigitsInInt = 5;
    }
    else if (fabsValue >= 1000)
    {
        numDigitsInInt = 4;
    }
    else if (fabsValue >= 100)
    {
        numDigitsInInt = 3;
    }
    else if (fabsValue >= 10)
    {
        numDigitsInInt = 2;
    }
    else
    {
        numDigitsInInt = 1;
    }

    return numDigitsInInt;
}


float Float::Round() const
{
    return (value + 0.5F);
}
