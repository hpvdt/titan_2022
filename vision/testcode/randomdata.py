
from random import randint

import random
import time
from sys import stdout

front_pwr_channel = [10,11,5]
front_hrm_channel = [20,120,1]
rear_pwr_channel = [30,11,5]
rear_hrm_channel = [40,120,1]


print("CHANNEL CONFIG:")
print("Front pedals: ", front_pwr_channel)
print("Front HRM:    ", front_hrm_channel)
print("Rear pedals:  ", rear_pwr_channel)
print("Rear HRM:     ", rear_hrm_channel)

print("\n   FRONT   |   REAR    ")
print("HRM CAD PWR|HRM CAD PWR")


while (1) :
    data = [0, 0, 0, 0, 0, 0]
    for i in range(6) :
        data[i] = randint(0,499)

    # Need to write to stdout explicitly, print doesn't work
    stdout.write("%03d,%03d,%03d,%03d,%03d,%03d\n" % (data[0],data[1], data[2], data[3], data[4], data[5]))
    stdout.flush()
    time.sleep(1)