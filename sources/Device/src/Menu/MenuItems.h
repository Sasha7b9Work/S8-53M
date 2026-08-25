// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Utils/Text/String.h"
#include "common/Panel/Controls.h"



#define MAX_NUM_SUBITEMS_IN_CHOICE  12  // Максимальное количество вариантов выбора в элементе Choice.
#define MAX_NUM_ITEMS_IN_PAGE       15  // Максимальное количество пунктов на странице.
#define MENU_ITEMS_ON_DISPLAY       5   // Сколько пунктов меню помещается на экране по вертикали.
#define MAX_NUM_CHOICE_SMALL_BUTTON 6   // Максимальное количество вариантов маленькой кнопки + 1


// Разные виды пунктов меню
struct TypeItem
{
    enum E
    {
        None,
        Choice,        // Пункт выбора - позволяет выбрать одно из нескольких заданных значений.
        Button,        // Кнопка.
        Page,          // Страница.
        Governor,      // Регулятор - позволяет выбрать любое целое числовое значение из заранее заданного диапазаона.
        Time,          // Позволяет ввести время.
        IP,            // Позволяет ввести IP-адрес.
        SwitchButton,
        GovernorColor, // Позволяет выбрать цвет.
        Formula,       // Позволяет выбрать знак и коэффициенты для математической формулы (умножение и сложение)
        MAC,           // MAC-адрес
        ChoiceReg,     // Элемент выбора, в котором выбор осуществляется не кнопкой, а ручкой
        SmallButton,   // Кнопка для режима малых кнопок
        Count
    };
};

// Здесь ID всех представленных в меню страниц
struct NamePage
{
    enum E
    {
        Display,               // ДИСПЕЙ
        DisplayAccumulation,   // ДИСПЛЕЙ - НАКОПЛЕНИЕ
        DisplayAverage,        // ДИСПЛЕЙ - УСРЕДНЕНИЕ
        DisplayGrid,           // ДИСПЛЕЙ - СЕТКА
        DisplayRefresh,        // ДИСПЛЕЙ - ОБНОВЛЕНИЕ
        MainPage,              // Главная страница меню. Из неё растут все остальные
        Debug,                 // ОТЛАДКА
        DebugConsole,          // ОТЛАДКА - КОНСОЛЬ
        DebugADC,              // ОТЛАДКА - АЦП
        DebugADCbalance,       // ОТЛАДКА - АЦП - БАЛАНС
        DebugADCstretch,       // ОТЛАДКА - АЦП - РАСТЯЖКА
        DebugADCshift,
        DebugADCrShift,        // ОТЛАДКА - АЦП - ДОП. СМЕЩЕНИЕ
        DebugADCaltShift,      // ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ.
        DebugShowRegisters,    // ОТЛАДКА - РЕГИСТРЫ
        DebugRandomizer,       // ОТЛАДКА - РАНД-ТОР
        DebugInformation,      // ОТЛАДКА - ИНФОРМАЦИЯ
        DebugSound,            // ОТЛАДКА - ЗВУК
        DebugAltera,           // ОТЛАДКА - АЛЬТЕРА
        Channel0,              // КАНАЛ 1
        Channel1,              // КАНАЛ 2
        Cursors,               // КУРСОРЫ
        Trig,                  // СИНХР.
        Memory,                // ПАМЯТЬ
        MemoryExt,             // ПАМЯТЬ - ВНЕШН. ЗУ
        Measures,              // ИЗМЕРЕНИЯ
        MeasuresField,         // ИЗМЕРЕНИЯ - ЗОНА
        Service,               // СЕРВИС
        ServiceDisplay,        // СЕРВИС - ДИСПЛЕЙ
        ServiceDisplayColors,  // СЕРВИС - ДИСПЛЕЙ - ЦВЕТА
        ServiceCalibrator,     // СЕРВИС - КАЛИБРАТОР
        ServiceEthernet,       // СЕРВИС - ETHERNET
        Help,                  // ПОМОЩЬ
        Time,                  // РАЗВЕРТКА
        Math,                  // СЕРВИС - МАТЕМАТИКА
        MathFFT,
        TrigAuto,              // СИНХР - АВТ ПОИСК
        SB_Curs,               // Открыта страница КУРСОРЫ - Измерения
        SB_MeasTuneMeas,       // Открыта страница ИЗМЕРЕНИЯ - Настроить
        SB_MeasTuneField,      // Открыта страница ИЗМЕРЕНИЯ - ЗОНА - Задать
        SB_MemLatest,          // Открыта страница ПАМЯТЬ - Последние
        SB_MemInt,             // Открыта страница ПАМЯТЬ - Внутр ЗУ
        SB_MemExtSetMask,      // Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - Маска
        SB_MemExtSetName,      // Появляется перед сохранением файла на флешку для задания имени файла при соответствующей опции ВНЕШН ЗУ - Имя файла
        SB_FileManager,        // Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - Обзор
        SB_Help,               // Открыта страница HELP
        SB_MathCursorsFFT,     // Открыта страница СЕРВИС - Математика - Курсоры
        SB_MathFunction,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
        SB_Information,        // СЕРВИС - ИНФОРМАЦИЯ
        SB_SerialNumber,       // ОТАДКА - С/Н
        Count,
        NoPage
    };
};             // При добавлении страниц нужно изменять инициализацию SettingsMenu в SSettings.c

