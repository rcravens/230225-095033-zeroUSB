/*
  4G Trail Camera 
*/
#include <Arduino.h>
#include "Config.h"
#include "Console.h"
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
#include "GpsCalculator.h"
#include "MyMath.h"
#include "Camera.h"

#ifdef tsimcam
rlc::Console console(Serial);
HardwareSerial SerialAT(1);
rlc::AtCommand command_helper(SerialAT, console, false);
#endif
#ifdef maduino
rlc::Console console(SerialUSB);
rlc::AtCommand command_helper(Serial1, console, false);
#endif

const char content_type[] = "application/x-www-form-urlencoded";

String gps_data_file_name = "/gps_data.csv";
unsigned long num_points_in_cache = 0;

String battery_data_file_name = "/bat_data.csv";

String last_point_file_name = "/last_gps_point.txt";

String photo_file_name = "/photo.jpg";
String date_photo_file_name = "-photo.jpg";

rlc::Hardware hw(command_helper, console, false);
rlc::FileHelper file_helper(console, false);
rlc::Gps gps(command_helper);
rlc::Http http(command_helper, console);
rlc::Sleep sleep_helper(hw, console);
rlc::Battery battery(rlc::Config::battery_zero_point_voltage, rlc::Config::battery_max_voltage, rlc::Config::battery_low_mode_percent);
rlc::Sms sms(command_helper);
rlc::Camera camera;
rlc::DateTime datetime;

unsigned long total_points_sent = 0;
bool has_entered_low_power_mode = false;

rlc::GpsPoint last_point_cached;
rlc::GpsPoint new_point;

float total_distance_traveled_feet = 0;
float distance_since_last_post_feet = 0;

void setup()
{
    hw.init();

    bool is_sd_ready = hw.init_sd();
    bool is_module_on = hw.turn_on_module();
    bool is_module_configured = is_module_on && hw.init_module();

    String sd_status = is_sd_ready ? "YES" : "NO";
    String module_on_status = is_module_on ? "YES" : "NO";
    String module_configured_status = is_module_configured ? "YES" : "NO";

    sleep(5);
    console.println("\n------------------------------------------NEW START------------------------------------------");
#ifdef maduino
    console.println("Maduino Zero 4G LTE(SIM7600X)");
    console.println("                         Board: https://www.makerfabs.com/maduino-zero-4g-lte-sim7600.html");
    console.println("                    Board Wiki: https://wiki.makerfabs.com/Maduino_Zero_4G_LTE.html");
    console.println("                  Board Github: https://github.com/Makerfabs/Maduino-Zero-4G-LTE");
    console.println("           SIM7600 Command Set: https://github.com/Makerfabs/Maduino-Zero-4G-LTE/blob/main/A76XX%20Series_AT_Command_Manual_V1.05.pdf");
#endif
#ifdef tsimcam
    console.println("T-SIMCAM ESP32-S3 LTE(SIM7600X)");
    console.println("                         Board: https://www.lilygo.cc/products/t-simcam");
    console.println("                  Board Github: https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series");
#endif
    console.println("           SIM7600 Command Set: https://github.com/Makerfabs/Maduino-Zero-4G-LTE/blob/main/A76XX%20Series_AT_Command_Manual_V1.05.pdf");
    console.println("               Twilio SIM Card: https://www.twilio.com/docs/iot/supersim");
    console.println("---------------------------------------------------------------------------------------------");
    console.println("        SD Storage Initialized: " + sd_status);
    console.println("             SIM7600 Module On: " + module_on_status);
    console.println("     SIM7600 Module Configured: " + module_configured_status);
    console.println("                  Manufacturer: " + hw.manufacturer);
    console.println("                         Model: " + hw.model);
    console.println("                          IMEI: " + hw.imei);
    console.println("---------------------------------------------------------------------------------------------");

    if (!is_sd_ready || !is_module_on || !is_module_configured)
    {
        console.println("\n\n!!!!!! HALTING EXECUTION - BOARD NOT READY !!!!!!");
        while (true)
        {
            // Halt indefinitely....something on the board is not ready
        }
    }

    num_points_in_cache = file_helper.line_count(gps_data_file_name);

    // deserialize from SD card the last gps point that was cached
    //
    String last_point_content = file_helper.read_content(last_point_file_name);
    if (last_point_content.length() > 0)
    {
        last_point_cached.deserialize(last_point_content);
    }

    // file_helper.remove(gps_data_file_name);

    console.println("Battery Data");
    file_helper.print_all_lines(battery_data_file_name);
    if (false || file_helper.line_count(battery_data_file_name) > 500)
    {
        file_helper.remove(battery_data_file_name);
    }
    //console.println("---------------------------------------------------------------------------------------------");

    // Print GPS data
    // Serial.println(last_point_cached.to_string());

    // Print the wakeup reason for ESP32
    console.println(sleep_helper.wakeup_reason());

    // Initialize the camera
    camera.initialize();

    // console.println("\n\n");
    
}


