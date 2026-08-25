// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Screen/Grid.h"
#include "Utils/Math.h"
#include "Hardware/Sound.h"
#include "Hardware/FDrive/FDrive.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Text/Text.h"
#include "Utils/Containers/Queue.h"
#include <cstring>


namespace FM
{
    const int RECS_ON_PAGE = 23;
    const int WIDTH_COL = 135;

    String<256> currentDir("\\");

    struct State
    {
        int first_dir;        // Номер первого выведенного каталога в левой панели. Всего может быть выведено RECS_ON_PAGE каталогов
        int cur_dir;          // Номер подсвеченного каталога
        int first_file;       // Номер первого выведенного файла в правой панели. Всего может быть выведено RECS_ON_PAGE файлов.
        int cur_file;         // Номер подсвеченного файла

        State(int v = 0) : first_dir(v), cur_dir(v), first_file(v), cur_file(v) {}
        void Clear()
        {
            first_dir = cur_dir = first_file = cur_file = 0;
        }
    };

    Queue<State, 20> queue;     // Сюда будем сохранять состояния предыдущих каталогов по мере движения по дереву каталогов вглубь

    State s;

    int numDirs = 0;
    int numFiles = 0;

    bool cursorInDirs = true;   // Если true, то ручка УСТАНОВКА перемещает по каталогам
    int  needRedraw = 1;
    bool needOpen = false;

    bool FileIsExist(FileName &);

    void DrawLongString(int x, int y, char *string, bool hightlight);

    void DrawDirs(int x, int y);

    void DrawFiles(int x, int y);

    void DecCurrentDir();

    void DecCurrentFile();

    void DrawHat(int x, int y, char *string, int num1, int num2);

    void DrawNameCurrentDir(int left, int top);

    void IncCurrentDir();

    void IncCurrentFile();
}



void FM::Init()
{
    currentDir.SetFormat("\\");
    s.first_dir = s.first_file = s.cur_dir = s.cur_file = 0;
}


void FM::DrawLongString(int x, int y, char *string, bool hightlight)
{
    int length = Font::GetLengthText(string);

    Color::E color = COLOR_FILL;
    if (hightlight)
    {
        Region(WIDTH_COL + 9, 8).Fill(x - 1, y, color);
        color = COLOR_BACK;
    }

    if (length <= WIDTH_COL)
    {
        DisplayString(string).Decode().Draw(x, y, color);
    }
    else
    {
        DisplayString(string).Decode().DrawWithLimitation(x, y, color, x, y, WIDTH_COL, 10);
        String<>("...").Draw(x + WIDTH_COL + 3, y);
    }
}


void FM::DrawHat(int x, int y, char *string, int num1, int num2)
{
    Region(WIDTH_COL + 9, RECS_ON_PAGE * 9 + 11).Fill(x - 1, y, COLOR_BACK);
    String<>(string, num1, num2).Draw(x + 60, y, COLOR_FILL);
    Painter::DrawHLine(true, y + 10, x + 2, x + 140);
}


void FM::DrawDirs(int x, int y)
{
    Directory::GetNumDirsAndFiles(currentDir.c_str(), &numDirs, &numFiles);
    DrawHat(x, y, "Каталог : %d/%d", s.cur_dir + ((numDirs == 0) ? 0 : 1), numDirs);
    char nameDir[256];
    Directory directory;
    y += 12;

    if (directory.GetFirstDir(currentDir.c_str(), s.first_dir, nameDir))
    {
        int  drawingDirs = 0;
        DrawLongString(x, y, nameDir, cursorInDirs && (s.first_dir + drawingDirs == s.cur_dir));

        while (drawingDirs < (RECS_ON_PAGE - 1) && directory.GetNextDir(nameDir))
        {
            drawingDirs++;
            DrawLongString(x, y + drawingDirs * 9, nameDir, cursorInDirs && (s.first_dir + drawingDirs == s.cur_dir));
        }
    }
}


