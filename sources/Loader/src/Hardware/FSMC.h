// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


#define ADDR_BANK           0x60000000
#define ADDR_DISPLAY_A0     ((uint8*)(ADDR_BANK + 0x00910000))
#define ADDR_DISPLAY_D7_D0  ((uint8*)(ADDR_BANK + 0x00900000))
#define ADDR_CDISPLAY       ((uint8*)(ADDR_BANK + 0x00800000))
#define ADDR_FPGA           ((uint8*)(ADDR_BANK + 0x00c80000))  ///< ����� ������ � ���������� ��������.
#define ADDR_NULL           ((uint8*)(ADDR_BANK + 0x00a00000))


uint8   FSMC_Read(uint8* address);

void    FSMC_Write(uint8 *address, uint8 value);
