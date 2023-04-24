#include "Http.h"
#include <SD.h>

namespace rlc
{
    Http::Http(rlc::AtCommand &command_helper, rlc::Console &console) : _command_helper(command_helper),
                                                                        _console(console)
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

    bool Http::post_file_buffer(String url, const uint8_t *buf, size_t size)
    {
        String boundary = "BOUNDARY12345";
        String multipart_boundary = "multipart/form-data; boundary=" + boundary;

        String content_disposition = "Content-Disposition: form-data; name=\"photo\"; filename=\"photo.jpg\"";
        String content_type = "Content-Type: image/jpeg";
        String content_tranfer_encoding = "Content-Transfer-Encoding: binary";

        String head = "--" + boundary + "\r\n" + content_disposition + "\r\n" + content_type + "\r\n\r\n"; // + content_tranfer_encoding + "\r\n\r\n";
        String tail = "\r\n--" + boundary + "--\r\n";

        int total_size = head.length() + size + tail.length();

        bool is_sent = false;

        // this line ensures that any previous sessions area terminated
        //
        _command_helper.send_command_and_wait("AT+HTTPTERM");

        if (_command_helper.send_command_and_wait("AT+HTTPINIT"))
        {
            if (_command_helper.send_command_and_wait("AT+HTTPPARA=\"URL\",\"" + url + "\""))
            {
                if (_command_helper.send_command_and_wait("AT+HTTPPARA=\"CONTENT\",\"" + multipart_boundary + "\""))
                {
                    if (_command_helper.send_command_and_wait("AT+HTTPDATA=" + String(total_size) + ",60000", "DOWNLOAD", 1000))
                    {
                        _command_helper.write(head, 1000);

                        size_t max_chunk_size = 4096;
                        uint8_t chunk_buf[max_chunk_size];
                        size_t current_position = 0;
                        while (current_position < size)
                        {
                            size_t chunk_size = 0;
                            for(size_t i=0;i<max_chunk_size;i++)
                            {
                                if(current_position + i < size)
                                {
                                    chunk_size += 1;
                                    chunk_buf[i] = buf[current_position + i];
                                }
                            }
                            size_t num_bytes_sent = _command_helper.write(chunk_buf, chunk_size, 2000);

                            current_position += max_chunk_size;
                        }

                        _command_helper.write(tail, 1000);
                    }

                    _command_helper.send_command_and_wait("AT+HTTPACTION=1", "HTTP_PEER_CLOSED", 30000);

                    is_sent = true;
                }
            }
            _command_helper.send_command_and_wait("AT+HTTPTERM");
        }

        return is_sent;
    }

}