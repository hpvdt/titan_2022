import serial # Needed to communicate between rpi and arduino

'''
This program is meant to send a request to an Arduino over serial. This is a
single character, e.g. 's' for speed. The Arduino is then expected to reply
with an appropriate message.

The message from the Arduino starts with a character which tells the rpi how
long the remainder of the message is. This length is then used to read the
remainder of the message which is then printed. NOTE: this length character
uses the character ASCII code, with an offset of 31 applied!
 
Data characters (requests are lowercase, setting values are uppercase):
a - Heart rate (front) (BPM)
b - Heart rate (rear) (BPM)
c - Cadence (front) (RPM)
d - Cadence (rear) (RPM)
e - Power (front) (W)
f - Power (rear) (W)

i - Front Battery %
j - Rear Battery %
k - Backup System Battery %

h - Humidity (RH%)
s - Speed (km/h)
t - Temperature (Celsius + 50, offset just in case we go into negative)

q - Distance (number of rotations)

l - Latitude (Degrees)
m - Longitude (Degrees)
n - Altitude (m)
o - GPS Speed (km/h)
p - GPS Distance (km)

y - Test 1
z - Test 2
'''

class serialLine:

    def __init__(self, BAUDRATE = 9600):
        # Opens serial port on GPIO pins, with default baudrate of 9600
        portTemp = serial.Serial('/dev/serial0', baudrate=BAUDRATE, timeout = 3.0)
        portTemp.flush() # Flushes anything in buffers
        # print('made port')  # Used for testing

        self.port = portTemp # Makes it accessible after init

    def requestData(self, requestChar):
        # Sends a request and returns the response

        # Sends request
        self.port.write(requestChar)
        self.port.flush()    # Forces the output to sent (from the buffer)

        # Reads in data from response
        length = ord(self.port.read(1))  # Gets the data frame length from character code
        length = length - 31         # Removes offset of character
        # (I wanted to keep the length character in the printable range of ASCII (>31))

        response = str(self.port.read(length)) # Reads in expected message length
        
        return response

    def sendData(self, typeChar, data):
        # Sends data to over the serial line
        # Message structure is typeChar, length, data
        
        # Get length
        length = len(data)
        length = length + 31 # Move into printable range

        # Combine into one message and call to serial line
        output = str(typeChar) + str(chr(length)) + data
        self.port.write(output) 
        self.port.flush()               # Force output

    def testLine(self, testChar):
        print('- Testing serial communications with STM32 -')
        import random # Needed for random numbers
        
        print("Perfoming echo test on register %s" % testChar)
        randomVal = random.randint(32,100) # Gets a random number to use for test
        
        # Perform echo test
        self.sendData(testChar.upper(), chr(randomVal)) # Sends random number
        print("Sent echo value of %d to the register" % randomVal)

        assert self.requestData(testChar.lower()) == chr(randomVal) # Test echo was correct
        print("Echo returned properly")
        
        print("Successful serial test with STM32!")
        
if __name__ == "__main__":
    # Tests serial by bouncing a random number through the arduino
    serialTestLine = serialLine(57600)

    serialTestLine.testLine('z')
    

