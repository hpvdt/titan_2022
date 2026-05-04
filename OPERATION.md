# Setup and Operating Instructions

TITAN was designed and tested to be a pretty simple system to operate by requiring no user input beyond a the input to power on, once assembled correctly.

## Batteries and Power

_The system's life force!_

The batteries TITAN 2022 was **designed for are 3S LiFePO4 batteries**, as such their working range of voltages are generally 9.0&nbsp;V to 10.8&nbsp;V with their nominal voltage being 9.9&nbsp;V. Prior to operation they should be charged fully.

The exact power draw of TITAN is approximately 10&nbsp;W per RPi vision system, about half that for the analog back up system. As such a 1000&nbsp;mAh 3S LiFePO4 battery can supply a vision system for an hour of operation is is plenty for most of our 20 minute heats - but wannants changes for longer testing sessions.

## Assembly/Setup

TITAN was once, assembled, now it likely is no longer, so someone's got to put it back together. This will be written assuming TITAN's basically being entirely rebuilt.

### Lists of Parts

>[!NOTE]
> These lists do not account for any spare parts, those are up to your discretion.

There are a fair few parts to TITAN needed for operation that need to be packed listed below. Only one unit is needed of each unless specified otherwise.

- 2 x Raspberry Pis Model 3A+ or 3B+
- 2 x 16 GB micro SD cards (larger is fine but will not be used)
- 2 x approximately 10" 720p HDMI display, 12&nbsp;V supply
- 2 x Raspberry Pi "Spy" camera
- TITAN 2022 HAT assembled for primary operation (microcontroller present and more connectors)
- TITAN 2022 HAT assembled for secondary operation
- HDMI cable, 30&nbsp;cm long
- "Ultimate" GPS module
- GPS antenna with SMA connector on 3&nbsp;m cable
- nRF24L01+PA+LNA radio module
- 2.4 GHz antenna with 3&nbsp;m cable
- ANT+ reciever (USB-A)
- Analog pin hole video camera, 12&nbsp;V supply
- 7" analog input display, 12&nbsp;V supply

The following are needed but will likely be already integrated into TITAN's structure, so they don't need to be "packed".

- 3 x Battery extension leads, T connector to barrel jack, 1.5&nbsp;m long
- 2 x 5.5&nbsp;mm barrel jack to barrel jack cable, 3&nbsp;m long
- Rear wheel sensor board with harness soldered on
- Brake disk temperature sensor for front wheel with USB-A connector
- USB-A to USB-A cable, 3&nbsp;m long
- HDMI cable, 3&nbsp;m long
- MH-Z19 CO2 sensor with 3&nbsp;m long cable
- DHT22 sensor with 3&nbsp;m long cable
- 12&nbsp;V power distribution board
- RCA coax video cable, 3&nbsp;m long

### Setup Procedure

>[!CAUTION]
> **All systems must be entirely unpowered for setup!** Remove any and all batteries. The system was not designed for hot-plugging any component so damage may occur as a result.

This starts with the primary digital vision systems for each rider, then the backup system provided to the primary rider.

>[!TIP]
> These steps may be performed in any order. Here they are presented in the order that will likely be the most convenient to perform.

1. Check the sensors are installed
   - Front wheel brake temperature sensor in wheel bracket
   - Wheel sensor board in the rear wheel's bracket
   - DHT22 near the air inlet
   - MH-Z19 sensor between the heads of riders
2. Check that the auxiliary sensor connections are made
   - Front wheel sensor intermediate connector is mated near front yoke (might have been disconnected during maintenance or removal of yoke)
   - Front wheel to rear wheel cable harness connection
   - Camera cable extenders are properly securing the ribbon cables if used
   - Antenna cables are routed to electronics area
3. Image the micro SD cards for each RPi and insert them into the RPis
4. Connect the camera cables to each RPi, ensuring they are installed correctly by being perfectly normal to the board
5. Install the RPi HATs, mounted with threaded plastic spacer ideally
6. Insert the ANT+ receiver into the primary/front rider's RPi
7. Connect the sensor to the primary system's hat, all connectors will only fit where they are meant to
8. Connect the monitors to the RPis using HDMI cables for video and provide them power from same RPi's HAT
   - In TITAN there should be wires routed from the electronics area to the front rider's viewing area
   - Even the rear rider may require some extension cables to better position the electronics
