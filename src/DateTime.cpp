#include "DateTime.h"

namespace rlc
{
    DateTime::DateTime() : DateTime(0, 0, 0, 0, 0, 0)
    {
    }

    DateTime::DateTime(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second) : year(year),
                                                                                                                                               month(month),
                                                                                                                                               day(day),
                                                                                                                                               hour(hour),
                                                                                                                                               minute(minute),
                                                                                                                                               second(second)
    {
    }

    bool DateTime::set(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second)
    {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = second;

        return validate();
    }

    long DateTime::to_seconds()
    {
        tmElements_t tm;
        tm.Year = year - 1970;
        tm.Month = month;
        tm.Day = day;
        tm.Hour = hour;
        tm.Minute = minute;
        tm.Second = second;

        return makeTime(tm);
    }

    String DateTime::to_yyyymmddhhmmss() 
    {
        char buffer[16];
        sprintf(buffer, "%04d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second);
        return String(buffer);
    }

    long DateTime::diff_in_seconds(DateTime &ref)
    {
        return to_seconds() - ref.to_seconds();
    }

    String DateTime::to_date_string()
    {
        if (!is_valid)
        {
            return "Invalid Date";
        }

        return month_string() + " " + String(day) + ", " + String(year);
    }

    String DateTime::to_time_string()
    {
        if (!is_valid)
        {
            return "Invalid Date";
        }

        unsigned int h = hour <= 12 ? hour : hour - 12;
        String am_pm = hour < 12 ? " AM" : " PM";
        return String(h) + ":" + String(minute) + ":" + String(second) + am_pm;
    }

    String DateTime::to_date_time_string()
    {
        if (!is_valid)
        {
            return "Invalid Date";
        }

        return to_date_string() + " " + to_time_string();
    }

    String DateTime::to_timestamp_string()
    {
        if (!is_valid)
        {
            return "Invalid Date";
        }

        return String(year) + "-" + String(month) + "-" + String(day) + " " + String(hour) + ":" + String(minute) + ":" + String(second);
    }

    String DateTime::month_string()
    {
        switch (month)
        {
        case 1:
            return "January";
        case 2:
            return "February";
        case 3:
            return "March";
        case 4:
            return "April";
        case 5:
            return "May";
        case 6:
            return "June";
        case 7:
            return "July";
        case 8:
            return "August";
        case 9:
            return "September";
        case 10:
            return "October";
        case 11:
            return "November";
        case 12:
            return "December";

        default:
            return "Invalid Month";
        }
    }

    /*------------------------------------------------------------------------------------------
            PRIVATE METHODS
    ------------------------------------------------------------------------------------------*/
    bool DateTime::validate()
    {
        is_valid = true;

        if (month < 1 || month > 12)
        {
            is_valid = false;
        }

        if (day < 1 || day > 31)
        {
            is_valid = false;
        }

        bool is_leap_year = year % 4 == 0;
        unsigned int max_feb_days = is_leap_year ? 29 : 28;
        if (month == 2 && day > max_feb_days)
        {
            is_valid = false;
        }

        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        {
            is_valid = false;
        }

        return is_valid;
    }

}