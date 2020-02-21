/**
 * \file eeprom_access.c
 * \brief This is a place to organize all functions related to accessing EEPROM.
 *
 * MIT License
 *
 * Copyright (c) 2017 Gregory Gluszek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "eeprom_access.h"

#include "lpc_types.h"
#include "chip.h"
#include "romapi_11xx.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define EEPROM_SIZE (4 * 1024)

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void eepromCmdUsage(void) {
	printf(
		"usage: eeprom read word_size address num_words\n"
		"       eeprom write word_size address value\n"
		"\n"
		"word_size = specify read/write size of 8, 16 or 32 bit word\n"
		"address = specify read/write EEPROM address\n"
		"num_words = specify number of words to read\n"
		"value = specify value to write to EEPROM\n"
	);
}

/**
 * Read from EEPROM and print results to console.
 *
 * \param wordSize Number of bits per word read from EEPROM.
 * \param addr Start address to read from EEPROM.
 * \param numWords Number of words to read from EEPROM.
 *
 * \return 0 on success.
 */
static int eepromReadToConsole(uint32_t wordSize, uint32_t addr, 
	uint32_t numWords) {
	int retval = 0;
	uint32_t bytes_per_word = wordSize / 8;
	uint32_t num_read_bytes = bytes_per_word * numWords;
	uint32_t curr_addr = addr;

	if (wordSize != 8 && wordSize != 16 && wordSize != 32) {
		printf("Invalid word size %d\n", wordSize);
		return -1;
	}
	
	if (addr >= EEPROM_SIZE) {
		printf("Address of 0x%x exceeds EEPROM size of 0x%0x\n",
			addr, EEPROM_SIZE);
		return -1;
	}

	if (addr + numWords > EEPROM_SIZE) {
		printf("Read request of %d words from offset 0x%x"
			" exceeds EEPROM size of 0x%x\n", numWords, addr, 
			EEPROM_SIZE);
		return -1;
	}

	printf("Reading %d %d-bit words starting at 0x%X from EEPROM\n", 
		numWords, wordSize, addr);

	void* read_data = malloc(num_read_bytes);
	if (!read_data){
		printf("malloc failed for read_data buffer\n");
		return -1;
	}

	int err_code = eepromRead(addr, read_data, num_read_bytes);
	if (CMD_SUCCESS != err_code){
		printf("EEPROM Read failed with error code %d\n", 
			err_code);
		retval = -1;
		goto exit;
	}

	for (int word_cnt = 0; word_cnt < numWords; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			printf("\n%03X: ", curr_addr);
		}

		if (wordSize == 8)
		{
			printf("%02X ", ((uint8_t*)read_data)[word_cnt]);
		}
		else if (wordSize == 16)
		{
			printf("%04X ", ((uint16_t*)read_data)[word_cnt]);
		}
		else if (wordSize == 32)
		{
			printf("%08X ", ((uint32_t*)read_data)[word_cnt]);
		}

		curr_addr += bytes_per_word;
	}
	printf("\n");

exit:
	free(read_data);

	return retval;
}

/**
 * Handle EEPROM access command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int eepromCmdFnc(int argc, const char* argv[]) {
	int retval = 0;

	if (argc != 5) {
		eepromCmdUsage();
		return -1;
	}

	uint32_t word_size = strtol(argv[2], NULL, 0);
	uint32_t addr = strtol(argv[3], NULL, 0);

	if (!strcmp("read", argv[1])) {
		uint32_t num_words = strtol(argv[4], NULL, 0);
		retval = eepromReadToConsole(word_size, addr, num_words);
	} else if (!strcmp("write", argv[1])) {
		printf("experimental eeprom write\n");
		uint32_t arr[1] = { strtol(argv[4], NULL, 16) };
		eepromWrite(addr, arr, 1);
		return -1;
	} else {
		printf("Invalid argument \"%s\"\n", argv[1]);
		return -1;
	}

	return retval;
}

/**
 * Wrapper around IAP command for EEPROM read.
 *
 * \param offset EEPROM address to read from (EEPROM is 4 kB)
 * \param[out] readData Location of where to store read data.
 * \param numBytes Number of bytes to read.
 *
 * \return CMD_SUCCESS | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED |
 *	INVALID_COMMAND.
 */
int eepromRead(uint32_t offset, void* readData, uint32_t numBytes){
	unsigned int command_param[5];
	unsigned int status_result[4];

	status_result[0] = INVALID_COMMAND;

	// Command 62 for EEPROM Read
	command_param[0] = 62;
	// EEPROM address (4 kB available)
	command_param[1] = offset;
	// RAM address where to read data to write to EEPROM
	command_param[2] = (unsigned int)readData;
	// Number of bytes to write
	command_param[3] = numBytes;
	// System clock frequency in kHz
	command_param[4] = 46875;

	iap_entry(command_param, status_result);

	return status_result[0];
}

/**
 * Wrapper around IAP command for EEPROM write.
 *
 * \param offset EEPROM address to read from (EEPROM is 4 kB)
 * \param[in] writeData Location of where to store read data.
 * \param numBytes Number of bytes to write.
 *
 * \return CMD_SUCCESS | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED | 
 *	INVALID_COMMAND.
 */
int eepromWrite(uint32_t offset, const void* writeData, uint32_t numBytes){
	unsigned int command_param[5];
	unsigned int status_result[4];

	if (!writeData)
		return INVALID_COMMAND;

	status_result[0] = INVALID_COMMAND;

	// Command 61 for EEPROM Write
	command_param[0] = 61;
	// EEPROM address (4 kB available)
	command_param[1] = offset;
	// RAM address where to read data to write to EEPROM
	command_param[2] = (unsigned int)writeData;
	// Number of bytes to write
	command_param[3] = numBytes;
	// System clock frequency in kHz
	command_param[4] = 46875;

	iap_entry(command_param, status_result);

	return status_result[0];
}
