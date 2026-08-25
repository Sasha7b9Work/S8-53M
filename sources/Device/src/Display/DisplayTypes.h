// 2022/02/11 17:44:28 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


#define SCREEN_WIDTH            320
#define SCREEN_WIDTH_IN_BYTES   (320 / 8)
#define SCREEN_HEIGHT           240
#define SCREEN_HEIGHT_IN_BYTES  (240 / 8)
#define SIZE_BUFFER_FOR_SCREEN  ((SCREEN_WIDTH) * (SCREEN_HEIGHT) / 8)

#define GRID_DELTA              20
#define GRID_TOP                19
#define GRID_CELLS_IN_HEIGHT    10
#define GRID_HEIGHT             ((GRID_CELLS_IN_HEIGHT) * (GRID_DELTA))
#define GRID_BOTTOM             ((GRID_TOP) + (GRID_HEIGHT))
#define GRID_WIDTH              (280)

#define GRID_RIGHT              ((Grid::Left()) + (Grid::Width()))

#define CUR_WIDTH               6
#define CUR_HEIGHT              8

#define MP_Y                    ((GRID_TOP) + 1)
#define MP_TITLE_WIDTH          79
#define MP_X                    ((GRID_RIGHT) - (MP_TITLE_WIDTH) - 1)
#define MP_TITLE_HEIGHT         34
#define MI_WIDTH                ((MP_TITLE_WIDTH) + 1)
#define MI_HEIGHT_VALUE         13
#define MI_WIDTH_VALUE          ((MI_WIDTH) - 4)
#define MOSI_HEIGHT             14
#define MOI_HEIGHT_TITLE        19
#define MOI_WIDTH               MP_TITLE_WIDTH
#define MOI_WIDTH_D_IP          34      /* Увеличение ширины открытого ip-адреса в случае наличия порта */
#define WIDTH_SB                19
#define LEFT_SB                 299 

struct DrawMode
{
    enum E
    {
        Normal,         // Стандартный режим отрисовки
        Timer,          // В этом режиме устанавливаемая функция отрисовки вызывается автоматически по таймеру через каждые 40мс
        Hand            // В этом режиме при вызове Display::Update() вызывается установленная функция рисования взамен стандартной
    };
};


enum Command
{
    SET_COLOR                 = 1,    //  *  1 numColor:1
    FILL_REGION               = 2,    //  *  2 x:2 y:1 width:2 height:1
    END_SCENE                 = 3,    //  *  3
    DRAW_HLINE                = 4,    //  *  4 y:1 x0:2 x1:2
    DRAW_VLINE                = 5,    //  *  5 x:2 y0:1 y1:1
    SET_POINT                 = 6,    //     6 x:2 y:1
    DRAW_SIGNAL_LINES         = 7,    //     7 x:2 points:281
    DRAW_TEXT                 = 8,    //   
    SET_PALETTE               = 9,    //  *  9 numColor:1 colorValue:2
    SET_FONT                  = 10,   //     10 numFont:1
    DRAW_VLINES_ARRAY         = 11,   //     13 x0:2 numLines:1 lines:numLines * 2
    DRAW_SIGNAL_POINTS        = 12,   //     14 x:2 points:281
    DRAW_MULTI_HPOINT_LINES   = 13,   //     17 numLines:1 x:2 numPoints:1 dx:1 lines:numLines
    DRAW_MULTI_VPOINT_LINES   = 14,   //     18 numLines:1 y:1 numPoints:1 dy:1 empty:1 lines:numLines*2
    DRAW_CHAR                 = 15,
    NUM_COMMANDS
};
