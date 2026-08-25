// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#include "defines.h"
#include "Panel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include "common/Panel/Controls.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Utils/PasswordResolver.h"
#include <stm32f4xx_hal.h>


LED LED::Trig(LED::_Trig);
LED LED::RegSet(LED::_RegSet);
LED LED::ChanA(LED::_ChannelA);
LED LED::ChanB(LED::_ChannelB);
bool LED::dontFireTrig = false;


namespace Panel
{
    Key::E pressedKey = Key::None;
    volatile Key::E pressedButton = Key::None;      // Это используется для отслеживания нажатой кнопки при отключенной
                                                    // панели

    Queue<uint8, 64> data_for_send;                 // Здесь данные для пересылки в панель.

    Queue<KeyboardEvent, 10> input_buffer;          // Основной буфер событий. Отсюда панель берёт события для обработки.
                                                    // И лочит мютекс, чтобы сообщить прерыванию о том, что буфер занят
    Queue<KeyboardEvent, 10> aux_buffer;            // Вспомогательный буфер - сюда помещаются события, если во время
                                                    // прерывания идёт работа с основным буфером input_buffer
    bool isRunning = true;

    uint timeLastEvent = 0;

    namespace Processing
    {
        static const uint MIN_TIME = 500;


        static void ChangeRShift(TimeMeterMS *tMeter, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep);

        int CalculateCount(TimeMeterMS *tMeter)
        {
            uint delta = tMeter->ElapsedTime();
            tMeter->Reset();

            if (delta > 75)
            {
                return 1;
            }
            else if (delta > 50)
            {
                return 2;
            }
            else if (delta > 25)
            {
                return 3;
            }
            return 4;
        }

        bool CanChangeTShift(int16 tshift)
        {
            static uint time = 0;
            if (tshift == 0)
            {
                time = TIME_MS;
                return true;
            }
            else if (time == 0)
            {
                return true;
            }
            else if (TIME_MS - time > MIN_TIME)
            {
                time = 0;
                return true;
            }
            return false;
        }

        bool CanChangeRShiftOrTrigLev(TrigSource::E ch, int16 rshift)
        {
            static uint time[3] = {0, 0, 0};
            if (rshift == RShift::ZERO)
            {
                time[ch] = TIME_MS;
                return true;
            }
            else if (time[ch] == 0)
            {
                return true;
            }
            else if (TIME_MS - time[ch] > MIN_TIME)
            {
                time[ch] = 0;
                return true;
            }
            return false;
        }

        void ChangeRShift(TimeMeterMS *tMeter, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep)
        {
            int count = CalculateCount(tMeter);
            int rShiftOld = SET_RSHIFT(ch);
            int rshift = SET_RSHIFT(ch) + relStep * count;
            if ((rShiftOld > RShift::ZERO && rshift < RShift::ZERO) || (rShiftOld < RShift::ZERO && rshift > RShift::ZERO))
            {
                rshift = RShift::ZERO;
            }
            if (CanChangeRShiftOrTrigLev((TrigSource::E)ch, (int16)rshift))
            {
                f(ch, (int16)rshift);
            }
        }

        void ChangeTrigLev(TimeMeterMS *tMeter, void(*f)(TrigSource::E, int16), TrigSource::E trigSource, int16 relStep)
        {
            int count = CalculateCount(tMeter);
            int trigLevOld = TRIG_LEVEL(trigSource);
            int trigLev = TRIG_LEVEL(trigSource) + relStep * count;
            if ((trigLevOld > TrigLev::ZERO && trigLev < TrigLev::ZERO) || (trigLevOld < TrigLev::ZERO && trigLev > TrigLev::ZERO))
            {
                trigLev = TrigLev::ZERO;
            }
            if (CanChangeRShiftOrTrigLev(trigSource, (int16)trigLev))
            {
                f(trigSource, (int16)trigLev);
            }
        }

