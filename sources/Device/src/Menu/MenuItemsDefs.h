// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once

#define FuncDraw            EmptyFuncVII
#define FuncDrawPage        EmptyFuncVV
#define FuncActive          EmptyFuncBV
#define FuncPress           EmptyFuncVV
#define EmptyPressPage      EmptyFuncVV
#define FuncChanged         EmptyFuncVV
#define FuncBeforeDraw      EmptyFuncVV
#define FuncRegSetPage      EmptyFuncVI



#define DEF_BUTTON(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, funcDraw)                             \
static const ButtonBase name = { TypeItem::Button, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},   \
    funcPress, funcDraw };


#define DEF_TIME(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, cur, h, mi, s, mo, d, y)                           \
static const TimeBase name = { TypeItem::Time, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},       \
    &cur, &h, &mi, &s, &mo, &d, &y };


#define DEF_GOVERNOR(name, titleRU, titleEN, hintRU, hintEN, cell, min, max, keeper, funcActive, funcChanged, funcBeforeDraw)   \
static const GovernorBase name = {TypeItem::Governor, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},\
    min, max, (int16 *)(&(cell)), funcChanged, funcBeforeDraw};


#define DEF_IP_ADDRESS(name, titleRU, titleEN, hintRU, hintEN, ip0, ip1, ip2, ip3, port, keeper, funcActive, funcOfChanged)     \
static const IPaddressBase name = {TypeItem::IP, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},     \
    &ip0, &ip1, &ip2, &ip3, funcOfChanged, port};


#define DEF_MAC_ADDRESS(name, titleRU, titleEN, hintRU, hintEN, mac0, mac1, mac2, mac3, mac4, mac5, keeper, funcActive, funcOfChanged)  \
static const MACaddressBase name = {TypeItem::MAC, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},           \
    &mac0, &mac1, &mac2, &mac3, &mac4, &mac5, funcOfChanged};


#define DEF_GOVERNOR_COLOR(name, titleRU, titleEN, hintRU, hintEN, colorType, keeper)                                           \
static const GovernorColorBase name = {TypeItem::GovernorColor, 0, false, NamePage::NoPage, &keeper, FuncActive,                          \
    {titleRU, titleEN, hintRU, hintEN}, &colorType, 0};


#define DEF_PAGE_SB(name, titleRU, titleEN, hintRU, hintEN, item0, item1, item2, item3, item4, item5,                                           \
    namePage, keeper, funcActive, funcPress, funcDraw, funcRegSet)                                                                              \
static const Item * const  items##name[] = {(Item *)item0, (Item *)item1, (Item *)item2, (Item *)item3, (Item *)item4,        \
    (Item *)item5};                                                                                                                          \
