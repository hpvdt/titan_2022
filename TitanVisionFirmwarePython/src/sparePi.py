#!/usr/bin/env python2
from picamera import PiCamera, PiCameraRuntimeError, PiCameraError
from traceback import print_exc
from os import system
from time import sleep
from config import RESOLUTION

try:
    with PiCamera() as camera:
        camera.resolution = RESOLUTION
        print("==================================================")
        print("")
        print("              Starting Backup Monitor             ")
        print("")
        print("==================================================")
        sleep(5)
        while True:
           try:
              camera.start_preview()
              while (True):
                 assert camera.preview is not None # make sure that still previewing
           except PiCameraRuntimeError: # raised if connection lost
              print("camera failed. Trying to reconnect ...")
              print_exc()
              camera.stop_preview()
           except: # catch other exception
              print_exc()
              camera.stop_preview()
              break
except PiCameraError: # if camera not connected
    print_exc()
    for i in range(5, 0, -1):
        print("rebooting in %d seconds." % i)
        sleep(1)
    system("sudo reboot")
except KeyboardInterrupt: # if ctrl-c
    raise
except:
    print_exc()
    print("*"*80)
    print("Uncaught error: REBOOTING IN 30 SECONDS.")
    print("*"*80)
    sleep(30)
    system("sudo reboot")





