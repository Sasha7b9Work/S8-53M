// 2022/02/11 17:45:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Log.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Hardware/InterCom.h"
#include "Utils/Text/Text.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cctype>



#define  SYMBOL(x) (*(*(x)))


namespace SU
{
    static bool ChooseSymbols(pchar *string);    // Возвращает false, если выбор невозможен - строка кончилась.
    static bool ChooseSpaces(pchar *string);     // Возвращает false, если выбор невозможен - строка кончилась.

    static int NumDigitsInIntPart(float value);
}


namespace Text
{
    // Отрисовка непосредственно символа
    int DrawCharHard(bool send, int x, int y, char symbol);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    // Возвращает высоту экрана, которую займёт текст text, при выводе от left до right в переменной height. Если
    // bool == false, то текст не влезет на экран 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    bool ByteFontNotEmpty(int eChar, int byte);

    bool BitInFontIsExist(int eChar, int numByte, int bit);

    void DrawCharInColorDisplay(bool send, int eX, int eY, uchar symbol);

    int DrawBigChar(int eX, int eY, int size, char symbol);

    bool IsLetter(char symbol);

    bool IsConsonant(char symbol);

    // Находит следующий перенос. C letters начинается часть слово, где нужно найти перенос, в lettersInSyllable будет
    // записано число букв в найденном слоге. Если слово закончилось, функция возвращает false
    bool FindNextTransfer(char *letters, int8 *lettersInSyllable);

    int8 *BreakWord(char *word);

    bool CompareArrays(const bool *array1, const bool *array2, int numElems);

    // Возвращает часть слова до слога numSyllable(включительн) вместе со знаком переноса
    char *PartWordForTransfer(char *word, int8 *lengthSyllables, int numSyllables, int numSyllable, char buffer[30]);
}


void Font::Set(TypeFont::E typeFont)
{
    if (typeFont == current)
    {
        return;
    }

    Font::font = Font::fonts[typeFont];

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<2> command(SET_FONT);
        command.PushByte(typeFont);
        command.Transmit();
    }
}


bool Text::ByteFontNotEmpty(int eChar, int byte)
{
    static const uint8 *bytes = 0;
    static int prevChar = -1;

    if (eChar != prevChar)
    {
        prevChar = eChar;
        bytes = Font::font->symbol[prevChar].bytes;
    }

    return bytes[byte] != 0;
}


bool Text::BitInFontIsExist(int eChar, int numByte, int bit)
{
    static uint8 prevByte = 0;      // WARN здесь точно статики нужны?
    static int prevChar = -1;
    static int prevNumByte = -1;
    if (prevNumByte != numByte || prevChar != eChar)
    {
        prevByte = Font::font->symbol[eChar].bytes[numByte];
        prevChar = eChar;
        prevNumByte = numByte;
    }
    return prevByte & (1 << bit);
}


void Text::DrawCharInColorDisplay(bool send, int eX, int eY, uchar symbol)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Point().Set(false, x, y);
                }
                x++;
            }
        }
    }

    if (send && InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<6> command(DRAW_CHAR);
        command.PushHalfWord(eX);
        command.PushHalfWord(eY);
        command.PushByte(symbol);
        command.Transmit();
    }
}


int Text::DrawBigChar(int eX, int eY, int size, char symbol)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b * size + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    for (int i = 0; i < size; i++)
                    {
                        for (int j = 0; j < size; j++)
                        {
                            Point().Set(false, x + i, y + j);
                        }
                    }
                }
                x += size;
            }
        }
    }

    return eX + width * size;
}


int Text::DrawCharHard(bool send, int eX, int eY, char symbol)
{
    int8 width = (int8)Font::font->symbol[(uint8)symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Point().Set(false, x, y);
                }
                x++;
            }
        }
    }

    if (send && InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<6> command(DRAW_CHAR);
        command.PushHalfWord(eX);
        command.PushHalfWord(eY);
        command.PushByte(symbol);
        command.Transmit();
    }

    return eX + width;
}


int Char::Draw(bool send, int x, int y, Color::E color)
{
    Color::SetCurrent(color);

    if (Font::GetSize() == 5)
    {
        Text::DrawCharHard(send, x, y + 3, symbol);
    }
    else if (Font::GetSize() == 8)
    {
        Text::DrawCharHard(send, x, y, symbol);
    }
    else
    {
        Text::DrawCharInColorDisplay(send, x, y, (uint8)symbol);
    }

    return x + Font::GetLengthSymbol((uint8)symbol);
}


