#ifndef AtCommand_h
#define AtCommand_h

#include <Arduino.h>

namespace rlc
{
    class AtCommand
    {
    public:
        String last_command_response;
        
        AtCommand(bool is_debug);
        bool send_command_and_wait(String command, String desired_response, const int timeout);
        bool send_command_and_wait(String command);
        String send_data(String data, const int timeout);
        String send_data(String data);

    private:
        bool _is_debug;
    };
}

#endif