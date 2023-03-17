#include "FileHelper.h"

namespace rlc
{
    String FileHelper::strip_lines_from_top(String file_name, int num_lines_to_strip)
    {
        last_error = "";

        String lines = "";

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
                if (current_line.indexOf("+CGPSINFO:") >= 0)
                {
                    lines += current_line + "\n";
                    num_lines++;
                }

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
            const char gps_temp_file_name[] = "temp.csv";
            copy(file_name, gps_temp_file_name, skip);

            // copy file back to original filename
            //
            copy(gps_temp_file_name, file_name, 0);
        }
        else
        {
            // At the end of the file
            //
            remove(file_name);
        }

        return lines;
    }

    unsigned int FileHelper::print_all_lines(String file_name)
    {
        return print_lines(file_name, UINT16_MAX);
    }

    unsigned int FileHelper::print_lines(String file_name, unsigned int num_lines_to_print)
    {
        unsigned int num_lines = 0;
        char c = ' ';
        if (SD.exists(file_name))
        {
            File gps_data = SD.open(file_name);
            if (gps_data)
            {
                SerialUSB.println("-------FILE BEGIN-------");
                num_lines = 0;
                while (gps_data.available() && num_lines < num_lines_to_print)
                {
                    c = gps_data.read();

                    SerialUSB.write(c);

                    if (c == '\n')
                    {
                        num_lines++;
                    }
                }
                SerialUSB.println("-------FILE END-------");
                gps_data.close();
            }
        }
        else
        {
            SerialUSB.println("File (" + file_name + ") does not exist");
        }

        return num_lines;
    }

    bool FileHelper::copy(String source_name, String destination_name, const unsigned int skip)
    {
        last_error = "";
        // SerialUSB.println("----copy file from: " + source_name + " to " + destination_name + "-----");

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

        File dst_file = SD.open(destination_name, FILE_WRITE);
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

    bool FileHelper::append(String file_name, String new_line)
    {
        File file = SD.open(file_name, FILE_WRITE);
        if (file)
        {
            file.seek(-1);
            file.println(new_line);
            file.close();

            return true;
        }

        return false;
    }

    bool FileHelper::remove(String file_name)
    {
        if (SD.exists(file_name))
        {
            return SD.remove(file_name);
        }

        return false;
    }
}