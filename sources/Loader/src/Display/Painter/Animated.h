// 2021/07/02 9:34:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct DirectionRotate { enum E {
    Left,
    Right
};};


class ABorder
{
public:

    ABorder(int width, int height, DirectionRotate::E rotate);

    void Draw(int x, int y);

    int GetPerimeter();

    BitSet64 CalculateCoord(int value);

private:

    int width;
    int height;
    DirectionRotate::E rotate;
};
