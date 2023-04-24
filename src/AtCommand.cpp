#include "AtCommand.h"

namespace rlc
{
    AtCommand::AtCommand(HardwareSerial &serial, rlc::Console &console, bool is_debug) : _serial(serial),
                                                                                         _console(console),
                                                                                         _is_debug(is_debug)
    {
    }

    bool AtCommand::begin(unsigned long timeout)
    {
#ifdef maduino
        _serial.begin(115200);
#endif
#ifdef tsimcam
        _serial.begin(115200, SERIAL_8N1, PCIE_RX_PIN, PCIE_TX_PIN);
#endif
        unsigned long now = millis();
        while ((now - millis()) < timeout && !_serial)
        {
        }

        return (bool)_serial;
    }

    void AtCommand::end()
    {
        if (_serial)
        {
            _serial.flush();
            _serial.end();
        }
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

        _serial.println(command);

        // Watch for desired response
        //
        unsigned long time = millis();
        while ((time + timeout) > millis() && !is_desired_response)
        {
            while (_serial.available())
            {
                char c = _serial.read();
                last_command_response += c;
            }

            is_desired_response = last_command_response.indexOf(desired_response) >= 0;
        }

        // Read any final characters (new lines...)
        //
        while (_serial.available())
        {
            char c = _serial.read();
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
            _console.println("----> " + command + " <----");
            _console.println(last_command_response);
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
        /*if (data.equals("1A") || data.equals("1a"))
        {
            _console.println();
            _console.println("Get a 1A, input a 0x1A");

            // _serial.write(0x1A);
            _serial.write(26);
            _serial.println();
            return "";
        }*/

        _serial.println(data);

        unsigned long time = millis();
        while ((time + timeout) > millis())
        {
            while (_serial.available())
            {
                char c = _serial.read();
                response += c;
            }
        }

        // Read any final characters (new lines...)
        //
        while (_serial.available())
        {
            char c = _serial.read();
            response += c;
        }

        if (_is_debug)
        {
            _console.print(response);
        }

        return response;
    }

    size_t AtCommand::write(String data, const int timeout)
    {
        size_t num_bytes = _serial.print(data);

        unsigned long time = millis();
        String response = "";
        while ((time + timeout) > millis())
        {
            while (_serial.available())
            {
                char c = _serial.read();
                response += c;
            }
        }

        // Read any final characters (new lines...)
        //
        while (_serial.available())
        {
            char c = _serial.read();
            response += c;
        }

        if (_is_debug)
        {
            _console.print(response);
        }

        return num_bytes;
    }

    size_t AtCommand::write(const uint8_t *buffer, size_t size, const int timeout)
    {
        size_t num_bytes = _serial.write(buffer, size);


        unsigned long time = millis();
        String response = "";
        while ((time + timeout) > millis())
        {
            while (_serial.available())
            {
                char c = _serial.read();
                response += c;
            }
        }

        // Read any final characters (new lines...)
        //
        while (_serial.available())
        {
            char c = _serial.read();
            response += c;
        }

        if (_is_debug)
        {
            _console.print(response);
        }

        return num_bytes;
    }

    void AtCommand::send_module_output_to_console_out()
    {
        while (_serial.available() > 0)
        {
            _console.write(Serial1.read());
            yield();
        }
    }

    void AtCommand::send_console_input_to_module()
    {
        _serial.available();
        while (_console.available() > 0)
        {
            int c = -1;
            c = _console.read();
            if (c != '\n' && c != '\r')
            {
                _from_usb += (char)c;
            }
            else
            {
                if (!_from_usb.equals(""))
                {
                    _console.println(_from_usb);
                    send_data(_from_usb, 1000);
                    _from_usb = "";
                }
            }
            yield();
        }
    }
}