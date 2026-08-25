// 2021/03/01 10:48:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


namespace Math
{
    uint ToUINT(const void *pointer);

    uint8 GetMaxFromArray(puchar data, int numPoints);

    uint8 GetMinFromArray(puchar data, int numPoints);

    // Возвращает координату x пересечения линии, проходящей через (x0, y0), (x1, y1), с горизонтальной линией,
    // проходящей через точку с ординатой yHorLine.
    float GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int y_hor_line);

    // Сравнивает два числа. Возвращает true, если числа отличаются друг от друга не более, чем на epsilon_part.
    // При этом для расчёта epsilon_part используется большее в смысле модуля число.
    bool FloatsIsEquals(float val1, float val2, float epsilon_part);

    float RandFloat(float min, float max);

    inline int Sign(int value)
    {
        if (value > 0) { return 1;  }
        if (value < 0) { return -1; }
        return 0;
    }

    // Вычисляет 10**pow.
    int Pow10(int pow);

    // Вычисляет число разрядов в целом типа int.
    int NumDigitsInNumber(int value);

    // Возвращает модуль value.
    inline int FabsInt(int value)
    {
        return value >= 0 ? value : -value;
    }

    void CalculateFiltrArray(const uint8 *data_in, uint8 *data_out, int num_elements, int num_smoothing);

    template<class T> T Abs(T value)
    {
        return (value < (T)0) ? -value : value;
    }

    template<class T> void Limitation(T *value, T min, T max)
    {
        if (*value < min)      { *value = min; }
        else if (*value > max) { *value = max; }
    }

    template<class T> T Limitation(T val, T min, T max)
    {
        if (val <= min) { return min; }

        if (val >= max) { return max; }

        return val;
    }

    template<class T> void LimitAbove(T *value, T max)
    {
        if (*value > max) { *value = max; }
    }

    template<class T> void LimitBelow(T *value, T min)
    {
        if (*value < min) { *value = min; }
    }

    template<class T> T    MinFrom2(T val1, T val2)
    {
        if (val1 < val2) { return val1; }

        return val2;
    }

    template<class T> T    Max(T value1, T value2, T value3)
    {
        T result = value1;

        if (value2 > result) { result = value2; }

        if (value3 > result) { result = value3; }

        return result;
    }

    template<class T> void Sort(T *value1, T *value2)
    {
        if (*value1 > *value2)
        {
            T temp = *value1;
            *value1 = *value2;
            *value2 = temp;
        }
    }

    template<class T> bool InRange(T value, T min, T max)
    {
        if (value < min) { return false; }

        if (value > max) { return false; }

        return true;
    }

    // Обменивает местами содержимое памяти по адресам value0 и value1
    template<class T> void Swap(T *value0, T *value1)
    {
        T temp = *value0; *value0 = *value1; *value1 = temp;
    }

    // Увеличивает значение по адресу val на 1. Затем, если результат превышает max, приравинвает его к min
    template<class T> T CircleIncrease(T *val, T min, T max)
    {
        (*val)++;

        if ((*val) > max) { (*val = min); }

        return (*val);
    }

    // Умеьшает значение по адресу val на 1. Затем, если результат меньше min, приравнивает его max
    template<class T> T CircleDecrease(T *val, T min, T max)
    {
        (*val)--;

        if ((*val) < min) { (*val) = max; }

        return *val;

    }

    // Увелечивает значение по адресу val на delta. Затем, если результат больше max, приравнивает его min
    template<class T> T CircleAdd(T *val, T delta, T min, T max);

    // Уменьшает значение по адресу val на delta. Затем, елси результат меньше min, приравнивает его masx
    template<class T> T CircleSub(T *val, T delta, T min, T max);

    // Сложить add1 и add2. Если результат не вписывается в диапазон [min; max], ограничить
    template<class T> T AddWithLimitation(T add1, T add2, T min, T max)
    {
        T sum = add1 + add2;

        if (sum < min)      { return min; }
        else if (sum > max) { return max; }

        return sum;
    }

    extern const float Pi;
}
