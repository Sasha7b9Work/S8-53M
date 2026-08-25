// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "LEDS.h"
#include "HAL.h"
#include "Keyboard.h"
#include <limits>


struct Buffer
{
    static void AppendEvent(Key, Action);
    static KeyboardEvent commands[10];
    static int pointer;
    static bool Empty() { return (pointer == 0); }
    static KeyboardEvent GetNextEvent()
    {
        KeyboardEvent event;
    
        if (Empty())
        {
            event.key = Key::None;
        }
        else
        {
            event = commands[0];
            for (int i = 1; i < pointer; i++)
            {
                commands[i - 1] = commands[i];
            }
            --pointer;
        }
    
        return event;
    }
};

KeyboardEvent Buffer::commands[10];
int Buffer::pointer = 0;


#define NUM_SL 6
#define NUM_RL 8    


struct KeyStruct
{
    KeyStruct(Key::E k) : key(k), timePress(0U), happendLongPressed(false) { }

    bool IsValid() const { return (key != Key::None); }

    void Process(uint time, bool pressed);

    Key::E key;
    uint   timePress;               // Время нажатия кнопки. Если 0 - кнопка не нажата
    bool   happendLongPressed;      // Если true, то случилось длинное нажатие кнопки
};


static KeyStruct keys[NUM_SL][NUM_RL] =     
//    RL0            RL1            RL2           RL3           RL4          RL5           RL6           RL7
    {{Key::ChannelA, Key::None,     Key::None,    Key::RangeA,  Key::None,   Key::None,    Key::RShiftA, Key::Menu},  // SL0
     {Key::ChannelB, Key::None,     Key::None,    Key::RangeB,  Key::None,   Key::None,    Key::RShiftB, Key::F1},    // SL1
     {Key::Time,     Key::None,     Key::None,    Key::TBase,   Key::None,   Key::None,    Key::TShift,  Key::F2},    // SL2
     {Key::Trig,     Key::None,     Key::None,    Key::TrigLev, Key::Start,  Key::None,    Key::None,    Key::F3},    // SL3
     {Key::Cursors,  Key::Measures, Key::Display, Key::Help,    Key::Memory, Key::Service, Key::None,    Key::F4},    // SL4
     {Key::Power,    Key::None,     Key::None,    Key::Setting, Key::None,   Key::None,    Key::None,    Key::F5}};   // SL5

static Pin *sls[NUM_SL] = { &pinSL0, &pinSL1, &pinSL2, &pinSL3, &pinSL4, &pinSL5 };
static Pin *rls[NUM_RL] = { &pinRL0, &pinRL1, &pinRL2, &pinRL3, &pinRL4, &pinRL5, &pinRL6, &pinRL7 };

#define SET_SL(n)       sls[n]->On();
#define RESET_SL(n)     sls[n]->Off();
#define READ_RL(n)      rls[n]->Read()


struct GovernorStruct
{
    GovernorStruct(Key::E k, uint8 rlA_, uint8 rlB_, uint8 sl_) :
        key(k), rlA(rlA_), rlB(rlB_), sl(sl_), prevStateIsSame(false), prevState(false), nextTime(0) { }
    void Process();

private:
    void SendEvent(Key key, Action action);

    Key::E key;
    uint8  rlA;
    uint8  rlB;
    uint8  sl;
    bool   prevStateIsSame;      // true, если предыдущие состояния одинаковы
    bool   prevState;            // В этом состоянии оба сигнала находились в прошлый совместынй раз
    uint   nextTime;             // Время, следующей обработке. Используется для исключения дребезга
    static const int dT = 10;
};

#define NUM_GOVERNORS 8

static GovernorStruct governors[NUM_GOVERNORS] =
{
    GovernorStruct(Key::RangeA,  1, 2, 0),
    GovernorStruct(Key::RangeB,  1, 2, 1),
    GovernorStruct(Key::RShiftA, 4, 5, 0),
    GovernorStruct(Key::RShiftB, 4, 5, 1),
    GovernorStruct(Key::TBase,   1, 2, 2),
    GovernorStruct(Key::TShift,  4, 5, 2),
    GovernorStruct(Key::TrigLev, 1, 2, 3),
    GovernorStruct(Key::Setting, 1, 2, 5)
};


