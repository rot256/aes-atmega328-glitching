#!/usr/bin/env python3

import sys
import serial
import binascii

ser = serial.Serial(sys.argv[1])
out = open(sys.argv[2], 'w')

cts = set([])

print('Collecting samples from:', ser.name)

line = b''
while 1:
    line += ser.read()
    if b'\n' in line:
        line = line.strip()
        try:
            line = binascii.unhexlify(line)
            if len(line) == 16 and line not in cts:
                print('New:', line.hex())
                out.write(line.hex() + '\n')
                out.flush()
                cts.add(line)
        except binascii.Error:
            pass
        line = b''
ser.close()
