// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct TypeLED
{
    enum E
    {
        // Вкл/выкл определяется так: если выкл, то посылается код лампочки, если включить, то устанавл. старший бит
        _None,
        Trig,   // 1 / 128 + 1
        RegSet, // 2 / 128 + 2
        ChanA,  // 3 / 128 + 3
        ChanB,  // 4 / 128 + 4
        Power   // 5
    };
};


struct Key { enum E
{                                   // Кнопки
        None,       // 
        F1,         // 1
        F2,         // 2
        F3,         // 3
        F4,         // 4
        F5,         // 5
        ChannelA,   // КАНАЛ 1
        ChannelB,   // КАНАЛ 2
        Time,       // РАЗВ
        Trig,       // СИНХР
        Cursors,    // КУРСОРЫ
        Measures,   // ИЗМЕР
        Display,    // ДИСПЛЕЙ
        Help,       // ПОМОЩЬ
        Start,      // ПУСК/СТОП
        Memory,     // ПАМЯТЬ
        Service,    // СЕРВИС
        Menu,       // МЕНЮ
        Power,      // ВЫКЛ
                                    // Ручки
        RangeA,     // ВОЛЬТ/ДЕЛ 1
        RangeB,     // ВОЛЬТ/ДЕЛ 2
        RShiftA,    // 
        RShiftB,    // 
        TBase,      // ВРЕМЯ/ДЕЛ
        TShift,     // 
        TrigLev,    // УРОВЕНЬ
        Setting,    // УСТАНОВКА
        Count
    } value;

    Key(E v = None) : value(v) { }

    bool IsGovernor() const;
    static bool Is(E);
    bool IsFunctional() const;
    static bool IsFunctional(E);
    pchar Name() const;
    operator int() const { return (int)value; }
    operator E() const { return value; }
};

Key::E& operator++(Key::E &right);


struct Action { enum E {
        Down,       // Нажатие кнопки
        Up,         // Отпускание кнопки
        Long,       // "Длинное" нажатие
        Left,       // Поворот ручки влево
        Right,      // Поворот ручки вправо
        Count
    } value;

    Action(E v = Count) : value(v) {}
    Action(uint8 v) : value((E)v) {}

    bool IsDown() const { return value == Down; }
    bool IsUp() const { return value == Up; }
    bool IsLong() const { return value == Long; }
    bool IsLeft() const { return value == Left; }
    bool IsRight() const { return value == Right; }
    bool IsActionButton() const { return (value == Down) || (value == Up) || (value == Long); }
    pchar Name() const;
};


struct KeyboardEvent
{
    KeyboardEvent(Key::E k = Key::Count, Action::E a = Action::Count) :key(k), action(a) {}
    KeyboardEvent(const uint8 *buffer);

    Key key;
    Action action;

    bool Is(Key::E c) const           { return (c == key.value); };
    bool Is(Key::E c, Action::E a) const { return (key.value == c) && (action.value == a); };
       
    bool IsUp() const   { return (action.value == Action::Up); }
    bool IsDown() const { return (action.value == Action::Down); }
    bool IsLong() const { return (action.value == Action::Long); }
    // Возвращает true в случае отпускания кнопки или "длинного" нажатия
    bool IsRelease() const;

    // true, если функциональная клавиша
    bool IsFunctional() const;

    bool IsRotate() const { return (action.value == Action::Right) || (action.value == Action::Left); }
    bool IsLeft() const { return action.value == Action::Left; }
    bool IsRight() const { return action.value == Action::Right; }

    void Log() const;
    
    bool operator==(const KeyboardEvent &rhl) const
    {
        return (rhl.key.value == key.value) && (rhl.action.value == action.value);
    }
};
