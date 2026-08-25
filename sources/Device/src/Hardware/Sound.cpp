// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <cmath>


namespace Sound
{
    struct TypeWave {
        enum E {
            Sine,
            Meandr,
            Triangle
        };
    };

    static bool warnIsBeep = false;
    static bool buttonIsPressed = false;
    static float mainVolume = 0.1f;

    static const int POINTS_IN_PERIOD = 10;
    static uint8 points[POINTS_IN_PERIOD] = {0};
    static float frequency = 0.0F;
    static float amplitude = 0.0F;
    static TypeWave::E typeWave = TypeWave::Sine;

    static void Beep(TypeWave::E typeWave, float frequency, float amplitude, int duration, float mainVolume = 0.1f);

    static void Stop();

    static void SetWave();

    static uint16 CalculatePeriodForTIM();

    static void CalculateSine();

    static void CalculateMeandr();

    static void CalculateTriangle();
}


void Sound::Init()
{
    HAL_DAC2::Init();
}


void Sound::Stop()
{
    HAL_DAC2::StopDMA();
    Sound::warnIsBeep = false;
}


void Sound::Beep(TypeWave::E _typeWave, float _frequency, float _amplitude, int duration, float _mainVolume)
{
    mainVolume = _mainVolume;

    if (Sound::warnIsBeep)
    {
        return;
    }

    if (!SOUND_ENABLED)
    {
        return;
    }

    _amplitude *= mainVolume;

    if (frequency != _frequency || amplitude != _amplitude || typeWave != _typeWave) //-V550
    {
        frequency = _frequency;
        amplitude = _amplitude;
        typeWave = _typeWave;
        
        Stop();
        SetWave();
    }

    Stop();
    
    HAL_DAC2::StartDMA(points, POINTS_IN_PERIOD);

    Timer::Enable(TypeTimer::StopSound, duration, Stop);
}


void Sound::ButtonPress()
{
    Beep(TypeWave::Sine, 2000.0F, 0.5F, 50);
    Sound::buttonIsPressed = true;
}


void Sound::ButtonRelease()
{
    if (buttonIsPressed)
    {
        Beep(TypeWave::Sine, 1000.0F, 0.25F, 50);
        buttonIsPressed = false;
    }
}


void Sound::GovernorChangedValue()
{
    Beep(TypeWave::Sine, 1000.0F, 0.5F, 50);
    buttonIsPressed = false;
}


void Sound::RegulatorShiftRotate()
{
    Beep(TypeWave::Sine, 1.0F, 0.35F, 3);
    buttonIsPressed = false;
}


void Sound::RegulatorSwitchRotate()
{
    Beep(TypeWave::Triangle, 2500.0F, 0.5F, 25);
    buttonIsPressed = false;
}


void Sound::WarnBeepBad()
{
    Beep(TypeWave::Meandr, 250.0F, 1.0F, 500);
    warnIsBeep = true;
    buttonIsPressed = false;
}


void Sound::WarnBeepGood()
{
    Beep(TypeWave::Triangle, 1000.0F, 0.5F, 250);
    buttonIsPressed = false;
}


void Sound::DeviceEnabled()
{
    Beep(TypeWave::Meandr, 250.0F, 1.0F, 500, 1.0F);
    warnIsBeep = true;
    buttonIsPressed = false;
}


void Sound::SetWave()
{
    HAL_TIM7::Config(0, CalculatePeriodForTIM());

    if(typeWave == TypeWave::Sine)
    {
        CalculateSine();
    }
    else if(typeWave == TypeWave::Meandr)
    {
        CalculateMeandr();
    }
    else if(typeWave == TypeWave::Triangle)
    {
        CalculateTriangle();
    }
}


uint16 Sound::CalculatePeriodForTIM()
{
    return (uint16)(120e6F / frequency / POINTS_IN_PERIOD);
}


void Sound::CalculateSine()
{
    for (int i = 0; i < POINTS_IN_PERIOD; i++)
    {
        float step = 2.0F * 3.1415926F / (POINTS_IN_PERIOD - 1);
        float value = (std::sin((float)(i) * step) + 1.0F) / 2.0F;
        points[i] = (uint8)(value * amplitude * 255);
    }
}


void Sound::CalculateMeandr()
{
    for (int i = 0; i < POINTS_IN_PERIOD / 2; i++)
    {
        points[i] = (uint8)(255 * amplitude);
    }
    for (int i = POINTS_IN_PERIOD / 2; i < POINTS_IN_PERIOD; i++)
    {
        points[i] = 0;
    }
}


void Sound::CalculateTriangle()
{
    float k = 255.0 / POINTS_IN_PERIOD;
    for (int i = 0; i < POINTS_IN_PERIOD; i++)
    {
        points[i] = (uint8)(k * i * amplitude);
    }
}
