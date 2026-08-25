// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct Word
{
    pchar address;
    int8  numSymbols;

    // Эта команда сразу преобразует к верхенму регистру слово
    // Возвращает слово номер numWord из строки string в word (если такое имеется, конечно)
    bool GetWord(pchar string, const int numWord);

    bool WordEqualZeroString(char *string);

private:

    // "Выбрать" символы. Возвращает nullptr, если выбор невозможен - строка кончилась
    pchar ChooseSymbols(pchar string);

    // "Выбрать" пробелы. Возвращает nullptr, если выбор невозможен - строка кончилась
    pchar ChooseSpaces(pchar string);
};


namespace SU
{
    char ToUpper(char symbol);

    char ToLower(char symbol);

    char *DoubleToString(double value);

    bool StringToDouble(double *value, pchar buffer);

    bool IsDigit(char symbol);

    bool EqualsStrings(pchar str1, pchar str2, uint size);
};
