// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h" 
#include "Hardware/HAL/HAL.h"
#include "Hardware/FDrive/USBH.h"
#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "Hardware/FDrive/FDrive.h"
#include "main.h"
#include <cctype>
#include <cstring>


MSC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;


FATFS FDrive::USBDISKFatFS;
char FDrive::USBDISKPath[4];
FIL FDrive::file;


struct StructForReadDir
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;
};



static bool GetNameFile(pchar fullPath, int numFile, char *nameFileOut, StructForReadDir *s);
static bool GetNextNameFile(char *nameFileOut, StructForReadDir *s);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id);



void FDrive::Init()
{
    if (FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == FR_OK)
    {
        USBH_Init((USBH_HandleTypeDef *)USBH::handle, USBH_UserProcess, 0);
        USBH_RegisterClass((USBH_HandleTypeDef *)USBH::handle, USBH_MSC_CLASS);
        USBH_Start((USBH_HandleTypeDef *)USBH::handle);
    }
}


void USBH_UserProcess(USBH_HandleTypeDef *, uint8 id)
{
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_CLASS_ACTIVE:
            MainStruct::state = State::DriveNeedMount;
            break;

        case HOST_USER_CLASS_SELECTED:
            break;

        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;

        case HOST_USER_DISCONNECTION:
            MainStruct::state = State::NoDrive;
            f_mount(NULL, (TCHAR const *)"", 0);
            break;

        default:
            break;
    }
}


void FDrive::Update()
{
    USBH_Process(reinterpret_cast<USBH_HandleTypeDef *>(USBH::handle));

    if (MainStruct::state == State::DriveNeedMount)
    {
        if (f_mount(&USBDISKFatFS, (TCHAR const*)USBDISKPath, 0) != FR_OK)
        {
            MainStruct::state = State::WrongDrive;
        }
        else
        {
            MainStruct::state = State::DriveIsMounted;
        }
    }
}


void ToLower(char *str)
{
    while (*str)
    {
        *str = (char)(std::tolower(*str));
        str++;
    }
}


bool FDrive::FileExist(pchar fileName)
{
    char nameFile[255];
    char fil[255];
    std::strcpy(fil, fileName);
    ToLower(fil);
    StructForReadDir strd;

    if (GetNameFile("\\", 0, nameFile, &strd))
    {
        ToLower(nameFile);
        if (std::strcmp(fil, nameFile) == 0)
        {
            return true;
        }
        while (GetNextNameFile(nameFile, &strd))
        {
            ToLower(nameFile);
            if (std::strcmp(fil, nameFile) == 0)
            {
                return true;
            }
        }
    }

    return false;
}


static bool GetNameFile(pchar fullPath, int numFile, char *nameFileOut, StructForReadDir *s)
{
    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    std::strcpy(s->fno.fname, s->lfn);

    s->fno.fsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameFileOut = '\0';
                f_closedir(pDir);
                return false;
            }

            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameFileOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }

            if (numFile == numFiles && (pFNO->fattrib & AM_DIR) == 0)
            {
                strcpy(nameFileOut, pFNO->fname);
                return true;
            }

            if ((pFNO->fattrib & AM_DIR) == 0 && (pFNO->fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }
    return false;
}


static bool GetNextNameFile(char *nameFileOut, StructForReadDir *s)
{
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    while (true)
    {
        if (f_readdir(&s->dir, &s->fno) != FR_OK)
        {
            *nameFileOut = '\0';
            f_closedir(&s->dir);
            return false;
        }

        if (s->fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameFileOut = '\0';
                f_closedir(&s->dir);
                return false;
            }

            alreadyNull = true;
        }
        else
        {
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
                strcpy(nameFileOut, pFNO->fname);
                return true;
            }
        }
    }
}


int FDrive::OpenFileForRead(pchar fileName)
{
    if (f_open(&file, fileName, FA_READ) == FR_OK)
    {
        return (int)f_size(&file);
    }

    return -1;
}


uint FDrive::ReadFromFile(int numBytes, uint8 *buffer)
{
    uint readed = 0;
    if (f_read(&file, buffer, (uint)(numBytes), &readed) == FR_OK)
    {
        return readed;
    }
    return (uint)-1;
}


void FDrive::CloseOpenedFile()
{
    f_close(&file);
}
