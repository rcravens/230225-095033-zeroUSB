#include "Http.h"

namespace rlc
{
    Http::Http(rlc::AtCommand command_helper) : _command_helper(command_helper)
    {
    }

    bool Http::post(String url, String content, String content_type)
    {
        bool is_sent = false;
        // this line ensures that any previous sessions area terminated
        //
        _command_helper.send_command_and_wait("AT+HTTPTERM");

        if (_command_helper.send_command_and_wait("AT+HTTPINIT"))
        {
            if (_command_helper.send_command_and_wait("AT+HTTPPARA=\"URL\",\"" + url + "\""))
            {
                if (_command_helper.send_command_and_wait("AT+HTTPPARA=\"CONTENT\",\"" + content_type + "\""))
                {
                    if (_command_helper.send_command_and_wait("AT+HTTPDATA=" + String(content.length()) + ",5000", "DOWNLOAD", 1000))
                    {
                        _command_helper.send_data(content + "\r\n\r\n");

                        _command_helper.send_command_and_wait("AT+HTTPACTION=1", "HTTP_PEER_CLOSED", 10000);

                        is_sent = true;
                    }
                }
            }
            _command_helper.send_command_and_wait("AT+HTTPTERM");
        }

        return is_sent;
    }
}