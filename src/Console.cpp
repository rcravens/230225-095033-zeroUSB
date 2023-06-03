#include "Console.h"

namespace rlc
{
    #ifdef maduino
    Console::Console(Serial_ &console) : _console(console) {
    }
    #endif
    #ifdef tsimcam
    Console::Console(HWCDC &console) : _console(console) {
    }
    #endif

    bool Console::begin(unsigned long timeout) {
        _console.begin(115200);
        unsigned long now = millis();
        while ((now - millis()) < timeout && !_console) {
        }

        return (bool)_console;
    }

    void Console::end() {
        if (_console) {
            _console.flush();
            _console.end();
        }
    }

    int Console::available() {
        return _console.available();
    }

    int Console::read(void) {
        return _console.read();
    }

    size_t Console::write(uint8_t val) {
        return _console.write(val);
    }

    size_t Console::write(const char *str) {
        return _console.write(str);
    }

    size_t Console::print(char c) {
        return _console.print(c);
    }

    size_t Console::print(const String &str) {
        return _console.print(str);
    }

    size_t Console::println() {
        return _console.println();
    }

    size_t Console::println(const String &str) {
        return _console.println(str);
    }
}
