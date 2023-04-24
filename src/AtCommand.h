#ifndef AtCommand_h
#define AtCommand_h

#include <Arduino.h>
#include "Console.h"

namespace rlc
{
    class AtCommand
    {
    public:
        String last_command_response;

        AtCommand(HardwareSerial &serial, rlc::Console &console, bool is_debug);

        bool begin(unsigned long timeout);
        void end();

        bool send_command_and_wait(String command, String desired_response, const int timeout);
        bool send_command_and_wait(String command);
        String send_data(String data, const int timeout);
        String send_data(String data);
        size_t write(String data, const int timeout);
        size_t write(const uint8_t *buffer, size_t size, const int timeout);
        void set_debug_for_next_command();
        void start_verbose();
        void end_verbose();
        void send_module_output_to_console_out();
        void send_console_input_to_module();

    private:
        HardwareSerial &_serial;
        rlc::Console &_console;
        String _from_usb = "";
        bool _is_debug;
    };
}

#endif