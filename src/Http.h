#ifndef Http_h
#define Http_h

#include <Arduino.h>
#include "AtCommand.h"

namespace rlc
{
    class Http
    {
    public:
        Http(rlc::AtCommand &command_helper, rlc::Console &console);
        bool post(String url, String content, String content_type);
        bool post_file_buffer(String url, const uint8_t *buf, size_t size);
 
    private:
        rlc::AtCommand _command_helper;
        rlc::Console _console;
    };
}

#endif