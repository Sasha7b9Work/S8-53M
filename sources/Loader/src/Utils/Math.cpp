// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include <cstdlib>
#include <cstring>
#include <cmath>


const float Math::Pi = 3.14159265358979323846F;


float Math::GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine)
{
    if(y0 == y1)
    {
        return (float)(x1);
    }

    return (yHorLine - y0) / ((float)(y1 - y0) / (float)(x1 - x0)) + x0;
}

bool Math::FloatsIsEquals(float value0, float value1, float epsilonPart)
{
    float max = std::fabsf(value0) > std::fabsf(value1) ? std::fabsf(value0) : std::fabsf(value1);

    float epsilonAbs = max * epsilonPart;

    return std::fabsf(value0 - value1) < epsilonAbs;
}

float Math_MinFrom3float(float value1, float value2, float value3)
{
    float retValue = value1;
    if (value2 < retValue)
    {
        retValue = value2;
    }
    if (value3 < retValue)
    {
        retValue = value3;
    }
    return retValue;
}


float Math::RandFloat(float min, float max)
{
    float delta = max - min;
    return min + ((std::rand() / (float)RAND_MAX) * delta);
}


int8 Math_AddInt8WithLimitation(int8 value, int8 delta, int8, int8 max)
{
    int8 retValue = value + delta;

    if (retValue < 0)
    {
        return 0;
    }

    if (retValue > max)
    {
        return max;
    }

    return retValue;
}


int Math::Pow10(int pow)
{
    int retValue = 1;

    while (pow--)
    {
        retValue *= 10;
    }

    return retValue;
}


int Math::NumDigitsInNumber(int value)
{
    value = Math::FabsInt(value);
    int num = 1;

    while ((value /= 10) > 0)
    {
        num++;
    }

    return num;
}


uint8 Math::GetMinFromArray(puchar data, int numPoints)
{
    uint8 min = 255;

    while (numPoints-- > 0)
    {
        uint8 d = *data++;

        if (d < min)
        {
            min = d;
        }
    }

    return min;
}


uint8 Math::GetMaxFromArray(puchar data, int numPoints)
{
    uint8 max = 0;

    while (numPoints-- > 0)
    {
        uint8 d = *data++;

        if (d > max)
        {
            max = d;
        }
    }

    return max;
}


void Math::CalculateFiltrArray(const uint8 *dataIn, uint8 *dataOut, int num_points, int numSmoothing)
{
    if (numSmoothing < 2)
    {
        std::memcpy(dataOut, dataIn, (uint)(num_points));
    }
    else
    {
        bool addCalculation = (numSmoothing % 2) == 1;
        int endDelta = numSmoothing / 2;
        int startDelta = 1;
        int d = numSmoothing / 2;

        for (int i = 0; i < num_points; i++)
        {
            int count = 1;
            int sum = dataIn[i];

            for (int delta = startDelta; delta <= endDelta; delta++)
            {
                if (((i - delta) >= 0) && ((i + delta) < (num_points)))
                {
                    sum += dataIn[i - delta];
                    sum += dataIn[i + delta];
                    count += 2;
                }
            }

            if (addCalculation)
            {
                if ((i + d) < num_points)
                {
                    sum += dataIn[i + d];
                    count++;
                }
            }

            dataOut[i] = (uint8)(sum / count);
        }
    }
}


template<class T>
T Math::CircleAdd(T *val, T delta, T min, T max)
{
    *val += delta;

    if (*val > max)
    {
        *val = min;
    }

    return *val;
}


template<class T>
T Math::CircleSub(T *val, T delta, T min, T max)
{
    *val -= delta;

    if (*val < min)
    {
        *val = max;
    }

    return *val;
}


uint Math::ToUINT(const void *pointer)
{
#ifndef __linux__
    return (uint)pointer;
#else
    uint64 result = (uint64)pointer;
    return (uint)(result >> 32);
#endif
}