class Page;
class SmallButton;

#define COMMON_PART_MENU_ITEM                                                                                     \
    TypeItem::E         type;             /* Тип итема */                                                     \
    const class Page*   keeper;           /* Адрес страницы, которой принадлежит. Для NamePage::MainPage = 0 */    \
    pFuncBV             funcOfActive;     /* Активен ли данный элемент */                                     \
    pchar               titleHint[4];     /* Название страницы на русском и английском языках. Также подсказка для режима помощи */



struct ItemStruct
{
    COMMON_PART_MENU_ITEM
};


class Item
{
public:
    TypeItem::E       type;         /* Тип итема */
    const class Page *keeper;       /* Адрес страницы, которой принадлежит. Для NamePage::MainPage = 0 */
    pFuncBV           funcOfActive; /* Активен ли данный элемент */
    pchar             titleHint[4]; /* Название страницы на русском и английском языках. Также подсказка для режима помощи */

public:
    Item(const ItemStruct *str);
    Item(TypeItem::E, const Page* keeper_, pFuncBV funcOfActive_, pchar tileRU, pchar titleEN, pchar hintRU, pchar hintEN);
    
    // Возвращает название элемента по адресу item, как оно выглядит на дисплее прибора.
    String<> Title();

    // Открыть/закрыть элемент меню по адрему item.
    void Open(bool open) const;

    // Уменьшает или увеличивает значение Governor, GovernorColor или Choice по адресу item в зависимости от знака delta
    void Change(int delta);

    bool ChangeOpened(int delta);

    // Возвращает высоту в пикселях открытого элемента Choice или NamePage по адресу item.
    int HeightOpened() const;

    // Сделать/разделать текущим пункт страницы.
    void SetCurrent(bool active) const;

    // Вовзращает true, если элемент меню item является ативным, т.е. может быть нажат.
    bool IsActive() const;

    // Возвращает true, если элемент меню по адрему item открыт.
    bool IsOpened() const;

    // Возвращает тип элемента меню по адресу address.
    TypeItem::E GetType() const;

    bool IsPage() const;
    bool IsChoice() const;
    bool IsChoiceReg() const;
    bool IsGovernor() const;
    bool IsGovernorColor() const;
    bool IsTime() const;
    bool IsIP() const;
    bool IsMAC() const;

    // Возвращает адрес элемента, которому принадлежит элемент по адресу item.
    Page *Keeper() const;

    // Возвращает true, если элемент меню item затенён (находится не на самом верхнем слое. Как правило, это означает,
    // что раскрыт раскрывающийся элемент меню вроде Choice или Governor.
    bool IsShade() const;

    // Возвращает true, если кнопка, соответствующая данному элементу меню, находится в нажатом положении.
    bool IsPressed() const;

    // Возвращает тип открытого элемента меню.
    static TypeItem::E TypeOpened();

