// 2022/02/11 17:48:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Log.h"
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "FDrive.h"
#include "Hardware/HAL/HAL.h"
#include "Display/Painter.h"
#include "Utils/Text/Warnings.h"
#include "Menu/Pages/Definition.h"
#include "Data/Storage.h"
#include "Data/Processing.h"
#include "FPGA/SettingsFPGA.h"


static FATFS USBDISKFatFs;
static char USBDISKPath[4];


static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id);


namespace FDrive
{
    bool isConnected = false;
    bool needSave = false;

    void ChangeState();

    static void SaveAsBMP(pchar fileName);

    static void SaveAsText(pchar fileName);
    static void SaveChannel(const DataStruct &, Chan, File &);
}


namespace Painter
{
    Color::E GetColor(int x, int y);
}


void FDrive::Init()
{
    if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == FR_OK) 
    {
        USBH_Init((USBH_HandleTypeDef *)HAL_USBH::handle, USBH_UserProcess, 0);
        USBH_RegisterClass((USBH_HandleTypeDef *)HAL_USBH::handle, USBH_MSC_CLASS);
        USBH_Start((USBH_HandleTypeDef *)HAL_USBH::handle);
    }
    else
    {
        LOG_ERROR_TRACE("Can not %s", __FUNCTION__);
    }
}


void FDrive::Update()
{
    USBH_Process((USBH_HandleTypeDef *)HAL_USBH::handle);
}


void USBH_UserProcess(USBH_HandleTypeDef *, uint8 id)
{
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_CLASS_ACTIVE:
            FM::Init();
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
            {
                FDrive::isConnected = false;
            }
            else
            {
                FDrive::isConnected = true;
            }
            FDrive::ChangeState();
            break;
        case HOST_USER_CLASS_SELECTED:
            break;
        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;
        case HOST_USER_DISCONNECTION:
            FDrive::isConnected = false;
            FDrive::ChangeState();
            break;
        default:
            break;
    }
}


void WriteToFile(FIL *file, char *string)
{
    uint bytesWritten;
    f_open(file, "list.txt", FA_OPEN_EXISTING);
    f_write(file, string, strlen(string), &bytesWritten);
    f_close(file);
}


void Directory::GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles)
{
    FILINFO fno;
    DIR dir;

    *numDirs = 0;
    *numFiles = 0;
    

    char nameDir[_MAX_LFN + 1];
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    fno.fname[0] = '\0';
    fno.fsize = 0;

    if (f_opendir(&dir, nameDir) == FR_OK)
    {
        int numReadingElements = 0;
        bool alreadyNull = false;

        while (true)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                break;
            }

            if (fno.fname[0] == 0)
            {
                if(alreadyNull)
                {
                    break;
                }
                alreadyNull = true;
                continue;
            }

            numReadingElements++;

            if (fno.fname[0] != '.')
            {
                if (fno.fattrib & AM_DIR)
                {
                    (*numDirs)++;
                }
                else
                {
                    (*numFiles)++;
                }
            }
        }

        f_closedir(&dir);
    }

    f_closedir(&dir);
}


bool Directory::GetFirstDir(pchar fullPath, int numDir, char *nameDirOut)
{
    FILINFO fno;

    if (f_opendir(&dir, fullPath) == FR_OK)
    {
        int numDirs = 0;
        FILINFO *pFNO = &fno;
        bool alreadyNull = false;

        while (true)
        {
            if (f_readdir(&dir, pFNO) != FR_OK)
            {
                *nameDirOut = '\0';
                f_closedir(&dir);
                return false;
            }

            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameDirOut = '\0';
                    f_closedir(&dir);
                    return false;
                }

                alreadyNull = true;
            }

            if (numDir == numDirs && (pFNO->fattrib & AM_DIR))
            {
                strcpy(nameDirOut, pFNO->fname);
                return true;
            }

            if ((pFNO->fattrib & AM_DIR) && (pFNO->fname[0] != '.'))
            {
                numDirs++;
            }
        }
    }

    f_closedir(&dir);

    return false;
}


