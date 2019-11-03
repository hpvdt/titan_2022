from nrf24 import Nrf24
import time

nrf = Nrf24(cePin=2,csnPin=3,channel=10,payload=8)
nrf.config()
nrf.setRADDR("rpi")
nrf.setTADDR("ard")

while (True):
    if not nrf.isSending():
        nrf.send(map(ord,"Helloooo"))

    while not nrf.dataReady():
        time.sleep(1)
    print nrf.getData()
    
    time.sleep(1)
    print("sent")

##while True:
##    nrf.printStatus()
##    if nrf.dataReady():
##    	print nrf.getData()
##    time.sleep(1)
