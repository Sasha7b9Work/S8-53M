// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Display/Screen/Grid.h"
#include "Hardware/Timer.h"
#include "FPGA/TypesFPGA.h"
#include <cmath>
#include <cstdlib>
#include <cstring>


template void  Math::AddLimitation<float>(float *, float, float, float);
template int8  Math::CircleIncrease<int8>(int8 *, int8, int8);
template int16 Math::CircleIncrease<int16>(int16 *, int16, int16);
template int   Math::CircleIncrease<int>(int *, int, int);
template int8  Math::CircleDecrease<int8>(int8 *, int8, int8);
template int16 Math::CircleDecrease<int16>(int16 *, int16, int16);
template int   Math::CircleDecrease<int>(int *, int, int);
template float Math::Max<float>(float, float, float);
template bool  Math::InRange<int>(int, int, int);


// Столько вольт в одной точке экрана
const float voltsInPixel[Range::Count] =
{
    2e-3f   / 20,  // 2mV
    5e-3f   / 20,  // 5mV
    10e-3f  / 20,  // 10mV
    20e-3f  / 20,  // 20mV
    50e-3f  / 20,  // 50mV
    100e-3f / 20,  // 100mV
    200e-3f / 20,  // 200mV
    500e-3f / 20,  // 500mV
    1.0f    / 20,  // 1V
    2.0f    / 20,  // 2V
    5.0f    / 20,  // 5V
    10.0f   / 20,  // 10V
    20.0f   / 20   // 20V
};


int Math::MinFrom2Int(int val0, int val1)
{
    if (val0 < val1)
    {
        return val0;
    }
    return val1;
}


float Math::VoltageCursor(float shiftCurU, Range::E range, RShift rshift)
{
    return Range::MaxOnScreen(range) - shiftCurU * voltsInPixel[range] - rshift.ToAbs(range);
}


float Math::TimeCursor(float shiftCurT, TBase::E tbase)
{
    return shiftCurT * TShift::absStep[tbase];
}


void Math::DataExtrapolation(uint8 *data, uint8 *there, int size)
{
    int pointer = 0;
    while(there[pointer] == 0)
    {
        pointer++;
    }
    for(int i = 0; i < pointer; i++)
    {
        data[i] = data[pointer];
    }
    
    while(pointer < size)
    {
        while(there[pointer] != 0)
        {
            pointer++;
            if(pointer >= size)
            {
                return;
            }
        }
        pointer--;
        int pointer2 = pointer + 1;
        while(there[pointer2] == 0)
        {
            pointer2++;
            if(pointer2 >= size)
            {
                return;
            }
        }
        int deltaY = pointer2 - pointer;
        if(deltaY >= 2)
        {
            float deltaX = (float)(data[pointer2] - data[pointer]) / deltaY;
            for(int i = 1; i < deltaY; i++)
            {
                data[pointer + i] = data[pointer] + i * deltaX;
            }
        }
        pointer = pointer2 + 1;
    }
}


float Math::GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine)
{
    if(y0 == y1)
    {
        return x1;
    }

    return (yHorLine - y0) / ((float)(y1 - y0) / (float)(x1 - x0)) + x0;
}


bool Math::FloatsIsEquals(float value0, float value1, float epsilonPart)
{
    float max = std::fabs(value0) > std::fabs(value1) ? std::fabs(value0) : std::fabs(value1);

    float epsilonAbs = max * epsilonPart;

    return std::fabs(value0 - value1) < epsilonAbs;
}


float Math::MinFrom3float(float value1, float value2, float value3)
{
    float retValue = value1;
    if(value2 < retValue)
    {
        retValue = value2;
    }
    if(value3 < retValue)
    {
        retValue = value3;
    }
    return retValue;
}


int Math::MinInt(int val1, int val2)
{
    return val1 < val2 ? val1 : val2;
}

/*
    Быстрое преобразование Фурье. Вычисляет модуль спектра для дейсвтительного сигнала.
    Количество отсчётов должно быть 2**N
*/

