# -*- coding: utf-8 -*-
""" Heart rate monitoring module
Uses this fork of python-ant
https://github.com/bissont/python-ant

Based on the example in demos with some minor changes
"""

import time

from ant.core import driver
from ant.core.node import Node, Network, ChannelID
from ant.core.constants import NETWORK_KEY_ANT_PLUS, NETWORK_NUMBER_PUBLIC
from ant.plus.heartrate import *
from ant.core.message import *

from config import *

device = driver.USB2Driver(log=LOG, debug=DEBUG, idProduct=0x1008)
antnode = Node(device)
antnode.start()
network = Network(key=NETWORK_KEY_ANT_PLUS, name='N:ANT+')
antnode.setNetworkKey(NETWORK_NUMBER_PUBLIC, network)

conDeviceID = 0

def device_found(self, channelID):
    conDeviceID = channelID.deviceNumber # Store the device's number
    print("Detect monitor device number: %d, transmission type: %d" %
          (conDeviceID , channelID.transmissionType))
    

def heartrate_data(computed_heartrate, event_time_ms, rr_interval_ms):
    if rr_interval_ms is not None:
        print("Heart rate: %d, event time(ms): %d, rr interval (ms): %d" %
              (computed_heartrate, event_time_ms, rr_interval_ms))
    else:
        print("Heart rate: %d" % (computed_heartrate, ))

hr = HeartRate(antnode, network, callbacks = {'onDevicePaired': device_found,
                                              'onHeartRateData': heartrate_data})

# Unpaired, search:
#hr.open()

# Paired to a specific device:
hr.open(ChannelID(52247, 120, 1)) # Our Garmin HRM1G
#hr.open(ChannelID(21840, 120 ,81))

monitor = None

time.sleep(10)

hr.close()
antnode.stop()
