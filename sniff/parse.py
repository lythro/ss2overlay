import matplotlib.pyplot as pl
import struct


def hex2float(s):
	bins = ''.join(chr(int(s[x:x+2], 16)) for x in range(0, len(s), 2))
	return struct.unpack('>f', bins)[0]


file = open( "hill" )
#file = open( "cap2" )

line = file.readline()

ENQ = "05"
EOT = "04"

split = line.split( "05" )
y = []
for sub in split:
	subsplit = sub.split( "04" )

	for s in subsplit:
		if len(s) == 2 or len(s) == 4:
			y.append( -int(s, 16) )

pl.plot( y )

pl.grid()
pl.show()

file.close()