        void ChangeTShift(TimeMeterMS *tMeter, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(tMeter);
            int tshift_old = SET_TSHIFT;
            float step = relStep * count;

            if (step < 0)
            {
                if (step > -1)
                {
                    step = -1;
                }
            }
            else
            {
                if (step < 1)
                {
                    step = 1;
                }
            }

            int16 tshift = SET_TSHIFT + step;
            if (((tshift_old > 0) && (tshift < 0)) || (tshift_old < 0 && tshift > 0))
            {
                tshift = 0;
            }
            if (CanChangeTShift(tshift))
            {
                f(tshift);
            }
        }

        void ChangeShiftScreen(TimeMeterMS *tMeter, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(tMeter);

            float step = relStep * count;

            if (step < 0)
            {
                if (step > -1)
                {
                    step = -1;
                }
            }
            else if (step < 1)
            {
                step = 1;
            }

            f(step);
        }

        static void SetRShift(Chan::E ch, int16 rshift)
        {
            RShift::Set(ch, rshift);
        }



        static void SetTrigLev(TrigSource::E ch, int16 trigLev)
        {
            TrigLev::Set(ch, trigLev);
        }


        static void ShiftScreen(int shift)
        {
            Display::ShiftScreen(shift);
        }

        static void SetTShift(int tshift)
        {
            TShift::Set(tshift);
        }

        void XShift(int delta)
        {
            static TimeMeterMS tMeter;

            if (!FPGA::IsRunning() || SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
            {
                if (!SET_ENUM_POINTS_IS_281)
                {
                    ChangeShiftScreen(&tMeter, ShiftScreen, 2 * delta);
                }
            }
            else
            {
                ChangeTShift(&tMeter, SetTShift, (int16)delta);
            }
        }
    }


    static void SoundOnKeyboardEvent(KeyboardEvent event)
    {
        if (event.IsDown())
        {
            Sound::ButtonPress();
        }
        else if (event.IsUp() || event.IsLong())
        {
            Sound::ButtonRelease();
        }
        else if (event.IsLeft() || event.IsRight())
        {
            if (event.key == Key::RShiftA || event.key == Key::RShiftB || event.key == Key::TShift || event.key == Key::TrigLev)
            {
                Sound::RegulatorShiftRotate();
            }
            else
            {
                if (event.key != Key::Setting)
                {
                    Sound::RegulatorSwitchRotate();
                }
            }
        }
    }


    static void FuncNone(Action)
    {

    }

    static void OnFunctionalKey(Action action, Key::E key)
    {
        if (Hint::show)
        {
            if (Menu::IsShown())
            {
                Hint::SetItem(ItemsUnderKey::Get(key));
            }
        }
        else
        {
            if (action.IsDown())
            {
                Item::now_pressed = ItemsUnderKey::Get(key);
            }
            else if (action.IsUp() || action.IsLong())
            {
                if (Menu::IsShown())
                {
                    if (action.IsUp())
                    {
                        Menu::ExecuteFuncForShortPressOnItem(Item::now_pressed);
                    }
                    else if (action.IsLong())
                    {
                        Menu::ExecuteFuncForLongPressureOnItem(Item::now_pressed);
                    }

                    Item::now_pressed = nullptr;
                }
            }
        }
    }

    static void BFuncF1(Action action)
    {
        OnFunctionalKey(action, Key::F1);
    }

    static void BFuncF2(Action action)
    {
        OnFunctionalKey(action, Key::F2);
    }

    static void BFuncF3(Action action)
    {
        OnFunctionalKey(action, Key::F3);
    }

    static void BFuncF4(Action action)
    {
        OnFunctionalKey(action, Key::F4);
    }

    static void BFuncF5(Action action)
    {
        OnFunctionalKey(action, Key::F5);
    }

