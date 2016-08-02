#! /usr/bin/python
import struct

NPack = 6000
f = open('/home/elvis/Workspace/automated-grading-new/data/packetout', 'wb')
for i in range(0,5678):
	f.write(b'S')
	f.write(str(i).zfill(8))
	#f.write(struct.pack("I",i))
	f.write(b'E')
f.close()
