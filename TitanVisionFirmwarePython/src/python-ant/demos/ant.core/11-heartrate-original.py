# -*- coding: utf-8 -*-
"""Demonstrate the use of the ANT+ Heart Rate Device Profile

"""

import time

from ant.core import driver
from ant.core.node import Node, Network, ChannelID
from ant.core.constants import NETWORK_KEY_ANT_PLUS, NETWORK_NUMBER_PUBLIC
from ant.plus.heartrate import *

from config import *

device = driver.USB2Driver(log=LOG, debug=DEBUG, idProduct=0x1008)
antnode = Node(device)
antnode.start()
network = Network(key=NETWORK_KEY_ANT_PLUS, name='N:ANT+')
antnode.setNetworkKey(NETWORK_NUMBER_PUBLIC, network)

def device_found(device_number, transmission_type):
    print("Detect monitor device number: %d, transmission type: %d" %
          (device_number, transmission_type))

def heartrate_data(computed_heartrate, event_time_ms, rr_interval_ms):
    print(hr.channel.id.deviceNumber)
    if rr_interval_ms is not None:
        print("Heart rate: %d, event time(ms): %d, rr interval (ms): %d" %
              (computed_heartrate, event_time_ms, rr_interval_ms))
    else:
        print("Heart rate: %d" % (computed_heartrate, ))

hr = HeartRate(antnode, network, callbacks = {'onDevicePaired': device_found,
                                              'onHeartRateData': heartrate_data})

# Unpaired, search:
hr.open()

# Paired to a specific device:
#hr.open(ChannelID(23359, 120, 1))
#hr.open(ChannelID(21840, 120 ,81))

monitor = None
while True:
    try:
        time.sleep(1)
    except KeyboardInterrupt:
        break

hr.close()
antnode.stop()
