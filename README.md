
# 4G Camera

This project started with [Adventure Time](https://www.youtube.com/@adventuretime5020) posting vides about it.

#### Project Goal
The goal is to create off grid remote camera to monitor a Radio Control Air Field. [TORKS.org](https://torks.org). 

Photos are written to the SD Card and posted to a remote API (web service). See the Configuration section.

The cam_server program can be used to post and read the images the camra pictures. The camera can only post non-TLS at this time. Web browsers expect TLS pages to be all tls connections. So, the cam_server program supplies both TLS port 8443 and non-TLS port 8080 because of this.  Files are kept in /images and archived in /images/archive.

To limit the 4G data bandwidth capture can be limited to daylight hours and a given rate.

#### Software
The code is developed with VisualStudio and the PlatformIO plugin using C++. 

#### Hardware
This code is written for:

The LilyGO [T-SimCam](https://www.lilygo.cc/products/t-simcam) and the LilyGO 4G [T-PCIE SIM7600G-H](https://www.lilygo.cc/products/a-t-pcie). 

GitHub support for these board are:
[T-SimCam]()
[PCIE SIM7600](https://github.com/Xinyuan-LilyGO/LilyGo-T-PCIE)  This is dependent on [TinyGSM](https://github.com/vshymanskyy/TinyGSM).


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

To build the cam_server program

* install GoLang
* run the Build.sh script in the cam_server directory

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
I've incluged a program, written in GoLang, to handle uploading and downloading the camera images.
It saves files into the /image/

Build it with:
    go build cam_file_server.go

To run it:
    cam_file_server

Curl example of posting a file.
    curl -F "<sec_key>=@photo.jpg" <host_name>:8080/upload

#### Other parts
These are Amazon links to the products I purchased to finsh this project. 

[LeMotech Electrical Box, IP67 Waterproof Hinged Clear Cover Stainless Steel Latch Junction Box](https://www.amazon.com/dp/B0BP7D5ZNL?ref=ppx_yo2ov_dt_b_product_details&th=1)

[Bingfu 4G LTE Outdoor Wall Mount Waterproof Antenna SMA Male Antenna](https://www.amazon.com/dp/B07R9JGLV5?ref=ppx_yo2ov_dt_b_product_details&th=1)

[Bingfu Vehicle Waterproof Active GPS Navigation Antenna with SMA Male Connector](https://www.amazon.com/dp/B07R7RC96G?ref=ppx_yo2ov_dt_b_product_details&th=1)

[Letgo 5pcs IPX IPEX-1 U.FL to SMA Female Pigtail](https://www.amazon.com/dp/B01HXU1PKS?psc=1&ref=ppx_yo2ov_dt_b_product_details)

[LUORNG 4PCS 18650 SMD Battery Clip Holder](https://www.amazon.com/dp/B09N76T4GL?psc=1&ref=ppx_yo2ov_dt_b_product_details)
 to and read the images from. 

[1/4" NPT Strain Relief Nylon Cord](https://www.amazon.com/gp/product/B08MYM2TSJ/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)


![All the parts](Doc/20230524_105729.jpg)

#### Ideas / TODOs:

These are just ideas that could be done.  I may or may not do them.

Use the Grove (I2C) interface to collect weather data and send it.

Turn on the WiFi for local camera capture.

Use the 4G with the WiFi to create a HotSpot. 
