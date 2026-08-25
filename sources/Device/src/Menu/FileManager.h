// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once 
#include "defines.h"
#include "Utils/Text/String.h"


// Вызывается один раз при обнаружении новой флешки.
namespace FM
{
    extern int needRedraw;      // Если 1, то файл-менеджер нуждается в полной перерисовке
                                // Если 2, то перерисовать только каталоги
                                // Если 3, то перерисовать только файлы

    extern bool needOpen;       // Если 1, то нужно открыть файловый менеджер (сработало автоподключение)

    void Init();

    void Draw();

    void PressLevelUp();

    void PressLevelDown();

    void RotateRegSet(int angle);

    String<> GetNameForNewFile();

    void PressTab();
};
