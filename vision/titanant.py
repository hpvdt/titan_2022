from ant.antplus.controller import AntPlusController
from ant.antplus.pwr import PwrDevice
from ant.antplus.hrm import HrmDevice
from sys import stdout # Needed to pipe data to another process
import errno
import time

'''
TITAN ANT+ INTERFACE (2022)

This script is used to collect all the ANT+ biometrics for TITAN and pipe the data into another 
process, our on screen display (OSD) to be shown to the riders.

This script is dependant on our modified "openant" python library to get ANT data from a USB dongle. 

Piping data is done using stdout in this script to the next process's stdin. To pipe data, call this 
function with a '|' between the next program to accept the data. Otherwise data will be streamed 

"python titanant.py | ./osdsystem"

Meant to be run in Python 3.
'''

controller = AntPlusController()

front_pwr = None
front_hrm = None
rear_pwr = None
rear_hrm = None

# Test/default channel parameters
front_pwr_channel = [10,11,5]
front_hrm_channel = [20,120,1]
rear_pwr_channel = [30,11,5]
rear_hrm_channel = [40,120,1]
#print("\n!!!!!!!!!!!!!!!!!!!!!!!!!!\nTEST CHANNEL DATA LOADED\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n")

# List of our devices
# device_number, device_type, transmission_type
calvin_pedals_channel = [30567,11,165] # Calvin's Pedals
calvin_hrm_channel = [52247, 120, 1] # Calvin's HRM
evan_pedals_channel = [36424,11,5] # Evan's Pedals
evan_hrm_channel = [41034, 120, 1] # Evan's HRM

'''
# Actual channels used, if uncommented
front_pwr_channel = calvin_pedals_channel
front_hrm_channel = calvin_hrm_channel

rear_hrm_channel = evan_hrm_channel
rear_hrm_channel = evan_hrm_channel

print("\n!!!!!!!!!!!!!!!!!!!!!!!!!!\nREAL CHANNEL DATA LOADED\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n")
'''

front_pwr = PwrDevice(controller.node, front_pwr_channel[0], front_pwr_channel[1], front_pwr_channel[2])
front_hrm = HrmDevice(controller.node, front_hrm_channel[0], front_hrm_channel[1], front_hrm_channel[2])
rear_pwr = PwrDevice(controller.node, rear_pwr_channel[0], rear_pwr_channel[1], rear_pwr_channel[2])
rear_hrm = HrmDevice(controller.node, rear_hrm_channel[0], rear_hrm_channel[1], rear_hrm_channel[2])

def stop():
    if front_pwr:
        front_pwr.close()
    if front_hrm:
        front_hrm.close()
    if rear_pwr:
        rear_pwr.close()
    if rear_hrm:
        rear_hrm.close()
    if controller:
        controller.close()

try:
    # Print configuration (only seen if run like a normal script (not piped))
    
    print("CHANNEL CONFIG:")
    print("Front pedals: ", front_pwr_channel)
    print("Front HRM:    ", front_hrm_channel)
    print("Rear pedals:  ", rear_pwr_channel)
    print("Rear HRM:     ", rear_hrm_channel)

    print("\n   FRONT   |   REAR    ")
    print("HRM CAD PWR|HRM CAD PWR")
    time.sleep(1)

    while (1):
        time.sleep(0.5)

        front_cur_hr = 0
        front_cur_cad = 0
        front_cur_pwr = 0
        rear_cur_hr = 0
        rear_cur_cad = 0
        rear_cur_pwr = 0

        if front_pwr:
            front_cur_pwr = front_pwr.status["instant_power"]
            front_cur_cad = front_pwr.status["instant_cadence"]
        if front_hrm:
            front_cur_hr = front_hrm.status["calculated_heart_rate"]
        if rear_pwr:
            rear_cur_pwr = rear_pwr.status["instant_power"]
            rear_cur_cad = rear_pwr.status["instant_cadence"]
        if rear_hrm:
            rear_cur_hr = rear_hrm.status["calculated_heart_rate"]
        
        #Use stdout to pipe data to another process (the display)
        stdout.write("%03d,%03d,%03d,%03d,%03d,%03d\n" % ( 
            front_cur_hr, front_cur_cad, front_cur_pwr,
            rear_cur_hr, rear_cur_cad, rear_cur_pwr))
        
        try:
            stdout.flush()
        except IOError as e:
            if e.errno == errno.EPIPE:
                pass # Don't let broken pipes cause issues
except KeyboardInterrupt or BrokenPipeError:
    # Broken pipe implies OSD has malfunctioned/closed so also treat as exit
    print("\n\nEnding ANT Data Collection")
    stop()
    print("Terminated ANT Data Collection")
    print("Exiting")
else:
    pass