void FM::DrawFiles(int x, int y)
{
    Directory::GetNumDirsAndFiles(currentDir.c_str(), &numDirs, &numFiles);
    DrawHat(x, y, "Файл : %d/%d", s.cur_file + ((numFiles == 0) ? 0 : 1), numFiles);
    FileName nameFile;
    Directory directory;
    y += 12;

    if (directory.GetFirstFile(currentDir.c_str(), s.first_file, nameFile))
    {
        int drawingFiles = 0;
        DrawLongString(x, y, nameFile.c_str(), !cursorInDirs && (s.first_file + drawingFiles == s.cur_file));

        while (drawingFiles < (RECS_ON_PAGE - 1) && directory.GetNextFile(nameFile))
        {
            drawingFiles++;
            DrawLongString(x, y + drawingFiles * 9, nameFile.c_str(), !cursorInDirs && (s.first_file + drawingFiles == s.cur_file));
        }
    }
}


bool FM::FileIsExist(FileName &_fileName)
{
    FileName nameFile;
    Directory::GetNumDirsAndFiles(currentDir.c_str(), &numDirs, &numFiles);
    Directory directory;

    if(directory.GetFirstFile(currentDir.c_str(), 0, nameFile))
    {
        if (std::strcmp(_fileName.Extract(), nameFile.c_str()) == 0)
        {
            return true;
        }

        while(directory.GetNextFile(nameFile))
        {
            if(std::strcmp(_fileName.Extract(), nameFile.c_str()) == 0)
            {
                return true;
            }
        }
    }

    return false;
}


void FM::DrawNameCurrentDir(int left, int top)
{
    Color::SetCurrent(COLOR_FILL);
    int length = Font::GetLengthText(currentDir.c_str());
    if (length < 277)
    {
        DisplayString(currentDir.c_str()).Decode().Draw(left + 1, top + 1);
    }
    else
    {
        char *pointer = currentDir.c_str() + 2;

        while (length > 277)
        {
            while (*pointer != '\\' && pointer < currentDir.c_str() + 255)
            {
                pointer++;
            }
            if (pointer >= currentDir.c_str() + 255)
            {
                return;
            }
            length = Font::GetLengthText(++pointer);
        }

        DisplayString(pointer).Decode().Draw(left + 1, top + 1);
    }
}


void FM::Draw()
{
    if (needRedraw == 0)
    {
        return;
    }

    int left = 1;
    int top = 1;
    int width = 297;
    int left2col = width / 2;

    if (needRedraw == 1)
    {
        Painter::BeginScene(COLOR_BACK);
        Menu::Draw();
        Rectangle(width, 239).Draw(0, 0, COLOR_FILL);
        Directory::GetNumDirsAndFiles(currentDir.c_str(), &numDirs, &numFiles);
        DrawNameCurrentDir(left + 1, top + 2);
        Painter::DrawVLine(true, left2col, top + 16, 239, COLOR_FILL);
        Painter::DrawHLine(true, top + 15, 0, width);
    }

    if (needRedraw != 3)
    {
        DrawDirs(left + 2, top + 18);
    }

    if (needRedraw != 2)
    {
        DrawFiles(left2col + 3, top + 18);
    }

    Painter::EndScene();

    needRedraw = 0;
}


void FM::PressTab()
{
    needRedraw = 1;

    if (cursorInDirs)
    {
        if (numFiles != 0)
        {
            cursorInDirs = false;
        }
    }
    else
    {
        if (numDirs != 0)
        {
            cursorInDirs = true;
        }
    }
}


void FM::PressLevelDown()
{
    needRedraw = 1;

    if (!cursorInDirs)
    {
        return;
    }

    char nameDir[256];
    Directory directory;

    if (directory.GetFirstDir(currentDir.c_str(), s.cur_dir, nameDir))
    {
        if (currentDir.Size() + std::strlen(nameDir) < 256)
        {
            currentDir.Append("\\");
            currentDir.Append(nameDir);
            queue.Push(s);
            s.Clear();
        }
    }
}


