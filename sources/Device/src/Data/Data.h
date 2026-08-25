// 2022/03/16 08:24:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "FPGA/SettingsFPGA.h"
#include "Utils/Containers/Buffer.h"


struct DataStruct;


struct PackedTime
{
    uint hours   : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year    : 7;
    uint month   : 4;
    uint day     : 5;
    PackedTime() : hours(0), minutes(0), seconds(0), year(0), month(0), day(0) {}
};


struct DataSettings
{
    void                *next;                      // Адрес следующей записи.
    void                *prev;                      // Адрес предыдущей записи.
    int                 id;                         // Уникальный id для каждого фрейма, сохраняемого в Storage;
    uint                crc32;                      // Данные для контрольной суммы идут от первого байта, следующего за crc32, до последнего байта данных
                                                    // (во фрейме хранения после собственно DataSettings идут данные первого канала (их количество возвращает
                                                    // BytesInChanStored()), а потом данные второго канала
    uint                rshiftA             : 10;   // Смещение по напряжению
    uint                rshiftB             : 10;
    uint                trigLevA            : 10;   // Уровень синхронизации
    int                 tshift;                     // Смещение по времени
    ModeCouple::E       coupleB             : 3;
    Range::E            range[2];                   // Масштаб по напряжению обоих каналов.

    uint                trigLevB            : 10;
    ENUM_POINTS_FPGA::E e_points_in_channel : 3;    // Точек в канале
    TBase::E            tbase               : 6;    // Масштаб по времени
    ModeCouple::E       coupleA             : 3;    // Режим канала по входу
    uint                peak_det            : 2;    // Включен ли пиковый детектор
    Divider::E          div_a               : 2;
    Divider::E          div_b               : 2;
    uint                valid               : 1;
    PackedTime          time;

    DataSettings() : next(nullptr), prev(nullptr), id(0), crc32(0) { FillFromCurrentSettings(); valid = 0; };

    // Заполнение полей текущими настройками
    void FillFromCurrentSettings();

    void Set(const DataSettings &);

    bool Equal(const DataSettings &) const;

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeFrame() const;

    // Реальное количество байт на канал.
    int BytesInChanReal() const;

    // Количество байт, каторое занимает каждый канал при хранении в Storage. Оно должно быть кратным четырём.
    int BytesInChanStored() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;
};


struct BufferFPGA : public Buffer2048<uint8>
{
    BufferFPGA() : Buffer2048<uint8>() { }
    BufferFPGA(int size) : Buffer2048<uint8>(size) { }
    BufferFPGA(int size, uint8 value) : Buffer2048<uint8>(size, value) { }
};


// Структура хранит в себе ссылки на данные, хранящиеся в хранилище. Их нельзя изменять.
// Это такая обёртка
struct DataFrame
{
    DataSettings *ds;

    static int rec_points;
    static int all_points;

    DataFrame(DataSettings *_ds = nullptr) : ds(_ds) { };

    uint8 *BeginFrame();

    uint8 *EndFrame();

    // Начало данных канала
    uint8 *DataBegin(Chan) const;

    // Конец данных канала
    const uint8 *DataEnd(Chan);

    // Взять данные канала из BufferFPGA
    void FillDataChannelFromBuffer(Chan, BufferFPGA &);

    // Заполнить фрейм ds из DataStruct
    void FillDataChannelsFromStruct(DataStruct &);

    // Заполнить фрейм данными из фрейма
    void FillDataChannelsFromFrame(DataFrame &);
};


// Структура хранит в себе полные данные без привязки к хранилищу
struct DataStruct
{
    BufferFPGA   A;
    BufferFPGA   B;
    DataSettings ds;

    int rec_points;         // Эти точки сейчас будут записываться. Не может быть больше количества байт в канале
    int all_points;         // Всего считано точек
    bool mode_p2p;          // Выводить ли его как поточечный фрейм

    DataStruct() : rec_points(0), all_points(0), mode_p2p(false) { }

    DataStruct(const DataFrame &);

    BufferFPGA &Data(Chan ch)                  { return ch.IsA() ? A : B; }
    const BufferFPGA &DataConst(Chan ch) const { return ch.IsA() ? A : B; }

    // Подготовить данные поточечного режима к выводу в поточечном режиме
    // Возвращает позицию точки (начиная с левой границы сетки), в которой нужно выводить вертикальную линию
    int PrepareForNormalDrawP2P();

    void Log(pchar point);
};
