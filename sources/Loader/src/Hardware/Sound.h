// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


namespace Sound
{
    void Init();

    void ButtonPress();

    // Функция вызовет звук отпускаемой кнопки только если перед этим проигрывался звук нажатия кнопки.
    void ButtonRelease();

    void GovernorChangedValue();

    void RegulatorShiftRotate();

    void RegulatorSwitchRotate();

    void WarnBeepBad();

    void WarnBeepGood();

    // Эту функцию надо вызывать перед записью/стиранием ППЗУ. Звук конфликтует с ППЗУ.
    void WaitForCompletion();

    // Звук включения
    void DeviceEnabled();
};
