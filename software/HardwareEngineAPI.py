#! /usr/bin/python
import serial
import string
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

def DUT_Program():
	#copy binaries to mbed 

def DUT_Reset():
	ser.write("SU000000E")
	time.sleep(0.1)

def Hardware_Engine_Reset():
	ser.write("SR000000E")
	time.sleep(0.1)

def Hardware_Engine_AnalogReading_Enable():
	ser.write("SO000000E")

def Hardware_Engine_StartSending():
	#Send out packets
	f1 = open('/home/embed-autograder/Workspace/autograder/backend/data/packetout', 'rb')
	data = f1.read()
	ser.write(data)

def Hardware_Engine_StartReceiving():
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

		f.write(rec_pack)

		if (rec_pack[1] == 'E'):	
			flag_end_of_tranfer = 1;


	