//#ifndef DEBUG
//#include "TablesWindow.h"
//#include "TablesLog.h"
//
//static float const *Koeff(int numPoints)
//{
//    float const *tables[3][4] = {
//        {koeffsNorm256, koeffsHamming256, koeffsBlack256, koeffsHann256},
//        {koeffsNorm512, koeffsHamming512, koeffsBlack512, koeffsHann512},
//        {koeffsNorm1024, koeffsHamming1024, koeffsBlack1024, koeffsHann1024},
//    };
//
//    int row = 0;
//    if (numPoints == 512)
//    {
//        row = 1;
//    }
//    else if (numPoints == 1024)
//    {
//        row = 2;
//    }
//
//    return tables[row][PageServiceMath_GetWindowFFT()];
//}
//
//#endif

static void Normalize(float *data, int)
{
    float max = 0.0;
    for (int i = 0; i < 256; i++)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
    }

    for (int i = 0; i < 256; i++)
    {
        data[i] /= max;
    }
}

static void MultiplyToWindow(float *data, int numPoints)
{
//#ifndef DEBUG
//    float const *koeff = Koeff(numPoints);
//
//    for (int i = 0; i < numPoints; i++)
//    {
//        data[i] *= koeff[i];
//    }
//#else
    if (WINDOW_FFT_IS_HAMMING)
    {
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= 0.53836 - 0.46164 * std::cos(2 * M_PI * i / (numPoints - 1));
        }
    }
    else if (WINDOW_FFT_IS_BLACKMAN)
    {
        float alpha = 0.16f;
        float a0 = (1.0f - alpha) / 2.0f;
        float a1 = 0.5f;
        float a2 = alpha / 2.0f;
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= a0 - a1 * std::cos(2 * M_PI * i / (numPoints - 1)) + a2 * std::cos(4 * M_PI * i / (numPoints - 1));
        }
    }
    else if (WINDOW_FFT_IS_HANN)
    {
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= 0.5f * (1.0f - std::cos(2.0 * M_PI * i / (numPoints - 1.0)));
        }
    }
//#endif
}

#define SET_MIN_MAX(x)		\
	if (x < min) min = x;	\
	if (x > max) max = x;

