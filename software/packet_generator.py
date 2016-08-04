#! /usr/bin/python
import struct

NPack = 5990
tvalue = 10000
f = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'wb')

f.write(b"SS")
f.write(struct.pack('I',0))
f.write(struct.pack('H',NPack))
f.write(b'CE')

f.write(b"SL")
f.write(struct.pack('I',100000)) #Define Sampling Duration here
f.write(struct.pack('H',0))
f.write(b'CE')


for i in range(0,NPack):
	f.write(b"SD")
	#f.write(str(i).zfill(8))
	#f.write(struct.pack("I",i))
	tvalue = tvalue - 1
	f.write(struct.pack('I',tvalue))
	if (i%2):
		f.write(b'\xFF\xFF')
	else:
		f.write(b'\xF0\xF0')
	f.write(b'CE')

f.close()
