#include "Camera.h"

namespace rlc
{
    Camera::Camera()
    {
        is_initialized = false;
#ifdef tsimcam
        _fb = NULL;
#endif
    }

    bool Camera::initialize()
    {
        is_initialized = false;
#ifdef tsimcam

        camera_config_t config;
        config.ledc_channel = LEDC_CHANNEL_0;
        config.ledc_timer = LEDC_TIMER_0;
        config.pin_d0 = CAM_Y2_PIN;
        config.pin_d1 = CAM_Y3_PIN;
        config.pin_d2 = CAM_Y4_PIN;
        config.pin_d3 = CAM_Y5_PIN;
        config.pin_d4 = CAM_Y6_PIN;
        config.pin_d5 = CAM_Y7_PIN;
        config.pin_d6 = CAM_Y8_PIN;
        config.pin_d7 = CAM_Y9_PIN;
        config.pin_xclk = CAM_XCLK_PIN;
        config.pin_pclk = CAM_PCLK_PIN;
        config.pin_vsync = CAM_VSYNC_PIN;
        config.pin_href = CAM_HREF_PIN;
        config.pin_sccb_sda = CAM_SIOD_PIN;
        config.pin_sccb_scl = CAM_SIOC_PIN;
        config.pin_pwdn = CAM_PWDN_PIN;
        config.pin_reset = CAM_RESET_PIN;
        config.xclk_freq_hz = 20000000;
        config.pixel_format = PIXFORMAT_JPEG; // for streaming
        // config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition

        // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
        //                      for larger pre-allocated frame buffer.
        if (psramFound())
        {
            config.frame_size = FRAMESIZE_UXGA;
            config.jpeg_quality = 10;
            config.fb_count = 2;
        }
        else
        {
            config.frame_size = FRAMESIZE_SVGA;
            config.jpeg_quality = 12;
            config.fb_count = 1;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }

        // Initialize camera
        //
        esp_err_t err = esp_camera_init(&config);
        if (err != ESP_OK)
        {
            last_error = "Camera init failed with error 0x" + String(err);
            is_initialized = false;

            return is_initialized;
        }
 
        is_initialized = true;
 
        // Adjust the sensor settings
        //
        sensor_t *s = esp_camera_sensor_get();

        s->set_brightness(s, 1);                 // -2 to 2
        s->set_contrast(s, 0);                   // -2 to 2
        s->set_saturation(s, -2);                // -2 to 2
        s->set_special_effect(s, 0);             // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
        s->set_whitebal(s, 1);                   // 0 = disable , 1 = enable
        s->set_awb_gain(s, 1);                   // 0 = disable , 1 = enable
        s->set_wb_mode(s, 0);                    // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        s->set_exposure_ctrl(s, 1);              // 0 = disable , 1 = enable
        s->set_aec2(s, 0);                       // 0 = disable , 1 = enable
        s->set_ae_level(s, 0);                   // -2 to 2
        s->set_aec_value(s, 300);                // 0 to 1200
        s->set_gain_ctrl(s, 1);                  // 0 = disable , 1 = enable
        s->set_agc_gain(s, 0);                   // 0 to 30
        s->set_gainceiling(s, (gainceiling_t)6); // 0 to 6
        s->set_bpc(s, 0);                        // 0 = disable , 1 = enable
        s->set_wpc(s, 1);                        // 0 = disable , 1 = enable
        s->set_raw_gma(s, 1);                    // 0 = disable , 1 = enable
        s->set_lenc(s, 1);                       // 0 = disable , 1 = enable
        s->set_hmirror(s, 0);                    // 0 = disable , 1 = enable
        s->set_vflip(s, 1);                      // 0 = disable , 1 = enable
        s->set_dcw(s, 1);                        // 0 = disable , 1 = enable
        s->set_colorbar(s, 0);                   // 0 = disable , 1 = enable
        s->set_framesize(s, FRAMESIZE_HD);
#endif

        return is_initialized;
    }

#ifdef maduino
    bool Camera::take_photo()
    {
        last_error = "No camera feature found.";

        return false;
    }

    void Camera::return_buffer()
    {
        // Do nothing
        //
        return;
    }
#endif

#ifdef tsimcam
    bool Camera::take_photo()
    {
        last_error = "";
        if (!is_initialized)
        {
            return false;
        }

        // Taking a picture is just getting a pointer to the sensor's frame buffer
        //
        _fb = esp_camera_fb_get();
        if (!_fb)
        {
            last_error = "Camera capture failed";
            return false;
        }

        photo_buffer = _fb->buf;
        photo_buffer_size = _fb->len;

        return true;
    }

    void Camera::return_buffer()
    {
        if (_fb)
        {
            esp_camera_fb_return(_fb);
            _fb = NULL;
        }
    }

#endif
}