    static void BFuncChannelA(Action action)
    {
        if (action.IsUp())
        {
            if (Item::Opened() == PageChannelA::self && Menu::IsShown())
            {
                SET_ENABLED_A = !SET_ENABLED_A;
                PageChannelA::OnChanged_Input(true);
            }
            else
            {
                PageChannelA::self->SetCurrent(true);
                PageChannelA::self->Open(true);
                Menu::Show(true);
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            RShift::Set(Chan::A, RShift::ZERO);
        }
    }

    static void BFuncChannelB(Action action)
    {
        if (action.IsUp())
        {
            if (Item::Opened() == PageChannelB::self && Menu::IsShown())
            {
                SET_ENABLED_B = !SET_ENABLED_B;
                PageChannelB::OnChanged_Input(true);
            }
            else
            {
                PageChannelB::self->SetCurrent(true);
                PageChannelB::self->Open(true);
                Menu::Show(true);
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            RShift::Set(Chan::B, RShift::ZERO);
        }
    }

    static void BFuncTime(Action action)
    {
        if (action.IsUp())
        {
            PageTime::self->SetCurrent(true);
            PageTime::self->Open(true);
            Menu::Show(true);
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            TShift::Set(0);
        }
    }

    static void BFuncTrig(Action action)
    {
        if (action.IsUp())
        {
            PageTrig::self->SetCurrent(true);
            PageTrig::self->Open(true);
            Menu::Show(true);
        }
        else if (action.IsLong())
        {
            Flags::needFinishDraw = true;
            Menu::SetAutoHide(true);

            if (MODE_LONG_PRESS_TRIG_IS_LEVEL_ZERO)
            {
                TrigLev::Set(TRIG_SOURCE, TrigLev::ZERO);
            }
            else
            {
                TrigLev::FindAndSet();
            }
        }
    }

    static void BFuncCursors(Action action)
    {
        if (action.IsUp())
        {
            PageCursors::self->SetCurrent(true);
            PageCursors::self->Open(true);
            Menu::Show(true);
        }
    }

    static void BFuncMeasures(Action action)
    {
        if (action.IsUp())
        {
            PageMeasures::self->SetCurrent(true);
            PageMeasures::self->Open(true);
            Menu::Show(true);
        }
    }

    static void BFuncDisplay(Action action)
    {
        if (action.IsUp())
        {
            PageDisplay::self->SetCurrent(true);
            PageDisplay::self->Open(true);
            Menu::Show(true);
        }

    }

    static void BFuncHelp(Action action)
    {
        if (action.IsUp())
        {
            if(!Hint::show)
            {
                Hint::show = true;
                Hint::string = nullptr;
                Hint::item = nullptr;
            }
        }
        else if (action.IsLong())
        {
            Hint::show = !Hint::show;
            Hint::string = nullptr;
            Hint::item = nullptr;
        }
    }

    static void BFuncStart(Action action)
    {
        if (action.IsDown())
        {
            if (MODE_WORK_IS_DIRECT)
            {
                FPGA::OnPressStartStop();
            }
            else if (MODE_WORK_IS_MEMINT && !PageMemory::Resolver::exitFromIntToLast)
            {
                FPGA::OnPressStartStop();
            }
        }
    }

    static void BFuncMemory(Action action)
    {
        if (action.IsUp())
        {
            if (MODE_BTN_MEMORY_IS_SAVE && FDrive::isConnected)
            {
                PageMemory::SaveSignalToFlashDrive();
            }
            else
            {
                PageMemory::self->SetCurrent(true);
                PageMemory::self->Open(true);
                Menu::Show(true);
            }
        }
    }

    static void BFuncService(Action action)
    {
        if (action.IsUp())
        {
            PageService::self->SetCurrent(true);
            PageService::self->Open(true);
            Menu::Show(true);
        }
    }

    static void BFuncMenu(Action action)
    {
        if (action.IsUp())
        {
            if (!Menu::IsShown())
            {
                Menu::Show(true);
            }
            else
            {
                Item::CloseOpened();
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            if (Menu::AdditionPage::current)
            {
                Item::CloseOpened();
            }
            else
            {
                Menu::Show(!Menu::IsShown());
            }
        }
    }

    static void BFuncPower(Action)
    {
        if (TIME_MS < 2000)
        {
            return;
        }

        while (Item::Opened()->IsPage())
        {
            Page *page = (Page *)Item::Opened();

            if (page->IsSB())
            {
                Item::CloseOpened();
            }
            else
            {
                break;
            }
        }

        __disable_irq();
        gset.Save();
        __enable_irq();

        while (true)
        {
            Panel::TransmitData(0x05);
        }
    }

    static void RFuncRangeA(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncChannelA(action);
        }
        else if (action.IsLeft())
        {
            Range::Increase(Chan::A);
        }
        else if (action.IsRight())
        {
            Range::Decrease(Chan::A);
        }
    }

    static void RFuncRangeB(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncChannelB(action);
        }
        else if (action.IsLeft())
        {
            Range::Increase(Chan::B);
        }
        else if (action.IsRight())
        {
            Range::Decrease(Chan::B);
        }
    }

    static void RFuncRShiftA(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncChannelA(action);
        }
        else if (action.IsLeft())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::A, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::A, +RShift::STEP);
        }
    }

    static void RFuncRShiftB(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncChannelB(action);
        }
        else if (action.IsLeft())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::B, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::B, +RShift::STEP);
        }
    }

    static void RFuncTShift(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncTime(action);
        }
        else if (action.IsLeft())
        {
            Processing::XShift(-1);
        }
        else if (action.IsRight())
        {
            Processing::XShift(1);
        }
    }

    static void RFuncTBase(Action action)
    {
        if (action.IsActionButton())
        {
            RFuncTShift(action);
        }
        else
        {
            Sound::RegulatorSwitchRotate();

            if (action.IsLeft())
            {
                TBase::Increase();
            }
            else if (action.IsRight())
            {
                TBase::Decrease();
            }
        }
    }

    static void RFuncTrigLev(Action action)
    {
        if (action.IsActionButton())
        {
            BFuncTrig(action);
        }
        else if (action.IsLeft())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeTrigLev(&tMeter, Processing::SetTrigLev, TRIG_SOURCE, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeTrigLev(&tMeter, Processing::SetTrigLev, TRIG_SOURCE, +RShift::STEP);
        }
    }

    static void RFuncSetting(Action action)
    { 
        static const int step = 3;
        static int angle = 0;

        if (!Hint::show)
        {
            if (action.IsActionButton())
            {
                BFuncMenu(action);
            }
            else if (action.IsLeft() || action.IsRight())
            {
                angle = action.IsLeft() ? (angle - 1) : (angle + 1);

                if (angle != 0)
                {
                    if (Menu::IsShown() || !Item::Opened()->IsPage())
                    {
                        Item *item = Item::Current();
                        TypeItem::E type = item->GetType();

                        if (Item::Opened()->IsPage() && (type == TypeItem::ChoiceReg ||
                            type == TypeItem::Governor || type == TypeItem::IP || type == TypeItem::MAC))
                        {
                            if (angle > step || angle < -step)
                            {
                                Flags::needFinishDraw = true;
                                item->Change(angle);
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                        }
                        else
                        {
                            item = Item::Opened();
                            type = item->GetType();

                            if (Menu::IsMinimize())
                            {
                                Flags::needFinishDraw = true;
                                Page::RotateRegSetSB(angle);
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                            else if (type == TypeItem::Page || type == TypeItem::IP || type == TypeItem::MAC ||
                                type == TypeItem::Choice || type == TypeItem::ChoiceReg || type == TypeItem::Governor)
                            {
                                if (angle > step || angle < -step)
                                {
                                    item->ChangeOpened(angle);
                                    Sound::RegulatorSwitchRotate();
                                    Flags::needFinishDraw = true;
                                    angle = 0;
                                }
                            }
                            else if (type == TypeItem::GovernorColor)
                            {
                                Flags::needFinishDraw = true;
                                item->Change(angle);
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                            else if (type == TypeItem::Time)
                            {
                                Flags::needFinishDraw = true;
                                (angle > 0) ? ((TimeControl *)item)->IncCurrentPosition() : ((TimeControl *)item)->DecCurrentPosition();
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    static void (* const funcOnKey[Key::Count])(Action) =
    {
        FuncNone,
        BFuncF1,
        BFuncF2,
        BFuncF3,
        BFuncF4,
        BFuncF5,
        BFuncChannelA,
        BFuncChannelB,
        BFuncTime,
        BFuncTrig,
        BFuncCursors,
        BFuncMeasures,
        BFuncDisplay,
        BFuncHelp,
        BFuncStart,
        BFuncMemory,
        BFuncService,
        BFuncMenu,
        BFuncPower,

        RFuncRangeA,
        RFuncRangeB,
        RFuncRShiftA,
        RFuncRShiftB,
        RFuncTBase,
        RFuncTShift,
        RFuncTrigLev,
        RFuncSetting
    };
}


void Panel::ProcessEvent(KeyboardEvent event)
{
    if (!isRunning)
    {
        if (event.IsDown())
        {
            pressedButton = event.key;
        }

        return;
    }

    SoundOnKeyboardEvent(event);

    if (Hint::show && event.key != Key::Help && !event.IsFunctional())
    {
        Hint::ProcessButton(event.key);
    }
    else
    {
        funcOnKey[event.key](event.action);
    }

    Flags::needFinishDraw = true;
}


Key::E Panel::WaitPressingButton()
{
    Timer::PauseOnTime(500);

    input_buffer.Clear();

    pressedButton = Key::None;

    while (pressedButton == Key::None)
    {
        Panel::Update();
    };

    input_buffer.Clear();

    return pressedButton;
}


void Panel::TransmitData(uint8 data)
{
    static Queue<uint8, 10> buffer;

    if (data_for_send.mutex.Locked())
    {
        buffer.Push(data);
    }
    else
    {
        data_for_send.mutex.Lock();

        while (buffer.Size())
        {
            if (data_for_send.Size() < data_for_send.Capacity())
            {
                data_for_send.Push(buffer.Back());
            }
        }

        if (data_for_send.Size() < data_for_send.Capacity())
        {
            data_for_send.Push(data);
        }

        data_for_send.mutex.Unlock();
    }
}


uint16 Panel::Data::Next()
{
    uint16 result = 0;

    if (!data_for_send.mutex.Locked())
    {
        data_for_send.mutex.Lock();

        result = data_for_send.Back();

        data_for_send.mutex.Unlock();
    }

    return result;
}


bool Panel::Data::Exist()
{
    if (!data_for_send.mutex.Locked())
    {
        return data_for_send.Size() != 0;
    }

    return false;
}


void Panel::Disable()
{
    isRunning = false;
}


void Panel::Enable()
{
    isRunning = true;
}


void Panel::Init()
{
    LED::RegSet.Disable();
    LED::Trig.Disable();
}


void LED::Enable()
{
    uint8 data = (uint8)(type | 0x80);

    if (data != prev_data)
    {
        Panel::TransmitData(data);
        prev_data = data;
    }

    if (type == _Trig)
    {
        Display::SwitchTrigLabel(true);
    }
}


void LED::Disable()
{
    uint8 data = type;

    if (data != prev_data)
    {
        Panel::TransmitData(data);
        prev_data = data;
    }

    if (type == _Trig)
    {
        Display::SwitchTrigLabel(false);
    }
}


void LED::Switch(bool enable)
{
    enable ? Enable() : Disable();
}


void Panel::Callback::OnReceiveSPI5(const uint8 *data, uint)
{
    KeyboardEvent event(data);

    if (event.key != Key::None)
    {
        if (input_buffer.mutex.Locked())
        {
            aux_buffer.Push(event);
        }
        else
        {
            input_buffer.Push(event);
        }

        timeLastEvent = TIME_MS;

        Flags::needFinishRead = true;
    }
}


uint Panel::TimePassedAfterLastEvent()
{
    return TIME_MS - timeLastEvent;
}


void Panel::Update()
{
    input_buffer.mutex.Lock();

    while (!aux_buffer.Empty())
    {
        input_buffer.Push(aux_buffer.Back());
    }

    while (!input_buffer.Empty())
    {
        KeyboardEvent event = input_buffer.Back();

        PasswordResolver::ProcessEvent(event);

        ProcessEvent(event);
    }

    input_buffer.mutex.Unlock();
}
