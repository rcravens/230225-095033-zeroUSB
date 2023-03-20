#include "AtCommand.h"

namespace rlc
{
    AtCommand::AtCommand(Uart at_serial, Serial_ monitor_serial, bool is_debug) : _at_serial(at_serial), _monitor_serial(monitor_serial), _is_debug(is_debug)
    {
    }

    void AtCommand::start_verbose()
    {
        _is_debug = true;
    }

    void AtCommand::end_verbose()
    {
        _is_debug = false;
    }

    bool AtCommand::send_command_and_wait(String command)
    {
        return send_command_and_wait(command, "OK", 1000);
    }

    bool AtCommand::send_command_and_wait(String command, String desired_response, const int timeout)
    {
        bool is_desired_response = false;
        last_command_response = "";

        _at_serial.println(command);

        // Watch for desired response
        //
        unsigned long time = millis();
        while ((time + timeout) > millis() && !is_desired_response)
        {
            while (_at_serial.available())
            {
                char c = _at_serial.read();
                last_command_response += c;
            }

            is_desired_response = last_command_response.indexOf(desired_response) >= 0;
        }

        // Read any final characters (new lines...)
        //
        while (_at_serial.available())
        {
            char c = _at_serial.read();
            last_command_response += c;
        }

        // Strip out command and desired response
        //
        last_command_response.replace(command, "");

        if (is_desired_response)
        {
            last_command_response.replace(desired_response, "");
        }

        last_command_response.trim();

        if (_is_debug)
        {
            _monitor_serial.println("----> " + command + " <----");
            _monitor_serial.println(last_command_response);
        }

        return is_desired_response;
    }

    String AtCommand::send_data(String data)
    {
        return send_data(data, 1000);
    }

    String AtCommand::send_data(String data, const int timeout)
    {
        String response = "";
        if (data.equals("1A") || data.equals("1a"))
        {
            _monitor_serial.println();
            _monitor_serial.println("Get a 1A, input a 0x1A");

            // Serial1.write(0x1A);
            _at_serial.write(26);
            _at_serial.println();
            return "";
        }

        _at_serial.println(data);

        unsigned long time = millis();
        while ((time + timeout) > millis())
        {
            while (_at_serial.available())
            {
                char c = _at_serial.read();
                response += c;
            }
        }

        // Read any final characters (new lines...)
        //
        while (_at_serial.available())
        {
            char c = _at_serial.read();
            response += c;
        }

        if (_is_debug)
        {
            _monitor_serial.print(response);
        }

        return response;
    }

}