bool Directory::GetNextDir(char *nameDirOut)
{
    bool alreadyNull = false;

    FILINFO fno;

    while (true)
    {
        if (f_readdir(&dir, &fno) != FR_OK)
        {
            *nameDirOut = '\0';
            f_closedir(&dir);
            return false;
        }
        else if (fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameDirOut = '\0';
                f_closedir(&dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {

            if (fno.fattrib & AM_DIR)
            {
                strcpy(nameDirOut, fno.fname);
                return true;
            }
        }
    }
}


Directory::~Directory()
{
    f_closedir(&dir);
}


bool Directory::GetFirstFile(pchar fullPath, int numFile, FileName &fileName)
{
    FILINFO fno;

    if (f_opendir(&dir, fullPath) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;

        while (true)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                fileName.SetFormat("");
                f_closedir(&dir);
                return false;
            }

            if (fno.fname[0] == 0)
            {
                if (alreadyNull)
                {
                    fileName.SetFormat("");
                    f_closedir(&dir);
                    return false;
                }

                alreadyNull = true;
            }

            if (numFile == numFiles && (fno.fattrib & AM_DIR) == 0)
            {
                fileName.SetFormat(fno.fname);
                return true;
            }
            if ((fno.fattrib & AM_DIR) == 0 && (fno.fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }

    f_closedir(&dir);

    return false;
}


bool Directory::GetNextFile(FileName &fileName)
{
    bool alreadyNull = false;

    FILINFO fno;

    while (true)
    {
        if (f_readdir(&dir, &fno) != FR_OK)
        {
            fileName.SetFormat("");
            f_closedir(&dir);
            return false;
        }
        if (fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                fileName.SetFormat("");
                f_closedir(&dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            if ((fno.fattrib & AM_DIR) == 0 && fno.fname[0] != '.')
            {
                fileName.SetFormat(fno.fname);
                return true;
            }
        }
    }
}


bool File::OpenNewForWrite(pchar  fullPathToFile)
{
    if (f_open(&fileObj, fullPathToFile, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        return false;
    }

    name.SetFormat(fullPathToFile);
    sizeData = 0;

    return true;
}


bool File::Write(const void *_data, int size)
{
    uint8 *data = (uint8 *)_data;

    while (size > 0)
    {
        int dataToCopy = size;

        if (size + sizeData > SIZE_BUFFER)
        {
            dataToCopy = SIZE_BUFFER - sizeData;
        }

        size -= dataToCopy;
        memcpy(buffer + sizeData, data, (uint)dataToCopy);
        data += dataToCopy;
        sizeData += dataToCopy;

        if (sizeData == SIZE_BUFFER)
        {
            uint wr = 0;
            if (f_write(&fileObj, buffer, (uint)sizeData, &wr) != FR_OK || (uint)sizeData != wr)
            {
                return false;
            }
            sizeData = 0;
        }
    }

    return true;
}


bool File::WriteString(const String<> &string)
{
    if (Write(string.c_str(), string.Size()))
    {
        return Write("\x0d\x0a", 2);
    }

    return false;
}


File::~File()
{
    if (sizeData != 0)
    {
        uint wr = 0;

        if (f_write(&fileObj, buffer, (uint)sizeData, &wr) != FR_OK || (uint)sizeData != wr)
        {
            f_close(&fileObj);
            return;
        }
    }

    f_close(&fileObj);

    FILINFO fno;
    PackedTime time = HAL_RTC::GetPackedTime();
    fno.fdate = (WORD)(((time.year + 20) * 512) | (time.month * 32) | time.day);
    fno.ftime = (WORD)((time.hours * 2048) | (time.minutes * 32) | (time.seconds / 2));
    f_utime(name.c_str(), &fno);
}



String<> &DisplayString::Decode()
{
    char *text = c_str();

    while (*text)
    {
        char symbol = *text;

        if (symbol > 127)
        {
            if (symbol == 240) { *text = 'Е'; }
            else if (symbol == 241) { *text = (char)0xb8; }
            else if (symbol >= 224)
            {
                *text += 16;
            }
            else
            {
                *text += 64;
            }
        }

        text++;
    }

    return *this;
}


void FDrive::ChangeState()
{
    if (!FDrive::isConnected)
    {
        if (Page::NameOpened() == NamePage::SB_FileManager)
        {
            Item::Opened()->CloseOpened();
        }
    }
    else if (FLASH_AUTOCONNECT)
    {
        FM::needOpen = true;
    }
}


pchar FileName::Extract()
{
    if (Size() == 0)
    {
        return nullptr;
    }

    pchar pointer = &c_str()[Size()];

    do
    {
        pointer--;

        if (*pointer == '\\' || *pointer == '/')
        {
            return pointer + 1;
        }

    } while (pointer != c_str());

    return pointer;
}


void FDrive::SaveSignal()
{
    if (!needSave)
    {
        return;
    }

    needSave = false;

    String<> fileName = FM::GetNameForNewFile();

    if (!fileName.Size())
    {
        LOG_ERROR("Не получено имя для файла");
        return;
    }

    if (MODE_SAVE_SIGNAL_IS_BMP)
    {
        SaveAsBMP(fileName.c_str());
    }
    else
    {
        SaveAsText(fileName.c_str());
    }
}


void FDrive::SaveAsBMP(pchar fileName)
{
#pragma pack(1)
    struct BITMAPFILEHEADER
    {
        char    type0;      // 0
        char    type1;      // 1
        uint    size;       // 2
        uint16  res1;       // 6
        uint16  res2;       // 8
        uint    offBits;    // 10
    }
    bmFH =
    {
        0x42,
        0x4d,
        14 + 40 + 1024 + 320 * 240,
        0,
        0,
        14 + 40 + 1024
    };
    // 14

    struct BITMAPINFOHEADER
    {
        uint    size;           // 14
        int     width;          // 18
        int     height;         // 22
        uint16  planes;         // 26
        uint16  bitCount;       // 28
        uint    compression;    // 30
        uint    sizeImage;      // 34
        int     xPelsPerMeter;  // 38
        int     yPelsPerMeter;  // 42
        uint    clrUsed;        // 46
        uint    clrImportant;   // 50
        //uint    notUsed[15];
    }
    bmIH =
    {
        40, // size;
        320,// width;
        240,// height;
        1,  // planes;
        8,  // bitCount;
        0,  // compression;
        0,  // sizeImage;
        0,  // xPelsPerMeter;
        0,  // yPelsPerMeter;
        0,  // clrUsed;
        0   // clrImportant;
    };
    // 54
#pragma pack(4)

    File file;

    if (file.OpenNewForWrite(fileName))
    {
        file.Write((uint8 *)(&bmFH), 14);

        file.Write((uint8 *)(&bmIH), 40);

        uint8 buffer[320 * 3] = {0};

        struct RGBQUAD
        {
            uint8    blue;
            uint8    green;
            uint8    red;
            uint8    rgbReserved;
        };

        RGBQUAD colorStruct;

        for (int i = 0; i < 16; i++)
        {
            uint color = COLOR(i);
            colorStruct.blue = B_FROM_COLOR(color);
            colorStruct.green = G_FROM_COLOR(color);
            colorStruct.red = R_FROM_COLOR(color);
            colorStruct.rgbReserved = 0;
            ((RGBQUAD *)(buffer))[i] = colorStruct;
        }

        for (int i = 0; i < 4; i++)
        {
            file.Write(buffer, 256);
        }

        for (int y = 239; y >= 0; y--)
        {
            for (int x = 1; x < 320; x++)
            {
                buffer[x] = (uint8)Painter::GetColor(x, y);
            }

            file.Write(buffer, 320);
        }

        Warning::ShowGood(Warning::FileIsSaved);
    }
}


void FDrive::SaveAsText(pchar fileName)
{
    File file;

    const DataStruct &data = Processing::out;
    const DataSettings &ds = data.ds;

    if (file.OpenNewForWrite(fileName))
    {
        file.WriteString(String<>("points : %d", ds.PointsInChannel()));
        file.WriteString(String<>("peak det : %s", ds.peak_det ? "on" : "off"));

        file.WriteString(String<>("tbase : %s", TBase::ToString(ds.tbase)));

        file.WriteString(String<>("range 1 : %s", Range::ToName(ds.range[ChA])));
        file.WriteString(String<>("divider 1 : %d", Divider::ToAbs(ds.div_a)));
        file.WriteString(String<>("rshift 1 : %f V", RShift((int16)ds.rshiftA).ToAbs(ds.range[ChA]) * (float)Divider::ToAbs(ds.div_a)));
        
        file.WriteString(String<>("range 2 : %s", Range::ToName(ds.range[ChB])));
        file.WriteString(String<>("divider 2 : %d", Divider::ToAbs(ds.div_b)));
        file.WriteString(String<>("rshift 2 : %f V", RShift((int16)ds.rshiftB).ToAbs(ds.range[ChB]) * (float)Divider::ToAbs(ds.div_b)));

        file.WriteString(String<>("Channel 1:"));

        SaveChannel(data, ChA, file);

        file.WriteString(String<>("Channel 2:"));

        SaveChannel(data, ChB, file);

        Warning::ShowGood(Warning::FileIsSaved);
    }
}


void FDrive::SaveChannel(const DataStruct &data, Chan ch, File &file)
{
    const DataSettings &ds = data.ds;

    const uint8 *out = data.DataConst(ch).DataConst();

    int num_points = ds.PointsInChannel();

    int16 rshift = (int16)(ch.IsA() ? ds.rshiftA : ds.rshiftB);

    float divider = Divider::ToAbs(ch.IsA() ? ds.div_a : ds.div_b);

    Range::E range = ds.range[ch];

    if (ds.peak_det)
    {
        for (int i = 0; i < num_points; i++)
        {
            float value1 = ValueFPGA::ToVoltage(out[i * 2], range, rshift) * divider;
            float value2 = ValueFPGA::ToVoltage(out[i * 2 + 1], range, rshift) * divider;

            file.WriteString(String<>("%d : %f %f", i, value1, value2));
        }
    }
    else
    {
        for (int i = 0; i < num_points; i++)
        {
            float value = ValueFPGA::ToVoltage(out[i], range, rshift) * divider;

            file.WriteString(String<>("%d : %f", i, value));
        }
    }
}
