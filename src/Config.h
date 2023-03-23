#ifndef Config_h
#define Config_h

#include <Arduino.h>

namespace rlc
{
    class Config
    {
    public:
        static const float battery_zero_point_voltage;
        static const float battery_max_voltage;
        static const float battery_low_mode_percent;

        static const char api_url[];
        static const unsigned long api_num_gps_points_in_payload;
        static const int api_max_points_per_post;

        static const unsigned long gps_refresh_period_smallest_ms;
        static const unsigned long gps_refresh_period_default_ms;
        static const unsigned long gps_refresh_period_low_battery_ms;
        static const float gps_distance_threshold_feet;
        static const float gps_max_time_threshold_seconds;


    private:
    };
}

#endif