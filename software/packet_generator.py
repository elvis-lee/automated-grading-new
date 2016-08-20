#! /usr/bin/python
import struct

NPack = 500
tvalue = 1
f = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'wb')

f.write(b"SL")
f.write(struct.pack('I',1000)) #Define Sampling Duration here
f.write(struct.pack('H',0))
f.write(b'E')

for i in range(0,NPack):
	f.write(b"SA")
	f.write(struct.pack('I',tvalue))
	if (i%2):
		f.write(b'\xFF\xFF')
	else:
		f.write(b'\x00\x00')
	f.write(b'E')


	f.write(b"SD")
	f.write(struct.pack('I',tvalue))
	if (i%2):
		f.write(b'\x00\xFF')
	else:
		f.write(b'\xFF\x00')
	f.write(b'E')

	tvalue = tvalue + 1

f.close()
