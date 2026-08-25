// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned int      uint;
typedef const char *const pstring;
typedef const char       *pchar;


#ifndef WIN32
#define nullptr 0 //-V1059
#endif


#define LOG_WRITE(...)
