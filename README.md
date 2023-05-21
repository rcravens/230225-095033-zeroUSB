
# 4G Trail Camera

This project started with [Adventure Time](https://www.youtube.com/@adventuretime5020) posting vides about it.

#### Project Goal
My goal is to create "trail camera" to take pictures of the properity are my local Radio Control Air Field. [TORKS.org](https://torks.org). 

To keep the 4G data bandwidth down I'd like to send images only in daylight hours and when motion is detected. Because the board has a Grove interface I may include weather data. 

Pictures are written to the on board SD Card or uploaded to an Amazon S3 bucket or posted to a web API.  

#### Software
The code is developed with VisualStudio and the PlatformIO plugin using C++. 

#### Hardware
This code is written for:

The LilyGO [T-SimCam](https://www.lilygo.cc/products/t-simcam) and the LilyGO 4G [T-PCIE SIM7600G-H](https://www.lilygo.cc/products/a-t-pcie).

**T-SIMCAM ESP32-S3** CAM board specifications:

- Wireless MCU – ESP32-S3R8 dual-core Xtensa LX7 microcontroller with WiFi 4 and Bluetooth 5.0 LE and Mesh connectivity with 8MB PSRAM
- Storage – 16MB QSPI flash, MicroSD card socket
- Camera – 2MP OV2640 camera with 1622×1200 resolution
- Audio – I2S digital microphone
- Connectivity – Optional NB-IoT, 2G GSM, or 3G/4G LTE module via mPCIe socket and SIM card slot
- USB – 1x USB Type-C port for power and programming
- Grove connector
- mPCIe socket (UART) for optional cellular module
- 5V DC input via USB-C port
- Support for LiPo battery through a 2-pin JST connector

The sample code [LilyGo Cam series](https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series) provides the camara's capabilities and the [board scmatics](https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series/blob/master/schematic/T_SIMCAM-V1.0_Schematic.pdf). This code makes use of the AI capabilities from the ESP32-S3 microcontroller through the built-in camera and microphone.

#### 4G Connection
To make a 4G celular connection you will need a sim card. I odered a EIOTCLUB Prepaid 4G LTE Cellular SIM Card from Amazon. After creating the EIOTClub account and adding data, the sim card slipped into the LilyGO card and just worked.

----
#### Configuration

Configuration is in Config.cpp file.

For testing I'm using the [WebHook.site](https://webhook.site).  *Free* posts are limited to 400+ before you'll have to change the access key.  The URL for this is in the Config.cpp file as **api_url**. 

----

#### Build

* Install VisualStudio (VS)
* Install PlatformIO plugin for VS
* 'git clone' this repository
* Use VS "open folder" to open the directory git just created
* Edit config.cpp to your needs.
* Plugin the board to your USB. Then Build and Upload the code.

----
#### Output
It make take some time for the GPS to get link and the time to be set.

    ------------------------------TOP-OF-THE-LOOP------------------------------------------------
    Date/Time: May 19, 2023 7:34:32 PM
    Battery Millis=7243805, ADC Value=2423, Voltage=3.91, Percent=67.24
    Old GPS Data: Date/Time: May 19, 2023 7:34:32 PM, Latitude: 35.533298, Longitude: -97.621301, Altitude: 397.400000
    New GPS Data: Date/Time: May 19, 2023 7:41:2 PM, Latitude: 35.533305, Longitude: -97.621304, Altitude: 400.700000
    Distance: 0.0005mi = 2.4760ft, Velocity: 0.0043mph = 0.0063ft/s, Recommended GPS Period: 15.00s
    Number of points in cache=1
    API - 428 bytes sent
    Camera: Photo was taken.
    Camera: Photo saved to SD card.
    Camera: Photo uploaded to API
    Pausing for to save bandwidth.

#### Receiving Server
I've incluged a program, written in GoLang, to receive the images files.  It save the file into directory where it is run.

Build it with:
    go build receive_file.go

To run it:
    receive_file

Curl example of posting a file.
    curl -F "image=@image.jpeg" localhost:8080/upload


