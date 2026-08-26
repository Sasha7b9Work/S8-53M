#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include <stm32f4xx_hal.h>
#include <cstring>


/*
*   Алгоритм хранения данных такой
*   В секторе 12 последовательно хранятся структуры такого вида:
*   {
*     uint addr   - адрес, по которому сохранён фрейм данных
*     uint number - номер данных, как он представлен на экране ПАМЯТЬ-ВНУТР ЗУ
*     uint size   - сколько места занимает фрейм данных (DataSettings + 2 * BytesInChannelStored()). Если 0, то запись стёрта
*   }
*   Сами данные хранятся в секторах 21, 22, 23
*   Все байты одного фрейма всегда принадлежать одному сектору
*/


// Программа и константные данные
#define ADDR_SECTOR_0               ((uint)0x08000000)  //  0 16k
#define ADDR_SECTOR_1               ((uint)0x08004000)  //  1 16k
#define ADDR_SECTOR_2               ((uint)0x08008000)  //  2 16k
#define ADDR_SECTOR_3               ((uint)0x0800C000)  //  3 16k
#define ADDR_SECTOR_NRST            ((uint)0x08010000)  //  4 64k  // SettingsNRST
#define SIZE_SECTOR_NRST            (64 * 1024)
#define END_SECTOR_NRST             (ADDR_SECTOR_NRST + SIZE_SECTOR_NRST)
#define ADDR_FIRMWARE_1             ((uint)0x08020000)  //  5 128k \-
#define ADDR_FIRMWARE_2             ((uint)0x08040000)  //  6 128k |- Основная прошивка
#define ADDR_FIRMWARE_3             ((uint)0x08060000)  //  7 128k /-
#define ADDR_SECTOR_8               ((uint)0x08080000)  //  8 128k
#define ADDR_SECTOR_9               ((uint)0x080A0000)  //  9 128k
#define ADDR_SECTOR_10              ((uint)0x080C0000)  // 10 128k
#define ADDR_SECTOR_SETTINGS        ((uint)0x080E0000)  // 11 128k // Настройки
#define SIZE_SECTOR_SETTINGS        (128 * 1024)
#define END_SECTOR_SETTINGS         (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS)

#define ADDR_SECTOR_DATA_INFO       ((uint)0x08100000)  // 12 16k  Информация о сохранённых данных
#define ADDR_SECTOR_13              ((uint)0x08104000)  // 13 16k
#define ADDR_SECTOR_14              ((uint)0x08108000)  // 14 16k
#define ADDR_SECTOR_15              ((uint)0x0810С000)  // 15 16k
#define ADDR_SECTOR_16              ((uint)0x08110000)  // 16 64k
#define ADDR_SECTOR_17              ((uint)0x08120000)  // 17 128k
#define ADDR_SECTOR_18              ((uint)0x08140000)  // 18 128k
#define ADDR_SECTOR_19              ((uint)0x08160000)  // 19 128k
#define ADDR_SECTOR_20              ((uint)0x08180000)  // 20 128k
#define ADDR_SECTOR_DATA_FIRST      ((uint)0x081A0000)  // 21 128k
#define ADDR_SECTOR_DATA_2          ((uint)0x081C0000)  // 22 128k
#define ADDR_SECTOR_DATA_LATEST     ((uint)0x081E0000)  // 23 128k


#define READ_WORD(address) (*((volatile uint*)(address)))

static const uint MAX_UINT = 0xffffffff;


#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)


namespace HAL_ROM
{
    static void WriteBufferBytes(uint address, const void *buffer, int size);

    // Возвращает true, если это первое включение
    static bool TheFirstInclusion();

    static void EraseSector(uint startAddress);

    static void WriteWord(uint address, uint);
    static void WriteWord(void *address, uint);

    static uint GetSector(uint startAddress);

    namespace Settings
    {
        struct RecordConfig
        {
            uint addrData;  // Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
            int  sizeData;  // Размер в байтах записанных данных

            static RecordConfig *ForRead();
            static RecordConfig *FirstEmpty();
            static RecordConfig *FirstRecord();
        };
    }

    namespace Data
    {
        namespace SectorInfo
        {
            // Возвращает true, если заполнены все информационные поля (сектор ADDR_SECTOR_DATA_INFO)
            bool Filled();

        }

        namespace SectorData
        {
            // Возвращает начало свободного места для хранения данных
            static uint StartFreeeSpace();

            static uint Begin()
            {
                return ADDR_SECTOR_DATA_FIRST;
            }

            static uint End()
            {
                return (ADDR_SECTOR_DATA_LATEST + 128 * 1024);
            }

