// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "common/Panel/Controls.h"


namespace Keyboard
{
    void Init();

    void Update();

    namespace Buffer
    {
        // Возвращает true, если буфер пуст
        bool Empty();

        int NumEvents();

        // Возвращает следующий орган управления, если таковой имеется
        KeyboardEvent GetNextEvent();
    };
};