    // Возвращает адрес текущего элемента меню (текущим, как правило, является элемент, кнопка которого была нажата последней.
    static Item *Current();

    // Если функциональная клавиша находится в нижнем положении, то здесь находится соответствующий ей элемент меню.
    // Иначе - ноль
    static Item *now_pressed;

    // Возвращает адрес открытого элемента меню.
    static Item *Opened();

    // Закрыть открытый элемент меню.
    static void CloseOpened();

    static const int HEIGHT = 33;
};


#define TITLE(item) ((item)->titleHint[LANG])
#define HINT(item) ((item)->titleHint[2 + LANG])

typedef void * pVOID;
typedef pVOID arrayItems[MAX_NUM_ITEMS_IN_PAGE];


// Описывает страницу меню.
class Page : public Item
{
public:
    NamePage::E       name;         // Имя из перечисления NamePage
    const arrayItems *items;        // Здесь указатели на пункты этой страницы (в обычной странице)
                                    // для страницы малых кнопок  здесь хранятся 6 указателей на SmallButton :
                                    // 0 - Key::Menu, 1...5 - Key::F1...Key::F5
    pFuncVV           funcOnPress;  // Будет вызываться при нажатии на свёрнутую страницу
    pFuncVV           funcOnDraw;   // Будет вызываться после отрисовки кнопок
    pFuncVI           funcRegSetSB; // В странице малых кнопок вызывается при повороте ручки установка

    Page(const Page *keeper_, pFuncBV funcOfActive_,
         pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, NamePage::E name_,
         const arrayItems *items_, pFuncVV funcOnPress_ = 0, pFuncVV funcOnDraw_ = 0, pFuncVI funcRegSetSB_ = 0);

    void DrawOpened(int yTop);

    void Draw(int x, int y);

    void DrawTitle(int yTop);

    // Возвращает позицию текущего элемента странцы page.
    int8 PosCurrentItem() const;

    void DrawItems(int yTop);

    void DrawUGO(int right, int bottom);

    bool IsSB() const;

    // Возвращает имя страницы page.
    NamePage::E GetName() const;

    // Возвращает позицию первого элемента страницы по адресу page на экране. Если текущая подстраница 0, это будет 0,
    // если текущая подстраница 1, это будет 5 и т.д.
    int PosItemOnTop();

    // Возвращает номер текущей подстранцы элемента по адресу page.
    int NumCurrentSubPage() const;

    // Возвращает количество элементов в странице по адресу page.
    int NumItems() const;

    // Dозвращает число подстраниц в странице по адресу page.
    int NumSubPages() const;

    // Возвращает адрес элемента меню заданной страницы.
    Item *GetItem(int numElement) const;

    // Возвращает true, если текущий элемент страницы с именем namePage открыт.
    bool CurrentItemIsOpened();

    void ChangeSubPage(int delta) const;

    SmallButton *GetSmallButon(int numButton);

    void OpenAndSetCurrent() const;

    // Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page;
    void ShortPressOnItem(int numItem) const;

    // Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
    static void RotateRegSetSB(int angle);

    static NamePage::E NameOpened();

    static const Page *FromName(NamePage::E);
};


// Описывает кнопку.
class Button : public Item
{
public:
    pFuncVV         funcOnPress;    // Функция, которая вызывается при нажатии на кнопку.

    Button(const Page *keeper_, pFuncBV funcOfActive_,
            pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, pFuncVV funcOnPress_);

    void Draw(int x, int y);
};


class StructHelpSmallButton
{
public:
    pFuncVII    funcDrawUGO;    // Указатель на функцию отрисовки изображения варианта кнопки
    pchar helpUGO[2];     // Подпись к данному изображению.
};

typedef StructHelpSmallButton arrayHints[MAX_NUM_CHOICE_SMALL_BUTTON];


// Описывает кнопку для дополнительного режима меню.
class SmallButton : public Item
{
public:
    pFuncVV             funcOnPress;    // Эта функция вызвается для обработки нажатия кнопки. Возвращает true, если надо за
    pFuncVII            funcOnDraw;     // Эта функция вызывается для отрисовки кнопки в месте с координатами x, y.
    const arrayHints   *hintUGO;

    SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
                pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
                pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_ = 0);

    void Draw(int x, int y);
};


// Описывает регулятор.
class Governor : public Item
{ 
public:
    int16*      cell;
    int         minValue;               // Минмальное значение, которое может принимать регулятор.
    int16       maxValue;               // Максимальное значение.
    pFuncVV     funcOfChanged;          // Функция, которую нужно вызывать после того, как значение регулятора изменилось.
    pFuncVV     funcBeforeDraw;         // Функция, которая вызывается перед отрисовкой

    Governor(const Page *keeper_, pFuncBV funcOfActive_,
             pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
             int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_ = 0, pFuncVV funcBeforeDraw_ = 0);

    void Draw(int x, int y, bool opened);

    // Запускает процессс анимации инкремента или декремента элемента меню типа Governor (в зависимости от знака delta)
    void StartChange(int detla);

    // Рассчитывате следующий кадр анимации.
    float Step();

    // Изменяет значение в текущей позиции при раскрытом элементе
    void ChangeValue(int delta);

    // Возвращает следующее большее значение, которое может принять governor.
    int16 NextValue() const;

    // Возвращает следующее меньшее значение, которое может принять governor.
    int16 PrevValue() const;

    // При открытом элементе переставляет курсор на следующую позицию
    void NextPosition();

    // Возвращает число знакомест в поле для ввода элемента governor. Зависит от максимального значения, которое может принимать governor.
    int NumDigits() const;

    void DrawLowPart(int x, int y, bool pressed, bool shade);

    static int8 cur_digit;

    static bool inMoveIncrease;
    static bool inMoveDecrease;
    static void *address;
    static uint timeStart;

private:

    void DrawOpened(int x, int y);

    void DrawClosed(int x, int y);
};


class Choice
{
public:
    COMMON_PART_MENU_ITEM
    pchar       names[MAX_NUM_SUBITEMS_IN_CHOICE][2];   // Варианты выбора на русском и английском языках.
    int8       *cell;                                   // Адрес ячейки, в которой хранится позиция текущего выбора.
    pFuncVB	    funcOnChanged;                          // Функция должна вызываться после изменения значения элемента.
    pFuncVII    funcForDraw;                            // Функция вызывается после отрисовки элемента. 

    String<> NameSubItem(int i);

    // Возвращает имя текущего варианта выбора элемента choice, как оно записано в исходном коде программы.
    String<> NameCurrentSubItem();

    // Возвращает имя следующего варианта выбора элемента choice, как оно записано в исходном коде программы.
    pchar NameNextSubItem();

    pchar NamePrevSubItem();

    // Возвращает количество вариантов выбора в элементе по адресу choice.
    int NumSubItems();

    void StartChange(int delta);

    // Рассчитывает следующий кадр анимации.
    float Step();

    // Изменяет значение choice в зависимости от величины и знака delta.
    void ChangeValue(int delta);

    void FuncOnChanged(bool active);

    void FuncForDraw(int x, int y);

    void Draw(int x, int y);

    void Draw(int x, int y, bool opened);

private:

    void DrawClosed(int x, int y);

    void DrawOpened(int x, int y);
};


struct IPaddressStruct
{
    ItemStruct str;
    uint8* ip0;
    uint8* ip1;
    uint8* ip2;
    uint8* ip3;
    pFuncVB funcOfChanged;
    uint16 *port;
};


class IPaddress : public Item
{
public:
    uint8*  ip0;
    uint8*  ip1;
    uint8*  ip2;
    uint8*  ip3;
    pFuncVB funcOfChanged;
    uint16* port;

    IPaddress(const IPaddressStruct *str);

    void Draw(int x, int y, bool opened);

    // При открытом элементе переставляет курсор на следующую позицию
    void NextPosition() const;

    // Изменяет значение в текущей позиции при открытом элементе
    void ChangeValue(int delta);

    // Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.
    void GetNumPosIPvalue(int *numIP, int *selPos);

