#ifndef Hardware_h
#define Hardware_h

#include <Arduino.h>
#include <SD.h>
#include "AtCommand.h"

namespace rlc
{
    class Hardware
    {
    public:
        String manufacturer;
        String model;
        String imei;

        Hardware(rlc::AtCommand command_helper);
        void init();
        bool init_sd();
        bool is_module_on();
        bool turn_on_module();
        bool turn_off_module();
        bool configure_module();
        void send_module_output_to_console_out();
        void send_console_input_to_module();

    private:
        rlc::AtCommand _command_helper;
        String _from_usb = "";
    };
}

#endif