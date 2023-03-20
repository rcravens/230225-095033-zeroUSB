#ifndef GpsPoint_h
#define GpsPoint_h

#include <Arduino.h>
#include "DateTime.h"
#include "MyMath.h"

namespace rlc
{
    class GpsPoint
    {
    public:
        static double convert_nmea_to_degrees(String val_str);
        static GpsPoint from_gps_str(String &str);

        bool is_valid;
        double latitude;
        double longitude;
        double altitude;
        rlc::DateTime datetime;

        GpsPoint();
        GpsPoint(DateTime &datetime, double latitude, double longitude);
        GpsPoint(DateTime &datetime, double latitude, double longitude, double altitude);

        String to_string();

        double distance_in_miles(GpsPoint &from);

        bool copy(GpsPoint &pt);

    private:
        double haversine_calculation(double lat1, double lng1, double lat2, double lng2, double earth_radius);

        bool validate();
    };
}

#endif