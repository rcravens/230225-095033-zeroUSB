#ifndef Console_h
#define Console_h

#include <Arduino.h>
#include "Config.h"

namespace rlc
{
    class Console
    {
    public:
#ifdef atmelsam
        Console(Serial_ &console);
#endif
#ifdef espressif32
        Console(HWCDC &console);
#endif

        bool begin(unsigned long timeout);
        void end();

        int available();
        int read(void);

        size_t write(uint8_t);
        size_t write(const char *str);
        size_t print(char c);
        size_t print(const String &str);
        size_t println();
        size_t println(const String &str);

    private:
#ifdef atmelsam
        Serial_ &_console;
#endif
#ifdef espressif32
        HWCDC &_console;
#endif
    };
}

#endif