void FM::PressLevelUp()
{
    needRedraw = 1;

    if (currentDir.Size() == 1)
    {
        return;
    }

    char *pointer = currentDir.c_str() + currentDir.Size();

    while (*pointer != '\\')
    {
        pointer--;
    }

    *pointer = '\0';
    s = queue.Front();
    cursorInDirs = true;
}


void FM::IncCurrentDir()
{
    if (numDirs > 1)
    {
        s.cur_dir++;

        if (s.cur_dir > numDirs - 1)
        {
            s.cur_dir = 0;
            s.first_dir = 0;
        }

        if (s.cur_dir - s.first_dir > RECS_ON_PAGE - 1)
        {
            s.first_dir++;
        }
    }
}


void FM::DecCurrentDir()
{
    if (numDirs > 1)
    {
        s.cur_dir--;

        if (s.cur_dir < 0)
        {
            s.cur_dir = numDirs - 1;
            LIMITATION(s.first_dir, numDirs - RECS_ON_PAGE, 0, s.cur_dir);
        }

        if (s.cur_dir < s.first_dir)
        {
            s.first_dir = s.cur_dir;
        }
    }
}


void FM::IncCurrentFile()
{
    if (numFiles > 1)
    {
        s.cur_file++;

        if (s.cur_file > numFiles - 1)
        {
            s.cur_file = 0;
            s.first_file = 0;
        }

        if (s.cur_file - s.first_file > RECS_ON_PAGE - 1)
        {
            s.first_file++;
        }
    }
}


void FM::DecCurrentFile()
{
    if (numFiles > 1)
    {
        s.cur_file--;

        if (s.cur_file < 0)
        {
            s.cur_file = numFiles - 1;
            LIMITATION(s.first_file, numFiles - RECS_ON_PAGE, 0, s.cur_file);
        }

        if (s.cur_file < s.first_file)
        {
            s.first_file = s.cur_file;
        }
    }
}


void FM::RotateRegSet(int angle)
{
    if (cursorInDirs)
    {
        angle > 0 ? DecCurrentDir() : IncCurrentDir();
        needRedraw = 2;
    }
    else
    {
        angle > 0 ? DecCurrentFile() : IncCurrentFile();
        needRedraw = 3;
    }
}


String<> FM::GetNameForNewFile()
{
    int number = 1;

LabelNextNumber:

    FileName result(currentDir.c_str());
    result.Append("\\");

    int size = (int)std::strlen(FILE_NAME);
    if (size == 0)
    {
        return String<>("");
    }

    if (FILE_NAMING_MODE_IS_HAND)
    {
        LIMITATION(size, size, 1, 95);
        result.Append(FILE_NAME);
        result.Append(".");
        result.Append(MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");
    }
    else
    {
        PackedTime time = HAL_RTC::GetPackedTime();
                           //  1          2           3         4           5             6
        uint values[] = {0, time.year, time.month, time.day, time.hours, time.minutes, time.seconds};

        char *ch = FILE_NAME_MASK;

        while (*ch)
        {
            if (*ch >= 0x30)        // Если текстовый символ
            {
                result.Append(*ch);          // то записываем его в имя файла
            }
            else
            {
                if (*ch == 0x07)    // Если здесь надо записать порядковый номер
                {
                    String<> number_str = SU::Int2String(number, false, *(ch + 1));
                    result.Append(number_str);
                    ch++;
                }
                else
                {
                    if (*ch >= 0x01 && *ch <= 0x06)
                    {
                        String<> value_str = SU::Int2String((int)values[*ch], false, 2);
                        result.Append(value_str);
                    }
                }
            }
            ch++;
        }

        result.Append(MODE_SAVE_SIGNAL_IS_BMP ? ".bmp" : ".txt");

        if(FileIsExist(result))
        {
            number++;
            goto LabelNextNumber;
        }
    }

    return result;
}
