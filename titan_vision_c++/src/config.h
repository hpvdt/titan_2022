#pragma once

#define UPDATE_INTERVAL 0.2 // Target amount of time between OSD updates

#define DATA_LOGGING False // Is there going to data logging?
// Is there meant to be a record of camera footage?
#define VIDEO_RECORDING_FRONT true
#define VIDEO_RECORDING_REAR false

#define SERIAL_BAUDRATE 38400 // Baudrate to microcontroller, has to be matched on both

#define SCREEN_DELAY 5 // Delay for screen delay after boot

#define RUN_TIME -1 // Run screen for this many seconds (set to negative for endless)

#define WHEEL_CIRCUMFERENCE 2.082 // Wheel circumference in meters

#define ANT_TIMEOUT 1 // Timeout for searching for ANT devices

#define OVERRIDE_ENCODER false // Decide whether to use GPS over optical encoder
// This means the bike will use GPS to determine speed and distance in place
// of the optical encoder. However this will only update every second.

#define IS_BACKUP true // Is this system a backup? If true the system will only run
// the camera feed; no data overlay, logging, etc. Set to false if it is either
// of the main displays for the front or back rider.
