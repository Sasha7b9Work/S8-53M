// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include <ff.h>
#include "Display/Display.h"
#include "Hardware/FDrive/FDrive.h"


#define FILE_NAME "S8-53.bin"
#define ENABLE_UPDATE
    

// Key configuration
#define KEY_START_ADDRESS   (uint)0x0
#define KEY_PAGE_NUMBER     20
#define KEY_VALUE           0xAAAA5555

// Flash configuration

#define MAIN_PROGRAM_PAGE_NUMBER    21
#define NUM_OF_PAGES                256
#define FLASH_PAGE_SIZE             2048

#define TIME_WAIT   2000   // Время работы заставки


struct State { enum E {
    NoDrive,            // Стартовое значение. Если оно, то не обнаружена флешка
    DriveDetected,      // Исходное состояние
    DriveNeedMount,     // Требуется монтирование флешки
    DriveIsMounted,     // Флешка примонтирована
    WrongDrive,         // Флешка есть, но прочитать нельзя
    FileNotFound,       // Если диск примонтирован, но обновления на нём нету
    EraseSectors,       // Происходит стирание секторов
    UpdateInProgress,   // Идёт процесс обновления
    UpdateIsFinished    // Обновление удачно завершено
};};


struct MainStruct
{
    static float percentUpdate;
    static State::E state;

    static int sizeFirmware;        // Размер прошивки
    static int sizeUpdated;         // Обновлено байт
    static int speed;               // Скорость обновления в байтах/секунду
    static int timeLeft;            // Осталось времени
};