void Math::CalculateFFT(float *dataR, int numPoints, float *result, float *freq0, float *density0, float *freq1, float *density1, int *y0, int *y1)
{
    float scale = 1.0f / TShift::absStep[SET_TBASE] / 1024.0f;

    float K = 1024.0 / numPoints;

    *freq0 = scale * FFT_POS_CURSOR_0 * K;
    *freq1 = scale * FFT_POS_CURSOR_1 * K;
    if (SET_PEAKDET_IS_ENABLED)
    {
        *freq0 *= 2;
        *freq1 *= 2;
    }

    for (int i = 0; i < numPoints; i++)
    {
        result[i] = 0.0f;
    }

    MultiplyToWindow(dataR, numPoints);

    int logN = 8;
    if (numPoints == 512)
    {
        logN = 9;
    }
    else if (numPoints == 1024)
    {
        logN = 10;
    }
    static const float Rcoef[14] =
    {
        -1.0000000000000000F, 0.0000000000000000F, 0.7071067811865475F,
        0.9238795325112867F, 0.9807852804032304F, 0.9951847266721969F,
        0.9987954562051724F, 0.9996988186962042F, 0.9999247018391445F,
        0.9999811752826011F, 0.9999952938095761F, 0.9999988234517018F,
        0.9999997058628822F, 0.9999999264657178F
    };

    static const float Icoef[14] =
    {
        0.0000000000000000F, -1.0000000000000000F, -0.7071067811865474F,
        -0.3826834323650897F, -0.1950903220161282F, -0.0980171403295606F,
        -0.0490676743274180F, -0.0245412285229122F, -0.0122715382857199F,
        -0.0061358846491544F, -0.0030679567629659F, -0.0015339801862847F,
        -0.0007669903187427F, -0.0003834951875714F
    };

    int nn = numPoints >> 1;
    int ie = numPoints;

    for (int n = 1; n <= logN; n++) 
    {
        float rw = Rcoef[logN - n];
        float iw = Icoef[logN - n];
        int in = ie >> 1;
        float ru = 1.0f;
        float iu = 0.0f;
        for (int j = 0; j < in; j++) 
        {
            for (int i = j; i < numPoints; i += ie) 
            {
                int io = i + in;
                float dRi = dataR[i];
                float dRio = dataR[io];
                float ri = result[i];
                float rio = result[io];
                dataR[i] = dRi + dRio;
                result[i] = ri + rio;
                float rtq = dRi - dRio;
                float itq = ri - rio;
                dataR[io] = rtq * ru - itq * iu;
                result[io] = itq * ru + rtq * iu;
            }
            float sr = ru;
            ru = ru * rw - iu * iw;
            iu = iu * rw + sr * iw;
        }
        ie >>= 1;
    }

    for (int j = 1, i = 1; i < numPoints; i++) 
    {
        if (i < j)
        {
            int io = i - 1;
            int in = j - 1;
            float rtp = dataR[in];
            float itp = result[in];
            dataR[in] = dataR[io];
            result[in] = result[io];
            dataR[io] = rtp;
            result[io] = itp;
        }

        int k = nn;

        while (k < j)
        {
            j = j - k;
            k >>= 1;
        }

        j = j + k;
    }

    for (int i = 0; i < 256; i++)
    {
        result[i] = std::sqrt(dataR[i] * dataR[i] + result[i] * result[i]);
    }

    result[0] = 0.0f;       // WARN нулевая составляющая мешает постоянно. надо её убрать

    Normalize(result, 256);

    if (SCALE_FFT_IS_LOG)
    {
        float minDB = FFTmaxDB::Abs();

        for (int i = 0; i < 256; i++)
        {
//#ifdef DEBUG
            result[i] = 20 * std::log10(result[i]);
//#else
//            result[i] = Log10[(int)(result[i] * 10000)];
//#endif
            if (i == FFT_POS_CURSOR_0)
            {
                *density0 = result[i];
            }
            else if (i == FFT_POS_CURSOR_1)
            {
                *density1 = result[i];
            }
            if (result[i] < minDB)
            {
                result[i] = minDB;
            }
            result[i] = 1.0f - result[i] / minDB;
        }
    }
    else
    {
        *density0 = result[FFT_POS_CURSOR_0];
        *density1 = result[FFT_POS_CURSOR_1];
    }
    *y0 = Grid::MathBottom() - result[FFT_POS_CURSOR_0] * Grid::MathHeight();
    *y1 = Grid::MathBottom() - result[FFT_POS_CURSOR_1] * Grid::MathHeight();
}

void Math::CalculateMathFunction(float *inAout, float *inB, int numPoints)
{
    if (MATH_FUNC_IS_SUM)
    {
        int delta = inB - inAout;
        float *end = &inAout[numPoints];

        while (inAout < end)
        {
            *inAout += *(inAout + delta);
            inAout++;
        }
    }
    else if (MATH_FUNC_IS_MUL)
    {
        int delta = inB - inAout;
        float *end = &inAout[numPoints];

        while (inAout < end)
        {
            *inAout *= *(inAout + delta);
            inAout++;
        }
    }
}

float Math::RandFloat(float min, float max)
{
    float delta = max - min;
    return min + ((std::rand() / (float)RAND_MAX) * delta);
}

int8 Math::AddInt8WithLimitation(int8 value, int8 delta, int8, int8 max)
{
    int8 retValue = value + delta;
    if (retValue < 0)
    {
        return 0;
    }
    if (retValue > max)
    {
        return max;
    }
    return retValue;
}

int Math::Sign(int value)
{
    if (value > 0)
    {
        return 1;
    }
    if (value < 0)
    {
        return -1;
    }
    return 0;
}

int Math::Pow10(int pow)
{
    int retValue = 1;

    while (pow--)
    {
        retValue *= 10;
    }

    return retValue;
}

int Math::NumDigitsInNumber(int value)
{
    value = Math::FabsInt(value);
    int num = 1;
    while ((value /= 10) > 0)
    {
        num++;
    }
    return num;
}

int Math::FabsInt(int value)
{
    return value >= 0 ? value : -value;
}

uint8 Math::GetMaxFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint)
{
    uint8 max = Math::GetMaxFromArray(data, firstPoint, lastPoint);

    if (max >= ValueFPGA::MAX)
    {
        max = ERROR_VALUE_UINT8;
    }

    return max;
}

