// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Log.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Hardware/VCP/VCP.h"
#include "Display/Screen/Console.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Log
{
    const int SIZE_BUFFER_LOG = 1024;
}


void Log::Write(char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    Console::Append(buffer);
}


void Log::Error(char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];

    std::strcpy(buffer, "!!! ERROR !!! ");

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer + std::strlen(buffer), format, args);
    va_end(args);

    Console::Append(buffer);
}


void Log::ErrorTrace(pchar module, pchar func, int numLine, char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    char numBuffer[20];
    std::sprintf(numBuffer, ":%d", numLine);
    char message[SIZE_BUFFER_LOG];
    message[0] = 0;
    std::strcat(message, "!!!ERROR!!! ");
    std::strcat(message, module);
    std::strcat(message, " ");
    std::strcat(message, func);
    std::strcat(message, numBuffer);

    Console::Append(message);
    Console::Append(buffer);
}
