import serial # Needed to communicate between rpi and arduino

# This program is meant to send a request to an Arduino over serial. This is a
# single character, e.g. 's' for speed. The Arduino is then expected to reply
# with an appropriate message.
#
# The message from the Arduino starts with a character which tells the rpi how
# long the remainder of the message is. This length is then used to read the
# remainder of the message which is then printed. NOTE: this length character
# uses the character ASCII code, with an offset of 31 applied!

# Opens serial port on GPIO pins
port = serial.Serial('/dev/serial0', baudrate=9600, timeout = 3.0)
port.flush() # Flushes anything in buffers
# print('made port')  # Used for testing

# Sends request
port.write('t')
port.flush()    # Forces the output to sent (from the buffer)
# print('sent temp')  # Used for testing

# Reads in data
length = ord(port.read(1))  # Gets the data frame length from character code
length = length - 31         # Removes offset of character
# (I wanted to keep the length character in the printable range of ASCII (>31))
# print (length) # Used for testing

text = str(port.read(length)) # Reads in expected message length
print(text)

# print ('done script')  # Used for testing
