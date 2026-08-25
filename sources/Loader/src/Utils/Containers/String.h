// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct Color;


class String
{
public:
    explicit String();
    String(const String &);
    explicit String(char t);
    explicit String(pchar, ...);
    virtual ~String();

    void SetFormat(pchar format, ...);
    void SetString(const String &);
    void SetString(pchar);

    char *c_str() const { return buffer; }

    static pstring _ERROR;

    void Free();

    void Append(pchar str);
    void Append(pchar str, int numSymbols);
    void Append(char);
    void Append(const String &);

    int Size() const;

    bool ToInt(int *out);

    char &operator[](int i) const;

    String &operator=(const String &);

private:

    bool Allocate(int size);

    int NeedMemory(int size);

    char *buffer;

    int capacity;

    static const int SIZE_SEGMENT = 32;
};
