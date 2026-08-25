// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Settings/Settings.h"
#include "Display/Screen/Grid.h"
#include "Menu/Menu.h"
#include "Settings/SettingsCursors.h"
#include "Utils/Math.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Text/Text.h"
#include <cmath>
#include <cstring>


float PageCursors::GetCursPosU(Chan ch, int numCur)
{
    return CURS_POS_U(ch, numCur) / (Grid::ChannelBottom() == Grid::FullBottom() ? 1.0f : 2.0f);
}


bool PageCursors::NecessaryDrawCursors()
{
    return ((!CURS_CNTRL_U_IS_DISABLE(CURS_SOURCE)) || (!CURS_CNTRL_T_IS_DISABLE(CURS_SOURCE))) && 
        (CURS_SHOW || Page::NameOpened() == NamePage::SB_Curs);
}


String<> PageCursors::GetCursVoltage(Chan source, int numCur)
{
    float voltage = Math::VoltageCursor(PageCursors::GetCursPosU(source, numCur), SET_RANGE(source), SET_RSHIFT(source));
    return SU::Voltage2String(voltage, true);
}


String<> PageCursors::GetCursorTime(Chan source, int numCur)
{
    float time = Math::TimeCursor(CURS_POS_T(source, numCur), SET_TBASE);
    return SU::Time2String(time, true);
}


String<> PageCursors::GetCursorPercentsU(Chan source)
{
    float dPerc = DELTA_U100(source);
    float dValue = std::fabs(PageCursors::GetCursPosU(source, 0) - PageCursors::GetCursPosU(source, 1));

    String<> result = SU::Float2String(dValue / dPerc * 100.0f, false, 5);
    result.Append("%");

    return result;
}


String<> PageCursors::GetCursorPercentsT(Chan source)
{
    float dPerc = DELTA_T100(source);
    float dValue = std::fabs(CURS_POS_T0(source) - CURS_POS_T1(source));

    String<> result = SU::Float2String(dValue / dPerc * 100.0f, false, 6);
    result.Append("%d");

    return result;
}
