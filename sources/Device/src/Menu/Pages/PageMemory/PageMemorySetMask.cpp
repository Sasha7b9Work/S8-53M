#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "Display/Screen/Grid.h"
#include "Utils/Math.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Symbols.h"
#include "Utils/Text/Tables.h"
#include <cstring>


static void PressSB_SetMask_Exit()
{
}


static const SmallButton sbExitSetMask
(
    PageMemory::SetMask::self,
    COMMON_BEGIN_SB_EXIT,
    PressSB_SetMask_Exit,
    DrawSB_Exit
);


static void PressSB_SetMask_Delete()
{
    FILE_NAME_MASK[0] = '\0';
}


static void DrawSB_SetMask_Delete(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetMaskDelete
(
    PageMemory::SetMask::self, 0,
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered symbols",
    PressSB_SetMask_Delete,
    DrawSB_SetMask_Delete
);


static void PressSB_SetMask_Backspace()
{
    int size = (int)std::strlen(FILE_NAME_MASK);

    if (size > 0)
    {
        if (size > 1 && FILE_NAME_MASK[size - 2] == 0x07)
        {
            FILE_NAME_MASK[size - 2] = '\0';
        }
        else
        {
            FILE_NAME_MASK[size - 1] = '\0';
        }
    }

}


static void DrawSB_SetMask_Backspace(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetMaskBackspace
(
    PageMemory::SetMask::self, 0,
    "Backspace", "Backspace",
    "Удаляет последний введённый символ",
    "Deletes the last entered symbol",
    PressSB_SetMask_Backspace,
    DrawSB_SetMask_Backspace
);


static void PressSB_SetMask_Insert()
{
    int index = INDEX_SYMBOL;
    int size = (int)std::strlen(FILE_NAME_MASK);

    if (size == SettingsMemory::MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        return;
    }

    if (index < 0x41)
    {
        FILE_NAME_MASK[size] = Tables::symbolsAlphaBet[index][0];
        FILE_NAME_MASK[size + 1] = '\0';
    }
    else
    {
        index -= 0x40;

        if (index == 0x07)  // Для %nN - отдельный случай
        {
            if (size < SettingsMemory::MAX_SYMBOLS_IN_FILE_NAME - 2 && size > 0)
            {
                if (FILE_NAME_MASK[size - 1] >= 0x30 && FILE_NAME_MASK[size - 1] <= 0x39) // Если ранее введено число
                {
                    FILE_NAME_MASK[size] = FILE_NAME_MASK[size - 1] - 0x30;
                    FILE_NAME_MASK[size - 1] = 0x07;
                    FILE_NAME_MASK[size + 1] = '\0';
                }
            }
        }
        else
        {
            FILE_NAME_MASK[size] = (int8)index;
            FILE_NAME_MASK[size + 1] = '\0';
        }
    }
}


static void DrawSB_SetMask_Insert(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetMaskInsert
(
    PageMemory::SetMask::self, 0,
    "Вставить", "Insert",
    "Вставляет выбранный символ",
    "Inserts the chosen symbol",
    PressSB_SetMask_Insert,
    DrawSB_SetMask_Insert
);


static const arrayItems itemsSetMask =
{
    (void *)&sbExitSetMask,
    (void *)&sbSetMaskDelete,
    (void *)0,
    (void *)0,
    (void *)&sbSetMaskBackspace,
    (void *)&sbSetMaskInsert
};


static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    Color::SetCurrent(COLOR_FILL);

    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = Char(*ch).Draw(true, x, y);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = Char('%').Draw(true, x, y);
                x = Char((char)(0x30 | *(ch + 1))).Draw(true, x, y);
                x = Char('N').Draw(true, x, y);
                ch++;
            }
            else
            {
                x = String<>(Tables::symbolsAlphaBet[*ch + 0x40]).Draw(x, y);
            }
        }
        ch++;
        x++;
    }

    Region(5, 8).Fill(x, y, Color::FLASH_10);
}


// Эта функция рисует, когда выбран режим задания маски.
static void DrawSetMask()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Rectangle(width, height).Draw(x0, y0, COLOR_FILL);
    Region(width - 2, height - 2).Fill(x0 + 1, y0 + 1, COLOR_BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (Tables::symbolsAlphaBet[index][0] != ' ')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (Tables::symbolsAlphaBet[index][0] != 'a')
    {
        Tables::DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (Tables::symbolsAlphaBet[index][0] != '%')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    // Теперь рисуем спецсимволы
    position = 0;
    while (index < (int)(sizeof(Tables::symbolsAlphaBet) / 4))
    {
        Tables::DrawStr(index, x0 + deltaX + 26 + position * 20, y0 + deltaY0 + deltaY * 3);
        index++;
        position++;
    }

    DrawFileMask(x0 + deltaX, y0 + 65);

    static pchar strings[] =
    {
        "y - год, m - месяц, d - день",
        "H - часы, M - минуты, S - секунды",
        "nN - порядковый номер, где",
        "n - минимальное количество знаков для N"
    };

    deltaY--;
    Color::SetCurrent(COLOR_FILL);
    for (int i = 0; i < (int)sizeof(strings) / 4; i++)
    {
        String<>(strings[i]).Draw(x0 + deltaX, y0 + 100 + deltaY * i);
    }
}


void OnMemExtSetMaskNameRegSet(int angle, int maxIndex)
{
    int8(*func[3])(int8 *, int8, int8) =
    {
        Math::CircleDecrease<int8>,
        Math::CircleDecrease<int8>,
        Math::CircleIncrease<int8>
    };

    Color::ResetFlash();

    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = maxIndex - 1;
    }

    func[Math::Sign(angle) + 1](&INDEX_SYMBOL, 0, maxIndex - 1);
}


static void OnMemExtSetMaskRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(Tables::symbolsAlphaBet) / 4);
}


static const Page mspSetMask
(
    PageMemory::External::self, nullptr,
    "МАСКА", "MASK",
    "Режим ввода маски для автоматического именования файлов",
    "Input mode mask for automatic file naming",
    NamePage::SB_MemExtSetMask, &itemsSetMask, EmptyFuncVV, DrawSetMask, OnMemExtSetMaskRegSet
);


const Page *PageMemory::SetMask::self = &mspSetMask;