int Text::DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    if ((x >= limitX) && (x <= (limitX + limitWidth)) && (y >= limitY) && (y <= limitY + limitHeight))
                    {
                        Point().Set(false, x, y);
                    }
                }
                x++;
            }
        }
    }

    return eX + width + 1;
}


bool Text::IsLetter(char symbol)
{
    static const bool isLetter[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
    };

    return isLetter[(uint8)symbol];
}


String<> Text::GetWord(pchar firstSymbol, int *length)
{
    *length = 0;

    String<> result;

    while (IsLetter(*firstSymbol))
    {
        result.Append(*firstSymbol);
        firstSymbol++;
        (*length)++;
    }

    return result;
}


bool Text::IsConsonant(char symbol)
{
    static const bool isConsonat[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false
    };

    return isConsonat[(uint8)symbol];
}


bool Text::CompareArrays(const bool *array1, const bool *array2, int numElems)
{
    for (int i = 0; i < numElems; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}


bool Text::FindNextTransfer(char *letters, int8 *lettersInSyllable)
{

#define VOWEL       0   // Гласная
#define CONSONANT   1   // Согласная

    *lettersInSyllable = (int8)std::strlen(letters);

    if (std::strlen(letters) <= 3) //-V1051
    {
        return false;
    }

    static const bool template1[3] = {false, true, true};               //     011     2   // После второго символа перенос
    static const bool template2[4] = {true, false, true, false};        //     1010    2
    static const bool template3[4] = {false, true, false, true};        //     0101    3
    static const bool template4[4] = {true, false, true, true};         //     1011    3
    static const bool template5[4] = {false, true, false, false};       //     0100    3
    static const bool template6[4] = {true, false, true, true};         //     1011    3
    static const bool template7[5] = {true, true, false, true, false};  //     11010   3
    static const bool template8[6] = {true, true, false, true, true};   //     11011   4

    bool consonant[20];

    int size = (int)std::strlen(letters);

    for (int i = 0; i < size; i++)
    {
        consonant[i] = IsConsonant(letters[i]);
    }

    if (CompareArrays(template1, consonant, 3))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (CompareArrays(template2, consonant, 4))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (std::strlen(letters) < 5)
    {
        return false;
    }
    if (CompareArrays(template3, consonant, 4) || CompareArrays(template4, consonant, 4) || CompareArrays(template5, consonant, 4) || CompareArrays(template6, consonant, 4))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (std::strlen(letters) < 6)
    {
        return false;
    }
    if (CompareArrays(template7, consonant, 5))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (CompareArrays(template8, consonant, 5))
    {
        *lettersInSyllable = 4;
        return true;
    }
    return false;
}


int8* Text::BreakWord(char *word)
{
    int num = 0;
    static int8 lengthSyllables[10];
    char *position = word;
    while (FindNextTransfer(position, &(lengthSyllables[num])))
    {
        position += lengthSyllables[num];
        num++;
    }
    lengthSyllables[num + 1] = 0;
    if (std::strcmp(word, "структуру") == 0)
    {
        int8 lengths[] = {5, 2, 2, 0};
        std::memcpy(lengthSyllables, lengths, 4);
    }
    else if (std::strcmp(word, "соответствующей") == 0)
    {
        int8 lenghts[] = {4, 3, 4, 5, 3, 0};
        std::memcpy(lengthSyllables, lenghts, 6);
    }
    return lengthSyllables;
}


char* Text::PartWordForTransfer(char *word, int8* lengthSyllables, int, int numSyllable, char buffer[30])
{
    int length = 0;

    for (int i = 0; i <= numSyllable; i++)
    {
        length += lengthSyllables[i];
    }

    std::memcpy((void*)buffer, (void*)word, (uint)length);
    buffer[length] = '-';
    buffer[length + 1] = '\0';
    return buffer;
}


int Text::DrawPartWord(char *word, int x, int y, int xRight, bool draw)
{
    int8 *lengthSyllables = BreakWord(word);
    int numSyllabels = 0;
    char buffer[30];
    for (int i = 0; i < 10; i++)
    {
        if (lengthSyllables[i] == 0)
        {
            numSyllabels = i;
            break;
        }
    }

    for (int i = numSyllabels - 2; i >= 0; i--)
    {
        char *subString = PartWordForTransfer(word, lengthSyllables, numSyllabels, i, buffer);
        int length = Font::GetLengthText(subString);
        if (xRight - x > length - 5)
        {
            if (draw)
            {
                String<>(subString).Draw(x, y);
            }

            return (int)std::strlen(subString) - 1;
        }
    }

    return 0;
}


bool Text::GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height)
{
    int numSymb = (int)std::strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < 231 && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            String<> word = GetWord(text + curSymbol, &length);

            if (length <= 1)                            // Нет буквенных символов или один, т.е. слово не найдено
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x += Font::GetLengthSymbol((uint8)symbol);
            }
            else                                            // А здесь найдено по крайней мере два буквенных символа, т.е. найдено слово
            {
                int lengthString = Font::GetLengthText(word.c_str());
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word.c_str(), x, y, right, false);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x += Font::GetLengthText(word.c_str());
                }
            }
        }
        x = left;
        y += 9;
    }

    LIMITATION(*height, y - top + 4, 0, 239);

    return curSymbol == numSymb;
}