            // Возвращает true, если address принадлежит сектору
            static bool BelongAddress(uint address)
            {
                return (address >= Begin()) && (address < End());
            }
        }

        struct StructInfo
        {
            uint address;   // По этому адресу сохранены данные фрейма
            int  number;    // Этот номер выводится на странице ПАМЯТЬ-ВНУТР ЗУ
            uint size;      // Размер фрема данных (DataStruct + 2 * BytesInChannelStored()). Если 0, то запись стёрта


            // Возвращает true, если структура существует и указывает на реальный объект (либо указывала, если объект был стёрт)
            bool Exist() const
            {
                return (address != MAX_UINT);
            }


            // Возвращает true, если фрейм стёрт
            bool Erased() const
            {
                return Exist() && (size == 0);
            }


            // Возвращает указатель на первый элемент DataStructROM. Первый элемент всегда находится в начале сектора ADDR_SECTOR_DATA_INFO
            static StructInfo *First()
            {
                return (StructInfo *)ADDR_SECTOR_DATA_INFO;
            }


            // Последний элемент (конец сектора ADDR_SECTOR_DATA_INFO)
            static StructInfo *Latest()
            {
                return (StructInfo *)(ADDR_SECTOR_DATA_INFO + 16 * 1024);
            }


            // Возвращает указатель на последнюю существующую структуру. nullptr, если структур не существует
            static StructInfo *LatestExist()
            {
                StructInfo *info = First();

                if (!info->Exist())
                {
                    return nullptr;
                }

                while (info->Exist())
                {
                    if (info == Latest())
                    {
                        return info;
                    }

                    info++;
                }

                return info - 1;
            }


            // Первое свободное поле. Сюда будем сохранять следующие данные
            static StructInfo *FirstEmpty()
            {
                if (!LatestExist())
                {
                    return StructInfo::First();
                }

                if (SectorInfo::Filled())
                {
                    return nullptr;
                }

                return LatestExist() + 1;
            }


            // Возвращает указатель на информацию о данных с номером num. Если отсутствует - nullptr
            static StructInfo *Get(int num)
            {
                StructInfo *info = First();

                while (info < Latest())
                {
                    if (!info->Exist())
                    {
                        return nullptr;
                    }

                    if (!info->Erased())
                    {
                        if (info->number == num)
                        {
                            return info;
                        }
                    }

                    info++;
                }

                return nullptr;
            }


            DataSettings *GetDataSettings()
            {
                return (DataSettings *)address;
            }


            uint8 *GetDataChannel(Chan ch)
            {
                DataSettings *ds = GetDataSettings();

                uint8 *address_data = (uint8 *)ds + sizeof(DataSettings);

                if (ch.IsB())
                {
                    address_data += ds->BytesInChanStored();
                }

                return address_data;
            }


            // Сохраняет данные в данную структуру. Она должна быть пустая
            void SaveData(int num, const DataStruct &data)
            {
                if (Exist())
                {
                    LOG_ERROR("Структура уже заполнена");
                    return;
                }

                if ((uint)this < ADDR_SECTOR_DATA_INFO)
                {
                    LOG_ERROR_TRACE("StructInfo вне сектора записи");
                    return;
                }

                if ((uint)this + sizeof(StructInfo) > (ADDR_SECTOR_DATA_INFO + 16 * 1024))
                {
                    LOG_ERROR_TRACE("StructInfo вне сектора записи");
                    return;
                }

                uint address_rec = SectorData::StartFreeeSpace();         // Начальный адрес записи

                if (!SectorData::BelongAddress(address_rec) || !SectorData::BelongAddress(address_rec + data.ds.SizeFrame() - 1))
                {
                    LOG_ERROR_TRACE("Данные вне сектора записи");
                    return;
                }

                WriteWord(&address, address_rec);
                WriteWord(&number, (uint)num);
                WriteWord(&size, (uint)data.ds.SizeFrame());

                int num_bytes = data.ds.BytesInChanStored();

                WriteBufferBytes(address_rec, &data.ds, sizeof(data.ds));
                address_rec += sizeof(data.ds);
                WriteBufferBytes(address_rec, data.A.DataConst(), num_bytes);
                WriteBufferBytes(address_rec + num_bytes, data.B.DataConst(), num_bytes);
            }
        };

        // true, если в области данных есть место для сохранения DataStruct
        static bool ExistPlaceToSave(const DataStruct &);
    }
}


void HAL_ROM::Data::EraseAll()
{
    EraseSector(ADDR_SECTOR_DATA_INFO);

    EraseSector(ADDR_SECTOR_DATA_FIRST);
    EraseSector(ADDR_SECTOR_DATA_2);
    EraseSector(ADDR_SECTOR_DATA_LATEST);
}


