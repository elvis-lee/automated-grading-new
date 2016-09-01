#! /usr/bin/python
import struct

NPack = 0
tvalue = 0
f = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'wb')



f.write(b"SP")
f.write(struct.pack('I',NPack*2)) #Define total number of packet here
f.write(struct.pack('H',0))
f.write(b'E')

f.write(b"SL")
f.write(struct.pack('I',50000)) #Define Sampling Duration here
f.write(struct.pack('H',0))
f.write(b'E')

for i in range(0,NPack):
	f.write(b"SA")
	f.write(struct.pack('I',tvalue))
	if (i%2):
		f.write(b'\xFF\xFF')
	else:
		f.write(b'\xFF\xFF')
	f.write(b'E')


	f.write(b"SD")
	f.write(struct.pack('I',tvalue))
	if (i%2):
		f.write(b'\x00\x00')
	else:
		f.write(b'\xFF\xFF')
	f.write(b'E')

	tvalue = tvalue + 50

f.close()
