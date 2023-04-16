#include "Sleep.h"

namespace rlc
{
    Sleep::Sleep(rlc::Hardware &hardware, rlc::Console &console) : _hardware(hardware),
                                                                   _console(console)
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

    String Sleep::wakeup_reason()
{
#ifdef maduino
    return "Reason unknown on maduino";
#endif
#ifdef tsimcam
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        return "Wakeup caused by external signal using RTC_IO";
    case ESP_SLEEP_WAKEUP_EXT1:
        return "Wakeup caused by external signal using RTC_CNTL";
    case ESP_SLEEP_WAKEUP_TIMER:
        return "Wakeup caused by timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        return "Wakeup caused by touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
        return "Wakeup caused by ULP program";
    default:
        return "Wakeup was not caused by deep sleep: " + String(wakeup_reason);
    }
#endif
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
            _hardware.end_console();
#ifdef maduino
            LowPower.sleep(adjusted_sleep);
#endif
#ifdef tsimcam
            // TODO: investigate esp32 sleep options
            uint64_t sleep_time_us = adjusted_sleep * 1000;
            esp_sleep_enable_timer_wakeup(sleep_time_us);
            esp_light_sleep_start();
#endif
            _hardware.begin_console(100);
        }
        post_sleep();
    }

    void Sleep::mcu_deep_sleep(int sleep_time_ms)
    {
        int adjusted_sleep = pre_sleep(sleep_time_ms);
        if (adjusted_sleep > 0)
        {
            _hardware.end_console();
#ifdef maduino
            LowPower.deepSleep(adjusted_sleep);
#endif
#ifdef tsimcam
            // TODO: investigate esp32 deep sleep options
            uint64_t sleep_time_us = uint64_t(adjusted_sleep) * 1000;
            esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
            esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
            esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
            esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
            esp_sleep_enable_timer_wakeup(sleep_time_us);
            esp_deep_sleep_start();
#endif
            _hardware.begin_console(100);
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
        _console.println("Desired Sleep Interval (ms) = " + String(sleep_time_ms) + ", Actual sleep (ms) = " + String(adjusted_sleep) + ", Pre-nap (ms) = " + String(_ms_before_sleep) + ", Post-nap (ms) = " + String(_ms_after_sleep));
        _console.print("Good night...ZZZZZ");

        return adjusted_sleep;
    }

    void Sleep::post_sleep()
    {
        _console.println("...i'm awake!");
    }
}