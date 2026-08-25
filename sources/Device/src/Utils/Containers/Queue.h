// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Mutex.h"
#include <cstdlib>
#include <cstring>


// Очередь с фиксированым размером, который задаётся при создании объекта
template<typename T, int capacity>
struct Queue
{
    Queue() : pointer(0) { }

    void Clear() { pointer = 0; }

    void Push(T elem)
    {
        if (pointer < capacity)
        {
            buffer[pointer++] = elem;
        }
        else
        {
            LOG_ERROR_TRACE("буфер слишком мал");
        }
    }

    int Size() const { return pointer; }

    int Capacity() const { return capacity; }

    bool Empty() const { return (pointer == 0); }

    // Возвращает самый старый положенный элемент
    T Back()
    {
        if (pointer == 0)
        {
            return T(0);
        }

        if (pointer == 1)
        {
            pointer = 0;
            return buffer[0];
        }

        T result = buffer[0];

        pointer--;

        std::memmove(&buffer[0], &buffer[1], sizeof(T) * pointer);

        return result;
    }

    // Возвращает самый новый положенный элемент
    T Front()
    {
        if (pointer == 0)
        {
            return T(0);
        }

        return buffer[--pointer];
    }

    Mutex mutex;

private:

    T buffer[capacity];
    int pointer;                        // Здесь позиция элемента, в который будет производиться сохранение
};
