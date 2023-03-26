#include "Config.h"

namespace rlc
{

    // The smallest voltage (v) where the board no longer functions
    //
    const float Config::battery_zero_point_voltage = 3.3;

    // The max battery voltage (v) when the battery is fully charged
    //
    const float Config::battery_max_voltage = 4.2;

    // The battery percent below which the battery is considered in low batter mode
    //
    const float Config::battery_low_mode_percent = 5.0;



    // The API URL where data will be HTTP POSTed to
    //
    const char Config::api_url[] = "https://webhook.site/6927a322-215e-4e0e-a625-ca21ee24a7e9";

    // The number of points that are cached before sending to the API
    //
    const unsigned long Config::api_num_gps_points_in_payload = 1;

    // Max number of points in each API payload
    //
    const int Config::api_max_points_per_post = 60;



    // The smallest refresh period (ms) for gps data collection. This is used as a lower bound for the computed recommendation.
    //
    const unsigned long Config::gps_refresh_period_smallest_ms = 1000;

    // The default refresh period (ms) for gps data collection. This is used as an upper bound for the computed recommendation.
    //
    const unsigned long Config::gps_refresh_period_default_ms = 5000;

    // The low battery mode refresh period (ms) for gps data collection
    //
    const unsigned long Config::gps_refresh_period_low_battery_ms = 3600000;

    // Ideal distance betwen points. This is used to compute a recommended refresh rate based on current velocity.
    //
    const float Config::gps_ideal_distance_between_points_feet = 150.0;

    // The minimum distance (ft) between the last cached point and a new point before it is cached
    //
    const float Config::gps_distance_threshold_feet = 10;

    // The maximum time (s) between the last cached point and the new point regardless of distance
    //
    const float Config::gps_max_time_threshold_seconds = 120;

}