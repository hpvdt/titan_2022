#!/bin/python
'''
Start up script for TITAN 2022

This script is called to set up the RPi for use. This has a few steps:
1. Start safe shut down script
2. Determine if it is front or rear unit. Done by checking for ANT stick.
3. Launch the right bike binaries based on which RPi it is

Should be set to run at startup by editing rc.local
> "sudo nano /etc/rc.local"
'''

# Used to scan USB devices to derive purpose of station
import re
import subprocess

# Used to run main bike code
import os

print('!!!!!!!!!!!!!!!!!!!!!!!!!\n\nSTARTING UP TITAN SYSTEMS\n\n!!!!!!!!!!!!!!!!!!!!!!!!!\n')


os.system('python ./power_off.py &')
print('Started looking for safe shut down signal')


# Determine if it is main unit or not based on the presence of the ANT stick
isFront = False # Assume false
ANTStickID = b'0fcf:1008' #ID of ANT stick to check for ("b'" is needed to preceed)

print('Determining if this is front (main) RPi or not based on ANT+ stick (USB ID of {}).'.format(ANTStickID))

# Heavily based on code from https://stackoverflow.com/a/8265634

device_re = re.compile(b"Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)$", re.I)
df = subprocess.check_output("lsusb")
devices = []
for i in df.split(b'\n'):
    if i:
        info = device_re.match(i)
        if info:
            dinfo = info.groupdict()
            dinfo['device'] = '/dev/bus/usb/%s/%s' % (dinfo.pop('bus'), dinfo.pop('device'))
            devices.append(dinfo)
            
            # Check for ID match
            if ANTStickID == dinfo['id'] :
                isFront = True # Set true

# Act accordingly
if isFront == True:
    print('ANT Stick detected, this is the front (main) RPi!\n')
    
    # Run front bike code with ANT piping data in
    # Bike.bin arguments = front, camera, ANT piped in, serial enabled, logging
    os.system('python ./titanant.py | ./bike.bin fcasl') 
else:
    print('No ANT Stick detected, this is the rear (secondary) RPi!\n')
    
    # Run rear camera system
    # Bike.bin arguments = rear, camera, serial enabled
    os.system('./bike.bin rcs')