// 2021/03/22 14:37:11 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif


#ifdef WIN32
#else
    #pragma diag_suppress 177
#endif


typedef signed char        int8;
typedef unsigned char      uint8_t;
typedef signed short       int16;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64;
typedef unsigned char      uchar;
typedef const char        *pchar;
typedef const uint8       *puchar;
typedef pchar const  pstring;


typedef void  (*pFuncVV)();
typedef void  (*pFuncVII)(int, int);
typedef bool  (*pFuncBV)();
typedef void  (*pFuncVI)(int);
typedef void  (*pFuncVB)(bool);
typedef pchar (*pFuncCC)(pchar);
typedef char* (*pFuncCFBC)(float, bool, char*);


inline void EmptyFuncVV() {};


#define _GET_BIT(value, bit) (((value) >> (bit)) & 0x01)
#define _SET_BIT(value, bit) ((value) |= (1 << (bit)))
#define _CLEAR_BIT(value, bit) ((value) &= (~(1 << (bit))))

#ifndef UNUSED
#define UNUSED(X) (void)X
#endif


#define ERROR_VALUE_FLOAT   1.111e29f


union BitSet16
{
    BitSet16(uint16 value) : half_word(value) { }
    uint16 half_word;
    struct
    {
        uint8 byte0;
        uint8 byte1;
    };
};

// Объединение размером 32 бита
union BitSet32
{
    uint   word;
    uint16 half_word[2];
    uint8  byte[4];
    struct
    {
        uint16 half_word0;
        uint16 half_word1;
    };
};

// Объединение размером 64 бита
union BitSet64
{
    uint64  dword;
    uint    word[2];
    struct
    {
        int first;
        int second;
    };
    struct
    {
        uint u_first;
        uint u_second;
    };

    BitSet64(int _first = 0, int _second = 0) : first(_first), second(_second) {}
};

