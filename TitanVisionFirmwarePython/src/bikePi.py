#!/usr/bin/env python2
from __future__ import division, print_function

from config import * # Import configuartion values
assert CONFIG in ["main", "rear"], "CONFIG must be either main, rear, or backup"
from Osd2019 import Osd
from serial_coms import serialLine
##from Radio import Radio

import sys
import time
import random # Needed for random numbers
from racesim import * # Used for determining performance
#############
# Setup
#############
print("==================================================")
print("")
print("Preparing Titan Electonics")
print("")
print("==================================================")
print("")
print("")

#=============================================================
print("==================================================")
print("This screen is running the %s configuration" % CONFIG)

import re
import subprocess
device_re = re.compile("Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)$", re.I)
df = subprocess.check_output("lsusb")
print("Polled all conected USB devices")

# Do associated things with front/rear
battChar = 'f'
recordVideo = False # Used to determine if video will be recorded
if CONFIG == "main":
    # Front Unit
    battChar = 'i' # Set battery request character
    recordVideo = VIDEO_RECORDING_FRONT
else:
    # Rear unit
    battChar = 'j' # Set battery request character
    recordVideo = VIDEO_RECORDING_REAR

#===============================================================
# Log file
print("==================================================")
if DATA_LOGGING:
    print("Configuring data log file")
    logFileName = "/home/pi/Documents/{:s}.log".format(time.strftime('Bike Log %y%m%d-%H%M%S', time.localtime()))
    print("Filename is %s." % logFileName)
    logFile = open(logFileName, "w+")
    print("Logfile successfully opened to be written to.")
    logFile.write("Time,Distance (km),Performance %,Speed (KPH),Cadence (RPM),Power (W),Heart Rate (BPM),Temperature (C),Humidity,Battery (%)")
    print("Wrote header row")
else:
    print("No data log file will be stored on this device!")

#===============================================================
# RaceSim info
print("==================================================")
print("RaceSim Data Overview")
from racesim_config import trackFilename
print("Track data file being used (as recorded in 'racesim_config.py'):")
print(trackFilename)
print("")
print("To edit any RaceSim parameters, edit 'racesim_config.py'")

#===============================================================
# Start with serial comms
print("==================================================")
microController = serialLine(SERIAL_BAUDRATE) # Sets up serial communication line to STM32 (9600 baud)
print("Serial connection started.")
print("Baudrate of %d." % (microController.port.baudrate))

if CONFIG == "main":
    microController.testLine('z') # Use different chars in the event they start test simultaniously
else:
    microController.testLine('y')
#===============================================================
# ANT+ 
# Set up ANT network, but only if it's the rear unit
print("==================================================")
print("ANT+ Network setup")

frontHRM = None
rearHRM = None
frontPWR = None
rearPWR = None

# Functions for broadcasting data
def rear_power_data(eventCount, pedalDiff, pedalPowerRatio, cadence, accumPower, instantPower):
    #print ("Instant Power: %d, InstantCadence: %d" % (instantPower,cadence))
    microController.sendData('F', str(instantPower))
    microController.sendData('D', str(cadence))
def front_power_data(eventCount, pedalDiff, pedalPowerRatio, cadence, accumPower, instantPower):
    microController.sendData('E', str(instantPower))
    microController.sendData('C', str(cadence))
def rear_heartrate_data(computed_heartrate, event_time_ms, rr_interval_ms):
    #print("Current Heart Rate: %d" % computed_heartrate)
    microController.sendData('B', str(computed_heartrate))
def front_heartrate_data(computed_heartrate, event_time_ms, rr_interval_ms):
    microController.sendData('A', str(computed_heartrate))
    
if CONFIG == "main":
    print("No ANT setup needed on FRONT unit")
    print("ANT+ is done by rear unit")
else:
    from ant_module import * # Import and run all code
    frontHRM = HeartRate(antnode, network, callbacks = {'onDevicePaired': hr_device_found, 'onHeartRateData': front_heartrate_data})
    rearHRM = HeartRate(antnode, network, callbacks = {'onDevicePaired': hr_device_found, 'onHeartRateData': rear_heartrate_data})
    frontPWR = BicyclePower(antnode, network, callbacks = {'onDevicePaired': power_device_found, 'onPowerData': front_power_data})
    rearPWR = BicyclePower(antnode, network, callbacks = {'onDevicePaired': power_device_found, 'onPowerData': rear_power_data})
    
    frontPWR.open(calvinPedals, ANT_TIMEOUT)
    print("Connecting front pedals")

    frontHRM.open(calvinHRM, ANT_TIMEOUT)
    print("Connecting front HRM")
    
    rearPWR.open(evanPedals, ANT_TIMEOUT)
    print("Connecting rear pedals")
    
    rearHRM.open(evanHRM, ANT_TIMEOUT)
    print("Connecting rear HRM")
    

    # Go through devices until all are no longer searching
    connectionsMade = 0
    while 4 > connectionsMade:
        connectionsMade = 0

        from ant.plus.plus import ChannelState # Used to check the state of connections

        if frontPWR.state != ChannelState.SEARCHING:
            connectionsMade = connectionsMade + 1
    
        if rearPWR.state != ChannelState.SEARCHING:
            connectionsMade = connectionsMade + 1
    
        if frontHRM.state != ChannelState.SEARCHING:
            connectionsMade = connectionsMade + 1
    
        if rearHRM.state != ChannelState.SEARCHING:
            connectionsMade = connectionsMade + 1

        time.sleep(1)
    print("All connections successfully made or the search timed out")

