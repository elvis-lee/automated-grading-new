#! /usr/bin/python
import serial
import string
from timeit import Timer
import timeit

BAUDRATE = 460800
npackrec = 0
M = 100
flag_bad_pack = 0
flag_end_of_tranfer = 0
data = ""
success = 0
fail = 0


ser = serial.Serial()
ser.port = "/dev/ttyUSB0"    
ser.baudrate = BAUDRATE    
ser.parity=serial.PARITY_NONE
ser.bytesize=serial.EIGHTBITS
ser.stopbits=serial.STOPBITS_ONE
ser.timeout=0.01
ser.writeTimeout=None
ser.open() 

#Get Packet Ready here
f1 = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'rb')
data = f1.read()


def com1():
 global flag_bad_pack
 global npackrec
 global success
 global fail
 global flag_end_of_tranfer
 #Send Packet
 ser.write(data)

 #Receive Packet
 while(ser.in_waiting < 9):
 	pass
 while (not flag_end_of_tranfer):
 	for i in range(1, M+1): 
		rec_pack = ser.read(9)
		print "Received=%d packets" %i 
		print rec_pack
		print(' '.join(format(ord(x), 'b') for x in rec_pack[2])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[3])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[4])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[5])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[6])).zfill(8)
		print(' '.join(format(ord(x), 'b') for x in rec_pack[7])).zfill(8)
	

		if (rec_pack == ""):
			flag_bad_pack = 1
		elif (rec_pack[0]!='S'):
			flag_bad_pack = 1
		elif (rec_pack[1] == 'E'):
			flag_end_of_tranfer = 1;
			break;

		
		
	if flag_bad_pack:
		ser.write("SB000000E")
		print "------detected back packet------"
		print "------resend again------"
		flag_bad_pack = 0
		fail = fail + 1
	else:
		ser.write("SG000000E")
		print "------transfer is good-------"
		npackrec = npackrec + i
		success = success + 1

 return

t=Timer(com1,setup="") 
t1=t.timeit(1)
print "Success : %d time(s) Fail: %d time(s)" %(success,fail)
print "Total Time=%fs" %(t1) 