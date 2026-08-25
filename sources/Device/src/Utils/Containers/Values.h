// 2021/03/24 10:56:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Text/String.h"


template<class T>
struct ValueStruct
{
    ValueStruct(T v = (T)0) : value(v), valid(true) {}
    T value;
    void SetValue(T v) { valid = true; value = v; }
    void SetInvalid() { valid = false; }
    bool IsValid() const { return valid; }
    bool operator <(const T rhs) const { return value < rhs; }
    operator T() const { return value; }
protected:
    bool valid;
};


struct Float : public ValueStruct<float>
{
    Float(float v = 0.0f) : ValueStruct(v) {}

    String<> ToText(bool always_sign, int num_digits);

    float Round() const;

private:

    int NumDigitsInIntPart(float val);

    // Удалить из buffer все цифровые символы кроме num_digits первых
    void RemoveDigits(String<> &string, int max_digits);

    void ReplacePoints(const String<> &string);
};


struct InvalidFloat : public Float
{
    InvalidFloat() : Float() { SetInvalid(); }
};


struct Int : public ValueStruct<int>
{
    Int(int v = 0) : ValueStruct(v) {}

    // Конструирует число из текстового представления. Строка должна заканчиваться нулём
    Int(pchar buffer);

    String<> ToText(bool always_sign = false, int num_min_fields = 1);
};


struct InvalidInt : public Int
{
    InvalidInt() : Int() { SetInvalid(); }
};


struct Uint8 : public ValueStruct<uint8>
{
    Uint8(uint8 v = 0) : ValueStruct(v) {}

    String<> ToStringBin();
    String<> ToStringHex();
};


struct Uint16 : public ValueStruct<uint16>
{
    Uint16(uint16 v = 0) : ValueStruct(v) {}

    String<> ToStringBin();
    String<> ToStringHex();
};


struct Uint32 : public ValueStruct<uint>
{
    Uint32(uint v = 0) : ValueStruct(v) {}

    String<> ToStringHex();
};


struct Int16 : public ValueStruct<int16>
{
    Int16(int16 v = 0) : ValueStruct(v) {}
};


struct InvalidInt16 : public Int16
{
    InvalidInt16() : Int16() { SetInvalid(); }
};


struct InvalidUint16 : public Uint16
{
    InvalidUint16() : Uint16() { SetInvalid(); }
};


struct Voltage : public Float
{
    Voltage(float v = 0.0f) : Float(v) {}

    String<> ToText(bool always_sign = true);
};


struct Time : public Float
{
    Time(float v = 0.0f) : Float(v) {}

    String<> ToText(bool always_sign = true);
};


struct Frequency : public Float
{
    Frequency(float v = 0.0f) : Float(v) {}

    String<> ToText();
};


struct Phase : public Float
{
    Phase(float v = 0.0f) : Float(v) {}

    String<> ToText();
};


struct Decibel : public Float
{
    Decibel(float v = 0.0f) : Float(v) {}

    String<> ToText(int num_digits);
};
