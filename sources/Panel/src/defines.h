// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


#ifndef WIN32
    #pragma clang diagnostic ignored "-Winvalid-source-encoding"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#endif


typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned int      uint;
typedef const char *const pstring;
typedef const char       *pchar;


#ifndef WIN32
#define nullptr 0 //-V1059
#endif


#define LOG_WRITE(...)
