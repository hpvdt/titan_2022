# Microcontroller Code

There are three folders here related to the code for the STM32 used in TITAN. Some parts written for the Arduino IDE, some for PlatformIO in VS Code.

Folder | Purpose
------ | -------
`Microcontroller Unit Tests` | Collection of unit tests for different features
`TinyGPS-13` | Local copy of Mikal Hart's [TinyGPS](https://github.com/mikalhart/TinyGPS) library
`titan` | The complete code run in TITAN

## Unit Tests

This is just a bunch of Arduino sketches for us to test out different features of TITAN independantly prior to integration with the main code. 

These tests served to verify that the code would act as needed without any other parts of the code making it act unexpectedly or slowing down coimpilation time during these tests. The unit tests also allowed us to verify and tune the related hardware as needed as was the case for the telemetry features and optical encoder for wheel speed/distance.

## TinyGPS

Just a local copy of the TinyGPS library. Largely kept from the previous collection microcontroller code.

## TITAN STM32 Code

All code for the STM32 is written in C/C++ using the PlatformIO addon for VS Code. It primarily collects data and providing it on request over a serial (UART) line to the connected RPis. It also has telemetry capabilities using an nRF24L01 module and GPS capabilities using an "Ultimate GPS Module" (MTK3339). Data collected by the STM32:

* Optical encoder for wheel
  * Speed
  * Distance travelled
* Temperature and humidity of external air using a DHT22 module
* Battery levels
* GPS data using MTK3339 module
  * Position coordinates
  * Speed (as a backup to encoder)
* Biometric information for riders supplied by RPi with ANT+ module *(explained later)*
  * Heart rate (BPM)
  * Power output (W)
  * Cadence (RPM)
* Brake disk temperatures using MLX90614 sensors
* Internal CO2 levels from an MH-Z19C sensor

In addition to this data collected directly by itself, it also acts as a messenger to pass ANT+ data between the RPis. For each of our two riders the following data is provided to the STM32:

* Cadence
* Heart rate
* Instantanious power

*Note on the communication:* the data was all initially communicated in plaintext as seperate queries (e.g. `s` for speed from the RPi, and the STM32 would respond with `$123.3`). This was found to be increadibly slow and error prone, so we've move to more condensed, binary encoded data exchanges to speed up communication between the RPis, telemetry, and STM32.