// 2021/06/17 14:49:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once


struct USBD
{
    static void Init();
    static bool PrevSendingComplete();

    // Возвращет false, если засылка не удалась (не закончена предыдущая передача)
    static bool Transmit(uint8 *buffer, int size);
    static void Flush(uint8 *buffer, uint sizeBuffer);
    static void Wait();
    static void *handle;    // USBD_HandleTypeDef
};
