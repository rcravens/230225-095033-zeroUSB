#ifndef DateTime_h
#define DateTime_h

#include <Arduino.h>
#include <TimeLib.h>

namespace rlc
{
    class DateTime
    {
    public:
        bool is_valid;

        unsigned int year;
        unsigned int month;
        unsigned int day;
        unsigned int hour;
        unsigned int minute;
        unsigned int second;

        DateTime();
        DateTime(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);

        bool set(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);

        long to_seconds();

        long diff_in_seconds(DateTime &ref);

        String to_date_string();
        String to_time_string();
        String to_date_time_string();
        String to_timestamp_string();
        String month_string();
        String to_yyyymmddhhmmss();

    private:
        bool validate();
    };
}
#endif