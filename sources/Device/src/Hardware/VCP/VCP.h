// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Hardware/Timer.h"


//#define VCP_DEBUG_POINT()       VCP::DebugPoint(__MODULE__, __FUNCTION__, __LINE__)
//#define VCP_FORMAT_TRACE(...)   VCP::SendFormatTrace(__MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
//#define VCP_FORMAT(...)         VCP::SendFormat(__VA_ARGS__)

namespace VCP
{
    void Init();

    void SendFormatTrace(pchar module, pchar func, int line, char *format, ...);

    void DebugPoint(pchar module, pchar function, int line);

    extern bool cableIsConnected;       // true, если подсоединён кабель
    extern bool connectToHost;          // true, если есть подключение

    namespace Buffer
    {
        // Послать форматированную строку с завершающим символом '\n'
        void SendFormat0D(pchar format, ...);
        void Send(const void *buffer, int size);
        void Flush();
    }
};
