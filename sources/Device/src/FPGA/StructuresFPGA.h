// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Mutex.h"


struct StateCalibration
{
    enum E
    {
        None,
        RShift0start,
        RShift0inProgress,
        RShift1start,
        RShift1inProgress,
        ErrorCalibration0,
        ErrorCalibration1
    };
};

struct StateWorkFPGA
{
    enum E
    {
        Stop,    // СТОП - не занимается считыванием информации.
        Wait,    // Ждёт поступления синхроимпульса.
        Work     // Идёт работа.
    };

    E value;

    StateWorkFPGA(E v = Stop) : value(v) {}

    static StateWorkFPGA::E GetCurrent() { return current; }
    static void SetCurrent(StateWorkFPGA::E v) { current = v; }

private:

    static E current;
};


struct StateFPGA
{
    bool                needCalibration;               // Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA       stateWorkBeforeCalibration;
    StateCalibration::E stateCalibration;              // Текущее состояние калибровки. Используется в процессе калибровки.
};


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // Мьютекс на чтение данных

        // Эта функция должна вызываться только для чтения первого канала. При чтении второго канала нужно использовать
        // ранее полученное значение
        // Возвращаемое значение на 1 меньше реального. Т.е. первое чтение нужно отбросить - что-то с альтерой
        uint16 CalculateAddressRead();

        // Чтение данных в основном цикле
        void DataRead();

        // Читать массив точек от first до last
        void ReadPoints(Chan, uint8 *first, const uint8 *last);

        void ReadPoints(Chan);

        void Read1024Points(uint8 buffer[1024], Chan);

        namespace P2P
        {
            // Читать одиночные точки поточечного режима в промежуточный буфер
            void ReadPoints();

            // Переписать точки из промежуточного буфера в структуру данных
            void SavePoints();
        }
    }

    namespace Launch
    {
        // Загружает в аппаратную часть пред- и после- запуски
        void Load();

        // Возвращает значение предзапуска, готовое для записи в альтеру
        uint16 PredForWrite();
    }
}
