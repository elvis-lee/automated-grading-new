#! /usr/bin/python
import serial
import string
from timeit import Timer
import timeit
import time

BAUDRATE = 460800
flag_end_of_tranfer = 0
data = ""



ser = serial.Serial()
ser.port = "/dev/ttyUSB0"    
ser.baudrate = BAUDRATE    
ser.parity=serial.PARITY_NONE
ser.bytesize=serial.EIGHTBITS
ser.stopbits=serial.STOPBITS_ONE
ser.timeout=1
ser.writeTimeout=None
ser.open() 

#Get Packet Ready here
f1 = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'rb')
data = f1.read()


def com1():
 global flag_end_of_tranfer
 i = 0
 #Send Packet
 ser.write(data)

 #Send "T" Start Signal Generation on Haredware Engine
 ser.write("ST000000E")

 #Receive Packet

 while (not flag_end_of_tranfer):
 	  	i +=1 ;
 	  	while(ser.in_waiting < 9):
 			pass
		rec_pack = ser.read(9)
		print "Received=%d packets" %i 
		print rec_pack
		print(' '.join(format(ord(x), 'b') for x in rec_pack[2])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[3])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[4])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[5])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[6])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[7])).zfill(8)
	

		if (rec_pack[1] == 'E'):
			flag_end_of_tranfer = 1;
	
		
 return

t=Timer(com1,setup="") 
t1=t.timeit(1)
print "Total Time=%fs" %(t1) 