void Painter::DrawHintsForSmallButton(int x, int y, int width, void *smallButton)
{
    SmallButton *sb = (SmallButton*)smallButton;
    Region(width, 239 - y).Fill(x, y, COLOR_BACK);
    Rectangle(width, 239 - y).Draw(x, y, COLOR_FILL);

    if (sb->hintUGO)
    {
        const StructHelpSmallButton *structHelp = &(*sb->hintUGO)[0];

        x += 3;
        y += 3;

        while (structHelp->funcDrawUGO)
        {
            Rectangle(WIDTH_SB, WIDTH_SB).Draw(x, y);
            structHelp->funcDrawUGO(x, y);
            int yNew = String<>(structHelp->helpUGO[LANG]).DrawInRectWithTransfers(x + 23, y + 1, width - 30, 20);
            y = ((yNew - y) < 22) ? (y + 22) : yNew;
            structHelp++;
        }
    }
}


void Text::Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2)
{
    Char(symbol1).Draw(true, x, y, color1);
    Char(symbol2).Draw(true, x, y, color2);
}


void Text::Draw4SymbolsInRect(int x, int y, char eChar, Color::E color)
{
    Color::SetCurrent(color);

    for (int i = 0; i < 2; i++)
    {
        Char((char)(eChar + i)).Draw(true, x + 8 * i, y);
        Char((char)(eChar + i + 16)).Draw(true, x + 8 * i, y + 8);
    }
}


void Text::Draw10SymbolsInRect(int x, int y, char eChar)
{
    for (int i = 0; i < 5; i++)
    {
        Char((char)(eChar + i)).Draw(true, x + 8 * i, y);
        Char((char)(eChar + i + 16)).Draw(true, x + 8 * i, y + 8);
    }
}


void Text::DrawBig(int eX, int eY, int size, pchar text)
{
    int numSymbols = (int)std::strlen(text);

    int x = eX;

    for (int i = 0; i < numSymbols; i++)
    {
        x = DrawBigChar(x, eY, size, text[i]);
        x += size;
    }
}



