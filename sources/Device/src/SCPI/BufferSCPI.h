// 2022/01/21 16:38:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Text/String.h"
#include "Utils/Containers/Buffer.h"


class BufferSCPI : public Buffer<char, 1024>
{
public:

    BufferSCPI() : Buffer<char, 1024>() { Realloc(0); }

    void Append(pchar data, int length);

    // true, если присутствует разделитель - должна быть команда, которую можно обработать
    bool ExistDivider() const;

    String<> ExtractCommand();

    // Удалить разделители из начала буфера
    void RemoveFirstDividers();

    String<> ToString();

private:

    // Удлаить первые num_bytes из начала буфера
    void RemoveFirstBytes(int num_bytes);
};

