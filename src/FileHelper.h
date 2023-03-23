#ifndef FileHelper_h
#define FileHelper_h

#include <Arduino.h>
#include <SD.h>

namespace rlc
{
    class FileHelper
    {
    public:
        String last_error;

        String strip_lines_from_top(String &file_name, int num_lines_to_strip);
        unsigned int line_count(String &file_name);
        unsigned int print_all_lines(String &file_name);
        unsigned int print_lines(String &file_name, unsigned int num_lines_to_print);
        bool copy(String &source_name, String &destination_name, const unsigned int skip);
        bool append(String &file_name, String &new_line);
        bool remove(String &file_name);
    private:

    };
}

#endif