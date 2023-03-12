#include "AtCommand.h"

namespace rlc
{
    AtCommand::AtCommand(bool is_debug) : is_debug(is_debug)
    {
    }

    bool AtCommand::send_command_and_wait(String command)
    {
        return send_command_and_wait(command, "OK", 1000);
    }

    bool AtCommand::send_command_and_wait(String command, String desired_response, const int timeout)
    {
        bool is_desired_response = false;
        last_command_response = "";

        Serial1.println(command);

        // Watch for desired response
        //
        unsigned long time = millis();
        while ((time + timeout) > millis() && !is_desired_response)
        {
            while (Serial1.available())
            {
                char c = Serial1.read();
                last_command_response += c;
            }

            is_desired_response = last_command_response.indexOf(desired_response) >= 0;
        }

        // Read any final characters (new lines...)
        //
        while (Serial1.available())
        {
            char c = Serial1.read();
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

        if (this->is_debug)
        {
            SerialUSB.println("----> " + command + " <----");
            SerialUSB.println(last_command_response);
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
            SerialUSB.println();
            SerialUSB.println("Get a 1A, input a 0x1A");

            // Serial1.write(0x1A);
            Serial1.write(26);
            Serial1.println();
            return "";
        }

        Serial1.println(data);

        unsigned long time = millis();
        while ((time + timeout) > millis())
        {
            while (Serial1.available())
            {
                char c = Serial1.read();
                response += c;
            }
        }

        // Read any final characters (new lines...)
        //
        while (Serial1.available())
        {
            char c = Serial1.read();
            response += c;
        }

        if (this->is_debug)
        {
            SerialUSB.print(response);
        }

        return response;
    }

}