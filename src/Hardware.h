#ifndef Hardware_h
#define Hardware_h

#include <Arduino.h>
#include <SD.h>
#include "AtCommand.h"
#include "Console.h"

namespace rlc
{
    class Hardware
    {
    public:
        String manufacturer;
        String model;
        String imei;

        Hardware(rlc::AtCommand &command_helper, rlc::Console &console, bool is_debug);
        void init();
        bool init_sd();
        bool init_module();

        void begin_console(unsigned long timeout);
        void end_console();

        void begin_serial_module();
        void end_serial_module();

        bool is_module_on();
        bool is_cellular_connected();
        bool is_cellular_connected(bool is_config_if_not);

        bool turn_on_module();
        bool turn_off_module();

        bool set_module_in_minimum_functionality_mode();
        bool set_module_in_full_functionality_mode();

        void send_module_output_to_console_out();
        void send_console_input_to_module();

        String to_http_post();

    private:
        rlc::AtCommand _command_helper;
        rlc::Console &_console;
        bool _is_debug;
        bool _is_serial_usb_on_init = false;
    };
}

#endif