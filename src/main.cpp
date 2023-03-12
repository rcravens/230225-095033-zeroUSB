#include <Arduino.h>
#include "AtCommand.h"
#include "Hardware.h"
#include "FileHelper.h"
#include "Gps.h"
#include "Http.h"
#include "Sleep.h"

const char url[] = "https://webhook.site/6927a322-215e-4e0e-a625-ca21ee24a7e9";
const char content_type[] = "application/x-www-form-urlencoded";
const unsigned long api_num_gps_points_in_payload = 1;
const int api_max_points_per_post = 20;

const unsigned long gps_refresh_period = 600000;
const char gps_file_name[] = "gps_data.csv";
unsigned long num_points_in_cache = 0;

rlc::AtCommand command_helper(false);
rlc::Hardware hw(command_helper);
rlc::FileHelper file_helper;
rlc::Gps gps(command_helper);
rlc::Http http(command_helper);
rlc::Sleep sleep(hw);

unsigned long total_points_sent = 0;

void setup()
{
    hw.init();

    SerialUSB.println("------------------------------------------NEW START------------------------------------------");
    SerialUSB.println("Maduino Zero 4G LTE(SIM7600X)");
    SerialUSB.println("                         Board: https://www.makerfabs.com/maduino-zero-4g-lte-sim7600.html");
    SerialUSB.println("                    Board Wiki: https://wiki.makerfabs.com/Maduino_Zero_4G_LTE.html");
    SerialUSB.println("                  Board Github: https://github.com/Makerfabs/Maduino-Zero-4G-LTE");
    SerialUSB.println("           SIM7600 Command Set: https://github.com/Makerfabs/Maduino-Zero-4G-LTE/blob/main/A76XX%20Series_AT_Command_Manual_V1.05.pdf");
    SerialUSB.println("               Twilio SIM Card: https://www.twilio.com/docs/iot/supersim");
    SerialUSB.println("---------------------------------------------------------------------------------------------");

    bool is_sd_ready = hw.init_sd();
    bool is_module_on = hw.turn_on_module();
    bool is_module_configured = is_module_on && hw.configure_module();

    String sd_status = is_sd_ready ? "YES" : "NO";
    String module_on_status = is_module_on ? "YES" : "NO";
    String module_configured_status = is_module_configured ? "YES" : "NO";

    SerialUSB.println("        SD Storage Initialized: " + sd_status);
    SerialUSB.println("             SIM7600 Module On: " + module_on_status);
    SerialUSB.println("     SIM7600 Module Configured: " + module_configured_status);

    if (!is_sd_ready || !is_module_on || !is_module_configured)
    {
        SerialUSB.println("\n\n!!!!!! HALTING EXECUTION - BOARD NOT READY !!!!!!");
        while (true)
        {
            // Halt indefinitely....something on the board is not ready
        }
    }
    SerialUSB.println("                  Manufacturer: " + hw.manufacturer);
    SerialUSB.println("                         Model: " + hw.model);
    SerialUSB.println("                          IMEI: " + hw.imei);
}

void loop()
{
    SerialUSB.println("xxxxxx TOP OF THE LOOP xxxxxxxx");

    bool can_sleep = false;
    if (gps.current_location())
    {
        SerialUSB.println("New GPS Data: " + gps.location_data);

        if (!file_helper.append(gps_file_name, gps.location_data))
        {
            SerialUSB.println("Failed to cache new point.");
        }
        else
        {
            num_points_in_cache += 1;
            can_sleep = true;
        }
    }

    SerialUSB.println("Number of points in cache=" + String(num_points_in_cache));
    if (num_points_in_cache >= api_num_gps_points_in_payload)
    {
        if (hw.is_cellular_connected(true))
        {
            String lines = file_helper.strip_lines_from_top(gps_file_name, api_max_points_per_post);
            if (lines.length() > 0)
            {
                lines.trim();

                String content = "manufacturer=" + hw.manufacturer + "&model=" + hw.model + "&imei=" + hw.imei + "&total_point_since_power_on=" + String(total_points_sent + num_points_in_cache) + "&num_points=" + String(num_points_in_cache) + "&lines=" + lines;

                if (http.post(url, content, content_type))
                {
                    SerialUSB.println("API - " + String(content.length()) + " bytes sent");

                    total_points_sent += num_points_in_cache;

                    num_points_in_cache = 0;
                }
                else
                {
                    file_helper.append(gps_file_name, lines); // re-cache these points as they failed to send

                    SerialUSB.println("Failed to send content to API. " + String(lines.length()) + " bytes added back to cache.");
                }
            }
        }
        else
        {
            SerialUSB.println("No cellular found.");
        }
    }

    hw.send_module_output_to_console_out();

    hw.send_console_input_to_module();

    if (can_sleep)
    {
        sleep.mcu_delay_module_off(gps_refresh_period);
    }
}