static int SU::NumDigitsInIntPart(float value)
{
    float fabsValue = std::fabs(value);

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


int SU::GetNumWordsInString(pchar string)
{
    SU::ChooseSpaces(&string);

    while (true)
    {
        int numWords = 0;

        if (SU::ChooseSymbols(&string))
        {
            numWords++;
        }
        else
        {
            return numWords;
        }
        SU::ChooseSpaces(&string);
    }
}


bool Word::GetWord(pchar string, const int numWord)
{
    SU::ChooseSpaces(&string);

    int currentWord = 0;

    while (true)
    {
        if (currentWord == numWord)
        {
            address = (char *)string;
            SU::ChooseSymbols(&string);
            numSymbols = string - address;

            char *pointer = address;

            for (int i = 0; i < numSymbols; i++)
            {
                *pointer = (char)std::toupper(*pointer);
                pointer++;
            }

            return true;
        }
        if (SU::ChooseSymbols(&string))
        {
            currentWord++;
        }
        else
        {
            return false;
        }
        SU::ChooseSpaces(&string);
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


bool SU::WordEqualZeroString(Word *word, char *string)
{
    char *ch = string;
    char *w = (char *)(word->address);

    while (*ch != 0)
    {
        if (*ch++ != *w++)
        {
            return false;
        }
    }

    return (ch - string) == word->numSymbols;
}


bool SU::ChooseSymbols(pchar *string)
{
    if (SYMBOL(string) == 0)
    {
        return false;
    }

    while (SYMBOL(string) != ' ' && SYMBOL(string) != 0)
    {
        (*string)++;
    }

    return true;
}


bool SU::ChooseSpaces(pchar *string)
{
    if (SYMBOL(string) == 0)
    {
        return false;
    }

    while (SYMBOL(string) == ' ')
    {
        (*string)++;
    }

    return true;
}


bool SU::EqualsStrings(pchar str1, char *str2, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}


bool SU::EqualsZeroStrings(char *str1, char *str2)
{
    while ((*str1) == (*str2))
    {
        if ((*str1) == '\0')
        {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}


void SU::LogBufferU8(const uint8 *data, int num)
{
    String<> result;

    for (int i = 0; i < num; i++)
    {
        result.Append(SU::Int2String(data[i], false, 1));
        result.Append(" ");
    }

    LOG_WRITE(result.c_str());
}


void SU::LogBufferU8(pchar label, const uint8 *data, int num)
{
    String<> result(label);
    result.Append(" ");

    for (int i = 0; i < num; i++)
    {
        result.Append(SU::Int2String(data[i], false, 1));
        result.Append(" ");
    }

    LOG_WRITE(result.c_str());
}


void SU::LogBufferF(pchar label, const float *data, int num)
{
    String<> string(label);
    string.Append(" ");

    for (int i = 0; i < num; i++)
    {
        string.Append(SU::Float2String(data[i], false, 4));
        string.Append(" ");
    }

    LOG_WRITE(string.c_str());
}


String<> SU::FloatFract2String(float value, bool alwaysSign)
{
    return String<>(Float2String(value, alwaysSign, 4));
}


String<> SU::Bin2String(uint8 value)
{
    char buffer[9];

    for (int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(value, bit) ? '1' : '0';
    }

    return String<>(buffer);
}


String<> SU::Bin2String16(uint16 value)
{
    char buffer[19];

    std::strcpy(buffer, Bin2String((uint8)(value >> 8)).c_str());
    std::strcpy((buffer[8] = ' ', buffer + 9), Bin2String((uint8)value).c_str());

    return String<>(buffer);
}


String<> SU::Float2String(float value, bool alwaysSign, int numDigits)
{
    String<> result;

    char *buffer = result.c_str();

    if (value == ERROR_VALUE_FLOAT || isnan(value) || isinf(value))
    {
        return EmptyString();
    }

    if (!alwaysSign)
    {
        if (value < 0)
        {
            *buffer = '-';
            buffer++;
        }
    }
    else
    {
        *buffer = value < 0 ? '-' : '+';
        buffer++;
    }

    char format[] = "%4.2f\0\0";

    format[1] = (char)numDigits + 0x30;

    int numDigitsInInt = NumDigitsInIntPart(value);

    format[3] = (numDigits - numDigitsInInt) + 0x30;
    if (numDigits == numDigitsInInt)
    {
        format[5] = '.';
    }

    std::snprintf(buffer, 19, format, std::fabs(value));
    
    if((int)std::strlen(buffer) > numDigits + 1)        // Это затычка на то, что число -9.99999 выводится как -10.000, хотя должно быть четыре цифры - 2 цифры после запятой
    {
        buffer[numDigits + 1] = '\0';
    }

    float val = std::atof(buffer);

    if (NumDigitsInIntPart(val) != numDigitsInInt)
    {
        numDigitsInInt = NumDigitsInIntPart(val);
        format[3] = (numDigits - numDigitsInInt) + 0x30;

        if (numDigits == numDigitsInInt)
        {
            format[5] = '.';
        }
    }

    bool signExist = alwaysSign || value < 0;

    while (result.Size() < numDigits + (signExist ? 2 : 1))
    {
        result.Append("0");
    }

    if (result.Size() > numDigits + 2)
    {
        LOG_ERROR("Неправильное значение: %s", result.c_str());
    }

    return result;
}


String<> SU::Int2String(int value, bool alwaysSign, int numMinFields)
{
    char buffer[20];

    char format[20] = "%";
    std::sprintf(&(format[1]), "0%d", numMinFields);
    std::strcat(format, "d");

    if (alwaysSign && value >= 0)
    {
        buffer[0] = '+';
        std::sprintf(buffer + 1, format, value);
    }
    else
    {
        std::sprintf(buffer, format, value);
    }

    return String<>(buffer);
}


String<> SU::Hex8toString(uint8 value)
{
    char buffer[3];
    std::sprintf(value < 16 ? (buffer[0] = '0', buffer + 1) : (buffer), "%x", value);
    return String<>(buffer);
}


bool SU::String2Int(char *str, int *value)
{
    int sign = str[0] == '-' ? -1 : 1;

    if (str[0] < '0' || str[0] > '9')
    {
        str++;
    }

    int length = (int)std::strlen(str);

    if (length == 0)
    {
        return false;
    }

    *value = 0;
    int pow = 1;

    for (int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if (val < 0 || val > 9)
        {
            return false;
        }
        *value += val * pow;
        pow *= 10;
    }

    if (sign == -1)
    {
        *value *= -1;
    }

    return true;
}


String<> SU::Voltage2String(float voltage, bool alwaysSign)
{
    char *suffix;

    if (voltage == ERROR_VALUE_FLOAT)
    {
        return EmptyString();
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1e-3f)
    {
        suffix = LANG_RU ? "\x10мкВ" : "\x10uV";
        voltage *= 1e6f;
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1)
    {
        suffix = LANG_RU ? "\x10мВ" : "\x10mV";
        voltage *= 1e3f;
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1000)
    {
        suffix = LANG_RU ? "\x10В" : "\x10V";
    }
    else
    {
        suffix = LANG_RU ? "\x10кВ" : "\x10kV";
        voltage *= 1e-3f;
    }

    String<> result = SU::Float2String(voltage, alwaysSign, 4);
    result.Append(suffix);

    return result;
}


String<> SU::Time2String(float time, bool alwaysSign)
{
    char *suffix = 0;

    if (time == ERROR_VALUE_FLOAT)
    {
        return EmptyString();
    }
    else if (std::fabs(time) + 0.5e-10f < 1e-6f)
    {
        suffix = LANG_RU ? "нс" : "ns";
        time *= 1e9f;
    }
    else if (std::fabs(time) + 0.5e-7f < 1e-3f)
    {
        suffix = LANG_RU ? "мкс" : "us";
        time *= 1e6f;
    }
    else if (std::fabs(time) + 0.5e-3f < 1)
    {
        suffix = LANG_RU ? "мс" : "ms";
        time *= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "с" : "s";
    }

    String<> result = SU::Float2String(time, alwaysSign, 4);
    result.Append(suffix);

    return result;
}


String<> SU::Freq2String(float freq, bool)
{
    if (freq == ERROR_VALUE_FLOAT || isnan(freq) || isinf(freq))
    {
        return EmptyString();
    }

    char *suffix = 0;

    if (freq >= 1e6f)
    {
        suffix = LANG_RU ? "МГц" : "MHz";
        freq /= 1e6f;
    }
    else if (freq >= 1e3f)
    {
        suffix = LANG_RU ? "кГц" : "kHz";
        freq /= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "Гц" : "Hz";
    }

    String<> result = SU::Float2String(freq, false, 4);

    if (result.Size() > 6)
    {
        LOG_WRITE(result.c_str());
    }

    result.Append(suffix);

    return result;
}


String<> SU::Phase2String(float phase, bool)
{
    String<> result = SU::Float2String(phase, false, 4);
    result.Append("\xa8");
    return result;
}


String<> SU::Float2Db(float value, int numDigits)
{
    String<> result = SU::Float2String(value, false, numDigits);

    result.Append(LANG_RU ? "дБ" : "dB");

    return result;
}


bool SU::IsDigit(char symbol)
{
    return (symbol >= '0') && (symbol <= '9');
}


#undef SYMBOL
