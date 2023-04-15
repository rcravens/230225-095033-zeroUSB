#ifndef GpsCalculator_h
#define GpsCalculator_h

#include <Arduino.h>
#include "GpsPoint.h"
#include "MyMath.h"
#include "Config.h"

namespace rlc
{
    class GpsCalculator
    {
    public:
        bool is_valid;

        double distance_in_miles;
        double distance_in_feet;

        double time_diff_in_seconds;
        double time_diff_in_hours;

        double velocity_in_miles_per_hour;
        double velocity_in_feet_per_second;
        
        float recommended_gps_refresh_period_sec;

        GpsCalculator(GpsPoint &pt1, GpsPoint &pt2);

        String to_string();

    private:
        double haversine_calculation(double lat1, double lng1, double lat2, double lng2, double earth_radius);
    };
}

#endif