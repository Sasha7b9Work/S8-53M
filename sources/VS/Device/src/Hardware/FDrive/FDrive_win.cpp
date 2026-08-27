// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/FDrive/FDrive.h"


namespace FDrive
{
    bool isConnected = false;
    bool needSave = false;
}


void FDrive::Init()
{

}


void FDrive::Update()
{

}


void FDrive::SaveSignal()
{
}


Directory::~Directory()
{

}


bool Directory::GetFirstDir(pchar /*fullPath*/, int /*numDir*/, char * /*nameDirOut*/)
{
    return false;
}


bool Directory::GetFirstFile(pchar /*fullPath*/, int /*numFile*/, FileName & /*fileName*/)
{
    return false;
}


bool Directory::GetNextDir(char * /*nameDirOut*/)
{
    return false;
}


bool Directory::GetNextFile(FileName & /*fileName*/)
{
    return false;
}


void Directory::GetNumDirsAndFiles(pchar  /*fullPath*/, int * /*numDirs*/, int * /*numFiles*/)
{

}


pchar FileName::Extract()
{
    return nullptr;
}


String<> &DisplayString::Decode()
{
    static String<> null("");

    return null;
}


File::~File()
{

}


bool File::OpenNewForWrite(pchar  /*fullPathToFile*/)
{
    return false;
}


bool File::Write(const void * /*_data*/, int /*size*/)
{
    return false;
}