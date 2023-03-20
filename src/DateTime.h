#ifndef DateTime_h
#define DateTime_h

#include <Arduino.h>

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

        String to_date_string();
        String to_time_string();
        String to_date_time_string();
        String month_string();

    private:
        bool validate();
    };
}

//+CGPSINFO:
// 4300.413127,N,
// 08932.250075,W,
// 200323,170018.0,  = DDMMYY,HHMMSS
// 342.7,0.0,55.5
#endif