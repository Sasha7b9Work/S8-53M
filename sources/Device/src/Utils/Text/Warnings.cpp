// 2022/03/23 12:17:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Display/Colors.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Display/Screen/Grid.h"
#include "Utils/Text/Warnings.h"
#include "Tables.h"


namespace Warning
{
    static const int  NUM_WARNINGS = 10;
    static const char *warnings[NUM_WARNINGS] = {0};       // гДЕЯЭ ОПЕДСОПЕФДЮЧЫХЕ ЯННАЫЕМХЪ.
    static uint        timeWarnings[NUM_WARNINGS] = {0};   // гДЕЯЭ БПЕЛЪ, ЙНЦДЮ ОПЕДСОПЕФДЮЧЫЕЕ ЯННАЫЕМХЕ ОНЯРСОХКН МЮ ЩЙПЮМ.
    static void OnTimerShowWarning();
    static void DrawStringInRectangle(int x, int y, char const *text);
    static void ShowWarn(pchar);
    static pchar GetWarning(Warning::E);
}


void Warning::ShowBad(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(GetWarning(warning));
    Sound::WarnBeepBad();
}


void Warning::ShowWarn(pchar message)
{
    if (warnings[0] == 0)
    {
        Timer::Enable(TypeTimer::ShowMessages, 100, OnTimerShowWarning);
    }

    bool alreadyStored = false;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] == 0 && !alreadyStored)
        {
            warnings[i] = message;
            timeWarnings[i] = TIME_MS;
            alreadyStored = true;
        }
        else if (warnings[i] == message)
        {
            timeWarnings[i] = TIME_MS;
            return;
        }
    }
}


void Warning::OnTimerShowWarning()
{
    uint time = TIME_MS;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (time - timeWarnings[i] > (uint)TIME_MESSAGES * 1000)
        {
            timeWarnings[i] = 0;
            warnings[i] = 0;
        }
    }

    int pointer = 0;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] != 0)
        {
            warnings[pointer] = warnings[i];
            timeWarnings[pointer] = timeWarnings[i];

            if (pointer != i)
            {
                timeWarnings[i] = 0;
                warnings[i] = 0;
            }
            pointer++;
        }
    }

    if (pointer == 0)
    {
        Timer::Disable(TypeTimer::ShowMessages);
    }
}


void Warning::DrawWarnings()
{
    int delta = 12;
    int y = Grid::BottomMessages();

    for (int i = 0; i < 10; i++)
    {
        if (warnings[i] != 0)
        {
            DrawStringInRectangle(Grid::Left(), y, warnings[i]);
            y -= delta;
        }
    }
}


void Warning::DrawStringInRectangle(int, int y, char const *text)
{
    int width = Font::GetLengthText(text);
    int height = 8;
    Rectangle(width + 4, height + 4).Draw(Grid::Left(), y, COLOR_FILL);
    Rectangle(width + 2, height + 2).Draw(Grid::Left() + 1, y + 1, COLOR_BACK);
    Region(width, height).Fill(Grid::Left() + 2, y + 2, Color::FLASH_10);
    String<>(text).Draw(Grid::Left() + 3, y + 2, Color::FLASH_01);
}


void Warning::ShowGood(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(GetWarning(warning));
    Sound::WarnBeepGood();
}


pchar Warning::GetWarning(Warning::E warning)
{
    static pchar warns[Warning::Count][2] =
    {
        {"опедек йюмюк 1 - бнкэр/дек",      "LIMIT CHANNEL 1 - VOLTS/DIV"},
        {"опедек йюмюк 2 - бнкэр/дек",      "LIMIT CHANNEL 2 - VOLTS/DIV"},
        {"опедек пюгбепрйю - бпелъ/дек",    "LIMIT SWEEP - TIME/DIV"},
        {"бйкчвем охйнбши дерейрнп",        "SET_ENABLED PEAK. DET."},
        {"опедек йюмюк 1 - \x0d",           "LIMIT CHANNEL 1 - \x0d"},
        {"опедек йюмюк 2 - \x0d",           "LIMIT CHANNEL 2 - \x0d"},
        {"опедек пюгбепрйю - спнбемэ",      "LIMIT SWEEP - LEVEL"},
        {"опедек пюгбепрйю - \x97",         "LIMIT SWEEP - \x97"},
        {"охй. дер. ме пюанрюер мю пюгбепрйюу лемее 0.5ЛЙЯ/ДЕК", "PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"},
        {"яюлнохяеж ме лнфер пюанрюрэ мю ашярпшу пюгб╗прйюу", "SELF-RECORDER DOES NOT WORK FAST SCANS"},
        {"тюик янупюмем",                   "FILE IS SAVED"},
        {"яхцмюк янупюмем",                 "SIGNAL IS SAVED"},
        {"яхцмкю сдюкем",                   "SIGNAL IS DELETED"},
        {"лемч нркюдйю бйкчвемн",           "MENU DEBUG IS SET_ENABLED"},
        {"онднфдхре меяйнкэйн яейсмд, хдер оепелеыемхе дюммшу", "WAIT A FEW SECONDS, GOES A MOVEMENT DATA"},
        {"бпелъ ме сярюмнбкемн. лнфере сярюмнбхрэ ецн яеивюя", "TIME IS NOT SET. YOU CAN INSTALL IT NOW"},
        {"яхцмюк ме мюидем",                "SIGNAL IS NOT FOUND"},
        {"мю пюгбепрйюу ледкеммее 10ЛЯ/ДЕК фекюрекэмн сярюмюб-", "AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN"},
        {"кхбюрэ \"пюгбепрйю - \x7b\" б онкнфемхе \"кЕБН\" дкъ сяйнпе-", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"},
        {"мхъ бшбндю яхцмюкю",              ""},
        {"нвемэ люкн хглепемхи",            "VERY SMALL MEASURES"},
        {"дкъ бярсокемхъ хглемемхи б яхкс", "FOR THE INTRODUCTION OF CHANGES"},
        {"бшйкчвхре опханп",                "IN FORCE SWITCH OFF THE DEVICE"},
        {"нрйкчвхре бшвхякемхе аот",        "DISCONNECT CALCULATION OF FFT"},
        {"нрйкчвхре люрелюрхвеяйсч тсмйжхч", "DISCONNECT MATHEMATICAL FUNCTION"},
        {"опньхбйю янупюмемю",              "FIRMWARE SAVED"},
        {"оЮЛЪРЭ OTP ОНКМНЯРЭЧ ГЮОНКМЕМЮ",   "OPT memory fully completed"},
        {"мюярпнийх янупюмемш",             "SETTINGS SAVED"},
        {"мюярпнийх ме янупюмемш",          "SETTINGS NOT SAVED"},
        {"опханп ме мюярпнем",              "INSTRUMENT NOT SET UP"},
    };
    return warns[warning][LANG];
}
