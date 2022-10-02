# RPi Vision Code

This folder is the collection of vision system code running on the RPis. Split between C and Python code.

## Operation

The RPis are configured to boot this system automatically when powered by executing the `titan_startup.py` script when booted (done by editing [rc.local](https://linuxhint.com/use-etc-rc-local-boot/)). To speed this process up, the RPis are configured to boot to command line (CLI) rather than starting the graphical user interface; we believe this also slightly improves the speed of the system.

`titan_startup.py` then launches a series of other processes that collectively represent the TITAN vision system.

* `power_off.py` is launched to watch for a press of the 'OFF' button and react accordingly
* `go_to_desktop.py` watches for a press of the 'ON' button and shuts down the vision system to start the desktop environment (useful for development)
* `camera.py` starts the camera feed and camera recordings

Once these are running, the startup script then responsible for determining whether the RPi is supposed to be the front or rear rider's system based on the presence or absence (respectively) of an ANT+ reciever, and launching the overlay program, `bike.bin`, accordingly by passing it the right arguements. 

* If it is the rear system (no ANT+ modules connected), then the startup script just launches `bike.bin` with the arguments needed for the rear system.
* If it is the front system (ANT+ module present), then the startup script launches the `titanant.py` script to read the ANT+ data and pipe it into `bike.bin` to be overlaid and fed to the STM32.

**NOTE: All these scripts/executables are run as seperate processes so that if one fails, the others should continue unhindered. Namely the video is never interrupted to the riders.**

## Overlay Code

The overlay code is the main product of this folder, and written in C. It is split up across several function specific C files. Handles communication with the STM32 for data, overlay drawing, and logging as requested.

* `main` - Main file. Reads in arguements provided to determine behaviour (e.g. 'front' or 'rear' system, record logs or not) and then run the main loop to create overlays.
* `antInterface` - Used to process the piped in ANT data from `titanant.py` for use on the overlays and passing on the the STM32
* `overlay` - Responsible for the actually drawing of the overlay. Uses the chipset driver library unique to the RPi 3 hardware to draw the overlay over the video feed. This limits its use to only the 3B or 3B+, and `bike.bin` must be compiled specifically for each model.
* `logging` - Used to write a CSV file of all data collected on the bike
* `racesim` - A C version of our Race Simulation file used to determine how our bike should perform and then compare to how it actually is to see if we're exceeding expectations or not. **UNTESTED**.
* `serialComs` - Responsible for handling communication with the STM32
* `timeTrial` - Used to monitor timing of the code to see how long different sections take to execute. *Used for development purposes, not needed for the riders.*