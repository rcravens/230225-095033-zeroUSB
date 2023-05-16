
# 4G Trail Camera



This code is built for:

The LilyGO T-SimCam (https://www.lilygo.cc/products/t-simcam) and the
LilyGO T-PCIE SIM7600G-H  4G (https://www.lilygo.cc/products/a-t-pcie)

T-SIMCAM ESP32-S3 CAM board specifications:

* Wireless MCU – ESP32-S3R8 dual-core Xtensa LX7 microcontroller with WiFi 4 and Bluetooth 5.0 LE and Mesh connectivity with 8MB PSRAM
* Storage – 16MB QSPI flash, MicroSD card socket
* Camera – 2MP OV2640 camera with 1622×1200 resolution
* Audio – I2S digital microphone
* Connectivity – Optional NB-IoT, 2G GSM, or 3G/4G LTE module via mPCIe socket and SIM card slot
* USB – 1x USB Type-C port for power and programming
* Expansion
** Grove connector
** mPCIe socket (UART) for optional cellular module
** 5V DC input via USB-C port
** Support for LiPo battery through a 2-pin JST connector

The board can make use of the AI capabilities from the ESP32-S3 microcontroller through the built-in camera and microphone and offers two options for power with 5V through a USB Type-C port, as well as a 2-pin JST connector for connecting a battery.

The code started with the LilyGo Cam series (https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series)

My goal is to create "trail" camera used to take pictures only in daylight hours and when motion is detected to limit bandwidth.

Pictures should be written to the SD Card or uploaded to an Amazon S3 bucket or posted to a web API.  
File names should include a time stamp taken from the GPS. 


