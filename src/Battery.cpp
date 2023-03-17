#include "Battery.h"

namespace rlc
{
    Battery::Battery(float zero_percent_voltage, float max_voltage, float low_battery_mode_percent) : _zero_percent_voltage(zero_percent_voltage), _max_voltage(max_voltage)
    {
        pinMode(LED_BUILTIN, OUTPUT);

        analogReference(AR_DEFAULT);

        analogReadResolution(12);

        refresh();
    }

    String Battery::to_csv()
    {
        return String(_millis) + "," + String(_adc_value) + "," + String(_current_voltage) + "," + String(_current_percent);
    }

    String Battery::to_string()
    {
        return "Battery Miilis=" + String(_millis) + ", ADC Value=" + String(_adc_value) + ", Voltage=" + String(_current_voltage) + ", Percent=" + String(_current_percent);
    }

    String Battery::to_http_post()
    {
        return "battery_millis=" + String(_millis) + "&battery_adc_value=" + String(_adc_value) + "&battery_voltage=" + String(_current_voltage) + "&battery_percent=" + String(_current_percent);
    }

    void Battery::refresh()
    {
        _millis = millis();

        sample_adc();

        compute_percent();
    }

    void Battery::sample_adc()
    {
        // Maduino board has a voltage divider (2 1Mohm resisters) connected to the batter and the center point connected to A1
        //
        _adc_value = analogRead(A1); // 0 to 4095 corresponding to 0 to _battery_voltage_ref/2

        // The voltage divider would result in an ideal factor = 2, but since there is 1% tolerance on the resistors
        //  the actual factor is not exactly 2. Here we compute an actual factor. This is board dependent
        //  and probably should just default to value of 2.0
        //
        // 3.88V multimeter = 2426 adc value
        //
        // float factor = 2.0 // use the default
        float factor = 1.984636; // 3.88 * 4095 / (_max_voltage * 2426);

        float adc_reference_voltage = 3.3; // TODO: does droop as the battery voltage drops?

        _current_voltage = factor * _adc_value * adc_reference_voltage / 4095;
    }

    void Battery::compute_percent()
    {
        // Based on the following
        //
        //  0 percent when current_voltage = zero_percent_voltage
        //  100 percent when current_voltage = max_voltage
        //
        /*
        Notes:
            LP603449 3.7V, 1100mAh, 4.1Wh batter
                - max voltage after charging = 4.2
                - min voltage before shut off = 3.28

            AP7361C-33FGE (voltage regulator)
                - output voltage = 3.3V
                - max output current = 1A
                - max input voltage = 6V
                - drop out voltage = 0.36V
                    ---> min input voltage = 3.3 + 0.36 = 3.66 V
                    based on above then the predicted adc cutoff value = 3.66 * 4095 / (1.984636 * 3.3) = 2288.4

            measured cutoff = 2044  ---> 3.26V

        */

        _current_percent = 100 * (_current_voltage - _zero_percent_voltage) / (_max_voltage - _zero_percent_voltage);

        if (_current_percent < 0)
        {
            _current_percent = 0;
        }
        if (_current_percent > 100)
        {
            _current_percent = 100;
        }
    }
}