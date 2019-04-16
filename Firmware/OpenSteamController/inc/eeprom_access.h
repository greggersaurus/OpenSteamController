/**
 * \file eeprom_access.h
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

#ifndef _EEPROM_ACCESS_
#define _EEPROM_ACCESS_

#include <stdint.h>

#define CMD_SUCCESS (0) // Command is executed successfully. Sent by ISP handler
	// only when command given by the host has been completely and 
	// successfully executed.
#define INVALID_COMMAND (1) // Invalid command.
#define SRC_ADDR_NOT_MAPPED (2) // Source address is not mapped in the memory 
	// map. Count value is taken in to consideration where applicable.
#define DST_ADDR_NOT_MAPPED (3) // Destination address is not mapped in the 
	// memory map. Count value is taken in to consideration where applicable.

void eepromCmdUsage(void);
int eepromCmdFnc(int argc, const char* argv[]);

int eepromRead(uint32_t offset, void* readData, uint32_t numBytes);
int eepromWrite(uint32_t offset, const void* writeData, uint32_t numBytes);

#endif /* _EEPROM_ACCESS_ */
