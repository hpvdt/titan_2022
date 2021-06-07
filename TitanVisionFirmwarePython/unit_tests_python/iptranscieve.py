import socket
import time

UDP_IP = "192.168.0.11"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)


sock.sendto('1', (UDP_IP, UDP_PORT))
print('sent 1')
while (True):
    data, addr = sock.recvfrom(1024)
    print("Recieved ", data, " from ", addr[0])
    sock.sendto("b", addr)
    time.sleep(1)

class IPTX:
    
    def __init__(self):
        sock.bind(('', UDP_PORT)) # Binds socket

        
    def getStaticIP(self):
        dhcpConfigFile = open('/etc/dhcpcd.conf', 'r') # Opens config file to read
        ownIP = "none" # Initial assumption

        contents = dhcpConfigFile.readlines() # Gets contents and closes
        dhcpConfigFile.close()

        for line in contents:
            if line.startswith("static ip_address="):
                # Reached the line containing the static IP
                ownIP = line[line.find('192'):line.find('/24')]
                # Gets IP from the line knowing the start and end characters
                #print('Static IP address is: ' + ownIP)

        if ownIP == 'none':
            # What to do if there isn't a static IP declared
            print('No static IP address defined for this rpi!')
            print('Define a static ip in "/etc/dhcpcd.conf"')

        return ownIP

    def ping(self):
        
