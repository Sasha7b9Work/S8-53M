// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"


namespace Display
{
    const int WIDTH = 320;
    const int HEIGHT = 240;

    static const int DELTA = 5;

    extern uint8 *const back_buffer;

    extern bool framesElapsed;

    extern int topMeasures;         // Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - чтобы 
                                    // они ничего не перекрывали

    void Init();

    void Update();

    void RotateRShift(Chan);

    void RotateTrigLev();

    void SwitchTrigLabel(bool enable);

    void SetDrawMode(DrawMode::E, pFuncVV = nullptr);

    void Clear();

    void ShiftScreen(int delta);

    // После отрисовки очередного экрана эта функция будет вызвана один раз.
    void RunAfterDraw(pFuncVV func);
};