const PageBase name = {TypeItem::Page, 6, true, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                    \
    items##name, funcPress, funcDraw, funcRegSet};

#define DEF_PAGE_1(name, titleRU, titleEN, hintRU, hintEN, item1, namePage, keeper, funcActive, funcPress)                                      \
static const Item * const  items##name[] = {(Item *)&item1};                                                                              \
const PageBase name = {TypeItem::Page, 1, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_2(name, titleRU, titleEN, hintRU, hintEN, item1, item2, namePage, keeper, funcActive, funcPress)                               \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2};                                                           \
const PageBase name = {TypeItem::Page, 2, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_3(name, titleRU, titleEN, hintRU, hintEN, item1, item2, item3, namePage, keeper, funcActive, funcPress)                        \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3};                                        \
const PageBase name = {TypeItem::Page, 3, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_4(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, namePage, keeper, funcActive, funcPress)                                                                        \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4};                     \
const PageBase name = {TypeItem::Page, 4, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_5(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, item5, namePage, keeper, funcActive, funcPress)                                                                 \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5};  \
const PageBase name = {TypeItem::Page, 5, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_6(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, item5, item6, namePage, keeper, funcActive, funcPress)                                                          \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5,   \
    (Item *)&item6};                                                                                                                         \
const PageBase name = {TypeItem::Page, 6, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_7(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, item5, item6, item7, namePage, keeper, funcActive, funcPress)                                                   \
static const Item * const  items##name[] = {                                                                                                 \
    (Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5, (Item *)&item6, (Item *)&item7};       \
const PageBase name = {TypeItem::Page, 7, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_8(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, item5, item6, item7, item8, namePage, keeper, funcActive, funcPress)                                            \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5,   \
    (Item *)&item6, (Item *)&item7, (Item *)&item8};                                                                                   \
const PageBase name = {TypeItem::Page, 8, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_9(name, titleRU, titleEN, hintRU, hintEN,                                                                                      \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, namePage, keeper, funcActive, funcPress)                                     \
static const Item * const  items##name[] = {                                                                                                 \
    (Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5, (Item *)&item6, (Item *)&item7,        \
    (Item *)&item8, (Item *)&item9};                                                                                                      \
const PageBase name = {TypeItem::Page, 9, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                   \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_10(name, titleRU, titleEN, hintRU, hintEN,                                                                                     \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, namePage, keeper, funcActive, funcPress)                             \
static const Item * const  items##name[] = {                                                                                                 \
    (Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5, (Item *)&item6, (Item *)&item7,        \
    (Item *)&item8, (Item *)&item9, (Item *)&item10};                                                                                  \
const PageBase name = {TypeItem::Page, 10, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                  \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_11_GLOBAL(name, titleRU, titleEN, hintRU, hintEN,                                                                              \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, namePage, keeper, funcActive, funcPress)                     \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5,   \
    (Item *)&item6, (Item *)&item7, (Item *)&item8, (Item *)&item9, (Item *)&item10, (Item *)&item11};                        \
const PageBase name = {TypeItem::Page, 11, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                  \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};cd

#define DEF_PAGE_12(name, namePage, keeper, funcActive, funcPress, titleRU, titleEN, hintRU, hintEN,                                            \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12)                                                      \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5,   \
    (Item *)&item6, (Item *)&item7, (Item *)&item8, (Item *)&item9, (Item *)&item10, (Item *)&item11, (Item *)&item12};    \
const PageBase name = {TypeItem::Page, 12, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                  \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_13(name, namePage, keeper, funcActive, funcPress, titleRU, titleEN, hintRU, hintEN,                                            \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12, item13)                                              \
static const Item * const  items##name[] = {                                                                                                 \
    (Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5, (Item *)&item6, (Item *)&item7,        \
    (Item *)&item8, (Item *)&item9, (Item *)&item10, (Item *)&item11, (Item *)&item12, (Item *)&item13};                      \
const PageBase name = {TypeItem::Page, 13, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                  \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};

#define DEF_PAGE_15(name, namePage, keeper, funcActive, funcPress, titleRU, titleEN, hintRU, hintEN,                                            \
    item1, item2, item3, item4, item5, item6, item7, item8, item9, item10,                                                                      \
    item11, item12, item13, item14, item15)                                                                                                     \
static const Item * const  items##name[] = {(Item *)&item1, (Item *)&item2, (Item *)&item3, (Item *)&item4, (Item *)&item5,   \
    (Item *)&item6, (Item *)&item7, (Item *)&item8, (Item *)&item9, (Item *)&item10, (Item *)&item11, (Item *)&item12,     \
    (Item *)&item13, (Item *)&item14,                                                                                                     \
    (Item *)&item15};                                                                                                                        \
const PageBase name = {TypeItem::Page, 15, false, namePage, keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                                  \
    items##name, funcPress, FuncDrawPage, FuncRegSetPage};



#define DEF_CHOICE_PARAMETER(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, numParam,                                   \
    allow0, allow1, allow2, allow3, allow4, allow5, allow6, allow7)                                                                             \
ChoiceParameterBase name = {Item_ChoiceParameter, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN} ,       \
    funcPress, {allow0, allow1, allow2, allow3, allow4, allow5, allow6, allow7}, (uint8 *)&numParam};



#define DEF_SMALL_BUTTON(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, funcDraw)                                   \
static const SButtonBase name = { TypeItem::SmallButton, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},         \
    funcPress, funcDraw, 0, 0};

#define DEF_SMALL_BUTTON_EXIT(name, keeper, funcActive, funcPress, funcDraw)                                                                \
static const SButtonBase name = { TypeItem::SmallButton, 0, false, NamePage::NoPage, &keeper, funcActive, {"Выход", "Exit",                           \
    "Кнопка для выхода в предыдущее меню", "Button to return to the previous menu"}, funcPress, funcDraw, 0, 0};

#define DEF_SMALL_BUTTON_HINTS_2(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, funcDraw,                           \
    FuncDrawHint1, hintRu1, hintEn1, FuncDrawHint2, hintRu2, hintEn2)                                                                       \
static const StructHelpSmallButton hints##name[] = {{ FuncDrawHint1, hintRu1, hintEn1 }, { FuncDrawHint2, hintRu2, hintEn2 }};              \
static const SButtonBase name = { TypeItem::SmallButton, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},         \
    funcPress, funcDraw, hints##name, 2};

#define DEF_SMALL_BUTTON_HINTS_3(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, funcDraw,                           \
    FuncDrawHint1, hintRu1, hintEn1, FuncDrawHint2, hintRu2, hintEn2, FuncDrawHint3, hintRu3, hintEn3)                                      \
static const StructHelpSmallButton hints##name[] = {{ FuncDrawHint1, hintRu1, hintEn1 }, { FuncDrawHint2, hintRu2, hintEn2 },               \
    { FuncDrawHint3, hintRu3, hintEn3 } };                                                                                                  \
static const SButtonBase name = { TypeItem::SmallButton, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},         \
    funcPress, funcDraw, hints##name, 3};

