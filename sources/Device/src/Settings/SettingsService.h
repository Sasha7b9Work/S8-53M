// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SOUND_ENABLED       (set.service.soundEnabled)

#define CALIBRATOR          (set.service.calibrator)
#define CALIBRATOR_IS_FREQ  (CALIBRATOR == CalibratorMode::Freq)
#define CALIBRATOR_IS_DC    (CALIBRATOR == CalibratorMode::DC)

#define IP_ADDRESS          (set.service.IPaddress)

#define COLOR_SCHEME                    (set.service.colorScheme)
#define IS_COLOR_SCHEME_WHITE_LETTERS   (COLOR_SCHEME == ColorScheme::WhiteLetters)


// �������� �����
struct ColorScheme
{
    enum E
    {
        WhiteLetters,   // � ���� ������ ��������� ��������� ���� ������� ����� - �� ����� ������ ����� �����.
        BlackLetters    // � ���� ������ ��������� ��������� ���� ������� ������ - �� ����� ������� ��������.
    };
};


// ��������� ���� ������.
struct SettingsService
{ //-V802
    bool              screenWelcomeEnable;    // ����� �� ������������ ����� ����������� ��� ��������� �������. \todo ������ ���� ��������.
    bool              soundEnabled;           // �������� �� �����.
    CalibratorMode::E calibrator;             // ����� ������ �����������.
    int8              IPaddress;              // IP-����� (��������).
    ColorScheme::E    colorScheme;            // �������� ������.
};
