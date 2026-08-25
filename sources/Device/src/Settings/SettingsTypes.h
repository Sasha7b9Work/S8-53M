// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


// ChannelX
struct Chan 
{ 
    enum E
    {
        A,
        B,
        A_B,
        Math,
        Count = 2
    };

    E value;

    Chan(E v = A) : value(v) {}

    bool Enabled();

    // Возвращает номер канала от 1 до 2
    int ToNumber() { return (value == Chan::A) ? 1 : 2; }

    bool IsA()    const { return value == A; }
    bool IsB()    const { return value == B; }
    bool IsMath() const { return value == Math; }

    operator int() { return (int)value; }
};


#define ChA Chan::A
#define ChB Chan::B


// Делитель.
struct Divider
{
    enum E
    {
        _1,
        _10
    };

    static int ToAbs(Divider::E);
};

// Measures


// Сжимать ли сигналы при выводе измерений.
struct ModeViewSignals
{
    enum E
    {
        AsIs,       // Показывать сигналы как есть
        Compress    // Сжимать сетку с сигналами
    };
};


// Math


enum ScaleMath
{
    ScaleMath_Channel0,     // Масштаб берётся из канала 1
    ScaleMath_Channel1,     // Масштаб берётся из канала 2
    ScaleMath_Hand          // Масштаб задаётся вручную.
};


// Курсоры

// Каким курсором управлять.
struct CursCntrl
{
    enum E
    {
        _1,         // первым.
        _2,         // вторым.
        _1_2,       // обоими.
        Disable     // никаким.
    };
};


// Дискретность перемещения курсоров.
struct CursMovement
{
    enum E
    {
        Points,    // по точкам.
        Percents   // по процентам.
    };
};

// Какие курсоры сейчас активны. Какие активны, те и будут перемещаться по вращению ручки УСТАНОВКА.
struct CursActive
{
    enum E
    {
        U,
        T,
        None
    };
};

// Режим слежения курсоров.
struct CursLookMode
{
    enum E
    {
        None,      // Курсоры не следят.
        Voltage,   // Курсоры следят за напряжением автоматически.
        Time,      // Курсоры следят за временем автоматически.
        Both       // Курсоры следят за временем и напряжением, в зависимости от того, какой курсоры вращали последним.
    };
};


struct Lang
{
    enum E
    {
        Ru,    // Русский
        En     // Английский
    };
};
