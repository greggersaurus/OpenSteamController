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

/**
 * Wrapper around IAP command for EEPROM read.
 *
 * \param offset EEPROM address to read from (EEPROM is 4 kB)
 * \param readData Location of where to store read data.
 * \param numBytes Number of bytes to read.
 *
 * \return CMD_SUCCESS | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED.
 */
int eeprom_read(uint32_t offset, void* readData, uint32_t numBytes){
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

//TODO:
//	iap_entry(command_param, status_result);

	return status_result[0];
}

int eeprom_write(uint32_t offset, const void* writeData, uint32_t numBytes){
	//TODO: implement this
	return INVALID_COMMAND;
}
