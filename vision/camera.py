import picamera
from time import sleep
camera = picamera.PiCamera()
camera.resolution = (1280, 720)
camera.framerate = 60


camera.start_preview()
camera.start_recording('/home/pi/video.h264')

try:
    while (1):
        pass
except KeyboardInterrupt:
    camera.stop_preview()
    camera.stop_recording()
else:
    pass