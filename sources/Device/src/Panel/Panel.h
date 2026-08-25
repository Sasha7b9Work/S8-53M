// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once 
#include "defines.h"
#include "common/Panel/Controls.h"


class LED
{
public:

    static LED Trig;
    static LED RegSet;
    static LED ChanA;
    static LED ChanB;

    enum Type
    {
        _Trig     = 1,
        _RegSet   = 2,
        _ChannelA = 3,
        _ChannelB = 4,
    };

    LED(Type _type) : type(_type), prev_data(0xFF) { }
    void Enable();
    void Disable();
    void Switch(bool enable);

    // Если true, то не нунжно зажигать LED::Trig - произошёл автозапуск
    static bool dontFireTrig;

private:
    Type type;

    uint8 prev_data;
};


namespace Panel
{
    void Init();

    void Update();

    // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    void Disable();

    void Enable();

    // Ожидать нажатие клавиши.
    Key::E WaitPressingButton();

    void ProcessEvent(KeyboardEvent);

    // Передать даннные в мк панели управления.
    void TransmitData(uint8 data);

    // Времени прошло после последнего события
    uint TimePassedAfterLastEvent();

    namespace Data
    {
        uint16 Next();

        // Есть данные для передачи
        bool Exist();
    }

    namespace Callback
    {
        // Эта функция должна вызываться из приёмной фунции SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
