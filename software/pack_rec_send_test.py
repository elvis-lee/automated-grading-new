#! /usr/bin/python
import serial
import string
from timeit import Timer
import timeit
import time
import threading

BAUDRATE = 460800
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

def send_pack():
	#Get Packet Ready here
	f1 = open('/home/embed-autograder/Workspace/autograder/backend/data/packetout', 'rb')
	data = f1.read()
	#Reset the hardware engine first!
	ser.write("SR000000E")
	#Wait for 1 second! Important! the hardware engine needs time to reset
	time.sleep(1)
	#DUT start 
	ser.write("SU000000E")
	#Send Packet
	ser.write(data)

def receive_pack():
	f = open('/home/embed-autograder/Workspace/autograder/backend/data/packet_file_from_DUT', 'wb')
	i = 0
	digital = 0
	analog = 0
	flag_end_of_tranfer = 0
	#Receive Packet
	while (not flag_end_of_tranfer):
	  	i +=1 ;
	  	while(ser.in_waiting < 9):
			pass
		rec_pack = ser.read(9)
		if (rec_pack[1] == 'D'):
			digital = digital + 1
		if (rec_pack[1] == 'A'):
			analog = analog + 1
		print "Received=%d packets" %i 
		print rec_pack
		print(' '.join(format(ord(x), 'b') for x in rec_pack[2])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[3])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[4])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[5])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[6])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[7])).zfill(8)
		f.write(rec_pack)

		if (rec_pack[1] == 'E'):	
			flag_end_of_tranfer = 1;

	print "digital packet=%d analog packet=%d" %(digital,analog) 


threading.Thread(target=send_pack).start()
threading.Thread(target=receive_pack).start()




