#ifndef Http_h
#define Http_h

#include <Arduino.h>
#include "AtCommand.h"

namespace rlc
{
    class Http
    {
    public:
        Http(rlc::AtCommand &command_helper);
        bool post(String url, String content, String content_type);
 
    private:
        rlc::AtCommand _command_helper;
    };
}

#endif