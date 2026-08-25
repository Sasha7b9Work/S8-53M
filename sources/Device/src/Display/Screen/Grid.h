// 2022/02/11 17:44:39 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


namespace Grid
{
    int Left();
    int Right();
    int Width();
    float DeltaY();
    float DeltaX();

    int WidthInCells();

    int FullBottom();
    int FullHeight();
    int FullCenterHeight();

    int ChannelBottom();
    int ChannelHeight();
    int ChannelCenterHeight();

    int MathHeight();
    int MathBottom();
    int MathTop();

    int BottomMessages();

    // Нарисовать полную сетку.
    void Draw();
};
