// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Hardware/VCP/VCP.h"
#include "Hardware/LAN/LAN.h"
#include "Utils/Text/Text.h"


#define FIRST_SYMBOLS(word) (FirstSymbols(&buffer, word))

#define IF_REQUEST(sequence) if(FirstSymbols(&buffer, "?")) { sequence; }

#define SCPI_CYCLE(func)                                \
    const MapElement *it = map;                         \
    while (it->key)                                     \
    {                                                   \
        if FIRST_SYMBOLS(it->key) { func; break; }      \
        it++;                                           \
    }


namespace SCPI
{
    struct MapElement
    {
        char *key;
        uint8 value;

        uint8 GetValue(Word *key) const;      // Если значение не найдено, возвращеется 255;
    };

    struct StructCommand
    {
        typedef pchar(*pFuncCC)(pchar);

        char    *name;
        pFuncCC  func;
    };

    void AppendNewData(const void *buffer, int length);
    void Update();

    pchar ProcessingCommand(const StructCommand *, pchar);

    // Возвращает true, если первые символы в buffer повторяют word (без учёта завершающего нуля).
    // В этом случае записывает по *buffer адрес следующего за одинаоковыми символами
    bool FirstSymbols(pchar *buffer, pchar word);

    pchar ProcessDISPLAY(pchar);
    pchar ProcessCHANNEL(pchar);
    pchar ProcessTRIG(pchar);
    pchar ProcessTBASE(pchar);
    pchar ProcessSpeedTest(pchar);

    namespace COMMON
    {
        pchar IDN(pchar);
        pchar RUN(pchar);
        pchar STOP(pchar);
        pchar RESET(pchar);
        pchar AUTOSCALE(pchar);
        pchar REQUEST(pchar);
    };

    namespace CHANNEL
    {
        pchar INPUT(pchar);
        pchar COUPLE(pchar);
        pchar FILTR_(pchar);
        pchar INVERSE(pchar);
        pchar RANGE_(pchar);
        pchar OFFSET(pchar);
        pchar FACTOR(pchar);
    };

    namespace CONTROL
    {
        pchar KEY(pchar);
        pchar GOVERNOR(pchar);
    };

    namespace DISPLAY
    {
        pchar AUTOSEND(pchar);
        pchar MAPPING(pchar);
        pchar ACCUM(pchar);
        pchar ACCUM_NUMBER(pchar);
        pchar ACCUM_MODE(pchar);
        pchar ACCUM_CLEAR(pchar);
        pchar AVERAGE(pchar);
        pchar AVERAGE_NUMBER(pchar);
        pchar AVERAGE_MODE(pchar);
        pchar MINMAX(pchar);
        pchar FILTR_(pchar);
        pchar FPS(pchar);
        pchar WINDOW(pchar);
        pchar GRID(pchar);
        pchar GRID_TYPE(pchar);
        pchar GRID_BRIGHTNESS(pchar);
    };

    namespace TBASE_
    {
        pchar RANGE_(pchar);
        pchar OFFSET(pchar);
        pchar SAMPLING(pchar);
        pchar PEACKDET(pchar);
        pchar TPOS_(pchar);
        pchar SELFRECORDER(pchar);
        pchar FUNCTIMEDIV(pchar);
    };

    namespace TRIGGER
    {
        pchar MODE(pchar);
        pchar SOURCE(pchar);
        pchar POLARITY(pchar);
        pchar INPUT(pchar);
        pchar FIND(pchar);
        pchar OFFSET(pchar);
    };
};
