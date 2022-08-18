import picamera
import os
from time import sleep
camera = picamera.PiCamera()
camera.resolution = (1280, 720)
camera.framerate = 60


camera.start_preview()

# Get file count to append to video title
videoDir = '/home/pi/Videos'
fileCount = len([name for name in os.listdir(videoDir) if os.path.isfile(os.path.join(videoDir, name))])
print(fileCount)
recordingLocation = "{}/video{:03d}.h264".format(videoDir, fileCount)

camera.start_recording(recordingLocation)

# To convert from .h264 to .mp4 use the following command
# ffmpeg -r 60 -i video.h264 -c:v copy video.mp4

try:
    while (1):
        pass
except KeyboardInterrupt:
    camera.stop_preview()
    camera.stop_recording()
else:
    pass