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
        static GpsPoint from_nmea_str(String &str);

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
        bool validate();
    };
}

#endif