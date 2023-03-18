#ifndef Battery_h
#define Battery_h

#include <Arduino.h>
#include <ArduinoLowPower.h>
#include "Hardware.h"

namespace rlc
{
    class Battery
    {
    public:
        Battery(float zero_percent_voltage, float max_voltage, float low_battery_mode_percent);

        void refresh();

        unsigned long timestamp_ms() { return _millis; }
        int adc_value() { return _adc_value; }
        float voltage() { return _current_voltage; }
        float percentage() { return _current_percent; }
        bool is_low_battery_mode() { return _current_percent < _low_battery_mode_percent; }

        String to_csv();
        String to_string();
        String to_http_post();

    private:
        float _zero_percent_voltage;
        float _max_voltage;
        float _low_battery_mode_percent;

        unsigned long _millis;
        int _adc_value;
        float _current_voltage;
        float _current_percent;

        void sample_adc();
        void compute_percent();
    };
}

#endif