#define DEF_SMALL_BUTTON_HINTS_5(name, titleRU, titleEN, hintRU, hintEN, keeper, funcActive, funcPress, funcDraw,                           \
    FuncDrawHint1, hintRu1, hintEn1, FuncDrawHint2, hintRu2, hintEn2, FuncDrawHint3, hintRu3, hintEn3,                                      \
    FuncDrawHint4, hintRu4, hintEn4, FuncDrawHint5, hintRu5, hintEn5)                                                                       \
static const StructHelpSmallButton hints##name[] = {{ FuncDrawHint1, hintRu1, hintEn1 }, { FuncDrawHint2, hintRu2, hintEn2 },               \
    { FuncDrawHint3, hintRu3, hintEn3 }, { FuncDrawHint4, hintRu4, hintEn4 }, { FuncDrawHint5, hintRu5, hintEn5 } };                        \
static const SButtonBase name = { TypeItem::SmallButton, 0, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},         \
    funcPress, funcDraw, hints##name, 5};



#define DEF_CHOICE_2(name, titleRU, titleEN, hintRU, hintEN, nameRu1, nameEn1, nameRu2, nameEn2, cell, keeper, funcActive, funcChanged, funcDraw)   \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2};                                                                                \
static const ChoiceBase name = {TypeItem::Choice, 2, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN},                        \
    (int8 *)&cell, hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_3(name, titleRU, titleEN, hintRU, hintEN,                                                                                        \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, cell, keeper, funcActive, funcChanged, funcDraw)                                          \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3};                                                              \
static const ChoiceBase name = {TypeItem::Choice, 3, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_4(name, titleRU, titleEN, hintRU, hintEN,                                                                                        \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, cell, keeper, funcActive, funcChanged, funcDraw)                        \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4};                                            \
static const ChoiceBase name = {TypeItem::Choice, 4, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_5(name, titleRU, titleEN, hintRU, hintEN,                                                                                        \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, nameRu5, nameEn5, cell, keeper, funcActive, funcChanged, funcDraw)      \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4,                                             \
    nameRu5, nameEn5};                                                                                                                              \
static const ChoiceBase name = {TypeItem::Choice, 5, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_6(name, titleRU, titleEN, hintRU, hintEN,nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4,                          \
    nameEn4, nameRu5, nameEn5, nameRu6, nameEn6, cell, keeper, funcActive, funcChanged, funcDraw)                                                   \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4,                                             \
    nameRu5, nameEn5, nameRu6, nameEn6};                                                                                                            \
static const ChoiceBase name = {TypeItem::Choice, 6, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_7(name, titleRU, titleEN, hintRU, hintEN,                                                                                        \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, nameRu5, nameEn5,                                                       \
    nameRu6, nameEn6, nameRu7, nameEn7, cell, keeper, funcActive, funcChanged, funcDraw)                                                            \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4,                                                      \
    nameEn4, nameRu5, nameEn5, nameRu6, nameEn6, nameRu7, nameEn7};                                                                                 \
static const ChoiceBase name = {TypeItem::Choice, 7, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_8(name, titleRU, titleEN, hintRU, hintEN,                                                                                        \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, nameRu5, nameEn5,                                                       \
    nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8, cell, keeper, funcActive, funcChanged, funcDraw)                                          \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4,                                             \
    nameRu5, nameEn5, nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8};                                                                        \
static const ChoiceBase name = {TypeItem::Choice, 8, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,         \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_REG_9(name, titleRU, titleEN, hintRU, hintEN,                                                                                    \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, nameRu5, nameEn5,                                                       \
    nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8, nameRu9, nameEn9, cell, keeper, funcActive, funcChanged, funcDraw)                        \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4,                                             \
    nameRu5, nameEn5, nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8, nameRu9, nameEn9};                                                      \
static const ChoiceBase name = {TypeItem::ChoiceReg, 9, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,      \
    hints##name, funcChanged, funcDraw};

#define DEF_CHOICE_REG_10(name, titleRU, titleEN, hintRU, hintEN,                                                                                   \
    nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4, nameRu5, nameEn5,                                                       \
    nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8, nameRu9, nameEn9, nameRu10, nameEn10, cell, keeper, funcActive, funcChanged, funcDraw)    \
static pString hints##name[] = {nameRu1, nameEn1, nameRu2, nameEn2, nameRu3, nameEn3, nameRu4, nameEn4,                                             \
    nameRu5, nameEn5, nameRu6, nameEn6, nameRu7, nameEn7, nameRu8, nameEn8, nameRu9, nameEn9, nameRu10, nameEn10};                                  \
static const ChoiceBase name = {TypeItem::ChoiceReg, 10, false, NamePage::NoPage, &keeper, funcActive, {titleRU, titleEN, hintRU, hintEN}, (int8 *)&cell,     \
    hints##name, funcChanged, funcDraw};
