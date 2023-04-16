#ifndef Sleep_h
#define Sleep_h

#include <Arduino.h>

#include "Config.h"
#ifdef atmelsam
#include <ArduinoLowPower.h>
#endif
#include "Hardware.h"
#include "Console.h"

namespace rlc
{
    class Sleep
    {
    public:
        Sleep(rlc::Hardware &hardware, rlc::Console &console);

        void mcu_delay_module_on(int sleep_time_ms);
        void mcu_sleep_module_on(int sleep_time_ms);
        void mcu_deep_sleep_module_on(int sleep_time_ms);

        void mcu_delay_module_lower_power_mode(int sleep_time_ms);
        void mcu_sleep_module_lower_power_mode(int sleep_time_ms);
        void mcu_deep_sleep_module_lower_power_mode(int sleep_time_ms);

        void mcu_delay_module_off(int sleep_time_ms);
        void mcu_sleep_module_off(int sleep_time_ms);
        void mcu_deep_sleep_module_off(int sleep_time_ms);

        String wakeup_reason();

    private:
        int pre_sleep(int sleep_time_ms);
        void post_sleep();
        void mcu_delay(int sleep_time_ms);
        void mcu_sleep(int sleep_time_ms);
        void mcu_deep_sleep(int sleep_time_ms);
        void module_minimum_functionality();
        void module_full_functionality();
        void module_turn_off();
        void module_turn_on();

        rlc::Hardware &_hardware;
        rlc::Console &_console;

        unsigned long _ms_before_sleep = 0;
        unsigned long _ms_after_sleep = 0;
    };
}

#endif