uint8 Math::GetMinFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint)
{
    uint8 min = Math::GetMinFromArray(data, firstPoint, lastPoint);
    if (min < ValueFPGA::MIN || min >= ValueFPGA::MAX)
    {
        min = ERROR_VALUE_UINT8;
    }
    return min;
}

uint8 Math::GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint)
{
#define MIN_IF_LESS if(d < min) { min = d; }

    uint8 min = 255;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i++)
    {
        uint8 d = *pointer++;
        MIN_IF_LESS
    }

    return min;
}

uint8 Math::GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint)
{
#define MAX_IF_ABOVE if(d > max) { max = d; }

    uint8 max = 0;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i++)
    {
        uint8 d = *pointer++;
        MAX_IF_ABOVE;
    }

    return max;
}


void Math::CalculateFiltrArray(const uint8 *in, uint8 *out, int numPoints, int numSmoothing)
{
    if (numSmoothing < 2)
    {
        std::memcpy(out, in, (uint)numPoints);
    }
    else
    {
        bool addCalculation = (numSmoothing % 2) == 1;
        int endDelta = numSmoothing / 2;
        int startDelta = 1;
        int d = numSmoothing / 2;

        for (int i = 0; i < numPoints; i++)
        {
            int count = 1;
            int sum = in[i];

            for (int delta = startDelta; delta <= endDelta; delta++)
            {
                if (((i - delta) >= 0) && ((i + delta) < (numPoints)))
                {
                    sum += in[i - delta];
                    sum += in[i + delta];
                    count += 2;
                }
            }

            if (addCalculation)
            {
                if ((i + d) < numPoints)
                {
                    sum += in[i + d];
                    count++;
                }
            }

            out[i] = (uint8)(sum / count);
        }
    }
}


float ValueFPGA::ToVoltage(uint8 value, Range::E range, RShift rshift)
{
    return (((float)value - (float)ValueFPGA::MIN) * Range::voltsInPoint[range] - Range::MaxOnScreen(range) - rshift.ToAbs(range));
}


void ValueFPGA::ToVoltageArray(const uint8 *points, int numPoints, Range::E range, RShift rshift, float *voltage)
{
    for (int i = 0; i < numPoints; i++)
    {
        voltage[i] = ToVoltage(points[i], range, rshift);
    }
}


uint8 ValueFPGA::FromVoltage(float voltage, Range::E range, RShift rshift)
{
    int result = (voltage + Range::MaxOnScreen(range) + rshift.ToAbs(range)) / Range::voltsInPoint[range] + ValueFPGA::MIN;

    LIMITATION(result, result, 0, 255);

    return (uint8)result;
}


void ValueFPGA::FromVoltageArray(const float *voltage, int numPoints, Range::E range, RShift rshift, uint8 *points)
{
    for (int i = 0; i < numPoints; i++)
    {
        points[i] = FromVoltage(voltage[i], range, rshift);
    }
}


float RShift::ToAbs(Range::E range)
{
    return (-((float)RShift::ZERO - (float)(value)) * RShift::absStep[range]);
}


float TrigLev::ToAbs(Range::E range)
{
    return RShift(value).ToAbs(range);
}


template<class T>
void Math::AddLimitation(T *val, T delta, T min, T max)
{
    float sum = *val + delta;
    if (sum < min)
    {
        *val = min;
    }
    else if (sum > max)
    {
        *val = max;
    }
    else
    {
        *val = sum;
    }
}


template<class T>
T Math::CircleIncrease(T *val, T min, T max)
{
    (*val)++;

    if ((*val) > max)
    {
        (*val) = min;
    }

    return (*val);
}


template<class T>
T Math::CircleDecrease(T *val, T min, T max)
{
    (*val)--;

    if ((*val) < min)
    {
        (*val) = max;
    }

    return *val;
}


template<class T>
T Math::Max(T val1, T val2, T val3)
{
    float result = val1;

    if (val2 > result)
    {
        result = val2;
    }

    if (val3 > result)
    {
        result = val3;
    }

    return result;
}


template<class T>
bool Math::InRange(T value, T min, T max)
{
    return (value >= min) && (value <= max);
}


template<class T>
T Math::CircleAdd(T *val, T delta, T min, T max)
{
    *val += delta;
    if (*val > max)
    {
        *val = min;
    }
    return *val;
}
