// 2022/2/11 19:47:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines.h"


namespace HelpContent
{
    void Draw();
    void NextParagraph();
    void PrevParagraph();
    void EnterParagraph();
    void LeaveParagraph();
    bool LeaveParagraphIsActive();
    bool EnterParagraphIsActive();
}
