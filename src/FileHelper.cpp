#include "FileHelper.h"

namespace rlc
{
    FileHelper::FileHelper(rlc::Console &console, bool is_debug) : _console(console), _is_debug(is_debug)
    {
    }

    unsigned int FileHelper::line_count(String &file_name)
    {
        last_error = "";

        unsigned int line_count = 0;

        if (SD.exists(file_name))
        {
            File gps_data_file = SD.open(file_name);
            if (!gps_data_file)
            {
                last_error = "Failed to open gps_data.csv file";
                return line_count;
            }

            char current_char = ' ';
            while (gps_data_file.available() > 0)
            {
                current_char = gps_data_file.read();
                if (current_char == '\n')
                {
                    line_count++;
                }
            }
            gps_data_file.close();
        }

        return line_count;
    }

    String FileHelper::strip_lines_from_top(String &file_name, int num_lines_to_strip)
    {
        last_error = "";

        String lines = "";

        if (!SD.exists(file_name))
        {
            last_error = "Source file does not exist";
            return lines;
        }

        File gps_data_file = SD.open(file_name);
        if (!gps_data_file)
        {
            last_error = "Failed to open gps_data.csv file";
            return lines;
        }
        int skip = 0;
        int num_lines = 0;
        String current_line = "";
        char current_char = ' ';
        while ((gps_data_file.available() > 0) && (num_lines < num_lines_to_strip))
        {
            current_char = gps_data_file.read();
            current_line += current_char;
            skip++;

            if (current_char == '\n')
            {
                current_line.trim();
                lines += current_line + "\n";
                num_lines++;

                current_line = "";
            }
        }
        gps_data_file.close();

        // SerialUSB.println("num_lines=" + String(num_lines) + ", skip=" + String(skip));
        // SerialUSB.println(post_data);

        if (num_lines == num_lines_to_strip)
        {
            // copy the file but skip the above lines
            //
            String temp_file_name = "temp.csv";
            copy(file_name, temp_file_name, skip);

            // copy file back to original filename
            //
            copy(temp_file_name, file_name, 0);
        }
        else
        {
            // At the end of the file
            //
            remove(file_name);
        }

        return lines;
    }

    unsigned int FileHelper::print_all_lines(String &file_name)
    {
        return print_lines(file_name, UINT16_MAX);
    }

    unsigned int FileHelper::print_lines(String &file_name, unsigned int num_lines_to_print)
    {
        unsigned int num_lines = 0;
        char c = ' ';
        if (SD.exists(file_name))
        {
            File gps_data = SD.open(file_name);
            if (gps_data)
            {
                _console.println("-------FILE BEGIN-------");
                num_lines = 0;
                while (gps_data.available() && num_lines < num_lines_to_print)
                {
                    c = gps_data.read();

                    _console.print(c);

                    if (c == '\n')
                    {
                        num_lines++;
                    }
                }
                _console.println("-------FILE END-------");
                gps_data.close();
            }
        }
        else
        {
            _console.println("File (" + file_name + ") does not exist");
        }

        return num_lines;
    }

    bool FileHelper::copy(String &source_name, String &destination_name, const unsigned int skip)
    {
        last_error = "";

        if (!SD.exists(source_name))
        {
            last_error = "Source file does not exist";
            return false;
        }

        File src_file = SD.open(source_name, FILE_READ);
        if (!src_file)
        {
            last_error = "Failed to open source file";
            return false;
        }

        if (SD.exists(destination_name))
        {
            SD.remove(destination_name);
        }

#ifdef atmelsam
        File dst_file = SD.open(destination_name, FILE_WRITE);
#endif
#ifdef espressif32
        File dst_file = SD.open(destination_name, FILE_WRITE, true);
#endif
        if (!dst_file)
        {
            last_error = "Failed to open destination file";
            src_file.close();
            return false;
        }

        unsigned long total_src_bytes = src_file.size();
        unsigned long bytes_to_copy = total_src_bytes - skip;

        byte buf[1024];
        unsigned long buffer_size = sizeof(buf);

        unsigned long num_buffers = bytes_to_copy / buffer_size;
        if (bytes_to_copy % buffer_size != 0)
        {
            num_buffers++;
        }

        // Skip characters if necessary
        //
        src_file.seek(skip);

        // Copy the rest to the destination file
        //
        unsigned long remaining_bytes = bytes_to_copy;
        unsigned long bytes_read = buffer_size;
        for (unsigned long current_buffer = 0; current_buffer < num_buffers; current_buffer++)
        {
            // SerialUSB.println("current_buffer " + String(current_buffer) + " of " + String(num_buffers-1) + ", bytes_to_copy=" + String(bytes_to_copy) + ", remaining_bytes=" + String(remaining_bytes));

            if (remaining_bytes < buffer_size)
            {
                bytes_read = remaining_bytes;
            }

            src_file.read(buf, buffer_size);

            /*    for(i=0;i<bytes_read;i++)
                {
                  SerialUSB.print((char)buf[i]);
                }
                SerialUSB.println();*/

            dst_file.write(buf, bytes_read);

            remaining_bytes -= bytes_read;
        }

        dst_file.close();
        src_file.close();

        // SerialUSB.println("Copy success");

        return true;
    }

    bool FileHelper::append(String &file_name, String &new_line)
    {
#ifdef atmelsam
        File file = SD.open(file_name, FILE_WRITE);
#endif
#ifdef espressif32
        File file = SD.open(file_name, FILE_WRITE, true);
#endif
        if (file)
        {
            file.println(new_line);
            file.close();

            return true;
        }

        return false;
    }

    bool FileHelper::remove(String &file_name)
    {
        if (SD.exists(file_name))
        {
            return SD.remove(file_name);
        }

        return false;
    }

    bool FileHelper::write_content(String &file_name, String &content)
    {
        #ifdef atmelsam
        File file = SD.open(file_name, FILE_WRITE);
#endif
#ifdef espressif32
        File file = SD.open(file_name, FILE_WRITE, true);
#endif
        if (file)
        {
            file.println(content);
            file.close();

            return true;
        }
        return false;
    }

    String FileHelper::read_content(String &file_name)
    {
        String content = "";
        if (SD.exists(file_name))
        {
            File file = SD.open(file_name, FILE_READ);
            if (file)
            {
                content = file.readString();

                file.close();
            }
        }

        return content;
    }
}