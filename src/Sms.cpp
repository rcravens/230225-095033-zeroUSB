#include "Sms.h"

namespace rlc
{
    Sms::Sms(AtCommand &command_helper) : _command_helper(command_helper)
    {
    }

    bool Sms::send(String number, String message)
    {
        // This is a Twilio Super SIM special SMS call that is configured
        //  to forward the text message to the URL configured in the
        //  Twilio console. You cannot send actual text messages using
        //  the Twilio Super SIM.
        //
        // It does not matter what number you put in, the message is only
        //  delivered to the configured URL.
        //
        // This has is no better than using the Http helper to send the message
        //

        //_command_helper.start_verbose();

        _command_helper.send_command_and_wait("AT+CMGF=1");

        _command_helper.send_data("AT+CMGS=\"" + number + "\"");

        _command_helper.send_data(message + "\r\x1A");

        //_command_helper.end_verbose();

        return true;
    }
}