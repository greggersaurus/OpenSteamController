#!/usr/bin/env python

import sys 
import getopt
import struct

class FirmwareParser:
	"""
	Takes Steam Controller firmware and decodes it as best as possible.
	"""

	# List of all data words read from firmware file. Combined and
	#  categorized as best as possible.
	dataWords = []

##	traceBuffer # Array of instructions that have been execute during emulation
##	# TODO: create functions (read/write) to access memory (controls if emulator try to access inappropriate space, etc.)
##			# 0x0000 0000 (SIZE 0x0002 0000) On Chip Flash
##			binFile #TODO: able to query offset in file, or need separate variable (i.e. programCounter)?
##				# 0x0000 0000 - 0x0000 00BC Vector Table
##			vectorTable # Array of vector addresses? Create enums as logical means to access?
##		SRAM0 	# 0x1000 0000 (SIZE 0x0000 2000) SRAM0
##			stackBuffer # Memory for what is pushed on the stack
##			# 0x1FFF 0000 (SIZE 0x0000 4000) boot ROM
##		SRAM1 	# 0x2000 0000 (SIZE 0x0000 0800) SRAM1
##		SRAM2 	# 0x2000 4000 (SIZE 0x0000 0800) SRAM2
##			# 0x4000 0000 (SIZE 0x0000 4000) APB Peripheral I2C Bus
##			# 0x4000 4000 (SIZE 0x0000 4000) APB Peripheral WWDT
##			# 0x4000 8000 (SIZE 0x0000 4000) APB Peripheral USART/SMART CARD
##			# 0x4000 C000 (SIZE 0x0000 4000) APB Peripheral 16-bit counter/timer 0
##			# 0x4001 0000 (SIZE 0x0000 4000) APB Peripheral 16-bit counter/timer 1
##			# 0x4001 4000 (SIZE 0x0000 4000) APB Peripheral 32-bit counter/timer 0
##			# 0x4001 8000 (SIZE 0x0000 4000) APB Peripheral 32-bit counter/timer 1
##			# 0x4001 C000 (SIZE 0x0000 4000) APB Peripheral ADC
##			# 0x4003 8000 (SIZE 0x0000 4000) APB Peripheral PMU
##			# 0x4003 C000 (SIZE 0x0000 4000) APB Peripheral flash/EEPROM controller
##			# 0x4004 0000 (SIZE 0x0000 4000) APB Peripheral SSP0
##			# 0x4004 4000 (SIZE 0x0000 4000) APB Peripheral IOCON
##			# 0x4004 8000 (SIZE 0x0000 4000) APB Peripheral system control
##			# 0x4004 C000 (SIZE 0x0000 4000) APB Peripheral GPIO interrupts
##			# 0x4005 8000 (SIZE 0x0000 4000) APB Peripheral SSP1 
##			# 0x4005 C000 (SIZE 0x0000 4000) APB Peripheral GPIO GROUP0 INT
##			# 0x4006 0000 (SIZE 0x0000 4000) APB Peripheral GPIO GROUP1 INT
##			# 0x4008 0000 (SIZE 0x0000 4000) USB
##			# 0x5000 0000 (SIZE 0x0000 4000) GPIO
##			# 0xE000 0000 (SIZE 0x0010 0000) Private Peripheral Bus
##	# TODO registers for emulation
##		# Stack pointer
##		# Program counter

	def __init__(self, binFilename):
		"""
		Constructor
			
		Params:
		binFilename Path to binary firmware file
		"""

		# Create the initial list of dataWords by reading the fw file
		binFile = open(binFilename, 'rb')

		offset = 0
		word16 = self.__read16(binFile)	
		while (word16 != -1):
			self.dataWords += [DataWord(word16, offset)]
			offset += 2
			word16 = self.__read16(binFile)	
	
		binFile.close()

		# Identify vector table data

	def __read16(self, inFile):
		"""
		Read 16 bit word (assuming little endian)

		Params:
		inFile File to read from
		"""

#		print "enter"

		# Read byte 0
		byte0 = inFile.read(1)
		if (byte0 == ""):
			return -1

#		print byte0.encode('hex')
	
		# Read byte 1
		byte1 = inFile.read(1) 
		if (byte1 == ""):
			return struct.unpack('B', byte0)[0]
		
#		print byte1.encode('hex')
	
		retval = struct.unpack('B', byte1)[0] << 8 | struct.unpack('B', byte0)[0]

#		print retval 

		return retval

	def __str__(self):
#TODO: this is stupid, understand python way of doing things
		for data in self.dataWords:
			print data
		
		return ''
		

class DataWord:
	"""
	Class to encapsulate a 32 or 16-bit data word read from the firmware file.
	"""

	# Raw binary data from firmware file
	binData = 0
	# Memory location in firmware where this data word can be found
	__offset = 0

	def __init__(self, binData, offset):
		"""
		Constructor

		Params:
		binData Raw binary data read from firmware file
		offset Memory location offset in firmware where this data 
			was read from
		"""

		self.binData = binData
		self.__offset = offset

	def __str__(self):
		retval = '';

		# Offset address in hex
		retval += '{0:06x}: '.format(self.__offset)

		# Upper 16 bits of raw binary data
		if (self.binData > 0xFFFF):
			retval += ' {0:04x}'.format(self.binData/0x10000)
		else:
			retval += '     '

		# Lower 16 bits of raw binary data
		retval += ' {0:04x}'.format(self.binData%0x10000)

		return retval

def main(argv):
	print "Hello World"

	try:
		opts, args = getopt.getopt(argv,"hi:",["inputfile="])
	except getopt.GetoptError:
		print 'FirmwareParser.py -i <inputFile>'
	for opt, arg in opts:
		if opt == '-h':
			print 'FirmwareParser.py -i <inputFile>'
		elif opt in ("-i", "--inputfile"):
			parser = FirmwareParser(arg)
			print parser


if __name__ == "__main__":
	main(sys.argv[1:])
