// 2022/02/11 17:48:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Text/String.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <ff.h>

#ifdef _MS_VS
#define _WIN32
#endif

class FileName : public String<>
{
public:
    FileName() : String() { }
    FileName(char *name) : String<>(name) { }

    // Возвращает указатель на собственно имя файла, если имя включает в себя абсолютный путь
    pchar Extract();
};


struct File
{
    static const int SIZE_BUFFER = 512;

    uint8    buffer[SIZE_BUFFER];
    int      sizeData;
    FIL      fileObj;
    FileName name;

    ~File();

    // Функция создаст файл для записи. Если такой файл уже существует, сотрёт его, заменит новым нулевой длины и откроет его
    bool OpenNewForWrite(pchar  fullPathToFile);

    bool Write(const void *data, int sizeData);

    // Записывает стоку в файл с заменой завершающего нуля символами конца строки и перевода каретки
    bool WriteString(const String<> &string);
};


struct Directory
{
    DIR dir;

    ~Directory();

    bool GetFirstDir(pchar fuulPath, int numDir, char *nameDirOut);

    bool GetNextDir(char *nameDirOut);

    bool GetFirstFile(pchar fullPath, int numFile, FileName &);

    bool GetNextFile(FileName &);

    static void GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles);
};


namespace FDrive
{
    extern bool isConnected;
    extern bool needSave;           // Если true, то нужно сохранить после отрисовки на флешку.

    void Init();

    void Update();

    void SaveSignal();
};


// Класс для преобразования символов названий файлов в читаемые файлы
class DisplayString : public String<>
{
public:
    DisplayString(char *string) : String<>(string) {}

    String<> &Decode();
};
