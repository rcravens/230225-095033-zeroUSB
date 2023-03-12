#ifndef Sleep_h
#define Sleep_h

#include <Arduino.h>
#include <ArduinoLowPower.h>
#include "Hardware.h"

namespace rlc
{
    class Sleep
    {
    public:
        Sleep(rlc::Hardware &hardware);
        void mcu_delay_module_on(int sleep_time_ms);
        void mcu_sleep_module_on(int sleep_time_ms);
        void mcu_deep_sleep_module_on(int sleep_time_ms);
        void mcu_delay_module_lower_power_mode(int sleep_time_ms);
        void mcu_sleep_module_lower_power_mode(int sleep_time_ms);
        void mcu_deep_sleep_module_lower_power_mode(int sleep_time_ms);
        void mcu_delay_module_off(int sleep_time_ms);
        void mcu_sleep_module_off(int sleep_time_ms);
        void mcu_deep_sleep_module_off(int sleep_time_ms);

    private:
        int pre_sleep(int sleep_time_ms);
        void post_sleep();
        void mcu_delay(int sleep_time_ms);
        void mcu_sleep(int sleep_time_ms);
        void mcu_deep_sleep(int sleep_time_ms);
        void module_minium_functionality();
        void module_full_functionality();
        void module_turn_off();
        void module_turn_on();

        rlc::Hardware _hardware;

        unsigned long _ms_before_sleep = 0;
        unsigned long _ms_after_sleep = 0;
    };
}

#endif