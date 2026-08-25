// 2021/06/30 15:45:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Display/Colors.h"


namespace Display
{
    static const int WIDTH = 320;
    static const int HEIGHT = 240;

    void Init();

    void BeginFrame(const Color &color);

    void EndFrame();

    void Update();

    bool IsRunning();
};
