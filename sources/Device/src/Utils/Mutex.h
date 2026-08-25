// 2021/03/16 10:48:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


//  ласс дл€ защиты данных от прерываний
class Mutex
{
public:

    Mutex() : locked(false) {}

    // ¬ызываетс€ в основном цикле перед действи€ с данными, конкурирующими с прерыванием
    void Lock()
    {
        locked = true;
    };

    // ¬ызываетс€ в основном цикле после действи€ с данными, конкурирующими с прерывани€ми
    void Unlock()
    {
        locked = false;
    };

    // «десь прерывание может проверить, зан€т ли мьютекс
    bool Locked() const
    {
        return locked;
    };

private:

    bool locked;
};
