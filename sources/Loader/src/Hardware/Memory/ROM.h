// 2021/04/27 11:09:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "Utils/Containers/String.h"


struct DataSettings;
struct SettingsNRST;


namespace ROM
{
    static const uint SIZE = 2 * 1024 * 1024;

    // Функции для сохранения/загрузки настроек
    namespace Settings
    {
        // Возвращает указатель на хранящиеся в ROM несбрасываемые настройки. nullptr в случае, если настроек там нет
        template<class T>
        T *Load();

        // Сохраняет настройки из адреса sett в ROM
        template<class T>
        void Save(T *sett);
    };

    // Функция для сохранения/восстановления данных
    namespace Data
    {
        // \brief Число сохраняемых во внутреннем ППЗУ измерений. Пока ограничено количеством квадратиков, которые можно
        // вывести в одну линию внизу сетки.
        static const uint MAX_NUM_SAVED_WAVES = 23;

        // Если даннные есть, соответствующий элемент массива равен true.
        void GetInfo(bool existData[MAX_NUM_SAVED_WAVES]);

        void Save(uint numInROM, const DataSettings *ds);

        // Получает данные о сигнале номер num. Если сигнала нет, в ds записывается 0
        const DataSettings *Read(uint numInROM);

        void Erase(uint numInROM);

        void EraseAll();
    };
};
