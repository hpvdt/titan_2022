#!/bin/python
# this script simply prints the pin number of an event

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

for i in range(27):
    GPIO.add_event_detect(i, GPIO.RISING)  # add rising edge detection on a channel

start = time.time()
while True:
    for i in range(27):
        if GPIO.event_detected(i):
            print('Button %d pressed' % i)
