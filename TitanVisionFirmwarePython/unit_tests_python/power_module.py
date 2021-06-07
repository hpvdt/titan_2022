# -*- coding: utf-8 -*-
""" Power monitoring module
Uses this fork of python-ant
https://github.com/bissont/python-ant

Based on the example in demos with some minor changes
"""

import time

from ant.core import driver
from ant.core.node import Node, Network, ChannelID
from ant.core.constants import NETWORK_KEY_ANT_PLUS, NETWORK_NUMBER_PUBLIC
from ant.plus.power import *

device = driver.USB2Driver(log=None, debug=False, idProduct=0x1008)
antnode = Node(device)
antnode.start()
network = Network(key=NETWORK_KEY_ANT_PLUS, name='N:ANT+')
antnode.setNetworkKey(NETWORK_NUMBER_PUBLIC, network)

def device_found(self, channelID):
    conDeviceID = channelID.deviceNumber # Store the device's number
    print(channelID)
    print("Detect monitor device number: %d, transmission type: %d" %
          (conDeviceID , channelID.transmissionType))
    
def power_data(eventCount, pedalDiff, pedalPowerRatio, cadence, accumPower, instantPower):
    print ("Instant Power: %d, InstantCadence: %d" % (instantPower,cadence))

power  = BicyclePower(antnode, network, callbacks = {'onDevicePaired': device_found,
                                              'onPowerData': power_data})

# Unpaired, search:
#power.open() 
power.open(ChannelID(30567,11,165)) # Calvin's pedals ID is 30567

time.sleep(10)

power.close()
antnode.stop()
