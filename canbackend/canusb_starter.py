#!/usr/bin/env python

import serial, time, sys, getopt, array, struct
from datetime import datetime
# graphing and stuff? from pylab import *

# Time to wait between sending serial messages
txdelay = 0.05

# Map baud rates to CANUSB 's'-commands
baudmap = {10 : 'S0',
        20 : 'S1',
        50 : 'S2',
        100 : 'S3',
        125 : 'S4',
        250 : 'S5',
        500 : 'S6',
        800 : 'S7',
        1000 : 'S8'}    

canusb = None

"""Help message for commandline users"""
def usage():
    print "\nusage: candump -f <file to write> -b <baudrate> -p <serial port>"    

"""Write to the CANUSB (it expect \r, not \n\r at end of line"""
def canwrite(line):
    canusb.write(line+'\r')
    
"""Reads bytes from a stream. Parses CAN messages. Outputs CSV."""
def canconvert(stream):
    msgstr = ""
    byte = stream.read(1)
    while byte != '':
        msgstr += byte
        if msgstr[-2:] == "\\r":
            canmsg = canparsemsg(msgstr[:-2])
            # print "%d,%d,%f,%f"%(canmsg.devid, canmsg.msgid, canmsg.data[0], canmsg.data[1])
            msgstr = ""
        byte = stream.read(1)
   
"""Hackish function. Parses a CAN message in CANUSB ASCII format, writes CSV output if we care..."""   
def canparsemsg(msgstr):
    # SEE CANUSB DOCS ON *SENDING* PACKETS (RECV SEEMS TO USE SAME FORMAT)s
    if len(msgstr)==0:
        return
    msgtype = msgstr[0]
    if msgtype != 't':
        print>>sys.stderr, "Skipping CAN message, type %s"%msgtype
        return
        
    # PARSE CAN MESSAGE
    canid = int(msgstr[1:4], 16)
    candevid = canid // (2**6)
    canmsgid = canid % (2**6)
    canlen = int(msgstr[4])
    candatahex = msgstr[5:(6+canlen*2)]
    candatabin = ""
    for i in range(canlen):
        candatabin += chr(int(candatahex[2*i:2*(i+1)],16))
    assert len(candatabin) == canlen
    #assumes all can packets have two floats, which is not the case
    candata = struct.unpack("ff", candatabin)
    
    # OUTPUT THE ONES WE CARE ABOUT AS CSV
    tstamp = datetime.utcnow().isoformat()
    if (candevid, canmsgid) == (0x3, 0xA):
        print "pack_current,%s,%.3f"%(tstamp,candata[0]*0.001)
        print "pack_voltage,%s,%.3f"%(tstamp,candata[1])
    elif (candevid, canmsgid) == (0x3, 0xB):
        print "coulombs_consumed,%s,%.1f"%(tstamp,candata[1])
    else:
        print>>sys.stderr, "Skipping CAN message, don't care about this devid+msgid:"
    print>>sys.stderr, "%d,%d,%s"%(candevid,canmsgid,candatahex)

def main(argv):
    #canparsemsg("t0C78F72B224283855F3F4178")
    #canconvert(open("C:/Users/erkslappy/Desktop/data.txt","r"))

    # Parse arguments
    try:
        opts, args = getopt.getopt(argv, "hb:p:", ["help", "baud=", "port="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    baud = 115600
    port = "COM1"
    
    # Assign arguments
    for opt,arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        if opt in ('-b', '--baud'):
            baud = int(arg)
        if opt in ('-p', '--port'):
            port = arg
    
    
    # Open CANUSB
    canusb = serial.Serial(port, 115200, dsrdtr=False, timeout=5)
    time.sleep(txdelay)
            
    # Set the CANUSB baud rate
    canwrite(baudmap[baud])
    time.sleep(txdelay)
    # Open the CAN channel
    canwrite("O")
    time.sleep(txdelay)
    
    # do stuff
    print "measurement,timestamp_utc,value"
    try:
        canconvert(canub)
    except:
        print>>sys.stderr, "Caught error, dying..."
        
    # Close the CAN channel
    canwrite("C")
    time.sleep(txdelay)
    print>>sys.stderr, "Bye!"
    
if __name__ == "__main__":
    main(sys.argv[1:])