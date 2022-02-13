// 2022/02/11 17:48:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <ff.h>

#ifdef _MS_VS
#define _WIN32
#endif

struct StructForWrite
{
    static const int SIZE_FLASH_TEMP_BUFFER = 512;
    uint8 tempBuffer[SIZE_FLASH_TEMP_BUFFER];
    int sizeData;
    FIL fileObj;
    char name[255];
};

struct StructForReadDir
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;
};

class FlashDrive
{
public:
    static void Init();

    static void Update();

    static void GetNumDirsAndFiles(const char* fullPath, int *numDirs, int *numFiles);

    static bool GetNameDir(const char* fuulPath, int numDir, char *nameDirOut, StructForReadDir *sfrd);

    static bool GetNextNameDir(char *nameDirOut, StructForReadDir *sfrd);

    static void CloseCurrentDir(StructForReadDir *sfrd);

    static bool GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *sfrd);

    static bool GetNextNameFile(char *nameFileOut, StructForReadDir *sfrd);
    // ������� ������� ���� ��� ������. ���� ����� ���� ��� ����������, ����� ���, ������� ����� ������� ����� � ������� ���
    static bool OpenNewFileForWrite(const char* fullPathToFile, StructForWrite *structForWrite);

    static bool WriteToFile(uint8* data, int sizeData, StructForWrite *structForWrite);

    static bool CloseFile(StructForWrite *structForWrite);

    static bool AppendStringToFile(const char* string);
};
