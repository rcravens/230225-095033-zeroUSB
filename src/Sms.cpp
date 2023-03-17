#include "Sms.h"

namespace rlc
{
    Sms::Sms(AtCommand &command_helper) : _command_helper(command_helper)
    {
    }

    bool Sms::send(String number, String message)
    {
        //_command_helper.start_verbose();

        _command_helper.send_command_and_wait("AT+CMGF=1");

        _command_helper.send_data("AT+CMGS=\"" + number + "\"");

        _command_helper.send_data(message + "\r\x1A");

        //_command_helper.end_verbose();

        return true;
    }
}