UPDATE_INTERVAL = 0.2 # Target amount of time between OSD updates

DATA_LOGGING = False # Is there going to data logging?
# Is there meant to be a record of camera footage?
VIDEO_RECORDING_FRONT = False
VIDEO_RECORDING_REAR = False

SERIAL_BAUDRATE = 38400 # Baudrate to microcontroller, has to be matched on both

SCREEN_DELAY = 5 # Delay for screen delay after boot

RUN_TIME = -1 # Run screen for this many seconds (set to negative for endless)

WHEEL_CIRCUMFERENCE = 2.082 # Wheel circumference in meters

ANT_TIMEOUT = 1 # Timeout for searching for ANT devices

OVERRIDE_ENCODER = False # Decide whether to use GPS over optical encoder
# This means the bike will use GPS to determine speed and distance in place
# of the optical encoder. However this will only update every second.

CONFIG = "backup" # either "backup", "main", "rear" configurations. Backup only shows the camera.

RESOLUTION = (1280,720) # 720p screen resolution