void loop()
{
    console.println("------------------------------TOP-OF-THE-LOOP------------------------------------------------");

    new_point.copy(gps.last_gps_point);
//    while ( ! hw.is_cellular_connected(true))
//    while ( ! new_point.is_valid) {
//        new_point.copy(gps.last_gps_point);
//        console.println("Waiting for GPS lock.");
//        sleep( 15 );
/    }

    new_point.copy(gps.last_gps_point);
    if (new_point.is_valid)
    {
        console.println("Date/Time: " + new_point.datetime.to_date_time_string());
    } 
    else 
    {
        console.println("Invalid GPS data");
    }


    battery.refresh();          // Monitor the current battery voltage and state of charge

    String new_line = battery.to_csv();
    if (!file_helper.append(battery_data_file_name, new_line))
    {
        console.println("Failed to append battery data.");
        console.println(new_line);
    }

    console.println(battery.to_string());

    // Low power mode options
    unsigned long gps_refresh_period_sec = battery.is_low_battery_mode() ? rlc::Config::gps_refresh_period_low_battery_sec : rlc::Config::gps_refresh_period_default_sec;
    if (battery.is_low_battery_mode() && !has_entered_low_power_mode)
    {
        sms.send("000", "Switched to low power mode. Come find me and swap batteries.");
        has_entered_low_power_mode = true;
    }

    // Collect the current location
    // bool can_sleep = false;

    // Print cached GPS data
    console.println("Old GPS Data: " + last_point_cached.to_string());

    if (true && gps.current_location())
    {
        new_point.copy(gps.last_gps_point);
        console.println("New GPS Data: " + new_point.to_string());

        // Only cache new points if
        //  1. the distance moved from the last point exceeds the distance threshold
        //  2. the time since the last point was cached exceeds the time threshold
        //
        // This conserves battery by minimizing the LTE transmit power
        //
        if (new_point.is_valid)
        {
            rlc::GpsCalculator calc(new_point, last_point_cached);

            if (!last_point_cached.is_valid || calc.distance_in_feet >= rlc::Config::gps_distance_threshold_feet || calc.time_diff_in_seconds >= rlc::Config::gps_max_time_threshold_seconds)
            {
                String new_line = new_point.to_string();
                if (!file_helper.append(gps_data_file_name, new_line))
                {
                    console.println("Failed to cache new point.");
                }
                else
                {
                    last_point_cached.copy(new_point);

                    // Serialize this last cachced point to the SD card
                    //
                    String last_point_content = last_point_cached.serialize();
                    file_helper.write_content(last_point_file_name, last_point_content);

                    total_distance_traveled_feet += calc.distance_in_feet;
                    distance_since_last_post_feet += calc.distance_in_feet;

                    num_points_in_cache += 1;

                    // can_sleep = true;
                }
            }
            else
            {
                console.println("New point not far enough away from previously recorded point.");

                // can_sleep = true;
            }

            // Adjust the refresh period based on velocity
            //
            if (calc.is_valid)
            {
                console.println(calc.to_string());
                gps_refresh_period_sec = battery.is_low_battery_mode() ? rlc::Config::gps_refresh_period_low_battery_sec : calc.recommended_gps_refresh_period_sec;
            }
        }
    }

    // Post to API if we have collected enough location points
    //
    // console.println("\n");
    console.println("Number of points in cache=" + String(num_points_in_cache));
    if (num_points_in_cache >= rlc::Config::api_num_gps_points_in_payload)
    {
        if (hw.is_cellular_connected(true))
        {
            String lines = file_helper.strip_lines_from_top(gps_data_file_name, rlc::Config::api_max_points_per_post);
            if (lines.length() > 0)
            {
                lines.trim();

                String content = "millis=" + String(millis()) + "&gps_refresh_period_sec=" + String(gps_refresh_period_sec) + "&" + battery.to_http_post() + "&" + hw.to_http_post() + "&total_distance_traveled_feet=" + String(total_distance_traveled_feet, 2) + "&distance_since_last_post_feet=" + String(distance_since_last_post_feet, 2) + "&total_points_since_power_on=" + String(total_points_sent + num_points_in_cache) + "&num_points=" + String(num_points_in_cache) + "&lines=" + lines;

                if (http.post(rlc::Config::api_url, content, content_type))
                {
                    console.println("API - " + String(content.length()) + " bytes sent");

                    total_points_sent += num_points_in_cache;

                    distance_since_last_post_feet = 0;

                    num_points_in_cache = 0;
                }
                else
                {
                    file_helper.append(gps_data_file_name, lines); // re-cache these points as they failed to send

                    console.println("Failed to send content to API. " + String(lines.length()) + " bytes added back to cache.");
                }
            }
            else
            {
                console.println("No line data read from file. last error = " + file_helper.last_error);
            }
        }
        else
        {
            console.println("No cellular found.");
        }
    }


    // Collect a new photo
    //
    if (rlc::Config::is_camera_save_to_sd || rlc::Config::is_camera_upload_to_api)
    {
        if (camera.is_initialized && camera.take_photo())
        {
            console.println("Camera: Photo was taken.");

            if (rlc::Config::is_camera_save_to_sd)
            {
                photo_file_name = "/" + new_point.datetime.to_yyyymmddhhmmss() + date_photo_file_name;
                if (file_helper.write(photo_file_name, camera.photo_buffer, camera.photo_buffer_size))
                {
                    console.println("Camera: Photo saved to SD card.");
                }
                else
                {
                    console.println("Camera: Failed to write photo to SD card.");
                }
            }

            if (rlc::Config::is_camera_upload_to_api)
            {
                if (hw.is_cellular_connected(true))
                {
                    if (http.post_file_buffer(rlc::Config::api_url, camera.photo_buffer, camera.photo_buffer_size))
                    {
                        console.println("Camera: Photo uploaded to API");
                        // Pause to save bandwidth
                        console.println("Pausing for to save bandwidth.\n");
                        sleep(rlc::Config::wait_after_image_upload);
                    }
                    else
                    {
                        console.println("Camera: Failed to upload photo to API.");
                    }
                }
                else
                {
                    console.println("Photo Upload: No cellular found.");
                }
            }

            camera.return_buffer();
        }
        else
        {
            console.println("Camera Error: " + camera.last_error);
        }
    }

    // Inter serial port communications
    //
    //hw.send_module_output_to_console_out();
    //hw.send_console_input_to_module();

    sleep(10);
    
    // Sleep
    //
/*
    if (can_sleep && gps_refresh_period_sec > 0)
    {
        uint gps_refresh_period_ms = gps_refresh_period_sec * 1000;
        if (gps_refresh_period_sec < 120 && !battery.is_low_battery_mode())
        {
            // High gps collection rates (< 2min) / Unlimited power
            sleep_helper.mcu_delay_module_on(gps_refresh_period_ms);
        }
        else
        {
            // Low gps collection rates (> 2min) / Limited power
            sleep_helper.mcu_deep_sleep_module_off(gps_refresh_period_ms);
        }
    }
*/
}
