// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once   
#include "defines.h"


#ifdef WIN32
#define __MODULE__ ""
#endif

#ifdef __linux__
#define __MODULE__ ""
#endif


#define LOG_WRITE(...)   Log::Write(__VA_ARGS__)
#define TLOG_WRITE(...)  Log::TraceWrite(__FUNCTION__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...)   Log::Error(__FILE_NAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_FUNC_ENTER() Log::Write("%s:%d enter", __FUNCTION__, __LINE__);
#define LOG_FUNC_LEAVE() Log::Write("%s:%d leave", __FUNCTION__, __LINE__);
#define LOG_TRACE()      Log::Write("%s : %d", __FILE_NAME__, __LINE__);


namespace Log
{
    void Error(pchar module, pchar func, int numLine, char *format, ...);
    void Write(pchar format, ...);
    void TraceWrite(pchar func, int numLine, char *format, ...);
    void DisconnectLoggerUSB();
    void EnableLoggerUSB(bool enable);
};
