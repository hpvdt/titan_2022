#!/usr/bin/env python2
from __future__ import division, print_function

from config import * # Import configuartion values
from Osd2019 import Osd

import sys
import time
import random # Needed for random numbers

osdModule = Osd(False)

###################
# Setting variables
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
        speedKph = 10#float(microController.requestData('o'))
        distance = 10#float(microController.requestData('p'))
        temperature = updateCount#(ord(microController.requestData('t')) - 50) / 2
        humidity = 10#ord(microController.requestData('h')) / 2
        batteryPercentage = 10#int(microController.requestData(battChar))
        HR = 10#int(microController.requestData('a'))
        power = 10#int(microController.requestData('e'))
        cadence = 10#int(microController.requestData('c'))
        otherPower = 10#int(microController.requestData('f'))
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
   
        # Sleep between updates
        duration = time.time() - start_iteration_time # Find how long it took to render frame
        #print(duration)
        totalDuration = totalDuration + duration # Add to total
        updateCount = updateCount + 1
#         if duration < UPDATE_INTERVAL:
#             time.sleep(UPDATE_INTERVAL - duration)
        

    osdModule.Turnoff() # Clear screen afterwards
        
    print('Total time rendering ovarlays: ' + str(totalDuration))
    print('Updates made' + str(updateCount))
    print('Average update time was: ' + str(totalDuration / updateCount))
    
    
except KeyboardInterrupt:
    # Use ctrl + C to exit early
    osdModule.Turnoff() # Clear screen afterwards

