#include "Sleep.h"

namespace rlc
{
    Sleep::Sleep(rlc::Hardware &hardware) : _hardware(hardware)
    {
        _ms_before_sleep = 0;
        _ms_after_sleep = 0;
    }

    void Sleep::mcu_delay_module_on(int sleep_time_ms)
    {
        mcu_delay(sleep_time_ms);
    }

    void Sleep::mcu_sleep_module_on(int sleep_time_ms)
    {
        mcu_sleep(sleep_time_ms);
    }

    void Sleep::mcu_deep_sleep_module_on(int sleep_time_ms)
    {
        mcu_deep_sleep(sleep_time_ms);
    }

    void Sleep::mcu_delay_module_lower_power_mode(int sleep_time_ms)
    {
        module_minimum_functionality();

        mcu_delay(sleep_time_ms);

        module_full_functionality();
    }

    void Sleep::mcu_sleep_module_lower_power_mode(int sleep_time_ms)
    {
        module_minimum_functionality();

        mcu_sleep(sleep_time_ms);

        module_full_functionality();
    }

    void Sleep::mcu_deep_sleep_module_lower_power_mode(int sleep_time_ms)
    {
        module_minimum_functionality();

        mcu_deep_sleep(sleep_time_ms);

        module_full_functionality();
    }

    void Sleep::mcu_delay_module_off(int sleep_time_ms)
    {
        module_turn_off();

        mcu_delay(sleep_time_ms);

        module_turn_on();
    }

    void Sleep::mcu_sleep_module_off(int sleep_time_ms)
    {
        module_turn_off();

        mcu_sleep(sleep_time_ms);

        module_turn_on();
    }

    void Sleep::mcu_deep_sleep_module_off(int sleep_time_ms)
    {
        module_turn_off();

        mcu_deep_sleep(sleep_time_ms);

        module_turn_on();
    }

    /*------------------------------------------------------------------------------------------
            PRIVATE METHODS
    ------------------------------------------------------------------------------------------*/

    void Sleep::mcu_delay(int sleep_time_ms)
    {
        int adjusted_sleep = pre_sleep(sleep_time_ms);
        if (adjusted_sleep > 0)
        {
            delay(adjusted_sleep);
        }
        post_sleep();
    }

    void Sleep::mcu_sleep(int sleep_time_ms)
    {
        int adjusted_sleep = pre_sleep(sleep_time_ms);
        if (adjusted_sleep > 0)
        {
            _hardware.end_serial_usb();
            LowPower.sleep(adjusted_sleep);
            _hardware.begin_serial_usb(100);
        }
        post_sleep();
    }

    void Sleep::mcu_deep_sleep(int sleep_time_ms)
    {
        int adjusted_sleep = pre_sleep(sleep_time_ms);
        if (adjusted_sleep > 0)
        {
            _hardware.end_serial_usb();
            LowPower.deepSleep(adjusted_sleep);
            _hardware.begin_serial_usb(100);
        }
        post_sleep();
    }

    void Sleep::module_minimum_functionality()
    {
        _ms_before_sleep = millis();
        _hardware.set_module_in_minimum_functionality_mode();
        _ms_before_sleep = millis() - _ms_before_sleep;
    }

    void Sleep::module_full_functionality()
    {
        _ms_after_sleep = millis();
        _hardware.set_module_in_full_functionality_mode();
        _hardware.init_module();
        _ms_after_sleep = millis() - _ms_after_sleep;
    }

    void Sleep::module_turn_off()
    {
        _ms_before_sleep = millis();
        _hardware.turn_off_module();
        _ms_before_sleep = millis() - _ms_before_sleep;
    }

    void Sleep::module_turn_on()
    {
        _ms_after_sleep = millis();
        _hardware.turn_on_module();
        _hardware.init_module();
        _ms_after_sleep = millis() - _ms_after_sleep;
    }

    int Sleep::pre_sleep(int sleep_time_ms)
    {
        int adjusted_sleep = sleep_time_ms - _ms_before_sleep - _ms_after_sleep;
        SerialUSB.println("Desired Sleep Interval (ms) = " + String(sleep_time_ms) + ", Actual sleep (ms) = " + String(adjusted_sleep) + ", Pre-nap (ms) = " + String(_ms_before_sleep) + ", Post-nap (ms) = " + String(_ms_after_sleep));
        SerialUSB.print("Good night...ZZZZZ");

        return adjusted_sleep;
    }

    void Sleep::post_sleep()
    {
        SerialUSB.println("...i'm awake!");
    }
}