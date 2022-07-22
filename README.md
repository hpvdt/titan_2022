**NOTE THAT THIS IS LARGELY OPUT OF DATE.** It should be fixed soon.

***TODO: Fix this***

# TITAN Video System Code

This is the code that isn used to drive the electronics aboard TITAN, namely the "drive-by-video" system.

This code is spread across two types of devices, an STM32 microcontroller (STM32F103C8 is used in current models) and Raspberry Pis (RPi, Model 3B+'s currently). Each rider has an RPi for a total of two units, while there is only one single STM32 for the vehicle. The STM32 is primarily responsible for collecting vehicle data and passing it to the RPis when requested. The RPis are responsible for displaying the video feed from the RPi Camera's used (hence why each rider needs their own) with an overlay of data collected inside the vehicle.

*Note: there is a third RPi system for video but it is completely independant of the others to serve as a spare front-facing video feed, with no informational overlay*

## Code folders
Folder | Purpose
------ | -------
`a_titan_control` | STM32 code
`TitanVisionFirmwarePython` | RPi video code in Python
`titan_vision_c++` | RPi video code in C++

## STM32
All code for the STM32 is written in C/C++ using the Arduino IDE. It primarily collects data and providig it on request over a serial (UART) line to the connected RPis. It also has telemetry capabilities using an nRF24L01 module and GPS capabilities using an "Ultimate GPS Module" (MTK3339). Data collected:
* Optical encoder on wheel
  * Speed
  * Distance travelled
* Temperature and humidity within fairing using a DHT22 module
* Battery levels
* GPS data using MTK3339 module
  * Position coordinates
  * Speed (as a backup to encoder)
* Biometric information for riders supplied by RPi with ANT+ module (explained later)
  * Heart rate (BPM)
  * Power output (W)
  * Cadence (RPM)

## Raspberry Pis
These are responsible for putting video on the displays for the riders to see, overlaid with information about TITAN. One of the RPi's (the "front"/"main" unit) has an ANT+ USB module used to collect data from the heart rate monitor and power pedals each rider has and feed it to the STM32. These are configured using a config file present on each RPi to inform the code of parameters such as the resolution or whether the unit is the main, rear or backup unit.

### Python Code Variant
There are two sets of code for the RPis, written in C/C++ and Python. The original code for TITAN was in Python since it was adapted from Eta, this is the "complete" code as it has everything needed to display all the information as intended. 
### C/C++ Code Variant
The C-based version was created to improve the performance of the system so it could refresh the overlay faster than a rate of ~5 Hz, it currently exceeds the 60Hz of the displays used in TITAN. However it has some limitations, firstly **it does not have ANT+ integrated yet**. It also uses the chipset driver library unique to the RPi 3B+ hardware to draw the overlay so it cannot be used on other RPi models.
