/**
 * \file mem_access.c
 * \brief Encompasses functions for memory peak poke command handling.
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

#include "mem_access.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void memCmdUsage(void) {
	printf(
		"usage: mem read word_size address num_words\n"
		"       mem write word_size address value\n"
		"\n"
		"word_size = specify read/write size of 8, 16 or 32 bit word\n"
		"address = specify read/write memory mapped address\n"
		"num_words = specify number of words to read\n"
		"value = specify value to write to memory mapped address\n"
	);
}

/**
 * Read from memory mapped region and print results to console.
 *
 * \param wordSize Number of bits per word read from memory mapped region.
 * \param addr Start offset to read from in memory mapped region.
 * \param numWords Number of words to read from memory mapped region.
 *
 * \return 0 on success.
 */
static int memReadToConsole(uint32_t wordSize, uint32_t addr, 
	uint32_t numWords) {
	uint32_t bytes_per_word = wordSize / 8;
	uint32_t curr_addr = addr;

	if (wordSize != 8 && wordSize != 16 && wordSize != 32) {
		printf("Invalid word size %d\n", wordSize);
		return -1;
	}
	
	printf("Reading %d %d-bit words starting at 0x%X from memory "
		"mapped region\n", numWords, wordSize, addr);

	void* read_data = (void*)addr;

	for (int word_cnt = 0; word_cnt < numWords; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			printf("\n%08X: ", curr_addr);
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

	return 0;
}

/**
 * Handle memory (mapped) access command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int memCmdFnc(int argc, const char* argv[]) {
	int retval = 0;

	if (argc != 5) {
		memCmdUsage();
		return -1;
	}

	uint32_t word_size = strtol(argv[2], NULL, 0);
	uint32_t addr = strtol(argv[3], NULL, 0);

	if (!strcmp("read", argv[1])) {
		uint32_t num_words = strtol(argv[4], NULL, 0);
		retval = memReadToConsole(word_size, addr, num_words);
	} else if (!strcmp("write", argv[1])) {
		//TODO: implement this
		printf("memory mapped writing not implemented yet\n");
		return -1;
	} else {
		printf("Invalid argument \"%s\"\n", argv[1]);
		return -1;
	}

	return retval;
}