#===============================================================
# Onscreen display
# This last is so we can see the console beforehand
print("==================================================")
print("On Screen Display (OSD) setup")
if recordVideo:
    print("Camera feed WILL BE RECORDED (without the overlay)")
    print("Video file can be found in '/home/pi/Videos'.")
else:
    print("NO RECORDING of this camera's view!")

# Infor user of delay and how long the display will be on
print()
if RUN_TIME < 0:
    print("Starting on screen display in %d seconds. Running perpetually." % SCREEN_DELAY)
else:
    print("Starting on screen display in {:.1f} seconds. Running for {:.1f} seconds.".format(
        SCREEN_DELAY, RUN_TIME))

time.sleep(SCREEN_DELAY)
osdModule = Osd(recordVideo)

###################
# Setting variables
txIntervalCount = 0

totalDuration = 0 # Used to keep track of update time
updateCount = 0 # Used to keep track of update time

#############
# Main
#############

end_time = time.time() + RUN_TIME # Find end time, if the screen is meant to run for a limited time

# Initializing variables to be global
speedKph = 0
distance = 0
temperature = 0
humidity = 0
batteryPercentage = 0
HR = 0
cadence = 0
power = 0
spareBatteryPercentage = -1

try:
    while end_time > time.time() or RUN_TIME < 0:
        start_iteration_time = time.time()

        #############
        # Get sensor data
        if OVERRIDE_ENCODER: # Use optical encoder or GPS for speed/distance
            speedKph = float(microController.requestData('o'))
            distance = float(microController.requestData('p'))
            # GPS gives data as needed
        else:
            speedKph = float(microController.requestData('s'))
            distance = int(microController.requestData('q')) * WHEEL_CIRCUMFERENCE
            # Using encoder you get the rotation count, hence the multiplication of circumference
        temperature = (ord(microController.requestData('t')) - 50) / 2
        humidity = ord(microController.requestData('h')) / 2

        batteryPercentage = int(microController.requestData(battChar))
        
        if CONFIG == "main":
            # Request data
            HR = int(microController.requestData('a'))
            power = int(microController.requestData('e'))
            cadence = int(microController.requestData('c'))
            otherPower = int(microController.requestData('f'))
        else:
            from ant.plus.plus import ChannelState # Used to check the state of connections

            # Get data directly from ANT connections, checking they exist
            if rearHRM.state == ChannelState.OPEN:
                HR = rearHRM._computed_heart_rate
            else:
                HR = 0
                
            if rearPWR.state == ChannelState.OPEN:
                cadence = rearPWR.cadence
                power = rearPWR.instantaneousPower
                otherPower = frontPWR.instantaneousPower
            else:
                cadence = 0
                power = 0
            
        perfPercentage = 10 #getPerformancePercentage(power,otherPower, speedKph / 3.6, distance)

        # Update displayed info
        osdModule.Display(
            speedKph,
            perfPercentage,
            cadence,
            distance,
            batteryPercentage,
            power,
            HR,
            temperature,
            humidity)

        # Log data
        if DATA_LOGGING:
            logFile.write(",".join(map(str,(
                time.strftime('%H:%M:%S'),
                round(distance, 2),
                round(perfPercentage, 2),
                round(speedKph, 2),
                cadence,
                power,
                HR,
                round(temperature,0),
                round(humidity,0),
                round(batteryPercentage, 1)))) + "\n")

        # Sleep between updates
        duration = time.time() - start_iteration_time # Find how long it took to render frame
        totalDuration = totalDuration + duration # Add to total
        updateCount = updateCount + 1
        if duration < UPDATE_INTERVAL:
            time.sleep(UPDATE_INTERVAL - duration)

    osdModule.Turnoff() # Clear screen afterwards
        
    print('Total time rendering ovarlays: ' + str(totalDuration))
    print('Average update time was: ' + str(totalDuration / updateCount))
    
    
except KeyboardInterrupt:
    # Use ctrl + C to exit early
    osdModule.Turnoff() # Clear screen afterwards
    sys.exit()
   
if DATA_LOGGING:
    logFile.close() # Close log file

if CONFIG == "main":
    print("Nothing, just the front unit")
else:
    # Close ANT objects if they are connected
    from ant.plus.plus import ChannelState # Used to check the state of connections

    if frontHRM.state == ChannelState.OPEN:
        frontHRM.close()
    if rearHRM.state == ChannelState.OPEN:
        rearHRM.close()
    if frontPWR.state == ChannelState.OPEN:
        frontPWR.close()
    if rearPWR.state == ChannelState.OPEN:
        rearPWR.close()
    
    antnode.stop()

print("Entire BikePi script done!")
print("")
print("night night")
