// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SCALE_FFT                   (gset.math.scaleFFT)
#define SCALE_FFT_IS_LOG            (SCALE_FFT == ScaleFFT::Log)
#define SCALE_FFT_IS_LINEAR         (SCALE_FFT == ScaleFFT::Linear)

#define SOURCE_FFT                  (gset.math.sourceFFT)
#define SOURCE_FFT_IS_A             (SOURCE_FFT == SourceFFT::A)
#define SOURCE_FFT_IS_B             (SOURCE_FFT == SourceFFT::B)

#define WINDOW_FFT                  (gset.math.windowFFT)
#define WINDOW_FFT_IS_HAMMING       (WINDOW_FFT == WindowFFT::Hamming)
#define WINDOW_FFT_IS_BLACKMAN      (WINDOW_FFT == WindowFFT::Blackman)
#define WINDOW_FFT_IS_HANN          (WINDOW_FFT == WindowFFT::Hann)

#define FFT_MAX_DB                  (gset.math.fftMaxDB)

#define MATH_FUNC                   (gset.math.func)
#define MATH_FUNC_IS_SUM            (MATH_FUNC == Function::Sum)
#define MATH_FUNC_IS_MUL            (MATH_FUNC == Function::Mul)

#define FFT_CUR_CURSOR              (gset.math.currentCursor)
#define FFT_CUR_CURSOR_IS_0         (FFT_CUR_CURSOR == 0)

#define FFT_POS_CURSOR(num)         (gset.math.posCur[num])
#define FFT_POS_CURSOR_0            (FFT_POS_CURSOR(0))
#define FFT_POS_CURSOR_1            (FFT_POS_CURSOR(1))

#define ENABLED_FFT                 (gset.math.enableFFT)

#define MODE_DRAW_MATH              (gset.math.modeDraw)
#define DISABLED_DRAW_MATH          (MODE_DRAW_MATH == ModeDrawMath::Disable)
#define MODE_DRAW_MATH_IS_TOGETHER  (MODE_DRAW_MATH == ModeDrawMath::Together)
#define MODE_DRAW_MATH_IS_SEPARATE  (MODE_DRAW_MATH == ModeDrawMath::Separate)

#define MATH_MODE_REG_SET           (gset.math.modeRegSet)
#define MATH_MODE_REG_SET_IS_RSHIFT (MATH_MODE_REG_SET == ModeRegSet::RShift)
#define MATH_MODE_REG_SET_IS_RANGE  (MATH_MODE_REG_SET == ModeRegSet::Range)

#define MATH_MULTIPLIER             (gset.math.multiplier)




// ������� ����� ��� ����������� �������.
struct ScaleFFT
{
    enum E
    {
        Log,           // ��� �������� �������� ��������������� ����� ������ �������.
        Linear         // ��� �������� �������� �������� ����� ������ �������.
    };
};

// �������� ������� ��� ����������� �������.
struct SourceFFT
{
    enum E
    {
        A,            // ������ � ����������� ������� ������� ������.
        B,            // ������ � ����������� ������� ������� ������.
        Both          // ������ � ����������� �������� ����� �������.
    };
};

// ��� ���� ��� ��������� ������� ����� �������� �������.
struct WindowFFT
{
    enum E
    {
        Rectangle,    // ����� �������� ������� ������ �� ���������� �� ����.
        Hamming,      // ����� �������� ������� ������ ���������� �� ���� ��������.
        Blackman,     // ����� �������� ������� ������ ���������� �� ���� ��������.
        Hann          // ����� �������� ������� ������ ���������� �� ���� �����.
    };
};

// ����������� �������� �� ������������ ���������� ������� �������.
struct FFTmaxDB
{
    enum E
    {
        _40,            // ������������ ���������� �� -40��
        _60,            // ������������ ���������� �� -60��
        _80             // ������������ ���������� �� -80��
    };

    static float Abs();
};

// ����� ������� ������������.
struct Function
{
    enum E
    {
        Sum,           // ����� ���� �������.
        Mul            // ������������ ���� �������.
    };
};


// ��� ������������ �������������� ������.
struct ModeDrawMath
{
    enum E
    {
        Disable,   // ����������� ��������������� ������� ���������.
        Separate,  // �������������� ������ ������������ �� ��������� ������.
        Together   // �������������� ������ ������������ ������ �������� �������.
    };
};


// ��� ��������� ����� ���������, ����� �������� �������������� �������.
struct ModeRegSet
{
    enum E
    {
        Range,       // ����� ��������� ��������� ��������� �� ����������.
        RShift       // ����� ��������� ��������� ��������� �� ����������.
    };
};


struct SettingsMath
{
    ScaleFFT::E     scaleFFT;       // ������� ����� ��� ����������� �������.
    SourceFFT::E    sourceFFT;      // �������� ������� ��� ������� � ����������� �������.
    WindowFFT::E    windowFFT;      // ����� ��� ����, �� ������� ���������� ������ ����� �������� �������.
    FFTmaxDB::E     fftMaxDB;       // ����������� �������� �� ������������ ���������� ������� �������.
    Function::E     func;           // ���������� �������������� �������.
    uint8           currentCursor;  // ����������, ����� �������� ������� ��������� ����� ���������.
    uint8           posCur[2];      // ������� ������� �������. ���������� 0...256.
    int8            koeff1add;      // ����������� ��� ������ ��������� ��� ��������.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;      // \c true, ���� ������� ������.
    ModeDrawMath::E modeDraw;       // ���������� ��� ����� ������� � ������ �������������� �������.
    ModeRegSet::E   modeRegSet;     // ������� ����� ��������� - ������� �� ������� ��� �������� �� ���������.
    Range::E        range;          // ������� �� ���������� ��������������� �������.
    Divider::E      multiplier;     // ��������� �������� ��������������� �������.
    RShift          rshift;         // �������� �� ������������ ��� ��������������� �������.
};
