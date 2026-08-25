// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Menu/MenuItems.h"
#include "common/Panel/Controls.h"


// Здесь хранятся итемы, соответстующие фанкцинональным кнопкам
struct ItemsUnderKey
{
    static void Set(Key::E, Item *);
    static Item *Get(Key::E);
    static void Reset();
private:
    static Item *items[Key::Count];
};


namespace Menu
{
    extern bool showDebugMenu;
    extern bool needClosePageSB;    // Если 1, нужно закрывать страницу малых кнопок

    void Draw();

    // Функция должна вызываться в главном цикле.
    void UpdateInput();

    // Отобразить/скрыть меню.
    void Show(bool);

    // Установить время автоматического сокрытия меню в соответствии с установками.
    void SetAutoHide(bool active);

    // Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
    String<> StringNavigation();

    // Отображено ли меню на экране.
    bool IsShown();

    // Если true - меню находится в дополнительном режиме.
    bool IsMinimize();

    // Возвращает функцию обработки длинного нажатия на элемент меню item.
    void ExecuteFuncForLongPressureOnItem(Item *);

    // Возвращает функцию обработки короткого нажатия на элемент меню item.
    void ExecuteFuncForShortPressOnItem(Item *);

    namespace AdditionPage
    {
        // Устанавливает страницу для отрисовки. При этом отрисовываться будет только эта страница, у которой нет родителя, но не основное меню
        void Set(const Page *);

        // Удаляет страницу для отрисовки
        void Remove();

        extern const Page *current;
    }
};
