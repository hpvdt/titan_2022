# TITAN Video System

This is the code and hardware that is used to drive the electronics aboard TITAN for the "drive-by-video" system.

This code is spread across two types of devices, an STM32 microcontroller (STM32F103C8 is used in current models) and Raspberry Pis (RPi) Model 3B+'s. Each rider has an RPi for a total of two units, while there is only one STM32 for the vehicle. The STM32 is primarily responsible for collecting vehicle data and passing it to the RPis when requested. The RPis are responsible for displaying the video feed from the RPi Cameras used (hence why each rider needs their own) with an overlay of data collected inside the vehicle.

*Note: there is a third RPi system for video but it is completely independant of the others to serve as a spare front-facing video feed, with no informational overlay. It is a simple analog camera connected directly to a screen.*

## Folders

The repository is split into folders based on function.

Folder | Purpose
------ | -------
`Hardware` | Hardware files for our circuit boards
`Microcontroller Code` | STM32 Code
`vision` | RPi video code in C

## Hardware

The hardware for TITAN was all designed in [KiCad](https://www.kicad.org/) there are all three boards designed for TITAN with a `readme.md` explaining their purposes.

## Microcontroller Code

This is the collection of code prepared for the microcontroller on TITAN. It contains the unit tests we prepared when developing and testing different features that would eventually be integrated into TITAN, a local copy of the GPS library we use, and the overall program for TITAN.

TITAN was initially coded entirely within the Arduino IDE, however we migrated to using VS Code and PlatformIO to help make development easier and also make use of debugging features absent in the original Arduino IDE.

### STM32 Code
All code for the STM32 is written in C/C++ using the PlatformIO addon for VS Code. It primarily collects data and providig it on request over a serial (UART) line to the connected RPis. It also has telemetry capabilities using an nRF24L01 module and GPS capabilities using an "Ultimate GPS Module" (MTK3339). Data collected:
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

## Raspberry Pi Code
These are responsible for putting video on the displays for the riders to see, overlaid with information about TITAN. One of the RPi's has an ANT+ USB module used to collect data from the heart rate monitor and power pedals each rider has and feed it to the STM32 to pass onto the other RPi. 

**The program for running the video feed is run as a seperate process to the overlay**, this way if there is a hardware malfunction for the overlay (e.g. the STM32 freezes) then the video feed is uniterrupted for the riders.

The code for vision is split between Python and C. The video related code is written in Python, as well as a few other scripts for operation. The overlay code is written in C using the chipset driver library unique to the RPi 3 hardware to draw the overlay so it cannot be used on RPi models other then the 3B or 3B+ (an must be compiled specifically for each model).
