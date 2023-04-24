#ifndef Camera_h
#define Camera_h

#include <Arduino.h>
#include "FileHelper.h"

#ifdef tsimcam

#define CAM_PWDN_PIN -1
#define CAM_RESET_PIN 18
#define CAM_XCLK_PIN 14

#define CAM_SIOD_PIN 4
#define CAM_SIOC_PIN 5

#define CAM_Y9_PIN 15
#define CAM_Y8_PIN 16
#define CAM_Y7_PIN 17
#define CAM_Y6_PIN 12
#define CAM_Y5_PIN 10
#define CAM_Y4_PIN 8
#define CAM_Y3_PIN 9
#define CAM_Y2_PIN 11

#define CAM_VSYNC_PIN 6
#define CAM_HREF_PIN 7
#define CAM_PCLK_PIN 13

#include "esp_camera.h"

#endif

namespace rlc
{
    class Camera
    {
    public:
        bool is_initialized;
        const uint8_t *photo_buffer;
        size_t photo_buffer_size;
        String last_error;

        Camera();
        bool initialize();
        bool take_photo();
        void return_buffer();

    private:

#ifdef tsimcam
        camera_fb_t *_fb;
#endif
    };
}

#endif