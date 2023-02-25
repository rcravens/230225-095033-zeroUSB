#include "Gps.h"

namespace rlc
{
    Gps::Gps(rlc::AtCommand command_helper) : _command_helper(command_helper)
    {
    }

    bool Gps::current_location()
    {
        return current_location(60000, false);
    }

    bool Gps::current_location(const int timeout, bool is_close_session)
    {
        bool is_refreshed = false;

        // ensure a gps session is started
        //
        _command_helper.send_command_and_wait("AT+CGPS?");
        if (_command_helper.last_command_response.indexOf("+CGPS: 0,1") >= 0)
        {
            if (!_command_helper.send_command_and_wait("AT+CGPS=1"))
            {
                return is_refreshed;
            }
        }

        int attempt = 0;
        unsigned long time = millis();
        while ((time + timeout) > millis())
        {
            attempt++;

            if (_command_helper.send_command_and_wait("AT+CGPSINFO"))
            {
                // Get new location data
                _command_helper.last_command_response.trim();
                if (_command_helper.last_command_response == "" || _command_helper.last_command_response.indexOf(",,,,,,,,") > 0)
                {
                    // No data....try again
                    // SerialUSB.println("No location data found");
                    delay(2000);
                    continue;
                }

                // SerialUSB.print("-----start processing-----[attempt=");
                // SerialUSB.print(attempt);
                // SerialUSB.println("]");

                int eol1 = _command_helper.last_command_response.indexOf('\n');
                int eol2 = _command_helper.last_command_response.indexOf('\n', eol1 + 1);
                location_data = _command_helper.last_command_response.substring(eol1 + 1, eol2);
                location_data.trim();

                // SerialUSB.println(response);
                // SerialUSB.println(gps_data);

                // SerialUSB.println("-----end processing-----");

                is_refreshed = true;
                break;
            }
        }

        // end the gps session
        //
        if (is_close_session)
        {
            _command_helper.send_command_and_wait("AT+CGPS=0");
        }

        return is_refreshed;
    }

}