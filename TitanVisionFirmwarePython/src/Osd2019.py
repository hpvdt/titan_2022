import picamera
import time
import numpy
import os # Used to pipe the errors from the update instruction to null

fd = os.open('/dev/null',os.O_WRONLY) #Address for ignoring errors
savefd = os.dup(2) #Normal error address
'''
Currently we are only ignoring errors around the overlay update function. This means
we can see errors if they appear as a result of other code, however constanly switching
back and forth where we output errors reduces performance by 20%, raising updates from
~70 ms to ~90 ms.

Once we are satisifed with all code we should ignore all errors if performance increase
is needed/desired.
'''
from PIL import Image, ImageDraw, ImageFont

#############
# Constants
#############

KMTOMI = 0.621371 # Factor to multiple km to get miles
TOTAL_DISTANCE = 8046.72 # Race length (5 miles converted to meters)

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 225)
YELLOW = (225, 225, 0)
CYAN = (0, 225, 225)
PURPLE = (225, 0, 225)
WHITE = (225, 225, 225)

# Limits used to decide colour of performance percentage
lowerPerfLimit = 98
upperPerfLimit = 102

class Osd:
    # Video Settings
    VIDEO_HEIGHT = 720
    VIDEO_WIDTH = 1280
    FRAMERATE = 60
    BRIGHTNESS = 50

    RECORDING = False  # Recording video feed?
    
    mCurrentOverlayLayer = 6 # Used to prevent errors when updating overlay
    mOverlay = None
    
    def __init__(self, recording = False):
        # Camera settings
        camera = picamera.PiCamera()
        camera.resolution = (self.VIDEO_WIDTH, self.VIDEO_HEIGHT)
        camera.framerate = self.FRAMERATE
        camera.brightness = self.BRIGHTNESS
        self.camera = camera
        self.camera.start_preview()
        
        # Setup recording file
        self.RECORDING = recording # Record if recording or not
        if recording:
            video_title = "{}.h264".format(time.strftime('%y%m%d-%H:%M:%S', time.localtime()))
            self.video_title = '/home/pi/Videos/recording-' + video_title
            self.camera.start_recording(self.video_title)

        self.canvas = Image.new("RGB", (self.VIDEO_WIDTH, self.VIDEO_HEIGHT))
        self.img = self.canvas.copy()
        self.overlay = camera.add_overlay(self.img.tobytes(), layer=3, alpha=100)

    def _renderText(self, text, position, color=WHITE, size=40):
        font = ImageFont.truetype("/home/pi/Desktop/TitanVisionFirmware/res/consola.ttf", size)
        draw = ImageDraw.Draw(self.img)
        draw.text(position, text, color, font)

    def RenderTime(self):
        timeText = "Time: {0}".format(time.strftime('%H:%M:%S'))
        self._renderText(timeText, (10, 10))

    def RenderBatteryPercentage(self, batteryPercentage):
        batteryText = "Battery:{:3.0f}%".format(round(batteryPercentage,0))
        if batteryPercentage < 30:
            batteryTextColour = RED
        else:
            batteryTextColour = GREEN
        self._renderText(batteryText,
                         (1000, 10),
                         batteryTextColour,
                         30)

    def RenderSpeed(self, speedKph):
        speedText = "       Speed:"
        self._renderText(speedText,
                         (10, 560),
                         WHITE,
                         30)

        speedText = " {:5.1f} KPH /{:5.1f} MPH".format(speedKph, speedKph * KMTOMI)
        self._renderText(speedText,
                         (210, 550),
                         CYAN,
                         50)

    def RenderPerfPercentage(self, perfper):
        perfText = "Performance:"
        self._renderText(perfText,
                         (10, 610),
                         WHITE,
                         30)
        colour = YELLOW # Default colour for acceptable
        if perfper <= lowerPerfLimit:
            colour = RED # Below accepted
        elif perfper >= upperPerfLimit:
            colour = GREEN # Above accepted
        
        perfText = " {:3.1f} %".format(perfper)
        self._renderText(perfText,
                         (210, 600),
                         colour,
                         50)

    def RenderCadence(self, cadence):
        cadenceText = "Cadence:"
        self._renderText(cadenceText,
                         (905, 610),
                         WHITE,
                         30)

        cadenceText = "{:3d} RPM".format(cadence)
        self._renderText(cadenceText,
                         (1060, 600),
                         CYAN)
    def RenderPower(self, power):
        powerText = "Power:"
        self._renderText(powerText,
                         (940, 560),
                         WHITE,
                         30)

        powerText = "{:3d} W".format(power)
        self._renderText(powerText,
                         (1060, 550),
                         CYAN)
    def RenderHR(self, heartRate):
        heartRateText = "HR:"
        self._renderText(heartRateText,
                         (992, 510),
                         WHITE,
                         30)

        heartRateText = "{:3d} BPM".format(heartRate)
        self._renderText(heartRateText,
                         (1060, 500),
                         CYAN)
    def RenderTemp(self, temperature):
        temperatureText = "Temp: {:4.1f}C".format(temperature)
        self._renderText(temperatureText,
                         (700, 10),
                         GREEN,
                         30)
    def RenderHumidity(self, humidity):
        humidityText = "R.H.: {:4.1f}%".format(humidity)
        self._renderText(humidityText,
                         (400, 10),
                         GREEN,
                         30)
        
    def RenderDistance(self, distance):
        tickPosition = int((distance / TOTAL_DISTANCE) * 1170) + 50

        # Make the tick blink
        if ((time.time() % 1) < 0.5):
            self._renderText(">",
                             (tickPosition, 660),
                             WHITE,
                             30)

        # Distance comes in as m, convert to miles for text
        distanceText = "{:0.2f} M".format((distance / 1000) * KMTOMI)
        distanceTextPosition = tickPosition - 40
        if (distanceTextPosition < 50):
            distanceTextPosition = 50
        elif (distanceTextPosition > 1130):
            distanceTextPosition = 1130
        self._renderText(distanceText,
                         (distanceTextPosition, 690),
                         GREEN,
                         30)

        self._renderText("|",
                         (50, 660),
                         WHITE,
                         30)
        self._renderText("0 M",
                         (10, 690),
                         WHITE,
                         30)

        self._renderText("|",
                         (1220, 660),
                         WHITE,
                         30)
        self._renderText("5 M",
                         (1220, 690),
                         WHITE,
                         30)

    def Display(self,
            speedKph,
            perfPercent,
            cadence,
            distance,
            batteryPercentage,
            power,
            heartrate,
            temperature,
            humidity):
        self.img = self.canvas.copy()

        # Render everything
        self.RenderTime()
        self.RenderSpeed(speedKph)
        self.RenderPerfPercentage(perfPercent)
        self.RenderCadence(cadence)
        self.RenderDistance(distance)
        self.RenderBatteryPercentage(batteryPercentage)
        self.RenderPower(power)
        self.RenderHR(heartrate)
        self.RenderTemp(temperature)
        self.RenderHumidity(humidity)

        os.dup2(fd,2) #Pipe errors to null
        self.overlay.update(self.img.tobytes())
        os.dup2(savefd,2) #Pipe errors back normally

# The following code was to try and avoid having errors output by my code.
# It worked but had the side effect of occasionally cutting the video feed while also
# being slower to update the overlay by about 50 ms (~130ms)

#         theTmpOverlay = self.camera.add_overlay(self.img.tobytes(), alpha=100, layer=self.mCurrentOverlayLayer)
# 
#         self.mCurrentOverlayLayer = self.mCurrentOverlayLayer + 1
# 
#         if self.mCurrentOverlayLayer > 7:
#             self.mCurrentOverlayLayer = 6
#                         
#         if self.mOverlay != None:
#             self.camera.remove_overlay(self.mOverlay)
#                 
#         self.mOverlay = theTmpOverlay
            
    def Turnoff(self):
        if self.RECORDING:
            self.camera.stop_recording() #Stop recording if needed
        self.camera.stop_preview()
        self.camera.close()
        
        if self.RECORDING:
            print("Camera feed saved to: {:s}".format(self.video_title))