    void DrawLowPart(int x, int y, bool pressed, bool shade);

    static int8 cur_digit;

private:

    void DrawOpened(int x, int y);
};


class  MACaddress
{
public:
    COMMON_PART_MENU_ITEM
    uint8*  mac0;
    uint8*  mac1;
    uint8*  mac2;
    uint8*  mac3;
    uint8*  mac4;
    uint8*  mac5;
    pFuncVB funcOfChanged;

    void Draw(int x, int y, bool opened);

    void ChangeValue(int delta);

    void DrawValue(int x, int y);

    static int8 cur_digit;

private:

    void DrawOpened(int x, int y);
};


// Описывает элемент меню для установки коэффициентов и знаков математической формулы
#define FIELD_SIGN_MEMBER_1_ADD 0
#define FIELD_SIGN_MEMBER_1_MUL 1
#define FIELD_SIGN_MEMBER_2_ADD 2
#define FIELD_SIGN_MEMBER_2_MUL 3
#define POS_SIGN_MEMBER_1       0
#define POS_KOEFF_MEMBER_1      1
#define POS_SIGN_MEMBER_2       2
#define POS_KOEFF_MEMBER_2      3


class Formula
{
public:
    COMMON_PART_MENU_ITEM
    int8*           function;                   // Адрес ячейки, где хранится Function, из которой берётся знак операции
    int8*           koeff1add;                  // Адрес коэффициента при первом члене для сложения
    int8*           koeff2add;                  // Адрес коэффициента при втором члене для сложения
    int8*           koeff1mul;                  // Адрес коэффициента при первом члене для умножения
    int8*           koeff2mul;                  // Адрес коэффициента при втором члене для умножения
    int8*           curDigit;                   // Текущий разряд : 0 - знак первого члена, 1 - коэффициент первого члена,
                                                //  2 - знак второго члена, 3 - коэффициент второго члена
    pFuncVV         funcOfChanged;              // Эта функция вызывается после изменения состояния элемента управления.

    void Draw(int x, int y, bool opened);

    void WriteText(int x, int y, bool opened);

    void DrawLowPart(int x, int y, bool pressed, bool shade);
};


class  GovernorColor
{
public:
    COMMON_PART_MENU_ITEM
    ColorType* colorType;                  // Структура для описания цвета.

    void Draw(int x, int y, bool opened);

    // Изменить яркость цвета в governor
    void ChangeValue(int delta);

    void DrawValue(int x, int y, int delta);

private:

    void DrawOpened(int x, int y);

    void DrawClosed(int x, int y);
};


#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

// Устанавливает и показывает время.
struct TimeControl
{
    COMMON_PART_MENU_ITEM
    int8*       curField;       // Текущее поле установки. 0 - выход, 1 - сек, 2 - мин, 3 - часы, 4 - день, 5 - месяц, 6 - год, 7 - установить.
    int8*       hours;
    int8*       minutes;
    int8*       seconds;
    int8*       month;
    int8*       day;
    int8*       year;

    void Draw(int x, int y, bool opened);

    void SetOpened();

    void SetNewTime() const;

    void SelectNextPosition();

    void IncCurrentPosition() const;

    void DecCurrentPosition() const;

    static void Open();

private:

    void DrawOpened(int x, int y);

    void DrawClosed(int x, int y);
};


// Данные для подсказки
struct Hint
{
    static void SetItem(Item *item);

    static Item *item;      // Item, подсказку для которого нужно выводить в случае включённого режима подсказок.

    static pchar string;    // Строка подсказки, которую надо выводить в случае включённого режима подсказок.

    static bool show;       // Если true, то при нажатии кнопки вместо выполнения её фунции выводится подсказка о её назначении

    static void ProcessButton(Key::E);

    static void DrawHintItem(int x, int y, int width);
};


struct Regulator
{
    static char GetSymbol();

    // Повернуть по часовой стрелке
    static void RotateRight();

    // Повернуть против часовой стрелки
    static void RotateLeft();

private:

    // Возвращает изображение регулятора, соответствующее его текущему положению.
    static char GetSymbol(int value);

    static int angle;
};
