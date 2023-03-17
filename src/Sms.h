#ifndef Sms_h
#define Sms_h

#include <Arduino.h>
#include <ArduinoLowPower.h>
#include "AtCommand.h"

namespace rlc
{
    class Sms
    {
    public:
        Sms(AtCommand &command_helper);
        
        bool send(String number, String message);
    private:
        AtCommand _command_helper;
    };
}

#endif