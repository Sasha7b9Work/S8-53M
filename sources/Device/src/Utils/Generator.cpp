// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Generator.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "FPGA/TypesFPGA.h"
#include <cmath>


static void SetParametersWave(Chan ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);
static void StartNewWave(Chan ch);
static uint8 GetSampleWave(Chan ch);


const SGenerator Generator = 
{
    SetParametersWave,
    StartNewWave,
    GetSampleWave,
};


static float NewNoiseValue(Chan ch);
static uint8 GetSampleSinusWave(Chan ch, int numSample);
static uint8 GetSampleMeanderWave(Chan ch, int numSample);


static TypeWave type[2] = {Wave_Sinus, Wave_Meander};
static float freq[2] = {1000.0f, 500.0f};
static float angle[2] = {0.05f, 0.1f};
static float ampl[2] = {1.0f, 0.5f};
static float amplNoise[2] = {0.1f, 0.1f};
static int numSample[2] = {0, 0};

void SetParametersWave(Chan ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise_)
{
    type[ch] = typeWave;
    freq[ch] = frequency;
    angle[ch] = startAngle;
    ampl[ch] = amplWave;
    amplNoise[ch] = amplNoise_;
}

void StartNewWave(Chan)
{
    numSample[0] = numSample[1] = 0;
}

uint8 GetSampleWave(Chan ch)
{
    return (type[ch] == Wave_Sinus) ? GetSampleSinusWave(ch, (numSample[ch])++) : GetSampleMeanderWave(ch, (numSample[ch])++);
}

uint8 GetSampleSinusWave(Chan ch, int numSample_)
{
    float dT = numSample_ * TShift::ToAbs(1, SET_TBASE);
    float voltage = ampl[ch] * std::sin(2 * M_PI * freq[ch] * dT + angle[ch]) + NewNoiseValue(ch);
    return ValueFPGA::FromVoltage(voltage, SET_RANGE(ch), SET_RSHIFT(ch));
}

uint8 GetSampleMeanderWave(Chan, int)
{
    return 0;
}

float NewNoiseValue(Chan ch)
{
    static float prevNoise[2] = {0.0f, 0.0f};            // Здесь хранятся значения шума из предыдущих точек, необходимые для расчёта шума в текущей точке.

    float noise = prevNoise[ch];

    float halfAmplNoiseAbs = ampl[ch] * amplNoise[ch] / 2.0f;

    float deltaRand = halfAmplNoiseAbs;

    noise += Math::RandFloat(-deltaRand, deltaRand);

    while (noise < -halfAmplNoiseAbs)
    {
        noise += Math::RandFloat(0, deltaRand * 2);
    }

    while (noise > halfAmplNoiseAbs)
    {
        noise -= Math::RandFloat(0, deltaRand * 2);
    }

    prevNoise[ch] = noise;

    return noise;
}
