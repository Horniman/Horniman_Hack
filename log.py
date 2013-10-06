#! /usr/bin/env python
""" Log radios to file """

from __future__ import division, absolute_import, print_function, unicode_literals
import sys
import serial
import datetime
import time

def main(port):
    radio = serial.Serial(port, baudrate=9600)
    next = time.time() + 10
    while True:
        while radio.read() != 'a':
            if time.time() - next > 0:
                next = time.time() + 10
                radio.write('aHATEMP-----')
                radio.flush()
        id = radio.read(2)
        msg = radio.read(9)
        print("%s %s %s" % (
            datetime.datetime.now(),
            id,
            msg
        ))

if __name__ == '__main__':
    main(sys.argv[1])
