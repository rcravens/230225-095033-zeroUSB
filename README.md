
# 4G Trail Camera

#### Project Goal
My goal is to create "trail camera" to take pictures of the properity are my local Radio Control Air Field. [TORKS.org](torks.org). 

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
    Filename=20230512142007-image.jpg
    Battery Millis=28903250, ADC Value=2581, Voltage=4.16, Percent=95.54
    Old GPS Data: Date/Time: May 16, 2023 4:44:8 AM, Latitude: 35.533283, Longitude: -97.621324, Altitude: 375.800000
    New GPS Data: Date/Time: May 16, 2023 4:44:20 AM, Latitude: 35.533284, Longitude: -97.621324, Altitude: 376.300000
    Distance: 0.0000mi = 0.0451ft, Velocity: 0.0026mph = 0.0038ft/s, Recommended GPS Period: 15.00s
    Number of points in cache=366
    Failed to send content to API. 100 bytes added back to cache.
    Camera: Photo was taken.
    Camera: Photo saved to SD card.

