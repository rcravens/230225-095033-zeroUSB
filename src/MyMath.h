#ifndef MyMath_h
#define MyMath_h

#include <Arduino.h>

namespace rlc
{
    class MyMath
    {
    public:
        static const float earth_radius_in_miles;

        static double convert_degrees_to_radians(double degrees) { return degrees * PI / 180.0; }
        static double convert_meters_to_miles(double meters) { return meters / 1609.344; }
        static double convert_miles_to_feet(double miles) { return miles * 5280.0; }

        static double convert_seconds_to_milliseconds(double seconds) { return seconds * 1000.0; }
        static double convert_seconds_to_microseconds(double seconds) { return seconds * 1000000.0; }
    private:
    };
}

#endif