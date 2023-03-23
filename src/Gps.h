#ifndef Gps_h
#define Gps_h

#include <Arduino.h>
#include "AtCommand.h"
#include "GpsPoint.h"

namespace rlc
{
    class Gps
    {
    public:
        String location_data;
        GpsPoint last_gps_point;

        Gps(rlc::AtCommand &command_helper);
        bool current_location();
        bool current_location(const int timeout, bool is_close_session);

    private:
        rlc::AtCommand _command_helper;
    };
}

#endif