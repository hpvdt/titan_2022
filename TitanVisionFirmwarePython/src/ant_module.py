""" ANT+ Module
Has the functions and profiles

Uses this fork of python-ant
https://github.com/bissont/python-ant

Based on the example in demos with some minor changes
"""

from ant.core import driver
from ant.core.node import Node, Network, ChannelID
from ant.core.constants import NETWORK_KEY_ANT_PLUS, NETWORK_NUMBER_PUBLIC
from ant.plus.power import *
from ant.plus.heartrate import *

device = driver.USB2Driver(log=None, debug=False, idProduct=0x1008)
antnode = Node(device)
antnode.start()
network = Network(key=NETWORK_KEY_ANT_PLUS, name='N:ANT+')
antnode.setNetworkKey(NETWORK_NUMBER_PUBLIC, network)

# Known devices, update as needed
calvinPedals = ChannelID(30567,11,165) # Calvin's Pedals
calvinHRM = ChannelID(52247, 120, 1) # Calvin's HRM
evanPedals = ChannelID(36424,11,5) # Evan's Pedals
evanHRM = ChannelID(41034, 120, 1) # Evan's HRM

def power_device_found(self, channelID):
    self.conDeviceID = channelID.deviceNumber # Store the device's number
    # Below stores IDs of all the different power pedals
    powerNames = {
        calvinPedals.deviceNumber: "Calvin pedals",
        evanPedals.deviceNumber: "Evan pedals"
        }
    print(channelID)
    print("Detect monitor device number: %d, transmission type: %d, description: %s" %
          (self.conDeviceID, channelID.transmissionType, powerNames.get(self.conDeviceID, "Unknown")))
    
def power_data(eventCount, pedalDiff, pedalPowerRatio, cadence, accumPower, instantPower):
    print ("Instant Power: %d, InstantCadence: %d" % (instantPower,cadence))
##    self.power = instantPower
##    self.cadence = cadence

def hr_device_found(self, channelID):
    self.conDeviceID = channelID.deviceNumber # Store the device's number
    # Below stores IDs of all the different power pedals
    hrmNames = {
        calvinHRM.deviceNumber: "Calvin HRM",
        evanHRM.deviceNumber: "Evan HRM"
        }
    print(channelID)
    print("Detect monitor device number: %d, transmission type: %d, description: %s" %
          (self.conDeviceID, channelID.transmissionType, hrmNames.get(self.conDeviceID, "Unknown")))
    
def heartrate_data(computed_heartrate, event_time_ms, rr_interval_ms):
    print("Current Heart Rate: %d" % computed_heartrate)


# Example expressions:

##power  = BicyclePower(antnode, network, callbacks = {'onDevicePaired': power_device_found, 'onPowerData': power_data})
##hr = HeartRate(antnode, network, callbacks = {'onDevicePaired': hr_device_found, 'onHeartRateData': heartrate_data})

# Unpaired, search:
#power.open()
#hr.open()

if __name__ == "__main__":
    front_hr = HeartRate(antnode, network, callbacks = {'onDevicePaired': hr_device_found, 'onHeartRateData': heartrate_data})
    rear_hr = HeartRate(antnode, network, callbacks = {'onDevicePaired': hr_device_found, 'onHeartRateData': heartrate_data})

    front_power = BicyclePower(antnode, network, callbacks = {'onDevicePaired': power_device_found, 'onPowerData': power_data})
    rear_power = BicyclePower(antnode, network, callbacks = {'onDevicePaired': power_device_found, 'onPowerData': power_data})

    print("Connecting front devices")
    front_power.open(calvinPedals)
    front_hr.open(calvinHRM)

    print("Connecting rear devices")
    rear_power.open(evanPedals)
    rear_hr.open(evanHRM)

    print("Connected all devices")
