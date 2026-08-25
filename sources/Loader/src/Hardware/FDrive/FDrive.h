// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "main.h"


typedef enum
{
    APPLICATION_IDLE = 0,
    APPLICATION_START,
    APPLICATION_RUNNING,
}MSC_ApplicationTypeDef;


extern MSC_ApplicationTypeDef Appli_state;


struct FDrive
{
    static FATFS USBDISKFatFS;
    static char USBDISKPath[4];
    static FIL file;

    static void Init();

    static void Update();

    static bool FileExist(pchar fileName);

    static int OpenFileForRead(pchar fileName);

    // Считывает из открытого файла numBytes байт. Возвращает число реально считанных байт
    static uint ReadFromFile(int numBytes, uint8 *buffer);

    static void CloseOpenedFile();
};
