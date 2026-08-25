// 2022/01/21 16:39:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "SCPI/BufferSCPI.h"
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <cstdio>


void BufferSCPI::Append(pchar data, int length)
{
    if (Size() + length > MAX_SIZE)
    {
        LOG_ERROR("Veri small buffer size %d, need %d", MAX_SIZE, Size() + length);
    }
    else
    {
        std::memcpy(buffer + Size(), data, (uint)length);
        size += length;
    }
}


bool BufferSCPI::ExistDivider() const
{
    for (int i = 0; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            return true;
        }
    }

    return false;
}


void BufferSCPI::RemoveFirstDividers()
{
    while (Size() && (buffer[0] == 0x0d || buffer[0] == 0x0a))
    {
        RemoveFirstBytes(1);
    }
}


String<> BufferSCPI::ToString()
{
    char temp[1024];
    std::memcpy(temp, buffer, (uint)Size());
    temp[Size()] = '\0';

    return String<>(temp);
}


String<> BufferSCPI::ExtractCommand()
{
    int pos_start = 0;

    for (int i = 0; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            pos_start++;
        }
        else
        {
            break;
        }
    }

    int pos_end = -1;

    for (int i = pos_start; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            pos_end = i;
            break;
        }
    }

    if (pos_end == -1)
    {
        return String<>("");
    }

    String<> result;

    result.AppendBytes((char *)&buffer[pos_start], pos_end - pos_start);

    RemoveFirstBytes(pos_end);

    RemoveFirstDividers();

    for (int i = 0; i < result.Size(); i++)
    {
        result[i] = (char)(std::toupper((int8)(result[i])));
    }

    return result;
}


void BufferSCPI::RemoveFirstBytes(int num_bytes)
{
    if (num_bytes > Size())
    {
        LOG_ERROR("Слишком много байт для удаления");
        num_bytes = Size();
    }

    if (num_bytes == Size())
    {
        size = 0;
    }
    else
    {
        for (int i = num_bytes; i < size; i++)
        {
            buffer[i - num_bytes] = buffer[i];
        }

        size -= num_bytes;
    }
}