void Keyboard::Init()
{
    for (int i = 0; i < NUM_SL; i++)
    {
        SET_SL(i);
    }

    do 
    {
        Update();
        Buffer::GetNextEvent();

    } while (!Buffer::Empty());

    LED::DisableAll();
}


void Keyboard::Update()
{
    uint time = TIME_MS;

    for (int sl = 0; sl < NUM_SL; sl++)
    {
        RESET_SL(sl);

        for (int rl = 0; rl < NUM_RL; rl++)
        {
            keys[sl][rl].Process(time, (READ_RL(rl) == 0));
        }

        SET_SL(sl);
    }

    for (int i = 0; i < NUM_GOVERNORS; i++)
    {
        governors[i].Process();
    }
}


void KeyStruct::Process(uint time, bool pressed)
{
    if (IsValid())
    {
        if (timePress != 0 && !happendLongPressed)          // Если клавиша находится в нажатом положении
        {
            uint delta = time - timePress;
            if (delta > 500)                                // Если прошло более 500 мс с момента нажатия -
            {
                happendLongPressed = true;
                Buffer::AppendEvent(key, Action::Long);     // это будет длинное нажатие
            }
            else if (delta > 100 &&                         // Если прошло более 100 мс с момента нажатия
                !pressed)                                   // и сейчас кнопка находится в отжатом состоянии
            {
                timePress = 0;                              // То учитываем это в массиве
                Buffer::AppendEvent(key, Action::Up);       // И сохраняем отпускание кнопки в буфере команд
            }
        }
        else if (pressed && !happendLongPressed)            // Если кнопка нажата
        {
            timePress = time;                               // то сохраняем время её нажатия
            Buffer::AppendEvent(key, Action::Down);
        }
        else if (!pressed && happendLongPressed)
        {
            timePress = 0;
            happendLongPressed = false;
        }
    }
}


void GovernorStruct::Process()
{
    if (TIME_MS < nextTime)
    {
        return;
    }

    RESET_SL(sl);

    bool stateA = (READ_RL(rlA) != 0);
    bool stateB = (READ_RL(rlB) != 0);

    SET_SL(sl);

    if (stateA == stateB)
    {
        prevStateIsSame = true;
        prevState = stateA;
    }
    else if (prevStateIsSame && stateA && !stateB)
    {
        SendEvent(key, prevState ? Action::Left : Action::Right);
    }
    else if (prevStateIsSame && !stateA && stateB)
    {
        SendEvent(key, prevState ? Action::Right : Action::Left);
    }
}


void GovernorStruct::SendEvent(Key _key, Action action)
{
    static KeyboardEvent event;
    static uint prevTime = 0;

    if (event.key.value == _key.value)        // Не будем посылать событие, если это событие - вращение последней
    {                                   // вращаемой ручки менее чем через 50 мс в обратном направлении
        if (TIME_MS - prevTime < 100)
        {
            if (
                ((event.action.value == Action::Left) && action.IsRight())
                ||
                ((event.action.value == Action::Right) && action.IsLeft())
                )
            {
                return;
            }
        }
    }

    {
        event.key = _key.value;
        event.action = action.value;
        prevTime = TIME_MS;
    }

    Buffer::AppendEvent(_key, action);
    prevStateIsSame = false;
    nextTime = TIME_MS + dT;
}


// Возвращает true, если событие нужно игнорировать
static bool IgnoreEvent(Key key, Action action)
{
    static int ignored = 0;

    if (key == Key::Service)
    {
        if (action.IsDown())
        {
            ignored = 0;
        }
    }

    if (key == Key::RShiftA || key == Key::RShiftB || key == Key::TShift)
    {
        if (action.IsLeft() || action.IsRight())
        {
            ignored++;

            if (ignored < 3)
            {
                return true;
            }
        }
    }

    return false;
}


void Buffer::AppendEvent(Key key, Action action)
{
    if (IgnoreEvent(key, action))
    {
        return;
    }

    if (key.value == Key::Power && !action.IsDown())
    {
        return;
    }

    commands[pointer].key = key.value;
    commands[pointer].action = action.value;
    pointer++;
}


bool Keyboard::Buffer::Empty()
{
    return ::Buffer::Empty();
}


int Keyboard::Buffer::NumEvents()
{
    return ::Buffer::pointer;
}


KeyboardEvent Keyboard::Buffer::GetNextEvent()
{
    return ::Buffer::GetNextEvent();
}
