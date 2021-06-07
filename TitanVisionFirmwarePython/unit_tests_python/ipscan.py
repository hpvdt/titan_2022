import os

net = "192.168.0." #input("Enter the Network Address: ")

st1 = int(9)
en1 = int(15)
en1 = en1 + 1

pingcmd = "ping -c 1 "

print ("Scanning in Progress:")

for ip in range(st1,en1):
   addr = net + str(ip)
   comm = pingcmd + addr
   response = os.system(comm)
   

   
##   for line in response.readlines():
##      if(line.count("TTL")):
##         break
##      if (line.count("TTL")):
##         print (addr, "--> Live")

   print(response)
         
