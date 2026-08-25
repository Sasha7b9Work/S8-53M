// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Tables.h"
#include "Display/font/Font.h"


namespace Tables
{
    // Структура для описания диапазона масштаба по времени.
    struct TBaseStruct
    {
        pchar  name[2];    // Название диапазона в текстовом виде, пригодном для вывода на экран.
    };


    // Массив структур описаний масштабов по времени.
    static const TBaseStruct tBases[TBase::Count] =
    {
        {"2\x10нс",     "2\x10ns"},
        {"5\x10нс",     "5\x10ns"},
        {"10\x10нс",    "10\x10ns"},
        {"20\x10нс",    "20\x10ns"},
        {"50\x10нс",    "50\x10ns"},
        {"0.1\x10мкс",  "0.1\x10us"},
        {"0.2\x10мкс",  "0.2\x10us"},
        {"0.5\x10мкс",  "0.5\x10us"},
        {"1\x10мкс",    "1\x10us"},
        {"2\x10мкс",    "2\x10us"},
        {"5\x10мкс",    "5\x10us"},
        {"10\x10мкс",   "10\x10us"},
        {"20\x10мкс",   "20\x10us"},
        {"50\x10мкс",   "50\x10us"},
        {"0.1\x10мс",   "0.1\x10ms"},
        {"0.2\x10мс",   "0.2\x10ms"},
        {"0.5\x10мс",   "0.5\x10ms"},
        {"1\x10мс",     "1\x10ms"},
        {"2\x10мс",     "2\x10ms"},
        {"5\x10мс",     "5\x10ms"},
        {"10\x10мс",    "10\x10ms"},
        {"20\x10мс",    "20\x10ms"},
        {"50\x10мс",    "50\x10ms"},
        {"0.1\x10с",    "0.1\x10s"},
        {"0.2\x10с",    "0.2\x10s"},
        {"0.5\x10с",    "0.5\x10s"},
        {"1\x10с",      "1\x10s"},
        {"2\x10с",      "2\x10s"},
        {"5\x10с",      "5\x10s"},
        {"10\x10с",     "10\x10s"}
    };

    // Таблица символов алфавита и спецсимволов для ввода текста
    pchar  symbolsAlphaBet[0x48] =
    {
        /* 0x00 */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0x19 */
        /* 0x1a */ " ", "_", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /* 0x26 */
        /* 0x27 */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0x40 */
        //         0x41  0x42  0x43  0x44  0x45  0x46   0x47   - это порядковый номер элемента в этом массиве
        /* 0x41 */ "%%y", "%%m", "%%d", "%%H", "%%M", "%%S", "%%N" /* 0x47 */
        //         0x01  0x02  0x03  0x04  0x05  0x06   0x07   - под этими значениями элементы хранятся в gset.memory.fileNameMask
    };
}


pchar TBase::ToString(TBase::E tbase)
{
    return Tables::tBases[tbase].name[LANG];
}


pchar TBase::ToStringEN(TBase::E tbase)
{
    return Tables::tBases[tbase].name[Lang::En];
}


pchar Range::ToName(Range::E range)
{
    static pchar  names[Range::Count] =
    {
        "2mV",
        "5mV",
        "10mV",
        "20mV",
        "50mV",
        "100mV",
        "200mV",
        "500mV",
        "1V",
        "2V",
        "5V",
        "10V",
        "20V"
    };
    return names[range];
}


void Tables::DrawStr(int index, int x, int y)
{
    pchar str = symbolsAlphaBet[index];

    if (index == INDEX_SYMBOL)
    {
        int width = Font::GetLengthText(str);

        if (str[0] == '%')
        {
            width -= 6;
        }

        Region(width, 9).Fill(x - 1, y, Color::FLASH_10);
    }

    String<>(symbolsAlphaBet[index]).Draw(x, y, index == INDEX_SYMBOL ? Color::FLASH_01 : COLOR_FILL);
}