void HAL_ROM::Data::Delete(int num)
{
    StructInfo *info = StructInfo::Get(num);

    if (info)
    {
        WriteWord(&info->size, 0);
    }
}

// Если даннные есть, соответствующий элемент массива равен true/.
void HAL_ROM::Data::GetInfo(bool info[MAX_DATAS])
{
    for (int i = 0; i < MAX_DATAS; i++)
    {
        info[i] = (StructInfo::Get(i) != nullptr);
    }
}


// Возвращает true, если заполнены все информационные поля (сектор ADDR_SECTOR_DATA_INFO)
bool HAL_ROM::Data::SectorInfo::Filled()
{
    StructInfo *info = StructInfo::First();

    while (info->Exist())
    {
        info++;

        if (info == StructInfo::Latest())
        {
            return true;
        }
    }

    return false;
}


static bool HAL_ROM::Data::ExistPlaceToSave(const DataStruct &data)
{
    StructInfo *latest = StructInfo::LatestExist();

    if (!latest)
    {
        return true;
    }

    return ((SectorData::StartFreeeSpace() + data.ds.SizeFrame()) < SectorData::End());
}


static uint HAL_ROM::Data::SectorData::StartFreeeSpace()
{
    StructInfo *info = StructInfo::LatestExist();

    if (!info)
    {
        return Begin();
    }

    return info->address + info->GetDataSettings()->SizeFrame();
}


bool HAL_ROM::Data::Get(int num, DataStruct &data)
{
    StructInfo *info = StructInfo::Get(num);

    if (info)
    {
        data.ds = *info->GetDataSettings();

        int num_bytes = data.ds.BytesInChanStored();

        data.A.ReallocFromBuffer(info->GetDataChannel(ChA), num_bytes);
        data.B.ReallocFromBuffer(info->GetDataChannel(ChB), num_bytes);

        return true;
    }

    return false;
}


void HAL_ROM::Data::Save(int num, const DataStruct &data)
{
    Delete(num);

    if (!StructInfo::FirstEmpty())
    {
        EraseAll();
    }

    if (!ExistPlaceToSave(data))
    {
        EraseAll();
    }

    StructInfo *info = StructInfo::FirstEmpty();

    info->SaveData(num, data);
}


bool HAL_ROM::Settings::Save(::Settings *set)
{
    uint address = ADDR_SECTOR_SETTINGS;

    while (READ_WORD(address) != MAX_UINT)
    {
        address += ::Settings::SIZE_FIELD_RECORD;

        if (address == END_SECTOR_SETTINGS)
        {
            EraseSector(ADDR_SECTOR_SETTINGS);
            address = ADDR_SECTOR_SETTINGS;
            break;
        }
    }

    set->crc32 = set->CalculateCRC32();

    WriteBufferBytes(address, (uint8 *)set, sizeof(::Settings));

    return true;
}


bool HAL_ROM::Settings::SaveNRST(SettingsNRST *_nrst)
{
    uint address = ADDR_SECTOR_NRST;

    while (READ_WORD(address) != MAX_UINT)
    {
        address += SettingsNRST::SIZE_FIELD_RECORD;

        if (address == END_SECTOR_NRST)
        {
            EraseSector(ADDR_SECTOR_NRST);
            address = ADDR_SECTOR_NRST;
            break;
        }
    }

    _nrst->crc32 = _nrst->CalculateCRC32();

    WriteBufferBytes(address, (uint8 *)_nrst, sizeof(SettingsNRST));

    return true;
}


bool HAL_ROM::Settings::Load(::Settings *set)
{
    uint address = ADDR_SECTOR_SETTINGS;

    if (READ_WORD(address) == MAX_UINT)
    {
        return false;
    }

    while (READ_WORD(address) != MAX_UINT && address < END_SECTOR_SETTINGS)
    {
        address += ::Settings::SIZE_FIELD_RECORD;
    }

    // Здесь у нас адрес первой пустой строки

    while (true)
    {
        address -= ::Settings::SIZE_FIELD_RECORD;

        if (address < ADDR_SECTOR_SETTINGS)
        {
            return false;
        }

        ::Settings *src = (::Settings *)address;

        if (src->crc32 == src->CalculateCRC32())
        {
            *set = *src;

            return true;
        }
    }
}



