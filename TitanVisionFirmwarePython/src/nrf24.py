### https://github.com/r3ek0/python-nrf24
### ^ Source
import time
import spidev
import wiringpi #Originally 'import wiringpi2 as wiringpi'

class Nrf24:

	def __init__(self,cePin=2,csnPin=3,channel=1,payload=8):
		# we use wiringPi pin numbering!
		self.gpio = wiringpi.GPIO(wiringpi.GPIO.WPI_MODE_PINS) 
		self.cePin = cePin      # pin number for CE
		self.csnPin = csnPin    # pin number for CSN
		self.channel = channel  # RF channel to be used
		self.payload = payload	# size of payload in bytes
		
		self.gpio.pinMode(self.cePin,self.gpio.OUTPUT); #set ce as output
		self.gpio.pinMode(self.csnPin,self.gpio.OUTPUT); #set csn as output

		self.ceLow()
		self.csnHi()

		self.spi = spidev.SpiDev()
		self.spi.open(0,0)


	def printSetup(self):
		rf_setup = self.readRegister(self.RF_SETUP,1)
		rt_setup = self.readRegister(self.SETUP_RETR,1)
		rf_channel = self.readRegister(self.RF_CH,1)
		config = self.readRegister(self.CONFIG,1)
		
		print "RF_SETUP  : %s" % str(bin(rf_setup[0]))
		print "RT_SETUP  : %s" % str(bin(rt_setup[0]))
		print "RF_CH     : %s" % str(bin(rf_channel[0]))
		print "CONFIG    : %s" % str(bin(config[0]))

		
	def printStatus(self):
		fifostate = self.readRegister(self.FIFO_STATUS,1)
		observe_tx = self.readRegister(self.OBSERVE_TX,1)
		print "status     : %s" % str(bin(self.getStatus()))
		print "fifo status: %s" % str(bin(fifostate[0]))
		print "tx stats   : %s" % str(bin(observe_tx[0])) 


	def config(self): #void 
		self.powerDown()
		self.configRegister(self.SETUP_RETR,0b11111)
		self.configRegister(self.RF_CH, self.channel)     # set the RF channel
		self.configRegister(self.RX_PW_P0, self.payload)  # set size of payload
		self.configRegister(self.RX_PW_P1, self.payload)  #  for incoming pkgs
		self.powerUpRx()
		self.flushRx()

	def send(self,value): #void
		status = self.getStatus()
		while self.PTX:
			status = self.getStatus()
			if (status & ((1 << self.TX_DS) | (1 << self.MAX_RT))) is not 0:
				self.PTX = 0
				break

		self.ceLow()
		self.powerUpTx()
		
		self.csnLow()
		self.spi.xfer2([ self.FLUSH_TX ])
		self.csnHi()

		self.csnLow()
		self.spi.xfer2([ self.W_TX_PAYLOAD])
		self.spi.xfer2(value)
		self.csnHi()
		self.ceHi()			

	# set the own address to receive packages
	# raddr can be a string
	def setRADDR(self,raddr): 
		addr = map(ord,raddr)
		self.ceLow()
		self.writeRegister(self.RX_ADDR_P1,addr,self.mirf_ADDR_LEN)
		self.ceHi()

	# address of the device which should receive the package
	# taddr can be a string
	def setTADDR(self,taddr): 
		addr = map(ord,taddr)
		self.ceLow();
		self.writeRegister(self.RX_ADDR_P0,addr,self.mirf_ADDR_LEN)
		self.writeRegister(self.TX_ADDR,addr,self.mirf_ADDR_LEN)
		self.ceHi()


	# check if there is data in one of the FIFOs which can be read.
	def dataReady(self): #bool
		status = self.getStatus()
		if (status & (1 << self.RX_DR) ) is not 0:
			return True;
		return not self.rxFifoEmpty()

	# check if the RX FIFO is empty.
	def rxFifoEmpty(self): #bool
		fifoStatus = self.readRegister(self.FIFO_STATUS,1)
		return (fifoStatus[0] & (1 << self.RX_EMPTY))
	
	# check if the module is currently sending.
	def isSending(self): #bool
		if self.PTX > 0:
			status = self.getStatus()
			
			if  (( status & ((1<<self.TX_DS)  | (1<<self.MAX_RT)))) is not 0:
				self.powerUpRx()
				return False
			
			return True
		return False

	# read available RX data from the module
	def getData(self):
		self.ceLow()
		self.csnLow()
		self.spi.writebytes([ self.R_RX_PAYLOAD ])
		data = self.spi.xfer2([0] * self.payload)
		self.csnHi()
		self.ceHi()
		self.configRegister(self.STATUS,(1<<self.RX_DR))
		return data


	# read the status register
	def getStatus(self): #uint8_t
		self.csnLow()
		self.spi.writebytes([self.STATUS])
		state = self.spi.readbytes(1)
		self.csnHi()
		return state[0]

	# write configurations to the register reg
	def configRegister(self,reg,value):
		self.csnLow()
		self.spi.writebytes([self.W_REGISTER | (self.REGISTER_MASK & reg)])
		self.spi.writebytes([value])
		self.csnHi()

	# read length bytes from register reg 
	def readRegister(self,reg,length):
		self.csnLow()
		self.spi.writebytes([self.R_REGISTER | reg])
		return_value = self.spi.readbytes(length)
		self.csnHi()
		return return_value

	# write bytes to register reg
	def writeRegister(self,reg,value,length):
		self.csnLow()
		if not isinstance(value,list):
			value = [value]
		self.spi.xfer2([self.W_REGISTER | reg] + value)
		self.csnHi()
		
	def powerUpRx(self):
		self.PTX = 0
		self.ceLow()
		self.configRegister(self.CONFIG,self.mirf_CONFIG | ( (1<<self.PWR_UP |	(1<<self.PRIM_RX))))
		self.ceHi()
		self.configRegister(self.STATUS, (1 << self.TX_DS) | (1<<self.MAX_RT))

	def powerUpTx(self):
		self.PTX = 1
		self.configRegister(self.CONFIG, self.mirf_CONFIG | ( (1<<self.PWR_UP) | (0<<self.PRIM_RX) ))

	def powerDown(self):
		self.ceLow()
		self.configRegister(self.CONFIG,self.mirf_CONFIG)

	# set CSN pin HI
	def csnHi(self):
		self.gpio.digitalWrite(self.csnPin, self.gpio.HIGH)

	# set CSN pin LOW
	def csnLow(self):
		self.gpio.digitalWrite(self.csnPin, self.gpio.LOW)

	# set CE pin HI
	def ceHi(self):
		self.gpio.digitalWrite(self.cePin, self.gpio.HIGH)

	# set CE pin LOW
	def ceLow(self):
		self.gpio.digitalWrite(self.cePin, self.gpio.LOW)

	def flushRx(self):
		self.csnLow()
		self.spi.xfer2( [self.FLUSH_RX] )
		self.csnHi()

	# Memory Map 
	PTX         = 0 
	CONFIG      = 0x00
	EN_AA       = 0x01
	EN_RXADDR   = 0x02
	SETUP_AW    = 0x03
	SETUP_RETR  = 0x04
	RF_CH       = 0x05
	RF_SETUP    = 0x06
	STATUS      = 0x07
	OBSERVE_TX  = 0x08
	CD          = 0x09
	RX_ADDR_P0  = 0x0A
	RX_ADDR_P1  = 0x0B
	RX_ADDR_P2  = 0x0C
	RX_ADDR_P3  = 0x0D
	RX_ADDR_P4  = 0x0E
	RX_ADDR_P5  = 0x0F
	TX_ADDR     = 0x10
	RX_PW_P0    = 0x11
	RX_PW_P1    = 0x12
	RX_PW_P2    = 0x13
	RX_PW_P3    = 0x14
	RX_PW_P4    = 0x15
	RX_PW_P5    = 0x16
	FIFO_STATUS = 0x17

	# Bit Mnemonics
	MASK_RX_DR  = 6
	MASK_TX_DS =  5
	MASK_MAX_RT=  4
	EN_CRC     =  3
	CRCO       =  2
	PWR_UP     =  1
	PRIM_RX     = 0
	ENAA_P5    =  5
	ENAA_P4    =  4
	ENAA_P3    =  3
	ENAA_P2    =  2
	ENAA_P1    =  1
	ENAA_P0   =   0
	ERX_P5      = 5
	ERX_P4     =  4
	ERX_P3     =  3
	ERX_P2     =  2
	ERX_P1     =  1
	ERX_P0     =  0
	AW         =  0
	ARD        =  4
	ARC        =  0
	PLL_LOCK   =  4
	RF_DR      =  3
	RF_PWR     =  1
	LNA_HCURR  =  0        
	RX_DR      =  6
	TX_DS      =  5
	MAX_RT     =  4
	RX_P_NO    =  1
	TX_FULL    =  0
	PLOS_CNT   =  4
	ARC_CNT    =  0
	TX_REUSE   =  6
	FIFO_FULL  =  5
	TX_EMPTY   =  4
	RX_FULL    =  1
	RX_EMPTY   =  0

	# Instruction Mnemonics 
	R_REGISTER   =  0x00
	W_REGISTER   =  0x20
	REGISTER_MASK=  0x1F
	R_RX_PAYLOAD =  0x61
	W_TX_PAYLOAD =  0xA0
	FLUSH_TX     =  0xE1
	FLUSH_RX     =  0xE2
	REUSE_TX_PL  =  0xE3
	NOP          =  0xFF

	mirf_CONFIG  = ((1<<3) | (0<<2) )
	mirf_ADDR_LEN = 5

