#!/usr/bin/env python3

import sys
import serial
import binascii

ser = serial.Serial(sys.argv[1])

try:
    with open(sys.argv[2], 'r') as f:
        cts = map(str.strip, f.readlines())
        cts = [binascii.unhexlify(x) for x in cts]
        cts = set(cts)
except IOError:
    cts = set([])

out = open(sys.argv[2], 'a')

print('Samples:', len(cts))
print('Collecting samples from:', ser.name)

line = b''
while 1:
    line += ser.read()
    if b'\n' in line:
        line = line.strip()
        try:
            line = binascii.unhexlify(line)
            if len(line) == 16:
                print('Ciphertext:', line.hex())
                if line not in cts:
                    print('New:', line.hex())
                    out.write(line.hex() + '\n')
                    out.flush()
                    cts.add(line)
        except binascii.Error:
            pass
        line = b''
ser.close()
