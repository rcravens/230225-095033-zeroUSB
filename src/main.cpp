#include <Arduino.h>
#include "AtCommand.h"
#include "Hardware.h"
#include "FileHelper.h"
#include "Gps.h"
#include "Http.h"
#include "Sleep.h"
#include "Battery.h"
#include "Sms.h"
#include "DateTime.h"
#include "GpsPoint.h"
#include "MyMath.h"

const char url[] = "https://webhook.site/6927a322-215e-4e0e-a625-ca21ee24a7e9";
const char content_type[] = "application/x-www-form-urlencoded";
const unsigned long api_num_gps_points_in_payload = 30;
const int api_max_points_per_post = 60;

const unsigned long gps_refresh_period_low_battery = 3600000;
const unsigned long gps_refresh_period_default = 10000;
const float gps_distance_threshold_feet = 25.0;
const char gps_file_name[] = "gps_data.csv";
unsigned long num_points_in_cache = 0;

const char battery_data[] = "bat_data.csv";

rlc::AtCommand command_helper(false);
rlc::Hardware hw(command_helper);
rlc::FileHelper file_helper;
rlc::Gps gps(command_helper);
rlc::Http http(command_helper);
rlc::Sleep sleep(hw);
rlc::Battery battery(3.3, 4.2, 30.0);
rlc::Sms sms(command_helper);

unsigned long total_points_sent = 0;
bool has_entered_low_power_mode = false;

rlc::GpsPoint last_point;
rlc::GpsPoint new_point;

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
    bool is_module_configured = is_module_on && hw.init_module();

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

    SerialUSB.println("---------------------------------------------------------------------------------------------");
    SerialUSB.println("Battery Data");
    file_helper.print_all_lines(battery_data);
    if (true)
    {
        file_helper.remove(battery_data);
    }
    SerialUSB.println("---------------------------------------------------------------------------------------------");
}

void loop()
{
    SerialUSB.println("---------------------------------------------------------------------------------------------");
    SerialUSB.println("xxxxxx TOP OF THE LOOP xxxxxxxx");

    // Monitor the current battery voltage and state of charge
    //
    battery.refresh();
    /*String new_line = battery.to_csv();
    if (!file_helper.append(battery_data, new_line))
    {
        SerialUSB.println("Failed to append battery data.");
        SerialUSB.println(new_line);
    }*/
    SerialUSB.println(battery.to_string());

    // Low power mode options
    //
    unsigned long gps_refresh_period = battery.is_low_battery_mode() ? gps_refresh_period_low_battery : gps_refresh_period_default;
    if (battery.is_low_battery_mode() && !has_entered_low_power_mode)
    {
        sms.send("000", "Switched to low power mode. Come find me and swap batteries.");
        has_entered_low_power_mode = true;
    }

    // Collect the current location
    //
    bool can_sleep = false;
    if (gps.current_location())
    {
        // SerialUSB.println("New GPS Data: " + gps.location_data);

        last_point.copy(new_point);

        rlc::GpsPoint np = rlc::GpsPoint::from_gps_str(gps.location_data);
        new_point.copy(np);
        SerialUSB.println("New GPS Data: " + new_point.to_string());

        double distance_moved_in_feet = gps_distance_threshold_feet + 1.0;

        if (new_point.is_valid && last_point.is_valid)
        {
            double distance_moved_in_miles = new_point.distance_in_miles(last_point);
            distance_moved_in_feet = rlc::MyMath::convert_miles_to_feet(distance_moved_in_miles);
            SerialUSB.println("Distance (mi): " + String(distance_moved_in_miles, 6));
            SerialUSB.println("Distance (ft): " + String(distance_moved_in_feet, 6));
        }

        // Only cache points where the distance moved is larger than the threshold
        //  - for api_num_gps_points_in_payload > 1 this will conserve battery by minimizing LTE transmit power
        //
        if (new_point.is_valid)
        {
            if (distance_moved_in_feet >= gps_distance_threshold_feet)
            {
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
            else
            {
                can_sleep = true;
            }
        }
    }

    // Post to API if we have collected enough location points
    //
    SerialUSB.println("Number of points in cache=" + String(num_points_in_cache));
    if (num_points_in_cache >= api_num_gps_points_in_payload)
    {
        if (hw.is_cellular_connected(true))
        {
            String lines = file_helper.strip_lines_from_top(gps_file_name, api_max_points_per_post);
            if (lines.length() > 0)
            {
                lines.trim();

                String content = "millis=" + String(millis()) + "&gps_refresh_period=" + String(gps_refresh_period) + "&" + battery.to_http_post() + "&" + hw.to_http_post() + "&total_point_since_power_on=" + String(total_points_sent + num_points_in_cache) + "&num_points=" + String(num_points_in_cache) + "&lines=" + lines;

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

    // Inter serial port communications
    //
    hw.send_module_output_to_console_out();

    hw.send_console_input_to_module();

    // Sleep
    //
    if (can_sleep && gps_refresh_period > 0)
    {
        if (gps_refresh_period < 120000 && !battery.is_low_battery_mode())
        {
            // High gps collection rates (< 2min) / Unlimited power
            sleep.mcu_delay_module_on(gps_refresh_period);
        }
        else
        {
            // Low gps collection rates (> 2min) / Limited power
            sleep.mcu_deep_sleep_module_off(gps_refresh_period);
        }
    }
}