// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Measures.h"
#include "Utils/Math.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/Screen/Grid.h"
#include "Hardware/Sound.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Text/Text.h"
#include <cstdio>


namespace Measures
{
    struct StructMeasure
    {
        pchar name;
        const char UGO;
    };

    const StructMeasure measures[Measure::Count] =
    {
        {"",            '\x00'},
        {"U макс",      '\x20'},
        {"U мин",       '\x25'},
        {"U пик",       '\x2a'},
        {"U макс уст",  '\x40'},
        {"U мин уст",   '\x45'},
        {"U ампл",      '\x4a'},
        {"U ср",        '\x60'},
        {"U скз",       '\x65'},
        {"Выброс+",     '\x6a'},
        {"Выброс-",     '\x80'},
        {"Период",      '\x85'},
        {"Частота",     '\x8a'},
        {"Вр нараст",   '\xa0'},
        {"Вр спада",    '\xa5'},
        {"Длит+",       '\xaa'},
        {"Длит-",       '\xc0'},
        {"Скважн+",     '\xc5'},
        {"Скважн-",     '\xca'},
        {"Задержка\xa7",'\xe0'},
        {"Задержка\xa6",'\xe5'},
        {"Фаза\xa7",    '\xe0'},
        {"Фаза\xa6",    '\xe5'}
    };


    int posActive = 0;                  // Позиция активного измерения (на котором курсор)
    int posOnPageChoice = 0;            // Позиция курсора на странице выбора измерения

    void GetActive(int *row, int *col);
    void SetActive(int row, int col);
    char GetChar(Measure::E);
}


bool Measures::IsActive(int row, int col)
{
    if(posActive >= Measures::NumCols() * Measures::NumRows())
    {
        posActive = 0;
    }

    return (row * Measures::NumCols() + col) == posActive;
}


void Measures::GetActive(int *row, int *col)
{
    *row = posActive / Measures::NumCols();
    *col = posActive - (*row) * Measures::NumCols();
}


void Measures::SetActive(int row, int col)
{
    posActive = row * Measures::NumCols() + col;
}


char Measures::GetChar(Measure::E measure)
{
    return measures[measure].UGO;
}

int Measures::GetDY()
{
    if(MEAS_SOURCE_IS_A_B && SET_ENABLED_A && SET_ENABLED_B)
    {
        return 30;
    }
    return 21;
}

int Measures::GetDX()
{
    return GRID_WIDTH / 5; 
}

pchar  Measures::Name(int row, int col)
{
    int numMeasure = row * Measures::NumCols() + col;
    return measures[MEASURE(numMeasure)].name;
}

Measure::E Measures::Type(int row, int col)
{
    int numMeasure = row * Measures::NumCols() + col;
    return MEASURE(numMeasure);
}

int Measures::GetTopTable()
{
    if(MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2)
    {
        return GRID_BOTTOM - Measures::GetDY() * 6;
    }
    return GRID_BOTTOM - Measures::NumRows() * Measures::GetDY();
}

int Measures::NumCols()
{
    static const int cols[] = {1, 2, 5, 5, 5, 1, 2};
    return cols[MEAS_NUM];
}

int Measures::NumRows()
{
    static const int rows[] = {1, 1, 1, 2, 3, 6, 6};
    return rows[MEAS_NUM];
}

int Measures::GetDeltaGridLeft()
{
    if(SHOW_MEASURES && MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if(MEAS_NUM_IS_6_1)
        {
            return Measures::GetDX();
        }
        else if(MEAS_NUM_IS_6_2)
        {
            return Measures::GetDX() * 2;
        }
    }
    return 0;
}

int Measures::GetDeltaGridBottom()
{
    if(SHOW_MEASURES && MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if(MEAS_NUM_IS_1_5)
        {
            return Measures::GetDY();
        }
        else if(MEAS_NUM_IS_2_5)
        {
            return Measures::GetDY() * 2;
        }
        else if(MEAS_NUM_IS_3_5)
        {
            return Measures::GetDY() * 3;
        }
    }
    return 0;
}

void Measures::RotateRegSet(int angle)
{
    Math::Limitation<int>(&angle, -1, 1);

    if (PageMeasures::choiceMeasuresIsActive)
    {
        posOnPageChoice += angle;

        if (posOnPageChoice < 0)
        {
            posOnPageChoice = Measure::Count - 1;
        }
        else if (posOnPageChoice == Measure::Count)
        {
            posOnPageChoice = 0;
        }

        MEASURE(posActive) = (Measure::E)posOnPageChoice;
        Color::ResetFlash();
    }
    else
    {
        int row = 0;
        int col = 0;
        GetActive(&row, &col);
        col += angle;

        if (col < 0)
        {
            col = Measures::NumCols() - 1;
            row--;
            if (row < 0)
            {
                row = Measures::NumRows() - 1;
            }
        }
        else if (col == Measures::NumCols())
        {
            col = 0;
            row++;
            if (row >= Measures::NumRows())
            {
                row = 0;
            }
        }

        SetActive(row, col);
    }
}


void Measures::ShorPressOnSmallButtonSettings()
{
    PageMeasures::choiceMeasuresIsActive = !PageMeasures::choiceMeasuresIsActive;
    if(PageMeasures::choiceMeasuresIsActive)
    {
        posOnPageChoice = MEASURE(posActive);
    }
}


void Measures::ShortPressOnSmallButonMarker()
{
    if(MEASURE_IS_MARKED(posActive))
    {
        MEAS_MARKED = Measure::None;
    }
    else
    {
        MEAS_MARKED = MEASURE(posActive);
    }
}


// Нарисовать страницу выбора измерений.
void Measures::DrawPageChoice()
{
    if(!PageMeasures::choiceMeasuresIsActive)
    {
        return;
    }

    bool num61or62 = MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2;

    int x = num61or62 ? (Grid::Right() - 3 * GRID_WIDTH / 5) : Grid::Left();
    int y = GRID_TOP;
    int dX = GRID_WIDTH / 5;
    int dY = 22;
    int maxRow = num61or62 ? 8 : 5;
    int maxCol = num61or62 ? 3 : 5;
    Measure::E meas = Measure::None;
    Font::Set(TypeFont::UGO);

    for(int row = 0; row < maxRow; row++)
    {
        for(int col = 0; col < maxCol; col++)
        {
            if(meas >= Measure::Count)
            {
                break;
            }

            int x0 = x + col * dX;
            int y0 = y + row * dY;
            bool active = meas == posOnPageChoice;
            Rectangle(dX, dY).Draw(x0, y0, COLOR_FILL);
            Region(dX - 2, dY - 2).Fill(x0 + 1, y0 + 1, active ? Color::FLASH_10 : COLOR_BACK);
            Color::SetCurrent(active ? Color::FLASH_01 : COLOR_FILL);
            Text::Draw10SymbolsInRect(x0 + 2, y0 + 1, GetChar(meas));
            Font::Set(TypeFont::_5);
            String<>(measures[meas].name).DrawRelativelyRight(x0 + dX, y0 + 6, active ? Color::FLASH_01 : COLOR_FILL);
            Font::Set(TypeFont::UGO);
            ++meas;
        }
    }
    Font::Set(TypeFont::_8);
}


Measure::E& operator++(Measure::E &measure)
{
    measure = (Measure::E)((int)measure + 1);
    return measure;
}
