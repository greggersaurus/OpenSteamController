#!/usr/bin/env python

import usb.core
import usb.util

import sys, getopt

dev = None

def playSong(index):
	payload = [0xb6, 0x04, index, 0x00 , 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 
		0x00 , 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 
		0x00, 0x00 , 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00]

	global dev
	dev.ctrl_transfer(bmRequestType=0x21, bRequest=9, wValue=0x0300, wIndex=2, 
		data_or_wLength=payload)

def main(argv):
	try:
		opts, args = getopt.getopt(argv,"hp:",["playsong="])
	except getopt.GetoptError:
		print 'SteamControllerConfig.py -p <songIndex>'
	for opt, arg in opts:
		if opt == '-h':
			print 'SteamControllerConfig.py -p <songIndex>'
		elif opt in ("-p", "--playsong"):
			songIndex = int(arg)
		
	##TODO: make class that inherits from pyusb instead of using global?
	global dev

	# find our device
	dev = usb.core.find(idVendor=0x28DE, idProduct=0x1102)

	# was it found?
	if dev is None:
	    raise ValueError('Wired Steam Controller not found')

	print "Found Wired Steam Controller"

	# TODO: what about reattaching controller on exit?
	if (dev.is_kernel_driver_active(interface=2)):
		dev.detach_kernel_driver(interface=2)

	# TODO: need to claim interface?
	#dev.claim_interface(

	playSong(songIndex)

if __name__ == "__main__":
	main(sys.argv[1:])
