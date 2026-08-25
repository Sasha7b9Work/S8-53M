// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once   
#include "defines.h"


#ifdef WIN32
#define __MODULE__ ""
#endif

#ifdef DEBUG
    #define LOG_ERROR(...)       Log::Error(__VA_ARGS__)
    #define LOG_ERROR_TRACE(...) Log::ErrorTrace(__MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
    #define LOG_ERROR(...)
    #define LOG_ERROR_TRACE(...)
#endif


#define LOG_WRITE(...)       Log::Write(__VA_ARGS__)
#define LOG_FUNC_ENTER()     Log::Write("%s enter", __FUNCTION__);
#define LOG_FUNC_LEAVE()     Log::Write("%s leave", __FUNCTION__);
#define LOG_TRACE()          Log::Write("%s : %d", __FUNCTION__, __LINE__);

namespace Log
{
    void Error(char *format, ...);
    void ErrorTrace(pchar module, pchar func, int numLine, char *format, ...);
    void Write(char *format, ...);
}
