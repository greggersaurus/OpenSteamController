#!/usr/bin/env python

import usb.core
import usb.util

from datetime import datetime

import sys, getopt

class SteamControllerConfig:
	'Class for configuring of a Steam Controller'

	usbDev = None
	firmwareRev = 0
	bootloaderRev = 0	

	def __init__(self):
		# TODO: only searches for wired controller 
		self.usbDev = usb.core.find(idVendor=0x28DE, idProduct=0x1102)

		if self.usbDev is None:
		    raise ValueError('Wired Steam Controller not found')

		print "Found Wired Steam Controller"

		# TODO: what about reattaching controller on exit?
		if (self.usbDev.is_kernel_driver_active(interface=2)):
			usbDev.detach_kernel_driver(interface=2)

		# TODO: need to claim interface?
		#self.usbDev.claim_interface(

		payload = [0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
		
		# Send request for revision info
		self.usbDev.ctrl_transfer(bmRequestType=0x21, bRequest=9, wValue=0x0300, wIndex=2, 
			data_or_wLength=payload)

		# Get revision info
		payload = self.usbDev.ctrl_transfer(bmRequestType=0xa1, bRequest=1, wValue=0x0300, wIndex=2, 
			data_or_wLength=64)

		for index in range(26, 22, -1):
			self.firmwareRev *= 256 
			self.firmwareRev += payload[index]

		print 'Firmware Revision: 0x', format(self.firmwareRev, '08X'), datetime.fromtimestamp(self.firmwareRev)

		# TODO: what else can we pull from revision info payload? Bootloader revision?

	def playSong(self, index):
		payload = [0xb6, 0x04, index, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]

		self.usbDev.ctrl_transfer(bmRequestType=0x21, bRequest=9, wValue=0x0300, wIndex=2, 
			data_or_wLength=payload)


def main(argv):
	songIndex = 0

	try:
		opts, args = getopt.getopt(argv,"hp:",["playsong="])
	except getopt.GetoptError:
		print 'SteamControllerConfig.py -p <songIndex>'
	for opt, arg in opts:
		if opt == '-h':
			print 'SteamControllerConfig.py -p <songIndex>'
		elif opt in ("-p", "--playsong"):
			songIndex = int(arg)

	controller = SteamControllerConfig()

	controller.playSong(songIndex)

if __name__ == "__main__":
	main(sys.argv[1:])