9. Connect the RPi HATs with the bridge cable, from `SEC_OUT` on the secondary (rear rider's) HAT to `SEC_IN` on the primary HAT
10. Connect the backup camera to the backup monitor using an RCA cable
11. Connect the backup camera and screen to the 12&nbsp;V power board

## Start Up

With TITAN assembled correctly the process to launch the system is pretty straight forward.

> [!CAUTION]
> Failure to follow the battery connection sequence properly may damage the hardware.

1. Ensure batteries are fully charged
2. Plug in the batteries following this exact order. Each system should automatically begin booting when their battery is connected
   1. Primary (front) system
   2. Secondary (rear) system
   3. Backup system
3. Press the on button (`ON_RPi`) on RPi systems to boot them if needed - for example if they were shutdown without battery removal

The backup system should show the video feed immediately when turned on. The RPi systems will take about 30 seconds to put the video on screen, which will be preceded with boot messages and a printing of the system's configuration.

## Shut Down

Whenever power is going to be removed from TITAN the following procedure should be followed. Be it at the end of a run, or to conserve battery when staging before a run.

> [!CAUTION]
> Failure to follow this properly may damage the hardware or risk corrupting the data on the RPis.

1. Press the off button (`OFF_RPi`) on the HAT
2. Wait for the video feed to go off and the RPi to shut down properly
   - The video feed should disappear within seconds after the button is pressed, if not press the button again
   - Repeated presses of the off button will not adversely affect the system
   - If the system doesn't shutdown within a minute you may need to connect a keyboard or simply pull the power risking data loss
   - As the RPi shuts down its status LED will flash ten times at 2&nbsp;Hz before going out, that signals it is safe to remove power
3. If desired, disconnect the batteries. They **MUST** be removed in the reverse order they were installed in:
   1. Backup display
   2. Secondary system
   3. Primary system

If the batteries are left in place, the RPis can be turned on at the press of the on button (`ON_RPi`). This is useful to do for staging before a run. There is no required order to follow when turning on RPis in this manner because the power supplies have all stabilized.

## Transferring Data To or From the RPis

My _(Savo's)_ preferred method to transfer data was to remove the RPi's micro SD card _(after a proper shutdown with battery removal!)_ and insert it into my computer and directly copy the data of interest onto the computer that way. The only issue is that you'll likely need a Linux machine to recognize the file system the RPi's use. Having the SD card in my computer also made it easy to edit some of the scripts to handle things like new power pedals.

Other options include network access or using the RPi's as a normal desktop and a USB drive to ferry the data to another computer.

>[!IMPORTANT]
> To get to the desktop of a TITAN RPi one can press the RPi's on button (`ON_RPi`) during operation of the vision system to stop the vision system.

If the button is not working, one can try to manually enter the following sequence of command blindly into the terminal with a keyboard to shut the vision system down. _Best of luck console cowboy!_

```bash
sudo pkill bike
sudo pkill python
```

>[!TIP]
> Entering `startx` in the terminal will start the graphical interface if desired for easier operation of the RPis. _Brace yourself for a slow computer experience._

### Log Files and Locations

By default the TITAN system records data in two locations, one for the videos and another for the ride data.

> [!NOTE]
> All recorded files are timestamped in their names with the start of the run. However the **RPis do not keep time when they are off, so these times will likely lag behind the true start time**. To minimize this time discrepancy the RPis should be allowed to connect to the internet to synchronize their clocks regularly.

| Data | Location | Format |
| --- | --- | --- |
| Ride data | `/home/pi/Desktop/vision/bikelogs/` | `.csv` |
| Video recordings | `/home/pi/Videos` | `.h264` |

### Video Conversion

To convert from `.h264` recordings to the more commonly accepted `.mp4` one may use the following `ffmpeg` command with the appropriate file names substituted for "video":

```bash
ffmpeg -r 60 -i video.h264 -c:v copy video.mp4
```

>[!NOTE]
> The video recordings are known to have portions where the video seems to skip ahead for split seconds. This is likely due to the recording program getting overwhelmed or some hiccup in the `.mp4` conversion process.
>
> Furthermore, the video and overlay/logging are run as separate process so their staring time stamps will likely differ and need a little effort to align.
