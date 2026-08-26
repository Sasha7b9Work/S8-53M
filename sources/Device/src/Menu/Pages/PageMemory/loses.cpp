#include "defines.h"
#include "Display/font/Font.h"
#include "Settings/Settings.h"
#include "Utils/Text/Text.h"


// Здесь "лишние" функции - те, которые были в оригинале, но ссылки на них исчезли


void DrawSB_MemLastSelect(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 3, y + 2, gset.memory.isActiveModeSelect ? '\x2a' : '\x28');
    Font::Set(TypeFont::_8);
}


void PressSB_MemLastSelect()
{
    gset.memory.isActiveModeSelect = !gset.memory.isActiveModeSelect;
}


void DrawSB_MemExtNewFolder(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 1, y, '\x46');
    Font::Set(TypeFont::_8);
}
