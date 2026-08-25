// 2021/05/05 15:03:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


namespace PowerSupply
{
    // Возвращает true только в том случае, если в момент вызова произошло включение
    bool AttemptToTurnOn(const KeyboardEvent &event);

    void TurnOff();

    bool IsEnabled();
};