bool HAL_ROM::Settings::LoadNRST(SettingsNRST *_nrst)
{
    uint address = ADDR_SECTOR_NRST;

    if (READ_WORD(address) == MAX_UINT)
    {
        return false;
    }

    while (READ_WORD(address) != MAX_UINT && address < END_SECTOR_NRST)
    {
        address += SettingsNRST::SIZE_FIELD_RECORD;
    }

    // Здесь у нас адрес первой пустой записи

    while (true)
    {
        address -= SettingsNRST::SIZE_FIELD_RECORD;

        if (address < ADDR_SECTOR_NRST)
        {
            return false;
        }

        SettingsNRST *src = (SettingsNRST *)address;

        if (src->crc32 == src->CalculateCRC32())
        {
            *_nrst = *src;

            return true;
        }
    }
}


bool HAL_ROM::TheFirstInclusion()
{
    return READ_WORD(ADDR_SECTOR_SETTINGS) == MAX_UINT;
}


void HAL_ROM::EraseSector(uint startAddress)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;

    while (HAL_FLASHEx_Erase(&flashITD, &error) != HAL_OK)
    {
    }

    HAL_FLASH_Lock();
}


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::ForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmpty();
        return --record;
    }

    return nullptr;
}


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::FirstEmpty()
{
    RecordConfig *record = FirstRecord();
    int numRecord = 0;

    while (record->addrData != MAX_UINT)
    {
        record++;
        if ((++numRecord) == SIZE_ARRAY_POINTERS_IN_ELEMENTS)
        {
            return 0;
        }
    }

    return record;
}


void HAL_ROM::WriteWord(uint address, uint word)
{
    HAL_FLASH_Unlock();

    while (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteWord(void *address, uint word)
{
    WriteWord((uint)address, word);
}


uint HAL_ROM::GetSector(uint startAddress)
{
    switch (startAddress)
    {
    case ADDR_SECTOR_NRST:          return FLASH_SECTOR_4;
    case ADDR_SECTOR_SETTINGS:      return FLASH_SECTOR_11;
    case ADDR_SECTOR_DATA_INFO:     return FLASH_SECTOR_12;
    case ADDR_SECTOR_DATA_FIRST:    return FLASH_SECTOR_21;
    case ADDR_SECTOR_DATA_2:        return FLASH_SECTOR_22;
    case ADDR_SECTOR_DATA_LATEST:   return FLASH_SECTOR_23;
    }

    LOG_ERROR_TRACE("Недопустимый сектор");

    return FLASH_SECTOR_11;
}


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::FirstRecord()
{
    return (RecordConfig *)ADDR_ARRAY_POINTERS;
}


int OTP::GetSerialNumber(int *number, int *year)
{
    uint8 *address = (uint8 *)(FLASH_OTP_BASE + 512);

    do
    {
        address -= 2;
    } while (*address == 0xff && address > (uint8 *)FLASH_OTP_BASE);

    if (*address == 0xff)   // Не нашли строки с информацией, дойдя до начального адреса OTP
    {
        *number = 0;
        *year = 0;

        return 512 / 2;
    }

    *number = (int)*address;
    *year = (int)*(address + 1);

    return (int)((FLASH_OTP_BASE + 512 - (int)address) / 2 - 1);
}


bool OTP::SaveSerialNumber(int number, int year)
{
    // Сохраняется двумя подряд идущими байтами - перый номер, второй - (год - 2000)

    uint8 *address = (uint8 *)FLASH_OTP_BASE;

    while ((*address) != 0xff &&
        address < (uint8 *)(FLASH_OTP_BASE + 512))
    {
        address += 2;
    }

    if (address < (uint8 *)(FLASH_OTP_BASE + 512 - 1))
    {
        HAL_ROM::WriteBufferBytes((uint)address, &number, 1);
        HAL_ROM::WriteBufferBytes((uint)(address + 1), &year, 1);

        return true;
    }

    return false;
}


void HAL_ROM::WriteBufferBytes(uint address, const void *buffer, int size)
{
    uint8 *bufferU8 = (uint8 *)buffer;

    HAL_FLASH_Unlock();

    for (int i = 0; i < size; i++)
    {
        while (HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(bufferU8[i])) != HAL_OK)
        {
        }
        address++;
    }

    HAL_FLASH_Lock();
}


uint SettingsNRST::CalculateCRC32()
{
    int num_words = (sizeof(*this)) / 4 - 2;      // Откидываем два слова : первое - собственно контрольная сумма, и второе - последнее (неиспользуемое) поле

    uint *address = ((uint *)this) + 1;

    return HAL_CRC32::Calculate(address, num_words);
}


uint Settings::CalculateCRC32()
{
    int num_words = (sizeof(*this)) / 4 - 2;

    uint *address = ((uint *)this) + 1;

    return HAL_CRC32::Calculate(address, num_words);
}
